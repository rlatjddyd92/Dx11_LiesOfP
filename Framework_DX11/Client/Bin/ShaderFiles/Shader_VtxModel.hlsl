#include "Shader_Engine_Defines.hlsli"
#include "Shader_Client_InOut.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_OldWorldMatrix, g_OldViewMatrix;
float			g_fFar;

matrix			g_CascadeViewMatrix[3], g_CascadeProjMatrix[3];

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
texture2D		g_ARMTexture;
texture2D		g_EmessiveTexture;

float g_fEmessiveMask;

VS_OUT_MODEL VS_MAIN(VS_IN_MODEL In)
{
    VS_OUT_MODEL Out = (VS_OUT_MODEL) 0;
	
	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN_MODEL In)
{
	VS_OUT_NORMAL			Out = (VS_OUT_NORMAL)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
	Out.vTexcoord = In.vTexcoord;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));

	return Out;
}

struct VS_OUT_MOTIONBLUR
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vDir: TEXCOORD1;
};

VS_OUT_MOTIONBLUR VS_MAIN_MOTIONBLUR(VS_IN_MODEL In)
{
    VS_OUT_MOTIONBLUR Out = (VS_OUT_MOTIONBLUR) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matWV)).xyz;		// 확인하기
    Out.vTexcoord = In.vTexcoord;
	
    float4 vNewPos = Out.vPosition;
    float4 vOldPos = mul(vector(In.vPosition, 1.f), g_OldWorldMatrix);
    vOldPos = mul(vOldPos, g_OldViewMatrix);
    vOldPos = mul(vOldPos, g_ProjMatrix);
	
    float3 vDir = vNewPos.xyz - vOldPos.xyz;
	
    float a = dot(normalize(vDir), normalize(Out.vNormal));
	if(a < 0.f)
        Out.vPosition = vOldPos;
    else
        Out.vPosition = vNewPos;
    
    float2 vVelocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);
    Out.vDir.xy = vVelocity * 0.5f;
    Out.vDir.y *= -1.f;
    
    Out.vDir.z = Out.vPosition.z;
    Out.vDir.w = Out.vPosition.w;
    
    return Out;
}

struct GS_IN_CASCADE
{
    float4 vPosition : SV_POSITION;
};

GS_IN_CASCADE VS_MAIN_CASCADE(VS_IN_ANIMODEL In)
{
    GS_IN_CASCADE Out = (GS_IN_CASCADE) 0;
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
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


PS_OUT_MODEL PS_MAIN(PS_IN_MODEL In)
{
    PS_OUT_MODEL Out = (PS_OUT_MODEL) 0;
	
	vector			vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	
	if (0.3f >= vDiffuse.a)
		discard;

    vector vEmissive = g_EmessiveTexture.Sample(LinearClampSampler, In.vTexcoord) * g_fEmessiveMask;
	
    Out.vDiffuse += vEmissive;
	/* -1.f ~ 1.f -> 0.f ~ 1.f */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    Out.vARM = g_ARMTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmessive = vEmissive;
    Out.vRimLight = vector(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_MODEL PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
    PS_OUT_MODEL Out = (PS_OUT_MODEL) 0;

	vector			vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	vector			vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	/* 로컬상의 변환되지 않은 노말벡터를 구했다. */
	/* 로컬스페이스 => 정점의로컬스페이스(x), 노멀벡터당 하나씩 로컬스페이스를 독립적으로 구성했다. */
	float3			vNormal = vNormalDesc.xyz * 2.f - 1.f;

	float3x3		WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

	vNormal = normalize(mul(vNormal, WorldMatrix));

	if (0.3f >= vDiffuse.a)
		discard;

    vector vEmissive = g_EmessiveTexture.Sample(LinearClampSampler, In.vTexcoord) * g_fEmessiveMask;

    Out.vDiffuse += vDiffuse + vEmissive;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    Out.vARM = g_ARMTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmessive = g_EmessiveTexture.Sample(LinearClampSampler, In.vTexcoord) * g_fEmessiveMask;
    Out.vRimLight = vector(0.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_MODEL PS_MAIN_SOPHIA(PS_IN_MODEL In)
{
    PS_OUT_MODEL Out = (PS_OUT_MODEL) 0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);


    vector vARM = g_ARMTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.3f >= vARM.r)
        discard;
    vector vEmissive = g_EmessiveTexture.Sample(LinearClampSampler, In.vTexcoord) * g_fEmessiveMask;
	
    Out.vDiffuse = vDiffuse;
	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    Out.vARM = vector(1.f, vARM.g, vARM.b, 1.f);
    Out.vEmessive = vector(0.f, 0.f, 0.f, 0.f);
    Out.vRimLight = vector(0.f, 0.f, 0.f, 0.f);

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

struct PS_IN_MOTIONBLUR
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vDir : TEXCOORD1;
};

struct PS_OUT_MOTIONBLUR
{
    vector vColor : SV_TARGET0;
};


PS_OUT_MOTIONBLUR PS_MAIN_MOTIONBLUR(PS_IN_MOTIONBLUR In)
{
    PS_OUT_MOTIONBLUR Out = (PS_OUT_MOTIONBLUR) 0;
    
    Out.vColor.xy = In.vDir.xy;
    Out.vColor.z = 1.f;
    Out.vColor.w = In.vDir.z / In.vDir.w;

    return Out;
}


technique11	DefaultTechnique
{
	pass NonAnimModel //0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass NormalMapping //1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
	}

    pass Cascade //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_CASCADE();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_CASCADE();
    }

    pass MotionBlur //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_MOTIONBLUR();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MOTIONBLUR();
    }

    pass Sophia //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SOPHIA();
    }
}