// Bloom Compute Shader
Texture2D<float4> vHDRDownTexture : register(t0);
StructuredBuffer<float> AvgLum : register(t1);

RWTexture2D<float4> BloomOutput;

uint2 g_vRes;
float g_fBloomThreshold;
static const float4 LUM_FACTOR = float4(0.299f, 0.587f, 0.114f, 0);

[numthreads(1024, 1, 1)]
void CS_Bright(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 vCurPixel = uint2(dispatchThreadID.x % g_vRes.x,
    dispatchThreadID.x / g_vRes.x);

    if (vCurPixel.y < g_vRes.y)
    {
        float4 vColor = vHDRDownTexture.Load(int3(vCurPixel, 0));
        float fLum = dot(vColor, LUM_FACTOR);
        float fAvgLum = AvgLum[0];

        float fColorScale = saturate(fLum - fAvgLum * g_fBloomThreshold);

        BloomOutput[vCurPixel.xy] = vColor * fColorScale;
    }
}


static const float SampleWeights[13] =
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};

#define kernelhalf 6
#define groupthreads 128
    groupshared float4 SharedInput[groupthreads];

[numthreads(groupthreads, 1, 1)]
void CS_BlurX(uint3 GroupID : SV_GroupID, uint GroupIndex : SV_GroupIndex)
{
    int2 vCoord = int2(GroupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * GroupID.x, GroupID.y);
    vCoord = clamp(vCoord, int2(0, 0), int2(g_vRes.x - 1, g_vRes.y - 1));
    SharedInput[GroupIndex] = vHDRDownTexture.Load(int3(vCoord, 0));

    GroupMemoryBarrierWithGroupSync();
    
    if (GroupIndex >= kernelhalf && GroupIndex < (groupthreads - kernelhalf) &&
         ((GroupID.x * (groupthreads - 2 * kernelhalf) + GroupIndex - kernelhalf) < g_vRes.x))
    {
        float4 vOut = 0;
        
        [unroll]
        for (int i = -kernelhalf; i <= kernelhalf; ++i)
            vOut += SharedInput[GroupIndex + i] * SampleWeights[i + kernelhalf];

        BloomOutput[vCoord] = float4(vOut.rgb, 1.0f);
    }
}

[numthreads(groupthreads, 1, 1)]
void CS_BlurY(uint3 GroupID : SV_GroupID, uint GroupIndex : SV_GroupIndex)
{
    int2 vCoord = int2(GroupID.x, GroupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * GroupID.y);
    vCoord = clamp(vCoord, int2(0, 0), int2(g_vRes.x - 1, g_vRes.y - 1));
    SharedInput[GroupIndex] = vHDRDownTexture.Load(int3(vCoord, 0));

    GroupMemoryBarrierWithGroupSync();
    
    if (GroupIndex >= kernelhalf && GroupIndex < (groupthreads - kernelhalf) &&
         ((GroupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * GroupID.y) < g_vRes.y))
    {
        float4 vOut = 0;
        
        [unroll]
        for (int i = -kernelhalf; i <= kernelhalf; ++i)
        {
            vOut += SharedInput[GroupIndex + i] * SampleWeights[i + kernelhalf];
        }

        BloomOutput[vCoord] = float4(vOut.rgb, 1.0f);
    }
}

technique11 DefaultTechnique
{
    pass Pass_Bright
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_Bright();
    }
    pass Pass_BlurX
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_BlurX();
    }
    pass Pass_BlurY
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_BlurY();
    }
}