#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_MaskTexture_1;
texture2D g_MaskTexture_2;
texture2D g_NormalTexture;
vector g_vCamPosition;

float4 g_vColor;
float g_fRatio;
uint g_iNumInstance;

float2 g_vTileRepeat;

struct VS_IN
{
	/* InputSlot : 0 */
    float3 vPosition : POSITION;
	/* InputSlot : 1 */ 
    float3 vFirstTopPos : TEXCOORD0;
    float3 vFirstBottomPos : TEXCOORD1;
    
    float3 vSecondTopPos : TEXCOORD2;
    float3 vSecondBottomPos : TEXCOORD3;
    
    float3 vThirdTopPos : TEXCOORD4;
    float3 vThirdBottomPos : TEXCOORD5;
    
    float3 vForthTopPos : TEXCOORD6;
    float3 vForthBottomPos : TEXCOORD7;
    
    float2 vLifeTime : TEXCOORD8;
    float fIndex : TEXCOORD9;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    
    float3 vFirstTopPos : TEXCOORD0;
    float3 vFirstBottomPos : TEXCOORD1;
    
    float3 vSecondTopPos : TEXCOORD2;
    float3 vSecondBottomPos : TEXCOORD3;
    
    float3 vThirdTopPos : TEXCOORD4;
    float3 vThirdBottomPos : TEXCOORD5;
    
    float3 vForthTopPos : TEXCOORD6;
    float3 vForthBottomPos : TEXCOORD7;
    
    float2 vLifeTime : TEXCOORD8;
    float fIndex : TEXCOORD9;
};

/* 1. 정점의 변환과정을 수행한다. */
/* 2. 정점의 구성 정보를 변형한다. */
VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    float3 vPosition = In.vFirstBottomPos + (In.vFirstTopPos - In.vFirstBottomPos);
    
    Out.vPosition = float4(vPosition, 1.f);
    
    Out.vFirstTopPos = In.vFirstTopPos;
    Out.vFirstBottomPos = In.vFirstBottomPos;
    
    Out.vSecondTopPos = In.vSecondTopPos;
    Out.vSecondBottomPos = In.vSecondBottomPos;
    
    Out.vThirdTopPos = In.vThirdTopPos;
    Out.vThirdBottomPos = In.vThirdBottomPos;
    
    Out.vForthTopPos = In.vForthTopPos;
    Out.vForthBottomPos = In.vForthBottomPos;
    
    Out.vLifeTime = In.vLifeTime;
    
    Out.fIndex = In.fIndex;
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    
    float3 vFirstTopPos : TEXCOORD0;
    float3 vFirstBottomPos : TEXCOORD1;
    
    float3 vSecondTopPos : TEXCOORD2;
    float3 vSecondBottomPos : TEXCOORD3;
    
    float3 vThirdTopPos : TEXCOORD4;
    float3 vThirdBottomPos : TEXCOORD5;
    
    float3 vForthTopPos : TEXCOORD6;
    float3 vForthBottomPos : TEXCOORD7;
    
    float2 vLifeTime : TEXCOORD8;
    float fIndex : TEXCOORD9;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float fIndex : TEXCOORD2;
};

float3 CatmullRom(float3 p0, float3 p1, float3 p2, float3 p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;
    
    return 0.5 * (
        (2.0 * p1) +
        (-p0 + p2) * t +
        (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t2 +
        (-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t3
    );
}
[maxvertexcount(102)] // 꼭 해줘야 함. 점을 몇 번 찍을 건지.(인덱스 갯수) : 사각형 최대 16개
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];
    
    float3 fTop0 = In[0].vFirstTopPos;
    float3 fTop1 = In[0].vSecondTopPos;
    float3 fTop2 = In[0].vThirdTopPos;
    float3 fTop3 = In[0].vForthTopPos;

    float3 fBottom0 = In[0].vFirstBottomPos;
    float3 fBottom1 = In[0].vSecondBottomPos;
    float3 fBottom2 = In[0].vThirdBottomPos;
    float3 fBottom3 = In[0].vForthBottomPos;
    
    float3 vSaveTopPos = (float3) 0;
    float3 vSaveBottomPos = (float3) 0;
    
    for (int i = 0; i <= 16; ++i)
    {
        float t = i / 16.f;

        float3 vCurTopPos = (float3) 0;
        float3 vCurBottomPos = (float3) 0;
        float3 vPreTopPos = (float3) 0;
        float3 vPreBottomPos = (float3) 0;
        
        // Catmull-Rom 스플라인 보간 공식
        vCurTopPos = CatmullRom(fTop0, fTop1, fTop2, fTop3, t);
        vCurBottomPos = CatmullRom(fBottom0, fBottom1, fBottom2, fBottom3, t);
        
        if (0 == i)
        {
            vPreTopPos = vCurTopPos;
            vPreBottomPos = vCurBottomPos;
        }
        else
        {
            vPreTopPos = vSaveTopPos;
            vPreBottomPos = vSaveBottomPos;
        }
        
        vSaveTopPos = vCurTopPos;
        vSaveBottomPos = vCurBottomPos;
        
        
        if (0 != i)
        { // 인덱스를 기준으로 텍스처 좌표를 계산합니다.
            float fTexPosX = (In[0].fIndex + t) / (float) (g_iNumInstance);
            float fPreTexPosX = (In[0].fIndex + t + (1.f / 16.f)) / (float) (g_iNumInstance);
        
            fTexPosX = max(fTexPosX, 0.f);
            fPreTexPosX = min(fPreTexPosX, 1.f);
        
            Out[0].vPosition = float4(vCurTopPos, 1.f);
            Out[0].vTexcoord = float2(fPreTexPosX, 1.f);
            Out[0].vLifeTime = In[0].vLifeTime;
            Out[0].fIndex = In[0].fIndex;

            Out[1].vPosition = float4(vPreTopPos, 1.f);
            Out[1].vTexcoord = float2(fTexPosX, 1.f);
            Out[1].vLifeTime = In[0].vLifeTime;
            Out[1].fIndex = In[0].fIndex;
            
            Out[2].vPosition = float4(vPreBottomPos, 1.f);
            Out[2].vTexcoord = float2(fTexPosX, 0.f);
            Out[2].vLifeTime = In[0].vLifeTime;
            Out[2].fIndex = In[0].fIndex;
            
            Out[3].vPosition = float4(vCurBottomPos, 1.f);
            Out[3].vTexcoord = float2(fPreTexPosX, 0.f);
            Out[3].vLifeTime = In[0].vLifeTime;
            Out[3].fIndex = In[0].fIndex;
            
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
    }
}

[maxvertexcount(102)] // 꼭 해줘야 함. 점을 몇 번 찍을 건지.(인덱스 갯수) : 사각형 최대 16개
void GS_CONTINUOUS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];
    
    float3 fTop0 = In[0].vFirstTopPos;
    float3 fTop1 = In[0].vSecondTopPos;
    float3 fTop2 = In[0].vThirdTopPos;
    float3 fTop3 = In[0].vForthTopPos;

    float3 fBottom0 = In[0].vFirstBottomPos;
    float3 fBottom1 = In[0].vSecondBottomPos;
    float3 fBottom2 = In[0].vThirdBottomPos;
    float3 fBottom3 = In[0].vForthBottomPos;
    
    float3 vSaveTopPos = (float3) 0;
    float3 vSaveBottomPos = (float3) 0;
    
    for (int i = 0; i <= 16; ++i)
    {
        float t = i / 16.f;

        float3 vCurTopPos = (float3) 0;
        float3 vCurBottomPos = (float3) 0;
        float3 vPreTopPos = (float3) 0;
        float3 vPreBottomPos = (float3) 0;
        
        // Catmull-Rom 스플라인 보간 공식
        vCurTopPos = CatmullRom(fTop0, fTop1, fTop2, fTop3, t);
        vCurBottomPos = CatmullRom(fBottom0, fBottom1, fBottom2, fBottom3, t);
        
        if (0 == i)
        {
            vPreTopPos = vCurTopPos;
            vPreBottomPos = vCurBottomPos;
        }
        else
        {
            vPreTopPos = vSaveTopPos;
            vPreBottomPos = vSaveBottomPos;
        }
        
        vSaveTopPos = vCurTopPos;
        vSaveBottomPos = vCurBottomPos;
        
        
        if (0 != i)
        {
            Out[0].vPosition = float4(vCurTopPos, 1.f);
            Out[0].vTexcoord = float2(t + (1.f / 16.f), 0.f);
            Out[0].vLifeTime = In[0].vLifeTime;
            Out[0].fIndex = In[0].fIndex;

            Out[1].vPosition = float4(vPreTopPos, 1.f);
            Out[1].vTexcoord = float2(t, 0.f);
            Out[1].vLifeTime = In[0].vLifeTime;
            Out[1].fIndex = In[0].fIndex;
            
            Out[2].vPosition = float4(vPreBottomPos, 1.f);
            Out[2].vTexcoord = float2(t, 1.f);
            Out[2].vLifeTime = In[0].vLifeTime;
            Out[2].fIndex = In[0].fIndex;
            
            Out[3].vPosition = float4(vCurBottomPos, 1.f);
            Out[3].vTexcoord = float2(t + (1.f / 16.f), 1.f);
            Out[3].vLifeTime = In[0].vLifeTime;
            Out[3].fIndex = In[0].fIndex;
            
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
    }
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float fIndex : TEXCOORD2;
};

struct PS_EFFECT_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vBlur : SV_TARGET1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

struct PS_NORMAL_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vARM : SV_TARGET3;
    vector vEmessive : SV_TARGET4;
    vector vRimLight : SV_TARGET5;
};


/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_EFFECT_OUT PS_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;

    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vColor *= g_vColor;
    vColor.rgb *= 1.f - (((In.vTexcoord.x + In.fIndex) / (float) g_iNumInstance) + g_fRatio);

    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}

PS_OUT PS_BLEND_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    Out.vColor.a *= 1.f - (((In.vTexcoord.x + In.fIndex) / (float) g_iNumInstance) + g_fRatio);
        
    return Out;
}

PS_OUT PS_DISTORTION_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    Out.vColor.gb = Out.vColor.r;
    
    return Out;
}

PS_EFFECT_OUT PS_CONTINUOUS_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;

    float2 vTexcoord = In.vTexcoord * g_vTileRepeat;
    
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    vColor.rgb *= g_vColor.rgb;
    vColor.rgb *= 1.f - ((In.vTexcoord.x + In.fIndex) / (float) g_iNumInstance + g_fRatio);
    
    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}

PS_EFFECT_OUT PS_TRAIL_R_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;

    float2 vTexcoord = In.vTexcoord * g_vTileRepeat;
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    vColor.gb = vColor.r;
    vColor.rgb *= g_vColor.rgb;
    vColor.rgb *= 1.f - ((In.vTexcoord.x + In.fIndex) / (float) g_iNumInstance + g_fRatio);

    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}

PS_EFFECT_OUT PS_TRAIL_G_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;

    float2 vTexcoord = In.vTexcoord * g_vTileRepeat;
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    vColor.rb = vColor.g;
    vColor.rgb *= g_vColor.rgb;
    vColor.rgb *= 1.f - ((In.vTexcoord.x + In.fIndex) / (float) g_iNumInstance + g_fRatio);

    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}

PS_EFFECT_OUT PS_TRAIL_B_MAIN(PS_IN In)
{
    PS_EFFECT_OUT Out = (PS_EFFECT_OUT) 0;

    float2 vTexcoord = In.vTexcoord * g_vTileRepeat;
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    vColor.rg = vColor.b;
    vColor.rgb *= g_vColor.rgb;
    vColor.rgb *= 1.f - ((In.vTexcoord.x + In.fIndex) / (float) g_iNumInstance + g_fRatio);

    Out.vDiffuse = vColor;
    Out.vBlur = vColor;
    
    return Out;
}

PS_OUT PS_BLEND_RGBTOA_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat;
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    Out.vColor.a = max(Out.vColor.r, max(Out.vColor.g, Out.vColor.b));
    Out.vColor.a *= 1.f - (((In.vTexcoord.x + In.fIndex) / (float) g_iNumInstance) + g_fRatio);
        
    return Out;
}

PS_OUT PS_DISTORTION_TRAIL_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    vector vMask = g_MaskTexture_1.Sample(LinearSampler, vTexcoord);
    
    Out.vColor *= vMask;
    Out.vColor.rgb *= 1.f - (((In.vTexcoord.x + In.fIndex) / (float) g_iNumInstance) + g_fRatio);

    if (Out.vColor.r < 0.3f)
        discard;

    return Out;
}

PS_OUT PS_BLOOD_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = In.vTexcoord * g_vTileRepeat;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMask = g_MaskTexture_1.Sample(LinearSampler, vTexcoord);
    
    Out.vColor.a *= Out.vColor.r;
    vMask.a *= vMask.r;
    Out.vColor *= vMask;
    Out.vColor *= g_vColor;
    Out.vColor.a *= 1.f - (((In.vTexcoord.x + In.fIndex) / (float) g_iNumInstance) + g_fRatio);

    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_NORMAL_OUT PS_BLOOD_NORMAL_MAIN(PS_IN In)
{
    PS_NORMAL_OUT Out = (PS_NORMAL_OUT) 0;
    
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vARM = g_MaskTexture_1.Sample(LinearSampler, In.vTexcoord);
    
    Out.vARM.a *= 1.f - (((In.vTexcoord.x + In.fIndex) / (float) g_iNumInstance) + g_fRatio);
    if (Out.vARM.a < 0.3f)
        discard;
    
    Out.vEmessive = vector(0.f, 0.f, 0.f, 0.f);
    Out.vDepth = vector(0.f, 0.f, 0.f, 0.f);
    Out.vRimLight = vector(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
	/* 빛연산 + 림라이트 + ssao + 노멀맵핑 + pbr*/
    pass Default // 0
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Blend // 1
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_BLEND_MAIN();
    }

    pass Distortion // 2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_DISTORTION_MAIN();
    }

    pass Continuous_Default // 3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_CONTINUOUS_MAIN();
        PixelShader = compile ps_5_0 PS_CONTINUOUS_MAIN(); // 끝으로 갈수록 사라지는 효과. // 알파로 사라지는 거 아님.
    }

    pass Trail_R // 4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_TRAIL_R_MAIN();
    }

    pass Trail_G // 5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_TRAIL_G_MAIN();
    }

    pass Trail_B // 6
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_TRAIL_B_MAIN();
    }

    pass Blend_RGBTOA // 7
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_BLEND_RGBTOA_MAIN();
    }

    pass Distortion_Trail // 8
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_DISTORTION_TRAIL_MAIN();
    }

    pass Blood // 9
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_BLOOD_MAIN();
    }

    pass Blood_Normal // 10
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_BLOOD_NORMAL_MAIN();
    }
}
