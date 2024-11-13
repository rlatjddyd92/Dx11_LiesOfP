Texture2D g_InputTexture;
RWTexture2D<float4> g_OutputTexture;

[numthreads(32, 32, 1)]
void CS_DOWNSCALE(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

    int3 vFullPos = int3(vPixel * 4, 0);
    float4 vDownScale = float4(0.f, 0.f, 0.f, 0.f);
	[unroll]
    for (int i = 0; i < 4; i++)
    {
		[unroll]
        for (int j = 0; j < 4; j++)
            vDownScale += g_InputTexture.Load(vFullPos + int3(j, i, 0));
    }
    vDownScale /= 16.f;
    g_OutputTexture[vPixel.xy] = vDownScale;
}


#define kernelhalf 6
#define groupthreads 128

groupshared float4 SharedInput[groupthreads];

static const float SampleWeights[13] =
{
    0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033, 0.199471, 0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216,
};

[numthreads(groupthreads, 1, 1)]
void CS_BLURX(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    int2 coord = int2(groupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * groupID.x, groupID.y);
    SharedInput[groupIndex] = g_InputTexture.Load(int3(coord, 0));

    GroupMemoryBarrierWithGroupSync();

    if (groupIndex >= kernelhalf && groupIndex < (groupthreads - kernelhalf))
    {
        float4 vOut = 0;

		[unroll]
        for (int i = -kernelhalf; i <= kernelhalf; ++i)
            vOut += SharedInput[groupIndex + i] * SampleWeights[i + kernelhalf];

        g_OutputTexture[coord] = vOut;
    }
}

[numthreads(groupthreads, 1, 1)]
void CS_BLURY(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    int2 coord = int2(groupID.x, groupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * groupID.y);
    SharedInput[groupIndex] = g_InputTexture.Load(int3(coord, 0));

    GroupMemoryBarrierWithGroupSync();

    if (groupIndex >= kernelhalf && groupIndex < (groupthreads - kernelhalf))
    {
        float4 vOut = 0;

		[unroll]
        for (int i = -kernelhalf; i <= kernelhalf; ++i)
            vOut += SharedInput[groupIndex + i] * SampleWeights[i + kernelhalf];

        g_OutputTexture[coord] = vOut;
    }
}

technique11 Technique_Blur
{
    pass Pass_DownScale
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_DOWNSCALE();
    }
    pass Pass_Horizon
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_BLURX();
    }
    pass Pass_Vertical
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_BLURY();
    }
}