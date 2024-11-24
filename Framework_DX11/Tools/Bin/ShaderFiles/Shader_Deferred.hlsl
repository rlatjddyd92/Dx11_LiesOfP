#include "Shader_Engine_Defines.hlsli"
#include "Shader_Function.hlsli"

matrix			g_LightViewMatrix, g_LightProjMatrix;
matrix			g_StaticLightViewMatrix;
texture2D		g_Texture;

Texture2DArray	g_CascadeTextureArr;
matrix			g_CascadeViewMatrix[3], g_CascadeProjMatrix[3];

vector			g_vLightDir;
vector			g_vLightPos;
float			g_fLightRange;

float           g_fFar;

vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;
texture2D		g_NormalTexture;

texture2D		g_PriorityTexture;
texture2D		g_DiffuseTexture;
texture2D		g_ARMTexture;
vector			g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
texture2D		g_ShadeTexture;
texture2D		g_DepthTexture;
texture2D		g_SpecularTexture;
texture2D		g_LightDepthTexture;
texture2D		g_BackTexture;
texture2D		g_FinalTexture;

texture2D		g_BloomTexture;
texture2D		g_CascadeShadowTexture;

texture2D		g_DecalDiffuseTexture;
texture2D		g_DecalNormalTexture;
texture2D		g_DecalARMTexture;

texture2D       g_RimLightTexture;
vector			g_vCamPosition;

// 캐스캐이드용 그림자 계산
float ComputeShadow(float4 vPosition, int iCascadeIndex, float4 vNormalDesc)
{
	//    [unroll] 쓰면 빨리지나??
	
    // 변환 행렬 계산
    float4 vLightProjPos = mul(vPosition, g_CascadeViewMatrix[iCascadeIndex]);
    vLightProjPos = mul(vLightProjPos, g_CascadeProjMatrix[iCascadeIndex]);
    vLightProjPos = vLightProjPos / vLightProjPos.w;
    
	// 투영된 위치를 텍스쳐 좌표로 변환
    float2 vTextCoord;
    vTextCoord.x = vLightProjPos.x * 0.5f + 0.5f;
    vTextCoord.y = vLightProjPos.y * -0.5f + 0.5f;

    // 그림자 떨림 방지 
    float fNormalOffset = 0.0001f;
    float fDot = saturate(dot(normalize(g_vLightDir.xyz) * -1.f, vNormalDesc.xyz));
    float fBias = max((fNormalOffset * 5.0f) * (1.0f - (fDot * -1.0f)), fNormalOffset);
    
	// 이 사이에 있어야함
    if (vLightProjPos.z > 1.f || vLightProjPos.z < 0.f)
    {
        return 1.f;
    }
    
    float fShadowPower = 1.f;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            vector vLightDepth = g_CascadeTextureArr.SampleCmpLevelZero(DepthComparisonSampler, float3(vTextCoord, iCascadeIndex), vLightProjPos.z, int2(x, y));
            if (vLightProjPos.z - fBias > vLightDepth.x)
            {
                fShadowPower += 0.3f;
            }
            else
            {
                fShadowPower += 1.f;
            }
        }
    }
    return fShadowPower / 9.0f;
}


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


VS_OUT VS_MAIN(/*정점*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	

	/* 정점에 위치를 월드 뷰 투영변환한다.*/		
	/* 클라이언트에서 곱셈연산을 수행하는 TrnasformCoord함수와는 다르게 */
	/* mul함수의 경우에는 순수하게 행렬의 곱하기만 수행을 하고 w나누기연산자체는 수행하지 않는다. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;	
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

/* Triangle : 정점 세개가 다 vs_main을 통과할때까지 대기 */
/* 세개가 모두다 통과되면. 밑의 과정을 수행. */
/* 리턴된 정점의 w로 정점의 xyzw를 나눈다. 투영 */
/* 정점의 위치를 뷰포트로 변환다. (윈도우좌표로 변환한다)*/
/* 래스터라이즈 : 정점정보를 기반으로하여 픽셀이 만들어진다. */

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

struct PS_OUT_LIGHT
{
	vector vShade : SV_TARGET0;
	vector vSpecular : SV_TARGET1;
    vector vCascadeShadow : SV_TARGET2;
};

struct PS_OUT_LIGHT_POINT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT			Out = (PS_OUT_LIGHT)0;

	vector		vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	vector		vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    
    vector      vDecalDiffuse = g_DecalDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector      vDecalNormalDesc = g_DecalNormalTexture.Sample(PointSampler, In.vTexcoord);
	
    float		fViewZ = vDepthDesc.y * g_fFar;
    float3      vNormal = float3(vNormalDesc.xyz * 2.f - 1.f);
    float3      vDecalNormal = float3(vDecalNormalDesc.xyz * 2.f - 1.f);
	
    vNormal = vector(lerp(vNormal, vDecalNormal, vDecalDiffuse.a), 0.f); // 알파 값에 따라 혼합
    
    vector		vPosition = Compute_WorldPos(In.vTexcoord, vDepthDesc.x, fViewZ);
	
    
    float fHalfLambert = saturate(dot(normalize(g_vLightDir) * -1.f, vNormal) * 0.5f + 0.5f);
    Out.vShade = g_vLightDiffuse * saturate(fHalfLambert + (g_vLightAmbient * g_vMtrlAmbient));
    
    // PBR
    vector      vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector      vARM = g_ARMTexture.Sample(LinearSampler, In.vTexcoord);
    vector      vDecalARM = g_DecalARMTexture.Sample(LinearSampler, In.vTexcoord);
    
    vARM = vector(lerp(vARM, vDecalARM, vDecalDiffuse.a)); // 알파 값에 따라 혼합
    
    float       fAmbietnOcc = vARM.r;
    float		fRoughness = vARM.g;
    float		fMetallic = vARM.b;
    
    float3      vAlbedo = pow(vDiffuse.xyz, 2.2f);
    
    if ((0.f == fAmbietnOcc && 0.f == fRoughness && 0.f == fMetallic))
    {
        Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
    }
    else
    {
        float3 vLookToCamera = (normalize(g_vCamPosition - vPosition)).xyz; // 월드 위치에서 카메라 방향
    
        float fHalfLambert = dot(vNormal, vLookToCamera) * 0.5f + 0.5f;
        float cosLo = max(0.f, fHalfLambert);
        float3 Lr = 2.0 * cosLo * vNormal - vLookToCamera;
    
    
        float3 F0 = lerp(0.04f, vAlbedo, fMetallic); // 금속성이라면, albedo와 F0의 값을 선형보간 하고, 아니라면 0.04를 사용한다.
        float3 radiance = g_vLightDiffuse.xyz;
    
        float3 Li = -g_vLightDir.xyz;
    
        float3 Lh = normalize(Li + vLookToCamera);
        float cosLi = max(0.f, dot(vNormal, Li));
        float cosLh = max(0.f, dot(vNormal, Lh));
    
        float3 F = FresnelSchlick(max(0.f, dot(Lh, vLookToCamera)), F0);
        float D = ndfGGX(cosLh, fRoughness); //Diffuse BRDF
        float G = gaSchlickGGX(cosLi, cosLo, fRoughness);

        //비금속 - 알베도값에 의존
        //금속- Fresnel 값 F가 중요
        float3 kd = lerp(float3(1.f, 1.f, 1.f) - F, float3(0.f, 0.f, 0.f), fMetallic);
        float3 vDiffuseBRDF = kd * vAlbedo;
        float3 vSpecularBRDF = (F * D * G) / max(0.00001f, 4.0 * cosLi * cosLo);
    
        fAmbietnOcc = pow(fAmbietnOcc, 2.2f);
    
        Out.vSpecular = float4((((vDiffuseBRDF + vSpecularBRDF) * cosLi * radiance) * fAmbietnOcc), 1.f);
      
    }

    vector vLook = vPosition - g_vCamPosition;
    
    float fShadowPower = 1.f;
    if (fViewZ <= 15.f)
    {
        fShadowPower = ComputeShadow(vPosition, 0, vNormalDesc);
    }
    else if (fViewZ > 15.f && fViewZ <= 30.f)
    {
        fShadowPower = ComputeShadow(vPosition, 1, vNormalDesc);
    }
    else if (fViewZ > 30.f && fViewZ <= 300.f)
    {
        fShadowPower = ComputeShadow(vPosition, 2, vNormalDesc);
    }
    Out.vCascadeShadow = fShadowPower;
	
	return Out;
}

PS_OUT_LIGHT_POINT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT_POINT Out = (PS_OUT_LIGHT_POINT) 0;

	vector		vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	float		fViewZ = vDepthDesc.y * g_fFar;

	vector		vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
	vector		vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vPosition = Compute_WorldPos(In.vTexcoord, vDepthDesc.x, fViewZ);

	vector		vLightDir = vPosition - g_vLightPos;

	float		fAtt = saturate((g_fLightRange - length(vLightDir)) / g_fLightRange);

	Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

	vector		vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	vector		vLook = vPosition - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f) * fAtt;
	
	return Out;
}

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vPriority = g_PriorityTexture.Sample(LinearSampler, In.vTexcoord);
	
    float fViewZ = vDepthDesc.y * g_fFar;
    float3 vNormal = float3(vNormalDesc.xyz * 2.f - 1.f);
    
    vector vPosition = Compute_WorldPos(In.vTexcoord, vDepthDesc.x, fViewZ);

    vector vDecal = g_DecalDiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vDiffuse = vector(lerp(vDiffuse, vDecal, vDecal.a)); // 알파 값에 따라 혼합

    
	vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
	vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
	
    Out.vColor = (vDiffuse * vShade + vSpecular) * g_CascadeShadowTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a == 0)
    {
        Out.vColor = vPriority;
        Out.vColor.a = 0;

    }
        
    //림라이트
    float4 vRimLightColor = g_RimLightTexture.Sample(LinearSampler, In.vTexcoord);
	
    float3 vEyeToCamera = normalize(g_vCamPosition.xyz - vPosition.xyz);
    float fRim = 1.f - saturate(dot(vEyeToCamera, vNormal));

    fRim = pow(fRim, vRimLightColor.w);
    vRimLightColor *= fRim;
    Out.vColor.xyz = saturate(Out.vColor.xyz + vRimLightColor.xyz);

	return Out;	
}

PS_OUT PS_MAIN_BACKBUFFER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_BackTexture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}



technique11	DefaultTechnique
{	
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_OnebyOne, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_OnebyOne, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
	}

	pass Deferred
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
	}

	pass BackBuffer
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BACKBUFFER();
    }
}