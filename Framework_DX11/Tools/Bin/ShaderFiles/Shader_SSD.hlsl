#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
//matrix g_CameraViewMatrix;

texture2D g_Texture;

texture2D g_DepthTexture;
texture2D g_NormalTexture;
texture2D g_DiffuseTexture;

float4x4 g_vDecalWorld;
matrix g_vDecalWorldInverse;

float3 vScale = { 10.f, 10.f, 10.f };


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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vProjPos = In.vPosition;
    //vProjPos.x /= vProjPos.w;
    //vProjPos.y /= vProjPos.w;
    
    float2 vNewTexCoord = float2(vProjPos.x * 0.5f + 0.5f, vProjPos.y * -0.5f + 0.5f);
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vNewTexCoord);
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vNewTexCoord);
    vector vDecalDiffuse = g_Texture.Sample(LinearSampler, vNewTexCoord);
    
    float fViewZ = vDepthDesc.y * 1000.f;
    
    vector vPosition = (vector) 0;
    
    /* 투영공간상의 화면에 그려지는 픽셀의 위치를 구한다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 뷰스페이스 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
	/* 로컬위치 * 월드행렬 * 뷰행렬  */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);
    /* 월드 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    //데칼의 로컬로 이동
    vector vPositionDecalLocal = mul(vPosition, g_vDecalWorldInverse);
    
    float2 vAbsDecalLocal = abs(vPositionDecalLocal);
    vAbsDecalLocal.y = 0.f;
    clip(0.5f - vAbsDecalLocal);
    
    float2 vDecalTexCoord = vPositionDecalLocal.xz + 0.5f;

    
    Out.vColor = vDecalDiffuse;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Deacl
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}

    
    //bool isDecal;

    ////데칼 영역 내부인지 확인
    //if (vPositionDecalLocal.x >= -0.5 && vPositionDecalLocal.x <= 0.5
    //    && vPositionDecalLocal.y >= -0.5 && vPositionDecalLocal.y <= 0.5
    //    && vPositionDecalLocal.z >= -0.5 && vPositionDecalLocal.z <= 0.5)
    //{
    //    isDecal = true;
    //}
    //else
    //    isDecal = false;
    
    //if (isDecal)
    //{
    //    //로컬 좌표를 UV로 변환
        
    //    float u = vPositionDecalLocal.x + 0.5f;
    //    float v = vPositionDecalLocal.y + 0.5f;
        
    //    Out.vColor = g_Texture.Sample(LinearSampler, float2(u, v));
    //}