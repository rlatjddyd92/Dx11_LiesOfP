#include "Shader_Engine_Defines.hlsli"
#include "Shader_Function.hlsli"
float g_fFar;
//matrix g_CameraViewMatrix;

texture2D g_DeacalDiffuseTexture;
texture2D g_DeacalNormalTexture;
texture2D g_DeacalARMTexture;

texture2D g_DepthTexture;
texture2D g_NormalTexture;
texture2D g_DiffuseTexture;
texture2D g_ARMTexture;

float4x4 g_vDecalWorld;
matrix g_vDecalWorldInverse;

float3 vScale = { 10.f, 10.f, 10.f };
float3 vColor = { 0.f,0.f,0.f };
float4 g_fHashColor = { 0.f,0.f,0.f,0.f};

bool bNormal = false;
bool bARM = false;
bool bUseWorldColor = false;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vARM : SV_TARGET2;
    vector vPickDepth : SV_TARGET3;
    vector vPickObjectDepth : SV_TARGET4;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexUV;
    
    vTexUV.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vTexUV.y = In.vProjPos.y / In.vProjPos.w * -0.5f + 0.5f;
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, vTexUV);
    vector vARMDesc = g_ARMTexture.Sample(PointSampler, vTexUV);
    
    
    float fViewZ = vDepthDesc.y * g_fFar;
    
    vector vPosition = (vector) 0;
    
    /* 투영공간상의 화면에 그려지는 픽셀의 위치를 구한다. */
   /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / w */
    vPosition.x = vTexUV.x * 2.f - 1.f;
    vPosition.y = vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

   /* 뷰스페이스 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
   /* 로컬위치 * 월드행렬 * 뷰행렬  */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);
    /* 월드 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    //데칼의 로컬로 이동
    vector vLocalPos = mul(vPosition, g_vDecalWorldInverse);
    
    // 월드 노멀을 데칼 공간으로 이동하여 회전
    float3 vNormal = mul(float4(vNormalDesc.xyz, 0.f), g_vDecalWorldInverse);
    
    float3 ObjectAbsPos = abs(vLocalPos.xyz);
    clip(0.5f - ObjectAbsPos);
    
    float2 vNewTexUV = vLocalPos.xy;
    vNewTexUV -= vLocalPos.z * vNormal.xy;
    vNewTexUV *= 0.5f;
    
    //데칼 이미지 텍스 쿠드 조정
    vNewTexUV *= 2.f;
    vNewTexUV += 0.5f;
    
    //float2 vDecalTexCoord = vLocalPos.xz + 0.5f;
    vector vDecalDiffuse = g_DeacalDiffuseTexture.Sample(LinearClampSampler, vNewTexUV);
    vDecalDiffuse = vector(vDecalDiffuse.xyz, 0.8f);
    
    if (vDecalDiffuse.a <= 0.1f)
        discard;
    
    if (vDecalDiffuse.r <= 0.3f)
        discard;
    
    if (bUseWorldColor)
    {
        Out.vColor.a = 0;
        vector vWorldColor = g_DiffuseTexture.Sample(PointSampler, vTexUV);
        Out.vColor = vWorldColor;
    }
    else
        Out.vColor = vDecalDiffuse;
    
    if (bNormal)
    {
        vector vDecalNormal = g_DeacalNormalTexture.Sample(LinearClampSampler, vNewTexUV);
        Out.vNormal = vDecalNormal;
    }
    else   
        Out.vNormal = vNormalDesc;
    
    if (bARM)
    {
        vector vDecalARM = g_DeacalARMTexture.Sample(LinearClampSampler, vNewTexUV);
        Out.vARM = vDecalARM;
        
        if (vDecalARM.a <= 0.1f)
            discard;
    }
    else
        Out.vARM = float4(0.f, 0.f, 0.f, 0.f);
    
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 1.f);
    Out.vPickObjectDepth = g_fHashColor;
    
    return Out;
}

PS_OUT PS_Blood(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexUV;
    
    vTexUV.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vTexUV.y = In.vProjPos.y / In.vProjPos.w * -0.5f + 0.5f;
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, vTexUV);
    vector vARMDesc = g_ARMTexture.Sample(PointSampler, vTexUV);
    
    
    float fViewZ = vDepthDesc.y * g_fFar;
    
    vector vPosition = (vector) 0;
    
    /* 투영공간상의 화면에 그려지는 픽셀의 위치를 구한다. */
   /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / w */
    vPosition.x = vTexUV.x * 2.f - 1.f;
    vPosition.y = vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

   /* 뷰스페이스 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
   /* 로컬위치 * 월드행렬 * 뷰행렬  */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);
    /* 월드 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    //데칼의 로컬로 이동
    vector vLocalPos = mul(vPosition, g_vDecalWorldInverse);
    
    // 월드 노멀을 데칼 공간으로 이동하여 회전
    float3 vNormal = mul(float4(vNormalDesc.xyz, 0.f), g_vDecalWorldInverse);
    
    float3 ObjectAbsPos = abs(vLocalPos.xyz);
    clip(0.5f - ObjectAbsPos);
    
    float2 vNewTexUV = vLocalPos.xy;
    vNewTexUV -= vLocalPos.z * vNormal.xy;
    vNewTexUV *= 0.5f;
    
    //데칼 이미지 텍스 쿠드 조정
    vNewTexUV *= 2.f;
    vNewTexUV += 0.5f;
    
    //float2 vDecalTexCoord = vLocalPos.xz + 0.5f;
    vector vDecalDiffuse = g_DeacalDiffuseTexture.Sample(LinearClampSampler, vNewTexUV);
    vDecalDiffuse = vector(vDecalDiffuse.xyz, 0.8f);
    
    if (vDecalDiffuse.a <= 0.1f)
        discard;
    
    if (vDecalDiffuse.r <= 0.3f)
        discard;
    
    Out.vColor.rgb = vColor;
    Out.vColor.a = 1.f;

    vector vDecalNormal = g_DeacalNormalTexture.Sample(LinearClampSampler, vNewTexUV);
    Out.vNormal = vDecalNormal;

    

    Out.vARM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 1.f);
    Out.vPickObjectDepth = g_fHashColor;
    
    return Out;
}

PS_OUT PS_BLOODTRAIL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexUV;
    
    vTexUV.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vTexUV.y = In.vProjPos.y / In.vProjPos.w * -0.5f + 0.5f;
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, vTexUV);
    vector vARMDesc = g_ARMTexture.Sample(PointSampler, vTexUV);
    
    
    float fViewZ = vDepthDesc.y * g_fFar;
    
    vector vPosition = (vector) 0;
    
    /* 투영공간상의 화면에 그려지는 픽셀의 위치를 구한다. */
   /* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / w */
    vPosition.x = vTexUV.x * 2.f - 1.f;
    vPosition.y = vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

   /* 뷰스페이스 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
   /* 로컬위치 * 월드행렬 * 뷰행렬  */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);
    /* 월드 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
    vPosition = mul(vPosition, g_ViewMatrixInv);
    
    //데칼의 로컬로 이동
    vector vLocalPos = mul(vPosition, g_vDecalWorldInverse);
    
    // 월드 노멀을 데칼 공간으로 이동하여 회전
    float3 vNormal = mul(float4(vNormalDesc.xyz, 0.f), g_vDecalWorldInverse);
    
    float3 ObjectAbsPos = abs(vLocalPos.xyz);
    clip(0.5f - ObjectAbsPos);
    
    float2 vNewTexUV = vLocalPos.xy;
    vNewTexUV -= vLocalPos.z * vNormal.xy;
    vNewTexUV *= 0.5f;
    
    //데칼 이미지 텍스 쿠드 조정
    vNewTexUV *= 2.f;
    vNewTexUV += 0.5f;
    
    //float2 vDecalTexCoord = vLocalPos.xz + 0.5f;
    vector vDecalDiffuse = g_DeacalDiffuseTexture.Sample(LinearClampSampler, vNewTexUV);
    vDecalDiffuse = vector(vDecalDiffuse.xyz, 0.8f);
    
    float2 vMaskUV = vNewTexUV;
    vMaskUV.x *= 2.f;
    vector vDecalMask = g_DeacalNormalTexture.Sample(LinearClampSampler, vMaskUV);
    vDecalMask = vector(vDecalMask.xyz, 0.8f);
    
    if (vDecalDiffuse.a <= 0.1f)
        discard;
    
    if (vDecalDiffuse.r <= 0.3f)
        discard;
    
    Out.vColor = vector((vDecalDiffuse * vDecalMask).xyz, 1.f);
    Out.vNormal = vNormalDesc;
    Out.vARM = float4(0.f, 0.f, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 1.f);
    Out.vPickObjectDepth = g_fHashColor;
    
    return Out;
}

struct PS_OUT_PICKING
{
    vector vColor : SV_TARGET0;
};

PS_OUT_PICKING PS_MAIN_PICKING(PS_IN In)
{
    PS_OUT_PICKING Out = (PS_OUT_PICKING) 0;
    
    Out.vColor = g_fHashColor;
    
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

    pass Picking //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PICKING();
    }

    pass Monster_Blood //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
      
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Blood();
    }

    pass Monster_BloodTrail // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
      
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOODTRAIL();
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