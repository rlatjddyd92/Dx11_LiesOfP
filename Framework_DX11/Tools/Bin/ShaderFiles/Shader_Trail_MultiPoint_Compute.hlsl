#define PI 3.141592f

#define STATE_ORBIT 0x0001
#define STATE_RANDOM 0x0002
#define STATE_LOOP 0x0004
#define STATE_ACCEL 0x0008
#define STATE_DECEL 0x0010

struct Particle
{
    float3 vPosition;
    float2 vPSize;
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float2 vLifeTime;
    float4 vColor;
    float fSpeed;
    float4 vCurrenrRandomDir;
    float4 vNextRandomDir;
};

struct HeadParticle
{
    Particle particle;
    float4 vPreTranslation;
    float4 vCurrentRandomPos;
    float4 vNextRandomPos;
    uint iTailInitIndex;
};

struct TailParticle
{
    Particle particle;
    float4 vMoveDir;
};

// UAV로 접근할 수 있는 Structured Buffer 선언
RWStructuredBuffer<HeadParticle> HeadParticles : register(u0);
RWStructuredBuffer<TailParticle> TailParticles : register(u1);

StructuredBuffer<HeadParticle> InitParticles : register(t0);


cbuffer MovementBuffer : register(b0) // 받아온 걸 상수로 쓰기 위한 버퍼인듯?
{
    uint iNumInstance;
    uint iNumTailInstance;
    uint iState;
    float fGravity;

    float4 vPivot;

    float4 vMoveDir;

    float4x4 WorldMatrix;

    float3 vOrbitAxis;
    float fOrbitAngle;

    float fTimeInterval;
    float fRandomRatio;
    float fAccelSpeed;
    float fAccelLimit;

    float fTimeDelta;
    float fTailInterval;
    float fTailDistance;
    float fPadding;

    float4 vTailMoveDir;
}

float rand(float seed);
float3 RotateByAxis(float3 vVector, float3 vAxis, float fAngle);
float3 CosineInterpolate(float3 vFirst, float3 vSecond, float fT);

[numthreads(256, 1, 1)]
void CS_SPREAD_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= iNumInstance * iNumTailInstance)
        return;
    
    uint iHeadIndex = iIndex / iNumInstance;
    TailParticle TailParticle = TailParticles[iIndex];
    if (0.f == fPadding)
    {
        TailParticle.particle.vTranslation = mul(TailParticle.particle.vTranslation, WorldMatrix);
    }
    
    float4 vResultTailDir = vTailMoveDir + TailParticle.vMoveDir;
    TailParticle.particle.vTranslation += normalize(vResultTailDir) * TailParticle.particle.fSpeed * fTimeDelta;
    TailParticle.particle.vLifeTime.y += fTimeDelta;
    
    if (TailParticle.particle.vLifeTime.x < TailParticle.particle.vLifeTime.y)
        TailParticle.particle.vLifeTime.y = 0.f;
    
    TailParticles[iIndex] = TailParticle;
    
    if(iHeadIndex == iIndex / iNumTailInstance)
    {
        HeadParticle HeadParticle = HeadParticles[iHeadIndex];
    
        float4 vWorldPivot = mul(vPivot, WorldMatrix);
    
        if (0.f == fPadding)
        {
            HeadParticle.particle.vTranslation = mul(HeadParticle.particle.vTranslation, WorldMatrix);
            HeadParticle.particle.vRight = vWorldPivot;
            HeadParticle.particle.vUp = float4(vOrbitAxis.x, vOrbitAxis.y, vOrbitAxis.z, 0.f);
        }
    
        float4 vDir = HeadParticle.particle.vTranslation - vWorldPivot;
        vDir = normalize(vDir);
    
        HeadParticle.particle.vLifeTime.y += fTimeDelta;
        
        float fTime = fmod(HeadParticle.particle.vLifeTime.y, fTimeInterval);
        if (fTime < fTimeDelta)
        {
            if (iState & STATE_RANDOM)
            {
                HeadParticle.particle.vCurrenrRandomDir = HeadParticle.particle.vNextRandomDir;
            
                HeadParticle.particle.vNextRandomDir.x = rand(HeadParticle.particle.vTranslation.x);
                HeadParticle.particle.vNextRandomDir.y = rand(HeadParticle.particle.vTranslation.y);
                HeadParticle.particle.vNextRandomDir.z = rand(HeadParticle.particle.vTranslation.z);
            
                HeadParticle.particle.vNextRandomDir = normalize(HeadParticle.particle.vNextRandomDir);
                
                vDir += float4(lerp(HeadParticle.particle.vCurrenrRandomDir.xyz, HeadParticle.particle.vNextRandomDir.xyz, fTime / fTimeInterval) * fRandomRatio, 0.f);
                vDir = normalize(vDir);
            }
            
            HeadParticle.vCurrentRandomPos = HeadParticle.vNextRandomPos;
            HeadParticle.vNextRandomPos = rand(HeadParticle.particle.vTranslation.x * HeadParticle.particle.vTranslation.y);
            HeadParticle.vNextRandomPos = rand(HeadParticle.particle.vTranslation.y * HeadParticle.particle.vTranslation.z);
            HeadParticle.vNextRandomPos = rand(HeadParticle.particle.vTranslation.z * HeadParticle.particle.vTranslation.x);
            
            HeadParticle.vNextRandomPos = normalize(HeadParticle.vNextRandomPos) * fTailDistance;
        }
    
        float4 vRotateDir = (float4) 0;
        if (iState & STATE_ORBIT)
        {
            float4 vTargetDir = HeadParticle.particle.vRight - HeadParticle.particle.vTranslation;
            float3 vAxis = float3(HeadParticle.particle.vUp.x, HeadParticle.particle.vUp.y, HeadParticle.particle.vUp.z);
            vRotateDir = float4(RotateByAxis(vTargetDir.xyz, vAxis, radians(fOrbitAngle) * fTimeDelta), 0.f);
            vRotateDir = vTargetDir - vRotateDir;
        }
    
        float fAddSpeed = 1.f;
        if (iState & STATE_ACCEL)
        {
            fAddSpeed *= HeadParticle.particle.vLifeTime.y / HeadParticle.particle.vLifeTime.x * fAccelSpeed;
            if (fAddSpeed < fAccelLimit)
                fAddSpeed = fAccelLimit;
        }
        else if (iState & STATE_DECEL)
        {
            fAddSpeed *= 1.f - (HeadParticle.particle.vLifeTime.y / HeadParticle.particle.vLifeTime.x) * fAccelSpeed;
            if (fAddSpeed < 0.f)
                fAddSpeed = 0.f;
            if (fAddSpeed < fAccelLimit)
                fAddSpeed = fAccelLimit;
        }
    
        float4 vMoveDir = vDir * HeadParticle.particle.fSpeed;
        vMoveDir.y -= fGravity * HeadParticle.particle.vLifeTime.y;
    
        HeadParticle.particle.vTranslation = HeadParticle.particle.vTranslation + (vMoveDir * fTimeDelta + vRotateDir) * fAddSpeed;

        if ((iState & STATE_LOOP) && (HeadParticle.particle.vLifeTime.y >= HeadParticle.particle.vLifeTime.x))
        {
            HeadParticle.particle.vTranslation = InitParticles[iHeadIndex].particle.vTranslation;
            HeadParticle.particle.vLifeTime.y = 0.f;
            HeadParticle.particle.vTranslation = mul(HeadParticle.particle.vTranslation, WorldMatrix);
            HeadParticle.particle.vRight = vWorldPivot;
            HeadParticle.particle.vUp = float4(vOrbitAxis.x, vOrbitAxis.y, vOrbitAxis.z, 0.f);
        }
        
        
        float4 vPrePos = HeadParticle.vPreTranslation;
        while (true)
        {
            float3 vCurrentDir = (HeadParticle.particle.vTranslation - vPrePos).xyz;
            if (length(vCurrentDir) <= fTailInterval)
                break;
            
            vPrePos += float4(normalize(vCurrentDir) * fTailInterval, 0.f);
            float3 vRandomPos = CosineInterpolate(HeadParticle.vCurrentRandomPos.xyz, HeadParticle.vNextRandomPos.xyz, fTime);
            
            TailParticles[HeadParticle.iTailInitIndex].particle.vTranslation = vPrePos + float4(vRandomPos, 0.f);
            TailParticles[HeadParticle.iTailInitIndex].vMoveDir = float4(vRandomPos, 0.f);
            TailParticles[HeadParticle.iTailInitIndex].particle.vLifeTime.y = 0.f;
            ++HeadParticle.iTailInitIndex;
            
            if (HeadParticle.iTailInitIndex > InitParticles[iHeadIndex].iTailInitIndex + iNumTailInstance)
                HeadParticle.iTailInitIndex = InitParticles[iHeadIndex].iTailInitIndex;
        }
        HeadParticle.particle.vLook = normalize(vMoveDir * fTimeDelta + vRotateDir);
        HeadParticle.vPreTranslation = HeadParticle.particle.vTranslation;

        HeadParticles[iHeadIndex] = HeadParticle;
    }
}

float rand(float seed)
{
    float fRandom = frac(sin(seed) * 43758.5453123);
    return fRandom * 2.f - 1.f;
}

float3 RotateByAxis(float3 vVector, float3 vAxis, float fAngle)
{
    float fCosAngle = cos(fAngle);
    float fSinAngle = sin(fAngle);
    
    vAxis = normalize(vAxis);
    
    float3 vRotatedVec = vVector * fCosAngle + cross(vAxis, vVector) * fSinAngle + vAxis * dot(vAxis, vVector) * (1.f - fCosAngle);
    
    return vRotatedVec;
}

float3 CosineInterpolate(float3 vFirst, float3 vSecond, float fT)
{
    float3 vReturn;
    vReturn.x = vFirst.x + (vSecond.x - vFirst.x) * ((1.f - cos(fT * PI)) / 2.f);
    vReturn.y = vFirst.y + (vSecond.y - vFirst.y) * ((1.f - cos(fT * PI)) / 2.f);
    vReturn.z = vFirst.z + (vSecond.z - vFirst.z) * ((1.f - cos(fT * PI)) / 2.f);
    
    return vReturn;
}