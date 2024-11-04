#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vColor = vector(0.f, 0.f, 0.f, 0.f);
vector g_vMaskColor = vector(0.f, 0.f, 0.f, 0.f);

float2 g_vTexcoord = float2(0.5f, 0.5f);
float g_fRadian = 0.f;
float g_fTime = 0.f;

int g_iNumCol;
int g_iNumRow;
int g_iCol;
int g_iRow;

bool g_isChange = false;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_MaskTexture;
texture2D g_MaskTexture2;
texture2D g_OpacityTexture;

cbuffer Effect_Desc
{
    float4 vColor_Add = float4(0.f, 0.f, 0.f, 0.f);         // 더할 색상
    float4 vColor_Discard = float4(0.f, 0.f, 0.f, 0.f);     // 자를 색상
    float4 vColor_Mul = float4(1.f, 1.f, 1.f, 1.f);         // 곱할 색상
    
    float2 vNumSprite = float2(1.f, 1.f);       // 스프라이트 전체 개수
    float2 vSpriteIndex = float2(0.f, 0.f);     // 스프라이트 현재 인덱스
    
    bool isMoveDistortion = false;              // 디스토션 움직일거임?            
    float fDistortionSpeed = 1.f;               // 속도 얼마나 움직일거임?
};

cbuffer Effect_Strength
{
    float fBloomIntensity;      // Bloom 강도
    float fDistortionStrength;  // 왜곡 강도
    float fDissolveAmount;      // Disslove 진행 양
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

struct PS_OUT_EFFECT
{
    vector vColor : SV_TARGET0;
    vector vEmissive : SV_TARGET1;
    vector vDistortion : SV_TARGET2;
};

PS_OUT_EFFECT PS_MAIN_EFFECT(PS_IN In, bool isAlphaBlend)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
    
}

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse;
    Out.vColor.b = g_fTime;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Effect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT(false);
    }

    pass AlphaBlendEffect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT(true);
    }

    pass Distortion
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
    }
}
