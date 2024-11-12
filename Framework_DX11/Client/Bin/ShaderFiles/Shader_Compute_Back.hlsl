
texture2D g_InputBackTexture;
RWTexture2D<float4> g_OutputBackTexture;
StructuredBuffer<float> g_HDR : register(t1);

// 요즘엔 이렇게 한다~
sampler Sampler_Default = sampler_state
{
    Filter = min_mag_mip_linear;
};

//// 톤 매핑용
//cbuffer FinalPassConstants : register(b0)
//{
   
//}

float fMiddleGrey;
float fLumWhiteSqr;

static const float3 LUM_FACTOR = float3(0.2126, 0.7152, 0.0722);

float3 ToneMapping(float3 vHDRColor)
{
    float LScale = dot(vHDRColor, LUM_FACTOR);
    LScale *= fMiddleGrey / g_HDR[0];
    LScale = (LScale + LScale * LScale / fLumWhiteSqr) / (1.f + LScale);
    return vHDRColor * LScale;
}

[numthreads(32, 32, 1)]
void CS_ToneMapping(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

    float4 vBack = g_InputBackTexture.Load(int3(vPixel, 0));
    vBack = vector(ToneMapping(vBack.xyz),1.f);
    g_OutputBackTexture[vPixel] = vBack;
}

technique11 DefaultTechnique
{
    pass Pass_ToneMapping
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_ToneMapping();
    }
}
