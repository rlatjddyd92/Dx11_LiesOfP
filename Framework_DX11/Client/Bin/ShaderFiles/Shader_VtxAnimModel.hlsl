
#include "Shader_Engine_Defines.hlsli"
#include "Shader_Client_InOut.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float g_fFar;

matrix g_CascadeViewMatrix[3], g_CascadeProjMatrix[3];


texture2D       g_DiffuseTexture;
texture2D       g_NormalTexture;
texture2D       g_ARMTexture;
texture2D       g_EmessiveTexture;
texture2D       g_DissloveTexture;
texture2D       g_MaskTexture;

float4			g_vRimLight;

float			g_fEmessiveMask;
float			g_fRimLightMask;
float           g_fDissolveRatio;

vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.0f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

/* 그리는 메시에게 영향을 주는 뼈행렬들(o) */
matrix g_BoneMatrices[660];

VS_OUT_ANIMODEL VS_MAIN(VS_IN_ANIMODEL In)
{
    VS_OUT_ANIMODEL Out = (VS_OUT_ANIMODEL) 0;

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
		
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN_ANIMODEL In)
{
    VS_OUT_NORMAL Out = (VS_OUT_NORMAL) 0;

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
		
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    vector vTangent = mul(vector(In.vTangent, 0.f), BoneMatrix);
	
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));

    return Out;
}

struct GS_IN_CASCADE
{
    float4 vPosition : SV_POSITION;
};

GS_IN_CASCADE VS_MAIN_CASCADE(VS_IN_ANIMODEL In)
{
    GS_IN_CASCADE Out = (GS_IN_CASCADE) 0;
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
		
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    return Out;
}



struct GS_OUT_CASCADE
{
    float4 vPosition : SV_POSITION;
    uint iRTVIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(9)]
void GS_MAIN(triangle GS_IN_CASCADE In[3], inout TriangleStream<GS_OUT_CASCADE> Container)
{
    GS_OUT_CASCADE Out[3];
    
    // 각 렌더타겟 별로 찍어두기
    for (uint i = 0; i < 3; ++i)
    {
        for (uint j = 0; j < 3; ++j)
        {
            // 뷰상의 위치
            float4 vViewPos = mul(In[j].vPosition, g_CascadeViewMatrix[i]);
            Out[j].vPosition = mul(vViewPos, g_CascadeProjMatrix[i]);
            Out[j].iRTVIndex = i;
            Container.Append(Out[j]);
        }
        Container.RestartStrip();
    }
}

PS_OUT_MODEL PS_MAIN(PS_IN_ANIMODEL In)
{
    PS_OUT_MODEL Out = (PS_OUT_MODEL) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (0.1f > vMtrlDiffuse.a)
        discard;
	
    vector vEmissive = g_EmessiveTexture.Sample(LinearClampSampler, In.vTexcoord) * g_fEmessiveMask;
	
    Out.vDiffuse = vMtrlDiffuse + vEmissive;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f); // 0~F사이의 깊이, 정규화된 Z값
    Out.vARM = g_ARMTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmessive = vEmissive * g_fEmessiveMask * 0.5f;
    Out.vRimLight = g_vRimLight;

    return Out;
}

PS_OUT_MODEL PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
    PS_OUT_MODEL Out = (PS_OUT_MODEL) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDisslove = g_DissloveTexture.Sample(LinearSampler, In.vTexcoord);
    if (g_fDissolveRatio >= vDisslove.r)
        discard;
    
    if (0.1f >= vDiffuse.a)
        discard;
	
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    vector vEmissive = g_EmessiveTexture.Sample(LinearClampSampler, In.vTexcoord) * g_fEmessiveMask;
	
    Out.vDiffuse = vDiffuse + vEmissive;
	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    Out.vARM = g_ARMTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmessive = vEmissive * g_fEmessiveMask ;
    Out.vRimLight = g_vRimLight;

    return Out;
}

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN_MODEL In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;

    Out.vLightDepth = vector(In.vProjPos.w / g_fFar, 0.f, 0.f, 0.f);


    return Out;
}

struct PS_IN_CASCADE
{
    float4 vPosition : SV_POSITION;
    uint iRTVIndex : SV_RenderTargetArrayIndex; // 렌더타겟 배열 중에 어떤 것에 그릴거야?
};

float4 PS_MAIN_CASCADE(PS_IN_CASCADE In) : SV_Target0
{
    return float4(In.vPosition.z, 0.f, 0.f, 1.f);
}

PS_OUT_MODEL PS_MAIN_FUR_MAGENTA(PS_IN_NORMAL In)
{
    PS_OUT_MODEL Out = (PS_OUT_MODEL) 0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMask.r >= 1.f && vMask.b >= 1.f)
        discard;
	
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    Out.vARM = vector(0.f, 0.f, 0.f, 1.f);
    Out.vEmessive = vector(0.f, 0.f, 0.f, 0.f);
    Out.vRimLight = vector(0.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_MODEL PS_MAIN_FUR_BLACK(PS_IN_ANIMODEL In)
{
    PS_OUT_MODEL Out = (PS_OUT_MODEL) 0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if(vDiffuse.r < 0.05f)
        discard;
    
    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    Out.vARM = vector(0.f, 0.f, 0.f, 1.f);
    Out.vEmessive = vector(0.f, 0.f, 0.f, 0.f);
    Out.vRimLight = vector(0.f, 0.f, 0.f, 0.f);

    return Out;
}

technique11 DefaultTechnique
{
    pass AnimModel // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass LightDepth // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

    pass AnimModel_Normal // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
    }

    pass Cascade // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_CASCADE();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_CASCADE();
    }

    pass Fur_Magenta // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FUR_MAGENTA();
    }

    pass Fur_Black // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FUR_BLACK();
    }
}