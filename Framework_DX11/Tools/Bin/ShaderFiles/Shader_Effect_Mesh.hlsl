#include "Shader_Engine_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_MaskTexture_1;
texture2D g_MaskTexture_2;

float4 g_vColor;
float g_fAlpha;

float2 g_vTileRepeat;
float2 g_vTileMove;

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

VS_OUT VS_MAIN(/*Á¤Á¡*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	
	
	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
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

struct PS_EFFECT_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vBlur : SV_TARGET1;
};

PS_EFFECT_OUT PS_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
	
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
	
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
	
    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
		
    return Out;
}

PS_EFFECT_OUT PS_SELF_DISTORTION_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
	
    float fDistortion = g_MaskTexture_1.Sample(LinearSampler, vTexcoord).x * 0.3f;
	
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord + float2(fDistortion, fDistortion));
	
    vColor.rgb *= g_vColor.rgb;
    vColor.a *= g_fAlpha;
	
	
    Out.vDiffuse = vColor;
    Out.vBlur = vColor;

	
    return Out;
}

PS_OUT PS_BLEND_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
	
    vColor.rgb *= g_vColor.rgb;
    vColor.a *= g_fAlpha;
	
    Out.vColor = vColor;
	
    return Out;
}

PS_OUT PS_BLEND_DA_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
	
    vColor.rgb *= g_vColor.rgb;
    vColor.a *= g_fAlpha * vColor.a;
	
    Out.vColor = vColor;
	
    return Out;
}

PS_OUT PS_DISTORTION_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
	
    vColor.rgb *= g_vColor.rgb;
    vColor.a *= g_fAlpha;
    
    if(vColor.a < 0.1f)
        discard;
	
    Out.vColor = vColor;
	
    return Out;
}

struct PS_NORMAL_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vARM : SV_TARGET3;
    vector vPickDepth : SV_TARGET4;
};


PS_NORMAL_OUT PS_TEST(PS_IN In)
{
    PS_NORMAL_OUT Out = (PS_NORMAL_OUT) 0;
    
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    return Out;
}

technique11	DefaultTechnique
{
	pass Default //0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass SelfDistortion //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SELF_DISTORTION_MAIN();
    }

    pass Blend //2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_MAIN();
    }

    pass Blend_DA //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_DA_MAIN();
    }

    pass Distortion //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION_MAIN();
    }

    pass Test   //5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEST();
    }

}