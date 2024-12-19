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
texture2D g_MaskTexture_1;
texture2D g_MaskTexture_2;
texture2D g_OpacityTexture;

float2  g_vTexDivide;
int     g_iTexIndex;
float   g_fRatio;

float2 g_vTileRepeat;
float2 g_vTileMove;

cbuffer Effect_Desc
{
    float4 vColor_Add = float4(0.f, 0.f, 0.f, 0.f);         // 더할 색상
    float4 vColor_Discard = float4(0.f, 0.f, 0.f, 0.f);     // 자를 색상
    float4 vColor_Mul = float4(1.f, 1.f, 1.f, 1.f);         // 곱할 색상
    
    //float2 vNumSprite = float2(1.f, 1.f);       // 스프라이트 전체 개수
    //float2 vSpriteIndex = float2(0.f, 0.f);     // 스프라이트 현재 인덱스
    
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
    vector vColor : SV_Target0;
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

float2 Get_SpriteTexcoord(float2 vTexcoord);

PS_EFFECT_OUT PS_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
    
    if (vColor.a <= 0.3f)
        discard;
    
    vColor.rgb *= g_vColor.rgb;
    vColor.rgb *= g_fRatio;
    
    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}

PS_OUT PS_BLEND_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
    
    Out.vColor.rgb *= g_vColor.rgb;
    Out.vColor.a *= g_fRatio;
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_BLEND_GLOW_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
    
    Out.vColor.a *= g_fRatio;
    
    Out.vColor.r = 1.f - (1 - g_vColor.r) * (1 - Out.vColor.a);
    Out.vColor.g = 1.f - (1 - g_vColor.g) * (1 - Out.vColor.a);
    Out.vColor.b = 1.f - (1 - g_vColor.b) * (1 - Out.vColor.a);
    
    return Out;
}

PS_OUT PS_BLEND_RGBTOA_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
    
    Out.vColor.a = max(Out.vColor.r, max(Out.vColor.g, Out.vColor.b));

    Out.vColor.rgb *= g_vColor.rgb;
    Out.vColor.a *= g_fRatio;
    
    return Out;
}

PS_EFFECT_OUT PS_GLOW_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));

    if (vColor.a <= 0.1f)
        discard;
    
    vColor.r = 1.f - (1 - g_vColor.r) * (1 - vColor.a);
    vColor.g = 1.f - (1 - g_vColor.g) * (1 - vColor.a);
    vColor.b = 1.f - (1 - g_vColor.b) * (1 - vColor.a);

    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}

PS_EFFECT_OUT PS_GLOW_RGBTOA_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
    
    vColor.a = max(vColor.r, max(vColor.g, vColor.b));
    
    if (vColor.a <= 0.1f)
        discard;
    
    vColor.r = 1.f - (1 - g_vColor.r) * (1 - vColor.a);
    vColor.g = 1.f - (1 - g_vColor.g) * (1 - vColor.a);
    vColor.b = 1.f - (1 - g_vColor.b) * (1 - vColor.a);

    vColor.rgb *= g_fRatio;
    
    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}

PS_OUT PS_BLEND_RGBTOA_GLOW_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
    
    Out.vColor.a = max(Out.vColor.r, max(Out.vColor.g, Out.vColor.b));

    Out.vColor.r = 1.f - (1 - g_vColor.r) * (1 - Out.vColor.a);
    Out.vColor.g = 1.f - (1 - g_vColor.g) * (1 - Out.vColor.a);
    Out.vColor.b = 1.f - (1 - g_vColor.b) * (1 - Out.vColor.a);

    Out.vColor.a *= g_fRatio;
    
    if(Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_NORMAL_OUT PS_BLOOD_MAIN(PS_IN In)
{
    PS_NORMAL_OUT Out = (PS_NORMAL_OUT) 0;
    
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
    Out.vNormal = Out.vDiffuse * 2.f - 1.f;
    Out.vDepth = float4(0.f, 0.f, 0.f, 0.f);
    Out.vARM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vPickDepth = float4(0.f, 0.f, 0.f, 0.f);
    
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse *= g_vColor;

    return Out;
}

PS_OUT PS_BLEND_DIRT_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
    vector vMask = g_MaskTexture_1.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
    
    Out.vColor.rgb *= g_vColor.rgb;
    Out.vColor.a *= g_fRatio;
    Out.vColor *= vMask;
    
    return Out;
}

PS_OUT PS_BLEND_MASK_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
        
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    vector vMask = g_MaskTexture_1.Sample(LinearSampler, vTexcoord);
    
    Out.vColor *= vMask;
    Out.vColor.rgb *= g_vColor.rgb;
    
    Out.vColor.a *= g_fRatio;
    
    return Out;
}

PS_OUT PS_BLEND_RGBTOA_MASK_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
        
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    
    vector vMask = g_MaskTexture_1.Sample(LinearSampler, vTexcoord);
    
    Out.vColor.a = max(Out.vColor.r, max(Out.vColor.g, Out.vColor.b));
    
    Out.vColor *= vMask;
    Out.vColor.rgb *= g_vColor.rgb;
    
    Out.vColor.a *= g_fRatio;
    
    return Out;
}

PS_OUT PS_RGBTOA_MASK_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
        
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    
    vector vMask = g_MaskTexture_1.Sample(LinearSampler, vTexcoord);
    
    Out.vColor.a = max(Out.vColor.r, max(Out.vColor.g, Out.vColor.b));
    Out.vColor *= vMask;
    if(Out.vColor.a < 0.1f)
        discard;
    
    Out.vColor.rgb *= g_vColor.rgb;
    Out.vColor.rgb *= g_fRatio;
    
    return Out;
}


PS_OUT PS_POWERGUARD_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
    
    Out.vColor.a = max(Out.vColor.r, max(Out.vColor.g, Out.vColor.b));

    Out.vColor.r = g_vColor.r;
    Out.vColor.g = g_vColor.g * Out.vColor.a;
    Out.vColor.b = g_vColor.b * Out.vColor.a;;

    Out.vColor.a *= g_fRatio;
    
    return Out;
}

PS_EFFECT_OUT PS_RGBTOA_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord));
    
    vColor.a = max(vColor.r, max(vColor.g, vColor.b));
    
    if (vColor.a <= 0.1f)
        discard;
    
    vColor.rgb *= g_vColor.rgb;
    vColor.rgb *= g_fRatio;
    
    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass Default // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Blend // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_MAIN();
    }

    pass Glow // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GLOW_MAIN();
    }

    pass BLEND_RGBTOA // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_RGBTOA_MAIN();
    }

    pass GLOW_RGBTOA // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GLOW_RGBTOA_MAIN();
    }

    pass BLEND_GLOW // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_GLOW_MAIN();
    }

    pass BLEND_RGBTOA_GLOW // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_RGBTOA_GLOW_MAIN();
    }

    pass BLOOD // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOD_MAIN();
    }

    pass DIRT   // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_DIRT_MAIN();
    }

    pass BLEND_MASK   // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_MASK_MAIN();
    }

    pass POWER_GUARD // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_POWERGUARD_MAIN();
    }

    pass BLEND_MASK_RGBTOA  // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_RGBTOA_MASK_MAIN();
    }

    pass MASK_RGBTOA // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RGBTOA_MASK_MAIN();
    }

    pass RGBTOA // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RGBTOA_MAIN();
    }

    pass NonDSS_BLEND_GLOW // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_GLOW_MAIN();
    }
}


float2 Get_SpriteTexcoord(float2 vTexcoord)
{
    float2 start = (float2) 0;
    float2 over = (float2) 0;

    start.x = (1 / g_vTexDivide.x) * g_iTexIndex;
    start.y = (1 / g_vTexDivide.y) * (int) (g_iTexIndex / g_vTexDivide.x);
	
    over.x = start.x + (1 / g_vTexDivide.x);
    over.y = start.y + (1 / g_vTexDivide.y);
	
    return start + (over - start) * vTexcoord;
}
