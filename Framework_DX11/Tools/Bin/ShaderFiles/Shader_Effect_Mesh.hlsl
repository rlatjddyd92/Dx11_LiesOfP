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

float g_fAccumulateTime;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

float rand(float seed);

VS_OUT VS_MAIN(/*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

VS_OUT VS_ELECTRIC_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    
    vector vPosition = vector(In.vPosition, 1.f);
    float fOffset = rand(vPosition.x * vPosition.y * vPosition.z * g_fAccumulateTime);
    
    vPosition += Out.vNormal * fOffset * 0.025f;
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
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
	
    vColor.rgb *= g_vColor;
    vColor.rgb *= g_fAlpha;
    
    float fMax = max(vColor.r, max(vColor.g, vColor.b));
    
    if (fMax < 0.3f)
        discard;
    
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
	
    if(vColor.a < 0.3f)
        discard;
	
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

PS_OUT PS_BLEND_RGBTOA_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
	
    vColor.a = max(vColor.r, max(vColor.g, vColor.b));
    
    vColor.rgb *= g_vColor.rgb;
    vColor.a *= g_fAlpha;
	
    if (vColor.a < 0.1f)
        discard;
    
    Out.vColor = vColor;
	
    return Out;
}

PS_OUT PS_BLEND_DA_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
	
    vColor.rgb *= g_vColor.rgb * g_fAlpha;
	
    Out.vColor = vColor;
	
    return Out;
}

PS_OUT PS_DISTORTION_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    vector vMask = g_MaskTexture_1.Sample(LinearSampler, vTexcoord);
	
    vColor.rgb *= g_vColor.rgb;
    vColor.a *= g_fAlpha;
    
    vColor *= vMask;
    
    if(vColor.a < 0.1f)
        discard;
	
    Out.vColor = vColor;
	
    return Out;
}

float AdaptiveValue(float fValue)
{
    if(fValue <= 0.5f)
        return pow(fValue, 2.f);
    else
        return 1.f - pow(1.f - fValue, 2.f);
}

PS_EFFECT_OUT PS_POW_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
    
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);

    vColor.rgb *= g_fAlpha;
    vColor *= g_vColor;
    
    vColor.r = AdaptiveValue(saturate(vColor.r));
    vColor.g = AdaptiveValue(saturate(vColor.g));
    vColor.b = AdaptiveValue(saturate(vColor.b));
    
    float fMax = max(vColor.r, max(vColor.g, vColor.b));

    if (fMax < 0.1f)
        discard;
    
    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}

PS_EFFECT_OUT PS_POW_MASK_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
    
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    float2 vTexcoord = In.vTexcoord * g_vTileRepeat + g_vTileMove;
    vector vMask = g_MaskTexture_1.Sample(LinearSampler, vTexcoord);
    
    vColor.rgb *= g_fAlpha;
    vColor *= g_vColor;
    vColor *= vMask;
    
    vColor.r = AdaptiveValue(saturate(vColor.r));
    vColor.g = AdaptiveValue(saturate(vColor.g));
    vColor.b = AdaptiveValue(saturate(vColor.b));
    
    float fMax = max(vColor.r, max(vColor.g, vColor.b));

    if (fMax < 0.1f)
        discard;
    
    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}

PS_OUT PS_2P_HALFSPHERE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 반구 전체적인 색을 정할 텍스처 : Diffuse
    // 아우라 텍스처 : Mask1
    // 그 선 좌자작 그어진 텍스처 : Mask2
    // 반구 자르기 용 텍스처 : Normal
    
    float2 vCutCoord = In.vTexcoord;
    vCutCoord.y += g_fAlpha;
    vCutCoord.x *= g_vColor.a;
    vector vCut = g_NormalTexture.Sample(LinearYClampSampler, vCutCoord);
    vCut.r *= 2.f;
    if (vCut.r < 0.1f)
        discard;
    
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat;
    
    float2 vMask1TexMove = float2(g_vTileMove.x, 0.f);
    float2 vMask2TexMove = float2(0.f, g_vTileMove.y);
    vector vMask_1 = g_MaskTexture_1.Sample(LinearSampler, vTexcoord + vMask1TexMove);
    vector vMask_2 = g_MaskTexture_2.Sample(LinearSampler, vTexcoord + vMask2TexMove);
    
    vColor.rgb *= g_vColor.rgb;
    
    vMask_1 *= 0.1f;
    vMask_2 *= 0.1f;
    
    vColor += vMask_1 + vMask_2;
    
    Out.vColor = vColor;
	
    return Out;
}


technique11	DefaultTechnique
{
	pass Default //0
	{
        SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass SelfDistortion //1
    {
        SetRasterizerState(RS_Cull_None);
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
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_DA_MAIN();
    }

    pass Distortion //4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION_MAIN();
    }

    pass BLEND_RGBTOA   //5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLEND_RGBTOA_MAIN();
    }

    pass POW_EFFECT // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_POW_MAIN();
    }

    pass THUNDER_EFFECT // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ELECTRIC_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_POW_MAIN();
    }

    pass POW_MASK_EFFECT // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_POW_MASK_MAIN();
    }

    pass HALFSPHERE_MAP // 9
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_2P_HALFSPHERE_MAIN();
    }
}

float rand(float seed)
{
    float fRandom = frac(sin(seed) * 43758.5453123);
    return fRandom * 2.f - 1.f;
}
