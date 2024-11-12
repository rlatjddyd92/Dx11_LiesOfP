#include "Shader_Engine_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;
texture2D		g_MaskTexture_1;
texture2D		g_MaskTexture_2;

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

PS_OUT PS_TEST_MAIN_1(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
    float2 vTexcoord = (float2) 0;
    vTexcoord.x = In.vTexcoord.x * g_vTileRepeat.x + g_vTileMove.x;
    vTexcoord.y = In.vTexcoord.y * g_vTileRepeat.y + g_vTileMove.y;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    //if (Out.vColor.b < 0.1f)
    //    discard;
    Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;
	
    
	
    Out.vColor.rgb = g_vColor.rgb;
    Out.vColor.a *= g_fAlpha;
	
	return Out;
}

PS_OUT PS_TEST_MAIN_2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = (float2) 0;
    vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
	
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    Out.vColor.rgb += g_DiffuseTexture.Sample(LinearSampler, vTexcoord).rgb;
    
    //if (Out.vColor.b < 0.1f)
    //    discard;
	
    //Out.vColor.a = Out.vColor.b;
	
    //Out.vColor.rgb = g_vColor.rgb;
    //Out.vColor.a *= g_fAlpha;
	
    return Out;
}

technique11	DefaultTechnique
{
	pass Test_1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEST_MAIN_1();
    }

    pass Test_2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEST_MAIN_2();
    }
}