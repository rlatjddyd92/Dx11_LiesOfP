
texture2D g_InputBackTexture;
texture2D g_InputBloomTexture;
RWTexture2D<float4> g_OutputBackTexture;
StructuredBuffer<float> g_HDR : register(t1);

uint2 g_vRes;

// 요즘엔 이렇게 한다~
sampler Sampler_Default = sampler_state
{
    Filter = min_mag_mip_linear;
};

cbuffer ToneMapping
{
    float fMiddleGrey;
    float fLumWhiteSqr;
};

static const float3 LUM_FACTOR = float3(0.2126, 0.7152, 0.0722);

float3 ToneMapping(float3 vHDRColor)
{
    // // 휘도 값 계산
    //float fLScale = dot(vHDRColor, LUM_FACTOR);
    
    //// 중간 그레이를 기준으로 휘도 스케일링
    //fLScale *= fMiddleGrey / g_HDR[0]; // g_HDR[0]이 평균 휘도라면 이렇게 사용

    //// Reinhard 톤 매핑 공식
    //fLScale = fLScale / (1.0f + fLScale); // 기본 Reinhard 공식

    //// 휘도값이 지나치게 커지지 않도록 클램핑
    //fLScale = clamp(fLScale, 0.0f, 2.0f); // 휘도 범위를 [0, 1]로 제한
    
    //// 추가적인 밝기 조정: 화이트 포인트와 중간 그레이를 반영한 스케일링
    //fLScale = (fLScale + fLScale * fLScale / fLumWhiteSqr) / (1.0f + fLScale);

    //// 최종 색상 반환
    //return vHDRColor * fLScale;
    
    
    
    float fLScale = dot(vHDRColor, LUM_FACTOR);
    fLScale *= fMiddleGrey / g_HDR[0];
    fLScale = (fLScale + fLScale * fLScale / fLumWhiteSqr) / (1.f + fLScale);
    fLScale = clamp(fLScale, 0.5f, 1.1f);
    return vHDRColor * fLScale;
}

[numthreads(32, 32, 1)]
void CS_TONEMAPPING(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
    
    float4 vBack = g_InputBackTexture.Load(int3(vPixel, 0));
    vBack = vector(ToneMapping(vBack.xyz),1.f);
    g_OutputBackTexture[vPixel] = vBack;
}

[numthreads(32, 32, 1)]
void CS_BLOOMADD(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
    float2 vUV = float2(vPixel.x / g_vRes.x, vPixel.y / g_vRes.y);

    float4 vBack = g_InputBackTexture.Load(int3(vPixel, 0));
    float4 vBloom = g_InputBloomTexture.SampleLevel(Sampler_Default, vUV, 0);
    g_OutputBackTexture[vPixel] = vBack + vBloom;
}

technique11 DefaultTechnique
{
    pass ToneMapping
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_TONEMAPPING();
    }

    pass BloomAdd
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_BLOOMADD();
    }
}
