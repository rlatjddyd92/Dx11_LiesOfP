matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

// 월드 위치 계산
float4 Compute_WorldPos(float2 vTexcoord, float fDepth, float fViewZ)
{
    float4 vWorldPositoin = 0;
	/* 투영공간상의 화면에 그려지는 픽셀의 위치를 구한다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / w */
    vWorldPositoin.x = vTexcoord.x * 2.f - 1.f;
    vWorldPositoin.y = vTexcoord.y * -2.f + 1.f;
    vWorldPositoin.z = fDepth;
    vWorldPositoin.w = 1.f;

	/* 뷰스페이스 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
	/* 로컬위치 * 월드행렬 * 뷰행렬  */
    vWorldPositoin = vWorldPositoin * fViewZ;
    vWorldPositoin = mul(vWorldPositoin, g_ProjMatrixInv);

	/* 월드 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
    vWorldPositoin = mul(vWorldPositoin, g_ViewMatrixInv);
	
    return vWorldPositoin;
}

// 프레넬 - 입사와 반사각을 이용한 색상 계산(Specular)
float3 FresnelSchlick(float LdotH, float3 fSpecularColor)
{
    return fSpecularColor + (1.f - fSpecularColor) * pow(1.f - LdotH, 5.f);
}

// 거칠기에 따른 반사 분포 계산 법선과 반사벡터로 계산
float DistributionGGX(float3 vNormal, float3 vHalfVector, float fRoughness)
{
    float a = fRoughness * fRoughness;
    float a2 = a * a;
    float NdotH = max(dot(vNormal, vHalfVector), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.141592 * denom * denom;

    return num / denom;
}


// 에너지 보존
float GeometrySchlickGGX(float NdotV, float fRoughness)
{
    float r = (fRoughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float fRoughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, fRoughness);
    float ggx1 = GeometrySchlickGGX(NdotL, fRoughness);

    return ggx1 * ggx2;
}

float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    
    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (3.141592 * denom * denom);
}

float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);

}

float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}
