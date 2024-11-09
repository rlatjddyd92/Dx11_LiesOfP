#include "Shader_Engine_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ViewMatrixInv, g_ProjMatrixInv;
texture2D		g_Texture;

texture2D       g_DownSampleTexture;
texture2D		g_BlurTexture;

float2          g_vScreenSize;
bool            g_isSelectBright = false;

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

//float g_fWeights[13] =
//{
//    0.0561f, 0.1353f, 0.278f, 0.4868f, 0.7261f, 0.9231f, 1.f, 0.9231f, 0.7261f, 0.4868f, 0.278f, 0.1353f, 0.0561f
//};
//float g_fWeightTotal = 6.21f;

// 다른거
float g_fWeights[9] =
{
    0.1f, 0.18f, 0.55f, 0.9f, 1.f, 0.9f, 0.55f, 0.18f, 0.1f
};
float g_fWeightTotal = 4.46f;

VS_OUT VS_MAIN(/*정점*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	

	vector		vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

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

float Get_BloomCurve(float x)
{
    float result = x;
    result = max(0, x - 1.2f);
    result *= result;

    return result * 0.7f;
}

/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}


PS_OUT PS_MAIN_DOWNSAMPLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
    
    for (int y = -2; y <= 2; ++y)
    {
        for (int x = -2; x <= 2; ++x)
        {
            vector vSampleColor = g_DownSampleTexture.Sample(LinearClampSampler, In.vTexcoord, int2(x, y));
    
            
            vColor += vSampleColor;
        }
    }
    
    vColor = vector(vColor.rgb * 0.04f, 1.f);

    if (g_isSelectBright)
    {
        float fIntensity = max(dot(vColor.rgb, float3(5.f, 5.f, 5.f)) * vColor.a, 0.1f);
    
        float fBloomIntensity = min(Get_BloomCurve(fIntensity), 10.f);
        vColor = vColor * fBloomIntensity / fIntensity;
    }
    
    Out.vColor = vector(vColor.rgb, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_UPSAMPLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float3 vColor = float3(0.0f, 0.0f, 0.0f);
    
    float3 vBlur = g_BlurTexture.Sample(LinearClampSampler, In.vTexcoord).rgb;
    float3 vDown = g_DownSampleTexture.Sample(LinearClampSampler, In.vTexcoord).rgb;
    
    vColor = pow(pow(abs(vBlur), 2.2f) + pow(abs(vDown), 2.2f), 1.f / 2.2f);    // 감마보정
    Out.vColor = vector(vColor, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vBlurTexCoord = (float2) 0.f;

    for (int i = -4; i < 5; i++)
    {
        vBlurTexCoord = In.vTexcoord + float2(1.f / g_vScreenSize.x * i, 0.f);
        if (vBlurTexCoord.x > 1.f || vBlurTexCoord.y > 1.f || vBlurTexCoord.x < 0.f || vBlurTexCoord.y < 0.f)   // 화면 밖으로 나갈 경우 예외 처리
            continue;
        
        vector vDownSample = g_DownSampleTexture.Sample(LinearBorderSampler, vBlurTexCoord);
        
        // 알파값으로 판별해서 일정 이상이어야 블러를 먹일거임
       if (vDownSample.a > 0.03f)
            Out.vColor += g_fWeights[i + 4] * vDownSample;
		
    }

    Out.vColor /= g_fWeightTotal;
    if (Out.vColor.a - 0.05f < 0.f)
        discard;

    return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vBlurTexCoord = (float2) 0.f;

    for (int i = -4; i < 5; i++)
    {
        vBlurTexCoord = In.vTexcoord + float2(0.f, 1.f / g_vScreenSize.y * i);
        if (vBlurTexCoord.x > 1.f || vBlurTexCoord.y > 1.f || vBlurTexCoord.x < 0.f || vBlurTexCoord.y < 0.f)   // 화면 밖으로 나갈 경우 예외 처리
            continue;
        
        vector vDownSample = g_DownSampleTexture.Sample(LinearBorderSampler, vBlurTexCoord);
        
        // 알파값으로 판별해서 일정 이상이어야 블러를 먹일거임
        if (vDownSample.a > 0.03f)
            Out.vColor += g_fWeights[i + 4] * vDownSample;
		
    }

    Out.vColor /= g_fWeightTotal;
    if (Out.vColor.a - 0.05f < 0.f)
        discard;
    
    return Out;
}


technique11	DefaultTechnique
{	
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

    // 1
    pass DownSample
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DOWNSAMPLE();
    }

    // 2
    pass BlurX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    // 3
    pass BlurY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    // 4
    pass UpSample
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UPSAMPLE();
    }
}