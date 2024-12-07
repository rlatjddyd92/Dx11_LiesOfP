#include "Shader_Engine_Defines.hlsli"
#include "Shader_Function.hlsli"

matrix g_CameraViewMatrix, g_CameraProjMatrix;
matrix g_PrevCameraViewMatrix, g_PrevCameraProjMatrix;

float g_fFar;

texture2D g_BackTexture;
texture2D g_DepthTexture;
texture2D g_PriorityTexture;

/* Bloom */
texture2D g_BloomTexture;

/* DOF */
texture2D g_DofBlurTexture;
float g_fFocus; // 초점


/* Motion Blur */
float g_fMotionBlurPower;

/* Radial Blur */
float2 g_vRadialCenterPos;
float g_fRadius;
float g_RadialPower;

/* Effect */
texture2D g_EffectTexture;
texture2D g_EffectBlur;


static const float SampleWeights[13] =
{
    0.0561f, 0.1353f, 0.278f, 0.4868f, 0.7261f, 0.9231f, 1.f, 0.9231f, 0.7261f, 0.4868f, 0.278f, 0.1353f, 0.0561f
    //0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033, 0.199471, 0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216,
};


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

PS_OUT PS_MAIN_DOF(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord).y;
    vector vBack = g_BackTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBlur = g_DofBlurTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vColor = vBack;
    
    // 깊이가 클수록 더 블러를 먹음
    if(fDepth > g_fFocus)
    {
        float fFocusRatio = saturate(abs(fDepth - g_fFocus) / g_fFocus);
    
        vColor = lerp(vBack, vBlur, fFocusRatio);
    }
   
    Out.vColor = vColor;
    
    return Out;

}

PS_OUT PS_MAIN_MOTION_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fFar;
		    
    vector vCurrentlWorldPos;

    vCurrentlWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vCurrentlWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vCurrentlWorldPos.z = vDepthDesc.x;
    vCurrentlWorldPos.w = 1.0f;

    // 이걸로 비교할거임( 현재 카메라 좌표)
    vector vCurrentPos = vCurrentlWorldPos;
    
    // 월드 좌표로
    vCurrentlWorldPos *= fViewZ;
    vCurrentlWorldPos = mul(vCurrentlWorldPos, g_ProjMatrixInv);
    vCurrentlWorldPos = mul(vCurrentlWorldPos, g_ViewMatrixInv);
	
    // 이전 카메라 좌표로
    matrix PrveMatVP = mul(g_PrevCameraViewMatrix, g_CameraProjMatrix);

    vector vPrevPos = mul(vCurrentlWorldPos, PrveMatVP);
    vPrevPos /= vPrevPos.w;

    float2 vVelocity = ((vCurrentPos - vPrevPos) * 0.5f).xy;
    float2 vTexCoord = In.vTexcoord;

    vector vColor = vector(0.f, 0.f, 0.f, 0.f);

    for (int i = -10; i < 10; ++i)
    {
        vTexCoord += vVelocity * (0.005f + g_fMotionBlurPower) * i;
        float4 vCurrentColor = g_BackTexture.Sample(LinearClampSampler, vTexCoord);
        vColor += vCurrentColor;
    }

    Out.vColor = vColor / 20.f;
	
    return Out;
}

PS_OUT PS_MAIN_RADIALBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    //vector vBack = g_BackTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBlur = g_DofBlurTexture.Sample(LinearSampler, In.vTexcoord);
    
    float fBlurStart = 1.f;
    
    // 중심에서부터의 거리 구하기
    float2 vDirection = In.vTexcoord.xy - g_vRadialCenterPos;
    
    vector vColor = float4(0.f, 0.f, 0.f, 0.f);
    
    //for (uint i = 0; i < 13; ++i)
    //{
    //    // 각도 조정 (회전 방향 계산)
    //    float angle = g_RadialPower * i / 13; // 점진적으로 증가
    //    float s = sin(angle);
    //    float c = cos(angle);
        
    //    // 회전 행렬을 이용해 좌표 변환
    //    float2 rotatedUV = float2(
    //        c * vDirection.x - s * vDirection.y,
    //        s * vDirection.x + c * vDirection.y
    //    );

    //    // 샘플링 좌표 계산 (중심점 기준)
    //    float2 sampleUV = saturate(g_vRadialCenterPos + rotatedUV);

    //    // 텍스처 샘플링 및 누적
    //    vColor += g_BackTexture.Sample(LinearClampSampler, sampleUV);
    //}
    
    //int iDivision = 0;
    //for (uint i = 0; i < 20; ++i)
    //{
    //    float fScale = 1.f + ((float) i * (g_RadialPower * (1.0f / 19.f)));
    //    float2 UV = vDirection * fScale + g_vRadialCenterPos;
		
    //    if (0.f > UV.x || 1.f < UV.x || 0.f > UV.y || 1.f < UV.y)
    //        continue;

    //    vColor += g_BackTexture.Sample(LinearClampSampler, UV);
    //    ++iDivision;
    //}

    float2 vDiff = ((In.vTexcoord - g_vRadialCenterPos) * 0.5f);
    float2 vUV = In.vTexcoord;

    for (uint i = 1; i <= 10; ++i)
    {
        vUV += vDiff * g_RadialPower * i;
        
        vUV.x = saturate(vUV.x);
        float4 currentColor = g_BackTexture.Sample(LinearClampSampler, vUV);
        vColor += currentColor;
    }
    
    vColor /= 10.f;
    Out.vColor = vColor;
    
    return Out;

} 

PS_OUT PS_MAIN_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vEffect = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);
    vector vEffectBlur = g_EffectBlur.Sample(LinearSampler, In.vTexcoord);
    
    vector vBack = g_BackTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vBack + vEffect;
    
    return Out;
}

PS_OUT PS_MAIN_BLOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBloom = g_BloomTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBack = g_BackTexture.Sample(LinearSampler, In.vTexcoord);
    float fDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord).y * g_fFar;
    vector vPriority = g_PriorityTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vBack + vBloom;
    
    //if (fDepth >= g_fFar)
    //    Out.vColor += vPriority;
    
    return Out;
}

PS_OUT PS_MAIN_MOTIONBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBloom = g_BloomTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBack = g_BackTexture.Sample(LinearSampler, In.vTexcoord);
    float fDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord).y * g_fFar;
    vector vPriority = g_PriorityTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vBack + vBloom;
    
    //if (fDepth >= g_fFar)
    //    Out.vColor += vPriority;
    
    return Out;
}

PS_OUT PS_MAIN_DOF_INVERSE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord).y;
    vector vBack = g_BackTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBlur = g_DofBlurTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vColor = vBack;
    
    // 깊이가 작을수록 더 블러를 먹음
    if (fDepth < g_fFocus)
    {
        float fFocusRatio = saturate(abs(fDepth - g_fFocus) / g_fFocus);
    
        vColor = lerp(vBack, vBlur, fFocusRatio);
    }
   
    Out.vColor = vColor;
    
    return Out;

}

technique11	DefaultTechnique
{	
    // 0 
    pass DOF
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DOF();
    }

    // 1
    pass MotionBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MOTION_BLUR();
    }

    // 2
    pass RadialBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RADIALBLUR();
    }

    // 3
    pass Effeft
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
    }

    // 4
    pass Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM();
    }

    // 5
    pass DOF_Inverse
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DOF_INVERSE();
    }
}