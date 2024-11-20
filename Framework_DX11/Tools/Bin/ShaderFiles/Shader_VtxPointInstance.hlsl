#include "Shader_Engine_Defines.hlsli"

// 고준호
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

#define STATE_GROW          0x0001
#define STATE_SHRINK        0x0002
#define STATE_ROTATION      0x0004


StructuredBuffer<Particle> Particle_SRV : register(t0);

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;
texture2D       g_NormalTexture;
texture2D       g_MaskTexture_1;
texture2D       g_MaskTexture_2;
vector			g_vCamPosition;

float2          g_vTexDivide;
float2          g_vScaling;
int             g_iState = 0;
float           g_fStartRotation = 0.f;
float           g_fAngle = 0.f;
float           g_fSpriteSpeed = 0.f;


struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
    float3 vLook : TEXCOORD0;
};


VS_OUT VS_MAIN(uint instanceID : SV_InstanceID)
{
    VS_OUT Out = (VS_OUT) 0;
	
    row_major float4x4 TransformMatrix = float4x4(
    float4(Particle_SRV[instanceID].vRight), 
    float4(Particle_SRV[instanceID].vUp), 
    float4(Particle_SRV[instanceID].vLook), 
    float4(Particle_SRV[instanceID].vTranslation)
    );
    
    vector vPosition = mul(float4(Particle_SRV[instanceID].vPosition, 1.f), TransformMatrix);

    vPosition = mul(vPosition, g_WorldMatrix);
	
    Out.vPosition = vPosition;
    Out.vPSize = Particle_SRV[instanceID].vPSize;
    Out.vLifeTime = Particle_SRV[instanceID].vLifeTime;
    Out.vColor = Particle_SRV[instanceID].vColor;
    Out.vLook = mul(Particle_SRV[instanceID].vLook, g_WorldMatrix);
    
    return Out;
}

float3 RotateByAxis(float3 vVector, float3 vAxis, float fAngle)
{
    float fCosAngle = cos(fAngle);
    float fSinAngle = sin(fAngle);
    
    vAxis = normalize(vAxis);
    
    float3 vRotatedVec = vVector * fCosAngle + cross(vAxis, vVector) * fSinAngle + vAxis * dot(vAxis, vVector) * (1.f - fCosAngle);
    
    return vRotatedVec;
}


struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
    float3 vLook : TEXCOORD0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
	GS_OUT			Out[4];

	float3		vLook = (g_vCamPosition - In[0].vPosition).xyz;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
    float3      vUp = normalize(cross(vLook, vRight));
    
    vRight *= In[0].vPSize.x * 0.5f * g_vScaling.x;
    vUp *= In[0].vPSize.y * 0.5f * g_vScaling.y;

    vRight = RotateByAxis(vRight, vLook, radians(g_fStartRotation));
    vUp = RotateByAxis(vUp, vLook, radians(g_fStartRotation));
    
    if(g_iState & STATE_GROW)
    {
        vRight *= In[0].vLifeTime.y / In[0].vLifeTime.x;
        vUp *= In[0].vLifeTime.y / In[0].vLifeTime.x;

    }
    else if(g_iState & STATE_SHRINK)
    {
        vRight *= (1.f - In[0].vLifeTime.y / In[0].vLifeTime.x);
        vUp *= (1.f - In[0].vLifeTime.y / In[0].vLifeTime.x);
    }
    
    if(g_iState & STATE_ROTATION)
    {
        float fAngle = g_fAngle;
        fAngle *= In[0].vColor.a;
        if (In[0].vColor.a < 0.5f)
        {
            fAngle *= -1.f;
        }
        vRight = RotateByAxis(vRight, vLook, radians(fAngle) * (In[0].vLifeTime.y / In[0].vLifeTime.x));
        vUp = RotateByAxis(vUp, vLook, radians(fAngle) * (In[0].vLifeTime.y / In[0].vLifeTime.x));
    }
    
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vColor = In[0].vColor;

    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vColor = In[0].vColor;


	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);

	Container.Append(Out[0]);
	Container.Append(Out[1]);
	Container.Append(Out[2]);
	Container.RestartStrip();

	Container.Append(Out[0]);
	Container.Append(Out[2]);
	Container.Append(Out[3]);
	Container.RestartStrip();
}

[maxvertexcount(6)]
void GS_DIR_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];

    float3 vCamDir = normalize(g_vCamPosition - In[0].vPosition).xyz;
	// 빌보드용. : 룩 방향으로 점을 찍음.
    float3 vLook = normalize(In[0].vLook);
    float3 vUp = normalize(cross(vLook, vCamDir));
    float3 vRight = normalize(cross(vUp, vLook));
	
    vLook *= In[0].vPSize.x * 0.5f * g_vScaling.x;
    vUp *= In[0].vPSize.y * 0.5f * g_vScaling.y;
	
    vLook = RotateByAxis(vLook, vRight, radians(g_fStartRotation));
    vUp = RotateByAxis(vUp, vRight, radians(g_fStartRotation));

    if (g_iState & STATE_GROW)
    {
        vLook *= In[0].vLifeTime.y / In[0].vLifeTime.x;
        vUp *= In[0].vLifeTime.y / In[0].vLifeTime.x;

    }
    else if (g_iState & STATE_SHRINK)
    {
        vLook *= (1.f - In[0].vLifeTime.y / In[0].vLifeTime.x);
        vUp *= (1.f - In[0].vLifeTime.y / In[0].vLifeTime.x);
    }
    
    Out[0].vPosition = float4(In[0].vPosition.xyz - vLook + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.0f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vColor = In[0].vColor;

    Out[1].vPosition = float4(In[0].vPosition.xyz + vLook + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.0f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vColor = In[0].vColor;

    Out[2].vPosition = float4(In[0].vPosition.xyz + vLook - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vColor = In[0].vColor;

    Out[3].vPosition = float4(In[0].vPosition.xyz - vLook - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vColor = In[0].vColor;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);

    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip(); // 여기서부터 다시 찍겠다. 이거 안해주면 123으로 찍어버림.

    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}



struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
};

struct PS_EFFECT_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vBlur : SV_TARGET1;
};

struct PS_NORMAL_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vARM : SV_TARGET3;
    vector vPickDepth : SV_TARGET4;
};

PS_EFFECT_OUT PS_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
	
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vColor.a <= 0.3f)
        discard;

    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;

    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}

PS_EFFECT_OUT PS_GLOW_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
	
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vColor.a <= 0.1f)
        discard;

    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;

    vColor.r = 1.f - (1 - In.vColor.r) * (1 - vColor.a);
    vColor.g = 1.f - (1 - In.vColor.g) * (1 - vColor.a);
    vColor.b = 1.f - (1 - In.vColor.b) * (1 - vColor.a);

    Out.vDiffuse = vColor;
    Out.vBlur = vColor;

    return Out;
}

PS_NORMAL_OUT PS_SPRITE_NORMAL_MAIN(PS_IN In)
{
    PS_NORMAL_OUT Out = (PS_NORMAL_OUT) 0;
	
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    int iTexIndex = (int) ((In.vLifeTime.y / In.vLifeTime.x) * (g_vTexDivide.x * g_vTexDivide.y - 1.f) * g_fSpriteSpeed);
    
    start.x = (1 / g_vTexDivide.x) * iTexIndex;
    start.y = (1 / g_vTexDivide.y) * (int) (iTexIndex / g_vTexDivide.x);
	
    over.x = start.x + (1 / g_vTexDivide.x);
    over.y = start.y + (1 / g_vTexDivide.y);
	
    float2 vTexcoord = start + (over - start) * In.vTexcoord;

    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);

    if (Out.vDiffuse.a <= 0.3f)
        discard;

    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;
    
    vector vNomral = g_NormalTexture.Sample(LinearSampler, vTexcoord);
    Out.vNormal = vNomral * 2.f - 1.f;
    
    Out.vDepth = float4(0.f, 0.f, 0.f, 0.f);
    Out.vARM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vPickDepth = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_EFFECT_OUT PS_GLOW_RGBTOA_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
	
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    vColor.a = max(vColor.r, max(vColor.g, vColor.b));
    
    if (vColor.a <= 0.1f)
        discard;

    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;

    vColor.r = 1.f - (1 - In.vColor.r) * (1 - vColor.a);
    vColor.g = 1.f - (1 - In.vColor.g) * (1 - vColor.a);
    vColor.b = 1.f - (1 - In.vColor.b) * (1 - vColor.a);

    Out.vDiffuse = vColor;
    Out.vBlur = vColor;

    return Out;
}

technique11	DefaultTechnique
{
	pass DEFAULT // 0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass PARTICLE_GLOW // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_GLOW_MAIN();
    }

    pass PARTICLE_SPARK // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_DIR_MAIN();
        PixelShader = compile ps_5_0 PS_GLOW_RGBTOA_MAIN();
    }

    pass PARTICLE_SPRITE_STONE // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_SPRITE_NORMAL_MAIN();
    }
}
