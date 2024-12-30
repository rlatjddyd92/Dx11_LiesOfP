
#define STATE_ORBIT 0x0001
#define STATE_RANDOM 0x0002
#define STATE_LOOP 0x0004
#define STATE_ACCEL 0x0008
#define STATE_DECEL 0x0010

#define TYPE_NONANIM    0
#define TYPE_ANIM       1

#define MAX_BONES 660

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

struct Dissolve_Particle
{
    Particle Particle;
    float2 vTexcoord;
    uint4 vBlendIndices;
    float4 vBlendWeights;
    bool isActive;
};

// UAV로 접근할 수 있는 Structured Buffer 선언
RWStructuredBuffer<Dissolve_Particle> particles : register(u0);
StructuredBuffer<Dissolve_Particle> InitParticles : register(t0);

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

cbuffer InitialBuffer : register(b1)
{
    row_major matrix BoneMatrices[MAX_BONES];
    
    float   fThreshold;
    uint    iModelType;
    float2  fPadding;
}

//Texture2D g_NoiseTexture : register(t1); // 텍스처 레지스터에 연결
//SamplerState Sampler_Default : register(s0); // 샘플러 레지스터에 연결

float rand(float seed);
float3 RotateByAxis(float3 vVector, float3 vAxis, float fAngle);

[numthreads(256, 1, 1)]
void CS_MOVE_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint iIndex = DTid.x;
    
    if (iIndex >= iNumInstance)
        return;
    
    Dissolve_Particle DissolveParticle = particles[iIndex];
    
    float4 vWorldPivot = mul(vPivot, WorldMatrix);
    
    //if (0.f == pad_1.x)
    //{
    //    if (TYPE_ANIM == iModelType)
    //    {
    //        float fWeightW = 1.f - (DissolveParticle.vBlendWeights.x + DissolveParticle.vBlendWeights.y + DissolveParticle.vBlendWeights.z);

    //        matrix BoneMatrix = BoneMatrices[DissolveParticle.vBlendIndices.x] * DissolveParticle.vBlendWeights.x +
		  //  BoneMatrices[DissolveParticle.vBlendIndices.y] * DissolveParticle.vBlendWeights.y +
		  //  BoneMatrices[DissolveParticle.vBlendIndices.z] * DissolveParticle.vBlendWeights.z +
		  //  BoneMatrices[DissolveParticle.vBlendIndices.w] * fWeightW;
		
    //        DissolveParticle.Particle.vTranslation = mul(DissolveParticle.Particle.vTranslation, BoneMatrix);
    //    }
        
    //    DissolveParticle.Particle.vTranslation = mul(DissolveParticle.Particle.vTranslation, WorldMatrix);
    //    DissolveParticle.Particle.vRight = vWorldPivot;
    //    DissolveParticle.Particle.vUp = float4(vOrbitAxis.x, vOrbitAxis.y, vOrbitAxis.z, 0.f);
    //    DissolveParticle.Particle.vUp = mul(DissolveParticle.Particle.vUp, WorldMatrix);
    //}
    
    float4 vDir = vMoveDir;
    vDir = normalize(vDir);
    
    DissolveParticle.Particle.vLifeTime.y += fTimeDelta;
    
    if (iState & STATE_RANDOM)
    {
        float fTime = fmod(DissolveParticle.Particle.vLifeTime.y, fTimeInterval);
        if (fTime < fTimeDelta)
        {
            DissolveParticle.Particle.vCurrenrRandomDir = DissolveParticle.Particle.vNextRandomDir;
            
            DissolveParticle.Particle.vNextRandomDir.x = rand(DissolveParticle.Particle.vTranslation.x);
            DissolveParticle.Particle.vNextRandomDir.y = rand(DissolveParticle.Particle.vTranslation.y);
            DissolveParticle.Particle.vNextRandomDir.z = rand(DissolveParticle.Particle.vTranslation.z);
            
            DissolveParticle.Particle.vNextRandomDir = normalize(DissolveParticle.Particle.vNextRandomDir);
        }
        vDir += float4(lerp(DissolveParticle.Particle.vCurrenrRandomDir.xyz, DissolveParticle.Particle.vNextRandomDir.xyz, fTime / fTimeInterval) * fRandomRatio, 0.f);
        vDir = normalize(vDir);
    }
    
    float4 vRotateDir = (float4) 0;
    if (iState & STATE_ORBIT)
    {
        float4 vTargetDir = DissolveParticle.Particle.vRight - DissolveParticle.Particle.vTranslation;
        float3 vAxis = float3(DissolveParticle.Particle.vUp.x, DissolveParticle.Particle.vUp.y, DissolveParticle.Particle.vUp.z);
        vRotateDir = float4(RotateByAxis(vTargetDir.xyz, vAxis, radians(fOrbitAngle) * fTimeDelta), 0.f);
        vRotateDir = vTargetDir - vRotateDir;
    }
    
    float fAddSpeed = 1.f;
    if (iState & STATE_ACCEL)
    {
        fAddSpeed *= DissolveParticle.Particle.vLifeTime.y / DissolveParticle.Particle.vLifeTime.x * fAccelSpeed;
        if (fAddSpeed < fAccelLimit)
            fAddSpeed = fAccelLimit;
    }
    else if (iState & STATE_DECEL)
    {
        fAddSpeed *= 1.f - (DissolveParticle.Particle.vLifeTime.y / DissolveParticle.Particle.vLifeTime.x) * fAccelSpeed;
        if (fAddSpeed < 0.f)
            fAddSpeed = 0.f;
        if (fAddSpeed < fAccelLimit)
            fAddSpeed = fAccelLimit;
    }
    
    float4 vMoveDir = vDir * DissolveParticle.Particle.fSpeed;
    vMoveDir.y -= fGravity * DissolveParticle.Particle.vLifeTime.y;
    
    DissolveParticle.Particle.vTranslation = DissolveParticle.Particle.vTranslation + (vMoveDir * fTimeDelta + vRotateDir) * fAddSpeed;
    
    DissolveParticle.Particle.vLook = normalize(vMoveDir * fTimeDelta + vRotateDir);

    if ((iState & STATE_LOOP) && (DissolveParticle.Particle.vLifeTime.y >= DissolveParticle.Particle.vLifeTime.x))
    {
        DissolveParticle.Particle = InitParticles[iIndex].Particle;
        
        if (TYPE_ANIM == iModelType)
        {
            float fWeightW = saturate(1.f - (DissolveParticle.vBlendWeights.x + DissolveParticle.vBlendWeights.y + DissolveParticle.vBlendWeights.z));

            matrix BoneMatrix = BoneMatrices[DissolveParticle.vBlendIndices.x] * DissolveParticle.vBlendWeights.x +
		            BoneMatrices[DissolveParticle.vBlendIndices.y] * DissolveParticle.vBlendWeights.y +
		            BoneMatrices[DissolveParticle.vBlendIndices.z] * DissolveParticle.vBlendWeights.z +
		            BoneMatrices[DissolveParticle.vBlendIndices.w] * fWeightW;
        
            DissolveParticle.Particle.vTranslation = mul(DissolveParticle.Particle.vTranslation, BoneMatrix);
        }

        DissolveParticle.Particle.vTranslation = mul(DissolveParticle.Particle.vTranslation, WorldMatrix);
        DissolveParticle.Particle.vLook = normalize(vDir * DissolveParticle.Particle.fSpeed * fTimeDelta + vRotateDir);
        DissolveParticle.Particle.vLifeTime.y = 0.f;
        DissolveParticle.Particle.vRight = vWorldPivot;
        DissolveParticle.Particle.vUp = float4(vOrbitAxis.x, vOrbitAxis.y, vOrbitAxis.z, 0.f);
        DissolveParticle.Particle.vUp = mul(DissolveParticle.Particle.vUp, WorldMatrix);
        
        DissolveParticle.Particle.vLook = vMoveDir;
    }

    particles[iIndex] = DissolveParticle;
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
