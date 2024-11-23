
#include "Shader_Engine_Defines.hlsli"

// 텍스쳐 변동 사항 있으면 반드시 수정할 것 
#define MAX_TEXTURES_UI 653
#define MAX_TEXTURES_ITEM 334

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_DepthTexture;
float4 g_Color;

Texture2D g_Textures_UI[MAX_TEXTURES_UI] : register(t0); // 텍스처 배열 (MAX_TEXTURES는 최대 텍스처 개수)
Texture2D g_Textures_Item[MAX_TEXTURES_ITEM] : register(t0); // 텍스처 배열 (MAX_TEXTURES는 최대 텍스처 개수)
SamplerState samplerState : register(s0); // 샘플러 상태

struct VertexInput
{
    float4 position : POSITION;
    float2 texCoords : TEXCOORD;
    int textureID : TEXID; // 텍스처 ID (각 정점/인스턴스에 할당된 텍스처 번호)
};

float4 PS_Main(VertexInput input) : SV_Target
{
    // 텍스처 ID를 사용해 해당 텍스처 샘플링
    float4 color = textures[input.textureID].Sample(samplerState, input.texCoords);
    return color;
}



