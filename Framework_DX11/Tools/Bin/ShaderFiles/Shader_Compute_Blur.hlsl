Texture2D g_InputTexture;
RWTexture2D<float4> g_OutputTexture;

float2 g_vWinSize;

[numthreads(32, 32, 1)]
void CS_DOWNSCALE4X4(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint3 dispatchThreadID : SV_DispatchThreadID)
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

[numthreads(32, 32, 1)]
void CS_DOWNSCALE16X16(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

    int3 vFullPos = int3(vPixel * 16, 0);
    float4 vDownScale = float4(0.f, 0.f, 0.f, 0.f);
	[unroll]
    for (int i = 0; i < 16; i++)
    {
		[unroll]
        for (int j = 0; j < 16; j++)
            vDownScale += g_InputTexture.Load(vFullPos + int3(j, i, 0));
    }
    vDownScale /= 256.f;
    g_OutputTexture[vPixel.xy] = float4(vDownScale.xyz, 1.f);
}

[numthreads(32, 32, 1)]
void CS_UPSCALE16X16(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);

    // 원래 위치를 계산하여 4x4 영역으로 확장
    int2 vFullPos = vPixel * 16;

    // 현재 위치의 업샘플링된 픽셀을 계산
    float4 vUpscale = g_InputTexture[vPixel.xy];

    // 4x4 영역을 반복하며 업샘플링
    [unroll]
    for (int i = 0; i < 16; i++)
    {
        [unroll]
        for (int j = 0; j < 16; j++)
        {
            // 업샘플된 값을 4x4 영역에 복제
            g_OutputTexture[vFullPos + int2(j, i)] = vUpscale;
        }
    }
}

#define kernelhalf 6
#define groupthreads 128

groupshared float4 SharedInput[groupthreads];

static const float SampleWeights[13] =
{
    //0.0561f, 0.1353f, 0.278f, 0.4868f, 0.7261f, 0.9231f, 1.f, 0.9231f, 0.7261f, 0.4868f, 0.278f, 0.1353f, 0.0561f
    0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033, 0.199471, 0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216,
    //0.004f, 0.012f, 0.036f, 0.08f 0.14f, 0.18f, 0.22f, 0.18f, 0.14f, 0.08f, 0.036f, 0.012f, 0.004f,
};

[numthreads(groupthreads, 1, 1)]
void CS_BLURX(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    int2 coord = int2(groupIndex - kernelhalf + (groupthreads - kernelhalf * 2) * groupID.x, groupID.y);
    coord.x = clamp(coord.x, 0, g_vWinSize.x - 1);
    coord.y = clamp(coord.y, 0, g_vWinSize.y - 1);

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
    coord.x = clamp(coord.x, 0, g_vWinSize.x - 1);
    coord.y = clamp(coord.y, 0, g_vWinSize.y - 1);
    
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
        ComputeShader = compile cs_5_0 CS_DOWNSCALE4X4();
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
    pass Pass_DownScale2
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_DOWNSCALE16X16();
    }
    pass Pass_UpScale
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_DOWNSCALE16X16();
    }
}