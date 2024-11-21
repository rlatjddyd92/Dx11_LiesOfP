#include "Shader_Engine_Defines.hlsli"

#define STATE_GROW          0x0001
#define STATE_SHRINK        0x0002

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_MaskTexture_1;
texture2D g_MaskTexture_2;

vector g_vCamPosition;
float4 g_vColor;
float g_fScale;
int g_iState;
float2 g_vTexDivide;
float g_fSpriteSpeed;

struct VS_IN
{
	/* InputSlot : 0 */
    float3 vPosition : POSITION;
	/* InputSlot : 1 */ 
    float3 vCurPos : TEXCOORD0;
    float3 vPrePos : TEXCOORD1;
    float3 vMoveDir : TEXCOORD2;
    float2 vLifeTime : TEXCOORD3;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float3 vCurPos : TEXCOORD0;
    float3 vPrePos : TEXCOORD1;
    float3 vMoveDir : TEXCOORD2;
    float2 vLifeTime : TEXCOORD3;
};

/* 1. 정점의 변환과정을 수행한다. */
/* 2. 정점의 구성 정보를 변형한다. */
VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    Out.vPosition = float4(In.vCurPos, 1.f);
    Out.vCurPos = In.vCurPos;
    Out.vPrePos = In.vPrePos;
    Out.vMoveDir = In.vMoveDir;
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

struct GS_IN
{
	/* SV_ : ShaderValue */
	/* 내가 해야할 연산은 모두 마쳐놓은 것이므로 이후 dx가 추가적으로 해야할 이릉ㄹ 해라. */
    float4 vPosition : POSITION;
    float3 vCurPos : TEXCOORD0;
    float3 vPrePos : TEXCOORD1;
    float3 vMoveDir : TEXCOORD2;
    float2 vLifeTime : TEXCOORD3;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

[maxvertexcount(6)] // 꼭 해줘야 함. 점을 몇 번 찍을 건지.(인덱스 갯수)
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];

	// 빌보드용.
    float3 vLook = normalize((g_vCamPosition - In[0].vPosition).xyz);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
    float3 vUp = normalize(cross(vLook, vRight));

    vRight *= g_fScale;
    vUp *= g_fScale;
    
    if (g_iState & STATE_GROW)
    {
        vRight *= In[0].vLifeTime.y / In[0].vLifeTime.x;
        vUp *= In[0].vLifeTime.y / In[0].vLifeTime.x;

    }
    else if (g_iState & STATE_SHRINK)
    {
        vRight *= (1.f - In[0].vLifeTime.y / In[0].vLifeTime.x);
        vUp *= (1.f - In[0].vLifeTime.y / In[0].vLifeTime.x);
    }
    
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;

    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;

    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;

    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);

    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip(); // 여기서부터 다시 찍겠다. 이거 안해주면 123으로 찍어버림.

    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

struct PS_EFFECT_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vBlur : SV_TARGET1;
};

float2 Get_SpriteTexcoord(float2 vTexcoord, int iTexIndex);

PS_EFFECT_OUT PS_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;
    
    int iTexIndex = (int) ((In.vLifeTime.y / In.vLifeTime.x) * (g_vTexDivide.x * g_vTexDivide.y - 1.f) * g_fSpriteSpeed);

    vector vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord, iTexIndex));
    
    if (vColor.a < 0.1f)
        discard;
    
    if (In.vLifeTime.x < In.vLifeTime.y)
        discard;
    
    vColor.rgb *= g_vColor.rgb;
    
    Out.vDiffuse = vColor;
    Out.vDiffuse = vector(0.f, 0.f, 0.f, 0.f);
    Out.vBlur = vColor;
    
    return Out;
}

PS_OUT PS_BLEND_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    int iTexIndex = (int) ((In.vLifeTime.y / In.vLifeTime.x) * (g_vTexDivide.x * g_vTexDivide.y - 1.f) * g_fSpriteSpeed);

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, Get_SpriteTexcoord(In.vTexcoord, iTexIndex));
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    if (In.vLifeTime.x < In.vLifeTime.y)
        discard;
    
    Out.vColor.rgb *= g_vColor.rgb;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Blend
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_BLEND_MAIN();
    }

}



float2 Get_SpriteTexcoord(float2 vTexcoord, int iTexIndex)
{
        
    float2 start = (float2) 0;
    float2 over = (float2) 0;

    start.x = (1 / g_vTexDivide.x) * iTexIndex;
    start.y = (1 / g_vTexDivide.y) * (int) (iTexIndex / g_vTexDivide.x);
	
    over.x = start.x + (1 / g_vTexDivide.x);
    over.y = start.y + (1 / g_vTexDivide.y);
	
    return start + (over - start) * vTexcoord;
}

