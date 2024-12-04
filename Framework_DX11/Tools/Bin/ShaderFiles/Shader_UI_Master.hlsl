
#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_DepthTexture;

bool g_Multiple;
float4 g_Color;
float g_Alpha_Strash;
float4 g_Render_Area; // (그릴 좌표 범위 지정) (left,top,right,bottom)
float2 g_Render_Angle; // (그릴 각도 지정) (Start, end) (vec(1,0) 방향을 0도로 하여 0~360으로 지정) (방향 : CW)


// 컬러 변경 
float4 Input_Color(float4 vColor)
{
    float4 vReturn = vColor;
    
    if (vColor.r >= 0.f)
        vReturn.r = g_Color.r;
	
    if (vColor.g >= 0.f)
        vReturn.g = g_Color.g;
	
    if (vColor.b >= 0.f)
        vReturn.b = g_Color.b;
	
    if (vColor.a >= 0.f)
        vReturn.a = g_Color.a;
    
    return vReturn;
}

// 컬러 곱셈
float4 Multi_Color(float4 vColor)
{
    float4 vReturn = vColor;
    
    vReturn.r *= g_Color.r;
    vReturn.g *= g_Color.g;
    vReturn.b *= g_Color.b;
    vReturn.a *= g_Color.a;
    
    return vReturn;
}

// 범위 조정 
float2 Clamp_Range(float2 vTex)
{
    float2 vReturn = vTex;
    vReturn.x = clamp(vTex.x, g_Render_Area.x, g_Render_Area.z);
    vReturn.y = clamp(vTex.y, g_Render_Area.y, g_Render_Area.w);
    return vReturn;
}

 // 각도 조정
bool Check_Angle(float2 vTex, float3 vPos)
{
    float2 vCenter = { 0.5f, 0.5f };
    float2 vDirec = float2(vTex.x, vTex.y) - vCenter;
    float vRadian = atan2(vDirec.y, vDirec.x);
    
    if (g_Render_Angle.x <= g_Render_Angle.y)
        if ((vRadian < radians(g_Render_Angle.x)) || (vRadian > radians(g_Render_Angle.y)))
            return false;
        
    if (g_Render_Angle.x > g_Render_Angle.y)
        if ((vRadian < radians(g_Render_Angle.x)) && (vRadian > radians(g_Render_Angle.y)))
            return false;
    
    return true;
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* 내가 해야할 연산은 모두 마쳐놓은 것이므로 이후 dx가 추가적으로 해야할 이릉ㄹ 해라. */
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

/* 1. 정점의 변환과정을 수행한다. */
/* 2. 정점의 구성 정보를 변형한다. */
VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* 정점에 위치를 월드 뷰 투영변환한다.*/		
	/* 클라이언트에서 곱셈연산을 수행하는 TrnasformCoord함수와는 다르게 */
	/* mul함수의 경우에는 순수하게 행렬의 곱하기만 수행을 하고 w나누기연산자체는 수행하지 않는다. */
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

VS_OUT VS_RANGE( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* 정점에 위치를 월드 뷰 투영변환한다.*/		
	/* 클라이언트에서 곱셈연산을 수행하는 TrnasformCoord함수와는 다르게 */
	/* mul함수의 경우에는 순수하게 행렬의 곱하기만 수행을 하고 w나누기연산자체는 수행하지 않는다. */
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = Clamp_Range(In.vTexcoord);

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

vector Sample(float2 vTexcoord)
{
    return g_Texture.Sample(LinearSampler, vTexcoord);
}

// 1. 기본 기능 
PS_OUT PS_BASIC(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = Sample(In.vTexcoord);
	
    if (Out.vColor.a < g_Alpha_Strash)
        discard;

    return Out;
}

// 2. 컬러 수정 
PS_OUT PS_COLOR_INPUT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = Sample(In.vTexcoord);
    
    if (Out.vColor.a < g_Alpha_Strash)
        discard;
    
    Out.vColor = Input_Color(Out.vColor);
	
    return Out;
}

// 3. 컬러 곱셈
PS_OUT PS_COLOR_MULTIPLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = Sample(In.vTexcoord);
    
    if (Out.vColor.a < g_Alpha_Strash)
        discard;

    Out.vColor = Multi_Color(Out.vColor);
   
    return Out;
}

// 5. 각도 조정 
PS_OUT PS_ANGLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = Sample(In.vTexcoord);
	
    if (Out.vColor.a < g_Alpha_Strash)
        discard;
    
    if (Check_Angle(In.vTexcoord, In.vPosition) == false)
        discard;

    return Out;
}

// 8. 컬러 수정 + 각도 조정 
PS_OUT PS_COLOR_INPUT_ANGLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = Sample(In.vTexcoord);
    
    Out.vColor = Input_Color(Out.vColor);
    
    if (Check_Angle(In.vTexcoord, In.vPosition) == false)
        discard;
 
    return Out;
}

// 9. 컬러 곱셈 + 각도 조정 
PS_OUT PS_COLOR_MULTIPLE_ANGLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = Sample(In.vTexcoord);
	
    if (Out.vColor.a < g_Alpha_Strash)
        discard;

    Out.vColor = Multi_Color(Out.vColor);
    
    if (Check_Angle(In.vTexcoord, In.vPosition) == false)
        discard;

    return Out;
}

technique11 DefaultTechnique
{
	/* 빛연산 + 림라이트 + ssao + 노멀맵핑 + pbr*/

    // 0. 기본 기능
    pass BASIC
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BASIC();
    }

    // 1. 컬러 수정 
    pass COLOR_INPUT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_INPUT();
    }

    // 2. 컬러 곱셈
    pass COLOR_MULTIPLE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_MULTIPLE();
    }

    // 3. 범위 조정  
    pass RANGE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_RANGE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BASIC();
    }

    // 4. 각도 조정
    pass ANGLE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ANGLE();
    }

    // 5. 컬러 수정 + 범위 조정  
    pass COLOR_INPUT_RANGE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_RANGE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_INPUT();
    }

    // 6. 컬러 곱셈 + 범위 조정 
    pass COLOR_MULTIPLE_RANGE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_RANGE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_MULTIPLE();
    }

    // 7. 컬러 수정 + 각도 조정
    pass COLOR_INPUT_ANGLE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_INPUT_ANGLE();
    }

    // 8. 컬러 곱셈 + 각도 조정
    pass COLOR_MULTIPLE_ANGLE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_MULTIPLE_ANGLE();
    }
}