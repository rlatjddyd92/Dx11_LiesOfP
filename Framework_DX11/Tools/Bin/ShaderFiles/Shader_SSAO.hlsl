#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
texture2D g_Texture;

texture2D g_DepthTexture;
texture2D g_NormalTexture;
texture2D g_NoiseTexture;

float g_fRadius = 0.01f;

// -1에서 1 사이의 값을 가지며, 화면에서 반구의 형태로 고르게 퍼져있는 벡터
float3 vSampleKernels[16] =
{
    float3(0.5381, 0.1856, -0.4319),
    float3(0.1379, 0.2486, 0.4430),
    float3(0.3374, 0.5679, -0.0057),
    float3(-0.6999, -0.0451, -0.0019),
    float3(0.0689, -0.1598, -0.8547),
    float3(0.0560, 0.0069, -0.1843),
    float3(-0.0146, 0.1402, 0.0762),
    float3(0.0100, -0.1924, -0.0344),
    float3(-0.3577, -0.5301, -0.4358),
    float3(-0.3169, 0.1063, 0.0158),
    float3(0.0103, -0.5869, 0.0046),
    float3(-0.0897, -0.4940, 0.3287),
    float3(0.7119, -0.0154, -0.0918),
    float3(-0.0533, 0.0596, -0.5411),
    float3(0.0352, -0.0631, 0.5460),
    float3(-0.4777, 0.2847, -0.0271)
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
    float3 vNormal = float3(vNormalDesc.xyz * 2.f - 1.f); // 뷰 스페이스상의 노말이어야하는데?
    
    // 뷰 스페이스 상의 포지션
    float3 vViewPos = (vector) 0;
    
    /* 투영공간상의 화면에 그려지는 픽셀의 위치를 구한다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / w */
    vViewPos.x = In.vTexcoord.x * 2.f - 1.f;
    vViewPos.y = In.vTexcoord.y * -2.f + 1.f;
    vViewPos.z = vDepthDesc.x;  // 투영 공간상의 정규화된 Z

	/* 뷰스페이스 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
	/* 로컬위치 * 월드행렬 * 뷰행렬  */
    vViewPos = vViewPos * fViewZ;
    vViewPos = mul(vViewPos, g_ProjMatrixInv);
    
    int  iOcc = 0;
    
    //주변 깊이를 체크
    for (int i = 0; i < 16; i++)
    {
        // 노이즈 텍스처를 사용해 샘플 방향에 약간의 무작위성을 추가
        float3 vRay = normalize(vSampleKernels[i]); // 랜덤 노말값  //+ g_NoiseTexture.Sample(PointSampler, In.vTexcoord).xyz
        float3 vReflect = normalize(reflect(vNormal, vRay)) * g_fRadius;
        
        // 투영 공간으로 변환 후 정규화
        float2 vNewTexCoord = In.vTexcoord * vReflect.xy;
        
        // 이동한 좌표의 View 스페이스상의 Z값
        float fSampleViewZ = g_DepthTexture.Sample(PointSampler, vNewTexCoord).x;
        
        // 깊이 차이를 비교
        if (fSampleViewZ < vDepthDesc.x)
        {
            iOcc += 1; // occlusion이 발생했음을 나타냄
        }
    }
    
    Out.vColor = (float) iOcc / 16.f;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass SSAO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSAO();
    }

}