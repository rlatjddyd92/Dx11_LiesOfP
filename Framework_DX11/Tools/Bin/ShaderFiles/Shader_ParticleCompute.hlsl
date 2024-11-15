
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

// UAV로 접근할 수 있는 Structured Buffer 선언
RWStructuredBuffer<Particle> particles : register(u0);
StructuredBuffer<Particle> InitParticles : register(t0);

// 이게 매프레임 넘겨주는 버퍼
cbuffer MovementBuffer : register(b0)   // 받아온 걸 상수로 쓰기 위한 버퍼인듯?
{
    uint                iNumInstance;
    uint	            iState;
    float2              pad_1;      // 크기를 맞추기 위한 더미 변수 16바이트를 맞춰야함
    
    float4              vPivot;
    
	float	            fGravity;
    float3              pad_2;
    
    float4              vMoveDir;
    row_major matrix    WorldMatrix;
    
    float3              vOrbitAxis;
	float	            fOrbitAngle;
    
	float	            fTimeInterval;
	float	            fRandomRatio;
    float	            fAccelSpeed;
	float	            fAccelLimit;
    
	float	            fTimeDelta;
    float3              pad_3;
}

float rand(float seed);
float3 RotateByAxis(float3 vVector, float3 vAxis, float fAngle);

[numthreads(256, 1, 1)]
void CS_SPREAD_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= iNumInstance)
        return;
    
    Particle particle = particles[iIndex];
    
    float4 vWorldPivot = mul(vPivot, WorldMatrix);
    
    if (0.f == pad_1.x)
    {
        particle.vTranslation = mul(particle.vTranslation, WorldMatrix);
        particle.vRight = vWorldPivot;
        particle.vUp = float4(vOrbitAxis.x, vOrbitAxis.y, vOrbitAxis.z, 0.f);
    }
    
    float4 vDir = particle.vTranslation - vWorldPivot;
    vDir = normalize(vDir);
    
    particle.vLifeTime.y += fTimeDelta;
    
    if (iState & STATE_RANDOM)
    {
        float fTime = fmod(particle.vLifeTime.y, fTimeInterval);
        if (fTime < fTimeDelta)
        {
            particle.vCurrenrRandomDir = particle.vNextRandomDir;
            
            particle.vNextRandomDir.x = rand(particle.vTranslation.x);
            particle.vNextRandomDir.y = rand(particle.vTranslation.y);
            particle.vNextRandomDir.z = rand(particle.vTranslation.z);
            
            particle.vNextRandomDir = normalize(particle.vNextRandomDir);
        }
        vDir += float4(lerp(particle.vCurrenrRandomDir.xyz, particle.vNextRandomDir.xyz, fTime / fTimeInterval) * fRandomRatio, 0.f);
        vDir = normalize(vDir);
    }
    
    float4 vRotateDir = (float4) 0;
    if (iState & STATE_ORBIT)
    {
        float4 vTargetDir = particle.vRight - particle.vTranslation;
        float3 vAxis = float3(particle.vUp.x, particle.vUp.y, particle.vUp.z);
        vRotateDir = float4(RotateByAxis(vTargetDir.xyz, vAxis, radians(fOrbitAngle) * fTimeDelta), 0.f);
        vRotateDir = vTargetDir - vRotateDir;
    }
    
    float fAddSpeed = 1.f;
    if (iState & STATE_ACCEL)
    {
        fAddSpeed *= particle.vLifeTime.y / particle.vLifeTime.x * fAccelSpeed;
        if (fAddSpeed < fAccelLimit)
            fAddSpeed = fAccelLimit;
    }
    else if (iState & STATE_DECEL)
    {
        fAddSpeed *= 1.f - (particle.vLifeTime.y / particle.vLifeTime.x) * fAccelSpeed;
        if (fAddSpeed < 0.f)
            fAddSpeed = 0.f;
        if (fAddSpeed < fAccelLimit)
            fAddSpeed = fAccelLimit;
    }
    
    float4 vMoveDir = vDir * particle.fSpeed;
    vMoveDir.y -= fGravity * particle.vLifeTime.y;
    
    particle.vTranslation = particle.vTranslation + (vMoveDir * fTimeDelta + vRotateDir) * fAddSpeed;

    if ((iState & STATE_LOOP) && (particle.vLifeTime.y >= particle.vLifeTime.x))
    {
        particle = InitParticles[iIndex];
        particle.vLifeTime.y = 0.f;
        particle.vTranslation = mul(particle.vTranslation, WorldMatrix);
        particle.vRight = vWorldPivot;
        particle.vUp = float4(vOrbitAxis.x, vOrbitAxis.y, vOrbitAxis.z, 0.f);
    }
    
    particle.vLook = normalize(vMoveDir * fTimeDelta + vRotateDir);
    
    particles[iIndex] = particle;
}

[numthreads(256, 1, 1)]
void CS_MOVE_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= iNumInstance)
        return;
    
    Particle particle = particles[iIndex];
    
    float4 vWorldPivot = mul(vPivot, WorldMatrix);
    
    if (0.f == pad_1.x)
    {
        particle.vTranslation = mul(particle.vTranslation, WorldMatrix);
        particle.vRight = vWorldPivot;
        particle.vUp = float4(vOrbitAxis.x, vOrbitAxis.y, vOrbitAxis.z, 0.f);
    }
    
    float4 vDir = vMoveDir;
    vDir = normalize(vDir);
    
    particle.vLifeTime.y += fTimeDelta;
    
    if (iState & STATE_RANDOM)
    {
        float fTime = fmod(particle.vLifeTime.y, fTimeInterval);
        if (fTime < fTimeDelta)
        {
            particle.vCurrenrRandomDir = particle.vNextRandomDir;
            
            particle.vNextRandomDir.x = rand(particle.vTranslation.x);
            particle.vNextRandomDir.y = rand(particle.vTranslation.y);
            particle.vNextRandomDir.z = rand(particle.vTranslation.z);
            
            particle.vNextRandomDir = normalize(particle.vNextRandomDir);
        }
        vDir += float4(lerp(particle.vCurrenrRandomDir.xyz, particle.vNextRandomDir.xyz, fTime / fTimeInterval) * fRandomRatio, 0.f);
        vDir = normalize(vDir);
    }
    
    float4 vRotateDir = (float4) 0;
    if (iState & STATE_ORBIT)
    {
        float4 vTargetDir = particle.vRight - particle.vTranslation;
        float3 vAxis = float3(particle.vUp.x, particle.vUp.y, particle.vUp.z);
        vRotateDir = float4(RotateByAxis(vTargetDir.xyz, vAxis, radians(fOrbitAngle) * fTimeDelta), 0.f);
        vRotateDir = vTargetDir - vRotateDir;
    }
    
    float fAddSpeed = 1.f;
    if (iState & STATE_ACCEL)
    {
        fAddSpeed *= particle.vLifeTime.y / particle.vLifeTime.x * fAccelSpeed;
        if (fAddSpeed < fAccelLimit)
            fAddSpeed = fAccelLimit;
    }
    else if (iState & STATE_DECEL)
    {
        fAddSpeed *= 1.f - (particle.vLifeTime.y / particle.vLifeTime.x) * fAccelSpeed;
        if (fAddSpeed < 0.f)
            fAddSpeed = 0.f;
        if (fAddSpeed < fAccelLimit)
            fAddSpeed = fAccelLimit;
    }
    
    float4 vMoveDir = vDir * particle.fSpeed;
    vMoveDir.y -= fGravity * particle.vLifeTime.y;
    
    particle.vTranslation = particle.vTranslation + (vMoveDir * fTimeDelta + vRotateDir) * fAddSpeed;

    if ((iState & STATE_LOOP) && (particle.vLifeTime.y >= particle.vLifeTime.x))
    {
        particle = InitParticles[iIndex];
        particle.vLifeTime.y = 0.f;
        particle.vTranslation = mul(particle.vTranslation, WorldMatrix);
        particle.vRight = vWorldPivot;
        particle.vUp = float4(vOrbitAxis.x, vOrbitAxis.y, vOrbitAxis.z, 0.f);
    }
    
    particle.vLook = normalize(vMoveDir * fTimeDelta + vRotateDir);
    
    particles[iIndex] = particle;
}

[numthreads(256, 1, 1)]
void CS_CONVERGE_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= iNumInstance)
        return;
    
    Particle particle = particles[iIndex];
    
    float4 vWorldPivot = mul(vPivot, WorldMatrix);
    
    if (0.f == pad_1.x)
    {
        particle.vTranslation = mul(particle.vTranslation, WorldMatrix);
        particle.vRight = vWorldPivot;
        particle.vUp = float4(vOrbitAxis.x, vOrbitAxis.y, vOrbitAxis.z, 0.f);
    }
    
    float4 vDir = vWorldPivot - particle.vTranslation;
    vDir = normalize(vDir);
    
    particle.vLifeTime.y += fTimeDelta;
    
    if (iState & STATE_RANDOM)
    {
        float fTime = fmod(particle.vLifeTime.y, fTimeInterval);
        if (fTime < fTimeDelta)
        {
            particle.vCurrenrRandomDir = particle.vNextRandomDir;
            
            particle.vNextRandomDir.x = rand(particle.vTranslation.x);
            particle.vNextRandomDir.y = rand(particle.vTranslation.y);
            particle.vNextRandomDir.z = rand(particle.vTranslation.z);
            
            particle.vNextRandomDir = normalize(particle.vNextRandomDir);
        }
        vDir += float4(lerp(particle.vCurrenrRandomDir.xyz, particle.vNextRandomDir.xyz, fTime / fTimeInterval) * fRandomRatio, 0.f);
        vDir = normalize(vDir);
    }
    
    float4 vRotateDir = (float4) 0;
    if (iState & STATE_ORBIT)
    {
        float4 vTargetDir = particle.vRight - particle.vTranslation;
        float3 vAxis = float3(particle.vUp.x, particle.vUp.y, particle.vUp.z);
        vRotateDir = float4(RotateByAxis(vTargetDir.xyz, vAxis, radians(fOrbitAngle) * fTimeDelta), 0.f);
        vRotateDir = vTargetDir - vRotateDir;
    }
    
    float fAddSpeed = 1.f;
    if (iState & STATE_ACCEL)
    {
        fAddSpeed *= particle.vLifeTime.y / particle.vLifeTime.x * fAccelSpeed;
        if (fAddSpeed < fAccelLimit)
            fAddSpeed = fAccelLimit;
    }
    else if (iState & STATE_DECEL)
    {
        fAddSpeed *= 1.f - (particle.vLifeTime.y / particle.vLifeTime.x) * fAccelSpeed;
        if (fAddSpeed < 0.f)
            fAddSpeed = 0.f;
        if (fAddSpeed < fAccelLimit)
            fAddSpeed = fAccelLimit;
    }
    
    float4 vMoveDir = vDir * particle.fSpeed;
    
    particle.vTranslation = particle.vTranslation + (vMoveDir * fTimeDelta + vRotateDir) * fAddSpeed;
    
    if ((iState & STATE_LOOP) && (particle.vLifeTime.y >= particle.vLifeTime.x))
    {
        particle = InitParticles[iIndex];
        particle.vLifeTime.y = 0.f;
        particle.vTranslation = mul(particle.vTranslation, WorldMatrix);
        particle.vRight = vWorldPivot;
        particle.vUp = float4(vOrbitAxis.x, vOrbitAxis.y, vOrbitAxis.z, 0.f);
    }
    
    particle.vLook = normalize(vMoveDir * fTimeDelta + vRotateDir);
    
    particles[iIndex] = particle;
}

[numthreads(256, 1, 1)]
void CS_RESET_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= iNumInstance)
        return;
    
    particles[iIndex] = InitParticles[iIndex];
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
