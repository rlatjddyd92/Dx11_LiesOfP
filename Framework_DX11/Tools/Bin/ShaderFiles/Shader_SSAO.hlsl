#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_CameraViewMatrix;
texture2D g_Texture;

texture2D g_DepthTexture;
texture2D g_NormalTexture;
texture2D g_NoiseTexture;
texture2D g_SSAOTexture;

float g_fRadius = 0.02f;

// -1에서 1 사이의 값을 가지며, 화면에서 반구의 형태로 고르게 퍼져있는 벡터
float3 vSampleKernels[16] =
{
    float3(0.2024537f, 0.841204f, -0.9060141f),
    float3(-0.2200423f, 0.6282339f, -0.8275437f),
    float3(0.3677659f, 0.1086345f, -0.4466777f),
    float3(0.8775856f, 0.4617546f, -0.6427765f),
    float3(0.7867433f, -0.141479f, -0.1567597f),
    float3(0.4839356f, -0.8253108f, -0.1563844f),
    float3(0.4401554f, -0.4228428f, -0.3300118f),
    float3(0.0019193f, -0.8048455f, 0.0726584f),
    float3(-0.7578573f, -0.5583301f, 0.2347527f),
    float3(-0.4540417f, -0.252365f, 0.0694318f),
    float3(-0.0483353f, -0.2527294f, 0.5924745f),
    float3(-0.4192392f, 0.2084218f, -0.3672943f),
    float3(-0.8433938f, 0.1451271f, 0.2202872f),
    float3(-0.4037157f, -0.8263387f, 0.4698132f),
    float3(-0.6657394f, 0.6298575f, 0.6342437f),
    float3(-0.0001783f, 0.2834622f, 0.8343929f),
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

PS_OUT PS_MAIN_SSAO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f; // 뷰 스페이스상의 Z

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float3 vNormal = float3(vNormalDesc.xyz * 2.f - 1.f);
    vNormal = normalize(mul(vNormal, g_CameraViewMatrix).xyz);
    
    int  iOcc = 0;
    
    //주변 깊이를 체크
    for (int i = 0; i < 16; i++)
    {
        // 노이즈 텍스처를 사용해 샘플 방향에 약간의 무작위성을 추가
        float3 vRay = normalize(vSampleKernels[i] ); // 랜덤 Ray값
        float3 vReflect = normalize(reflect(vRay, vNormal.xyz)) * g_fRadius;
        vReflect.x *= -1.f;
        
        // 투영 공간으로 변환 후 정규화
        float2 vNewTexCoord;
        vNewTexCoord.x = In.vTexcoord.x + vReflect.x;
        vNewTexCoord.y = In.vTexcoord.y + vReflect.y;
        
        float fSampleViewZ = g_DepthTexture.Sample(PointSampler, vNewTexCoord).y * 1000.f;
       
        // 깊이 차이를 비교
        if (fSampleViewZ <= fViewZ + 0.002f)
        {
            iOcc += 1;
        }
    }
    
    Out.vColor = saturate((float) iOcc / 16.f);
    
    return Out;
}

float g_fWeights[9] =
{
    0.1f, 0.18f, 0.55f, 0.9f, 1.f, 0.9f, 0.55f, 0.18f, 0.1f
};
float g_fWeightTotal = 4.46f;

PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vBlurTexCoord = (float2) 0.f;

    for (int i = -4; i < 5; i++)
    {
        vBlurTexCoord = In.vTexcoord + float2(1.f / 1280.f * i, 0.f);
        if (vBlurTexCoord.x > 1.f || vBlurTexCoord.y > 1.f || vBlurTexCoord.x < 0.f || vBlurTexCoord.y < 0.f)   // 화면 밖으로 나갈 경우 예외 처리
            continue;
        
        vector vSSAO = g_SSAOTexture.Sample(LinearBorderSampler, vBlurTexCoord);
        Out.vColor += g_fWeights[i + 4] * vSSAO;
		
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
        vBlurTexCoord = In.vTexcoord + float2(0.f, 1.f / 720.f * i);
        if (vBlurTexCoord.x > 1.f || vBlurTexCoord.y > 1.f || vBlurTexCoord.x < 0.f || vBlurTexCoord.y < 0.f)   // 화면 밖으로 나갈 경우 예외 처리
            continue;
        
        vector vSSAO = g_SSAOTexture.Sample(LinearBorderSampler, vBlurTexCoord);
        Out.vColor += g_fWeights[i + 4] * vSSAO;
    }

    Out.vColor /= g_fWeightTotal;
    if (Out.vColor.a - 0.05f < 0.f)
        discard;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass SSAO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSAO();
    }

    pass SSAO_BlurX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass SSAO_BlurXY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

}