//#include "Shader_Engine_Defines.hlsli"

//matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

//Texture2D g_BackTexture : register(t0);
//StructuredBuffer<float> g_InputAvgLum : register(t0);
//RWStructuredBuffer<float> g_OutputAvgLum : register(u0);
//groupshared float SharedPositions[1024];

//uint2 g_vRes;
//uint g_iDomain;
//uint g_iGroupSize;
//static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

//float g_fMiddleGrey = 1.f;
//float g_fLumWhiteSqr = 1.f;
//StructuredBuffer<float> g_Avg : register(t1);

//float3 ToneMapping(float3 vHDRColor)
//{
//    // 현재 픽셀에 대한 휘도 스케일 계산
//    float fLScale = dot(vHDRColor, LUM_FACTOR);
//    fLScale *= g_fMiddleGrey / g_Avg[0]; //AvgLum[0];
//    fLScale = (fLScale + fLScale * fLScale / g_fLumWhiteSqr) / (1.f + fLScale);

//    // 휘도 스케일을 픽셀 색상에 적용
//    return vHDRColor * fLScale;
//}


//float DownScale4x4(uint2 vPixel, uint groupThreadId)
//{
//    float avgLum = 0.f;

//	//if (vPixel.y < g_vRes.y) {
//    int3 iFullResPos = int3(vPixel * 4, 0);
//    float4 vDownScaled = float4(0.f, 0.f, 0.f, 0.f);
//		[unroll]
//    for (int i = 0; i < 4; ++i)
//    {
//			[unroll]
//        for (int j = 0; j < 4; ++j)
//            vDownScaled += g_BackTexture.Load(iFullResPos, int2(j, i));
//    }
//    vDownScaled /= 16;

//    avgLum = dot(vDownScaled, LUM_FACTOR);
//    SharedPositions[groupThreadId] = avgLum;
//	//}

//    GroupMemoryBarrierWithGroupSync();
//    return avgLum;
//}
//float DownScale1024to4(uint groupThreadId, uint dispachThreadId, float avgLum)
//{
//    for (uint iGroupSize = 4, iStep1 = 1, iStep2 = 2, iStep3 = 3; iGroupSize < 1024; iGroupSize *= 4, iStep1 *= 4, iStep2 *= 4, iStep3 *= 4)
//    {
//        if (groupThreadId % iGroupSize == 0)
//        {
//            float fStepAvgLum = avgLum;

//            fStepAvgLum += dispachThreadId + iStep1 < g_iDomain ? SharedPositions[groupThreadId + iStep1] : avgLum;
//            fStepAvgLum += dispachThreadId + iStep2 < g_iDomain ? SharedPositions[groupThreadId + iStep2] : avgLum;
//            fStepAvgLum += dispachThreadId + iStep3 < g_iDomain ? SharedPositions[groupThreadId + iStep3] : avgLum;

//            avgLum = fStepAvgLum;
//            SharedPositions[groupThreadId] = fStepAvgLum;
//        }
//        GroupMemoryBarrierWithGroupSync();
//    }
//    return avgLum;
//}
//void DownScale4to1(uint groupId, uint groupThreadId, uint dispatchThreadId, float avgLum)
//{
//    if (groupThreadId == 0)
//    {
//        float fFinalAvgLum = avgLum;

//        fFinalAvgLum += dispatchThreadId + 256 < g_iDomain ? SharedPositions[groupThreadId + 256] : avgLum;
//        fFinalAvgLum += dispatchThreadId + 512 < g_iDomain ? SharedPositions[groupThreadId + 512] : avgLum;
//        fFinalAvgLum += dispatchThreadId + 768 < g_iDomain ? SharedPositions[groupThreadId + 768] : avgLum;

//        fFinalAvgLum /= 1024.f;
//        g_OutputAvgLum[groupId] = fFinalAvgLum;
//    }
//}
//[numthreads(1024, 1, 1)]
//void CS_FIRST(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID)
//{
//    uint2 vPixel = uint2(dispatchThreadId.x % g_vRes.x, dispatchThreadId.x / g_vRes.x);

//    float favgLum = DownScale4x4(vPixel, groupThreadId.x);
//    favgLum = DownScale1024to4(groupThreadId.x, dispatchThreadId.x, favgLum);
//    DownScale4to1(groupId.x, groupThreadId.x, dispatchThreadId.x, favgLum);
//}


//#define MAX_GROUPS 64
//// 공유 메모리 그룹에 값 저장
//groupshared float SharedAvgFinal[MAX_GROUPS];

//[numthreads(MAX_GROUPS, 1, 1)]
//void CS_SECOND(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID)
//{
//    float favgLum = 0.f;

//    if (dispatchThreadId.x < g_iGroupSize)
//    {
//        favgLum = g_InputAvgLum[dispatchThreadId.x];
//    }
//    SharedAvgFinal[dispatchThreadId.x] = favgLum;
//    GroupMemoryBarrierWithGroupSync();

//    if (dispatchThreadId.x % 4 == 0)
//    {
//        float fstepAvgLum = favgLum;
//        fstepAvgLum += dispatchThreadId.x + 1 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 1] : favgLum;
//        fstepAvgLum += dispatchThreadId.x + 2 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 2] : favgLum;
//        fstepAvgLum += dispatchThreadId.x + 3 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 3] : favgLum;
//        favgLum = fstepAvgLum;
//        SharedAvgFinal[dispatchThreadId.x] = fstepAvgLum;
//    }
//    GroupMemoryBarrierWithGroupSync();

//    if (dispatchThreadId.x % 16 == 0)
//    {
//        float fstepAvgLum = favgLum;
//        fstepAvgLum += dispatchThreadId.x + 4 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 4] : favgLum;
//        fstepAvgLum += dispatchThreadId.x + 8 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 8] : favgLum;
//        fstepAvgLum += dispatchThreadId.x + 12 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 12] : favgLum;
//        favgLum = fstepAvgLum;
//        SharedAvgFinal[dispatchThreadId.x] = fstepAvgLum;
//    }
//    GroupMemoryBarrierWithGroupSync();

//    if (dispatchThreadId.x == 0)
//    {
//        float fFinalLumValue = favgLum;
//        fFinalLumValue += dispatchThreadId.x + 16 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 16] : favgLum;
//        fFinalLumValue += dispatchThreadId.x + 32 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 32] : favgLum;
//        fFinalLumValue += dispatchThreadId.x + 48 < g_iGroupSize ? SharedAvgFinal[dispatchThreadId.x + 48] : favgLum;
//        fFinalLumValue /= 64.f;
//        g_OutputAvgLum[0] = max(fFinalLumValue, 0.0001);
//    }
//}

//struct VS_IN
//{
//    float3 vPosition : POSITION;
//    float2 vTexcoord : TEXCOORD0;
//};

//struct VS_OUT
//{
//    float4 vPosition : SV_POSITION;
//    float2 vTexcoord : TEXCOORD0;
//};

//VS_OUT VS_MAIN( /*정점*/VS_IN In)
//{
//    VS_OUT Out = (VS_OUT) 0;

//    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

//    vPosition = mul(vPosition, g_ViewMatrix);
//    vPosition = mul(vPosition, g_ProjMatrix);

//    Out.vPosition = vPosition;
//    Out.vTexcoord = In.vTexcoord;

//    return Out;
//}

//struct PS_IN
//{
//    float4 vPosition : SV_POSITION;
//    float2 vTexcoord : TEXCOORD0;
	
//};

//struct PS_OUT
//{
//    vector vColor : SV_TARGET0;
//};

//PS_OUT FinalPassPS(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;
    
//    float3 vColor = g_BackTexture.Sample(PointSampler, In.vTexcoord).xyz;
   
//    // 톤 매핑(HDR 색을 LDR색으로 변환)
//    vColor = ToneMapping(vColor);

//    Out.vColor = float4(vColor, 1.f);

//    // LDR 값 출력
//    return Out;
//}

//technique11 DefaultTechnique
//{
//    pass Pass_First
//    {
//        VertexShader = NULL;
//        GeometryShader = NULL;
//        PixelShader = NULL;
//        ComputeShader = compile cs_5_0 CS_FIRST();
//    }
//    pass Pass_Second
//    {
//        VertexShader = NULL;
//        GeometryShader = NULL;
//        PixelShader = NULL;
//        ComputeShader = compile cs_5_0 CS_SECOND();
//    }
//    pass Pass_Final
//    {
//        VertexShader = compile vs_5_0 VS_MAIN();
//        GeometryShader = NULL;
//        PixelShader = compile ps_5_0 FinalPassPS();
//        ComputeShader = NULL;
//    }
//}


#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_CameraViewMatrix;
texture2D g_Texture;

texture2D g_BackTexture;
texture2D g_DecalTexture;
texture2D g_BloomTexture;

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

    //LDR에서 HDR로
    //변환

    float exposure = 1.0; // 노출값 조정
    float3 vHdrColor = pow(vBack.rgb, 2.2f); // 감마 보정 해제
    
    vHdrColor *= 1.0f;
    
    
    Out.vColor = float4(vHdrColor, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_TONEMAPPING_ACES(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float3 vLdrColor = g_BackTexture.Sample(LinearSampler, In.vTexcoord).xyz;

    float4 vBloom = g_BloomTexture.Sample(LinearSampler, In.vTexcoord);
    
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    //vLdrColor += vBloom.rgb * vBloom.a * 0.5f;
    vLdrColor = saturate((vLdrColor * (a * vLdrColor + b)) / (vLdrColor * (c * vLdrColor + d) + e));

    
    vLdrColor = pow(vLdrColor, 1.f / 2.2f);
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