#include "Shader_Engine_Defines.hlsli"
#include "Shader_Client_InOut.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;

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

PS_OUT_MODEL PS_MAIN(PS_IN_MODEL In)
{
    PS_OUT_MODEL Out = (PS_OUT_MODEL) 0;
	
	vector			vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	
	if (0.3f >= vDiffuse.a)
		discard;

	Out.vDiffuse = vDiffuse;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 1.f);

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

	Out.vDiffuse = vDiffuse;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
	Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 1.f);

	return Out;
}

technique11	DefaultTechnique
{
	pass NonAnimModel
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass NormalMapping
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
	}
}