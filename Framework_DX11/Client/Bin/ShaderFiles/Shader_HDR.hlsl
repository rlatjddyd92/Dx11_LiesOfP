#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_CameraViewMatrix;
texture2D g_Texture;

texture2D g_BackTexture;
texture2D g_DecalTexture;

float g_fRadius = 0.02f;

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

PS_OUT PS_MAIN_HDR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vBack = g_BackTexture.Sample(LinearSampler, In.vTexcoord);

    // LDR에서 HDR로 변환
    // 감마 보정 예시를 사용합니다.
    float exposure = 1.0; // 노출값 조정
    float3 vHdrColor = pow(vBack.rgb, 2.2f); // 감마 보정 적용
    
    vHdrColor *= 2.0f;
    
    
    Out.vColor = float4(vHdrColor, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_TONEMAPPING_ACES(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float3 vLdrColor = g_BackTexture.Sample(LinearSampler, In.vTexcoord).xyz;

    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    
    vLdrColor = saturate((vLdrColor * (a * vLdrColor + b)) / (vLdrColor * (c * vLdrColor + d) + e));
    
    vLdrColor = pow(vLdrColor, 1 / 2.2f);
    Out.vColor = float4(vLdrColor, 1.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass HDR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HDR();
    }

    pass ToneMapping_ACES
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TONEMAPPING_ACES();
    }


    //pass SSAO_BlurX
    //{
    //    SetRasterizerState(RS_Default);
    //    SetDepthStencilState(DSS_None, 0);
    //    SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    GeometryShader = NULL;
    //    PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    //}

    //pass SSAO_BlurXY
    //{
    //    SetRasterizerState(RS_Default);
    //    SetDepthStencilState(DSS_None, 0);
    //    SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    GeometryShader = NULL;
    //    PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    //}

}