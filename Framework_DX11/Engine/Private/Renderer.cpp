#include "..\Public\Renderer.h"
#include "GameObject.h"

#include "GameInstance.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

_uint		g_iSizeX = 8192;
_uint		g_iSizeY = 4608;

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CRenderer::OnOff_IsRenderBloom()
{
	m_isUseBloom = !m_isUseBloom;
}

HRESULT CRenderer::Initialize()
{
	_uint			iNumViewports = { 1 };

	D3D11_VIEWPORT	ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_fSamplerRatio = 5.f; // 다운 업 샘플링 할 비율

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Priority"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PickDepth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_SINT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Height"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Final"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Y"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	// Bloom
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_DownSample0"), (_uint)ViewportDesc.Width / (_uint)m_fSamplerRatio, (_uint)ViewportDesc.Height / (_uint)m_fSamplerRatio, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_DownSample1"), (_uint)ViewportDesc.Width / (_uint)pow(m_fSamplerRatio,2), (_uint)ViewportDesc.Height / (_uint)pow(m_fSamplerRatio, 2), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_DownSample2"), (_uint)ViewportDesc.Width / (_uint)pow(m_fSamplerRatio, 3), (_uint)ViewportDesc.Height / (_uint)pow(m_fSamplerRatio, 3), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_BlurX0"), (_uint)ViewportDesc.Width / (_uint)pow(m_fSamplerRatio, 3), (_uint)ViewportDesc.Height / (_uint)pow(m_fSamplerRatio, 3), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_BlurXY0"), (_uint)ViewportDesc.Width / (_uint)pow(m_fSamplerRatio, 3), (_uint)ViewportDesc.Height / (_uint)pow(m_fSamplerRatio, 3), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_BlurX1"), (_uint)ViewportDesc.Width / (_uint)pow(m_fSamplerRatio, 2), (_uint)ViewportDesc.Height / (_uint)pow(m_fSamplerRatio, 2), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_BlurXY1"), (_uint)ViewportDesc.Width / (_uint)pow(m_fSamplerRatio, 2), (_uint)ViewportDesc.Height / (_uint)pow(m_fSamplerRatio, 2), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_BlurX2"), (_uint)ViewportDesc.Width / (_uint)m_fSamplerRatio, (_uint)ViewportDesc.Height / (_uint)m_fSamplerRatio, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_BlurXY2"), (_uint)ViewportDesc.Width / (_uint)m_fSamplerRatio, (_uint)ViewportDesc.Height / (_uint)m_fSamplerRatio, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_UpSample0"), (_uint)ViewportDesc.Width / (_uint)pow(m_fSamplerRatio, 2), (_uint)ViewportDesc.Height / (_uint)pow(m_fSamplerRatio, 2), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_UpSample1"), (_uint)ViewportDesc.Width / (_uint)m_fSamplerRatio, (_uint)ViewportDesc.Height / (_uint)m_fSamplerRatio, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* MRT_Priority */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Priority"), TEXT("Target_Priority"))))
		return E_FAIL;

	/* MRT_GameObjects */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;

	/* MRT_Lights */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* MRT_Height */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Height"), TEXT("Target_Height"))))
		return E_FAIL;

	/* MRT_ShadowObj*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowObj"), TEXT("Target_LightDepth"))))
		return E_FAIL;

	/* MRT_Picking */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Picking"), TEXT("Target_PickDepth"))))
		return E_FAIL;

#pragma region Bloom
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_DownSample0"), TEXT("Target_Bloom_DownSample0"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_DownSample1"), TEXT("Target_Bloom_DownSample1"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_DownSample2"), TEXT("Target_Bloom_DownSample2"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_BlurX0"), TEXT("Target_Bloom_BlurX0"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_BlurXY0"), TEXT("Target_Bloom_BlurXY0"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_BlurX1"), TEXT("Target_Bloom_BlurX1"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_BlurXY1"), TEXT("Target_Bloom_BlurXY1"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_BlurX2"), TEXT("Target_Bloom_BlurX2"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_BlurXY2"), TEXT("Target_Bloom_BlurXY2"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_UpSample0"), TEXT("Target_Bloom_UpSample0"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_UpSample1"), TEXT("Target_Bloom_UpSample1"))))
		return E_FAIL;
#pragma endregion

	/* MRT_Final */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Final"))))
		return E_FAIL;

	/* MRT_Blur_X */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		return E_FAIL;

	/* MRT_Blur_Y */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Y"), TEXT("Target_Blur_Y"))))
		return E_FAIL;


	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pBloomShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Bloom.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.f));

	if (FAILED(Ready_LightDepthStencilView()))
		return E_FAIL;

#ifdef _DEBUG
	
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Bloom_BlurXY0"), 100.f, 100.f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Bloom_BlurXY1"), 100.f, 300.f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Bloom_BlurXY2"), 100.f, 500.f, 200.f, 200.f)))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 350.f, 150.f, 300.f, 300.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 350.f, 450.f, 300.f, 300.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Height"), ViewportDesc.Width - 150.f, 150.f, 300.f, 300.f)))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_PickDepth"), 100.f, 100.f, 200.f, 200.f)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Final"), 600.f, 100.f, 200.f, 200.f)))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Blur_X"), 1280.f * 0.5f, 720.f * 0.5f, 1280.f, 720.f)))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Blur_Y"), 1280.f * 0.5f, 720.f * 0.5f, 1280.f, 720.f)))
	//	return E_FAIL;
	

#endif


	return S_OK;
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eRenderGroupID, CGameObject * pRenderObject)
{
	if (nullptr == pRenderObject || 
		eRenderGroupID >= RG_END)
		return E_FAIL;

	/* 실제 오브젝트 매니져에 추가된 객체의 주소를 여기에다가 공유했다. */
	m_RenderObjects[eRenderGroupID].emplace_back(pRenderObject);
	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_Height()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Lights()))
		return E_FAIL;
	if (FAILED(Render_ShadowObj()))
		return E_FAIL;


	if (FAILED(Render_Bloom()))
		return E_FAIL;


	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (FAILED(Render_Picking()))
		return E_FAIL;

	if (FAILED(Render_Final()))
		return E_FAIL;
	//if (FAILED(Render_Blur()))
	//	return E_FAIL;

	if (FAILED(Render_NonLights()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (KEY_TAP(KEY::F1))
		m_isDebugRender = !m_isDebugRender;

	if (FAILED(Render_Debug()))
		return E_FAIL;

#endif	

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Priority"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_PRIORITY].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Height()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Height"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_HEIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_Height();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_HEIGHT].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_ShadowObj()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowObj"), m_pLightDepthStencilView)))
		return E_FAIL;

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iSizeX;
	ViewPortDesc.Height = (_float)g_iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc); 

	for (auto& pGameObject : m_RenderObjects[RG_SHADOWOBJ])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_LightDepth();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_SHADOWOBJ].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.f;
	ViewPortDesc.Height = (_float)720.f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_NONBLEND].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	/* Shade타겟을 바인딩하고 빛연산한 결과를 그려주겠다. */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Lights"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_float4))))
		return E_FAIL;

	
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
	if (!m_isUseBloom)
	{
		m_pGameInstance->Clear_MRT(TEXT("MRT_Bloom_BlurXY2"));

		return S_OK;
	}

	if (FAILED(m_pBloomShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pBloomShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pBloomShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	_float fInitWidth = ViewportDesc.Width;
	_float fInitHeight = ViewportDesc.Height;

	_bool isSelectBright = true;

#pragma region 다운 샘플링
	// 다운 샘플 1번
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_DownSample0"), m_pDownSampleDepthStencilView0)))
		return E_FAIL;

	ViewportDesc.Width = fInitWidth / m_fSamplerRatio;
	ViewportDesc.Height = fInitHeight / m_fSamplerRatio;

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Diffuse"), "g_DownSampleTexture")))
		return E_FAIL;

	if (FAILED(m_pBloomShader->Bind_RawValue("g_isSelectBright", &isSelectBright, sizeof(_bool))))
		return E_FAIL;

	m_pBloomShader->Begin(1);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	// 다운 샘플 2번
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_DownSample1"), m_pDownSampleDepthStencilView1)))
		return E_FAIL;

	isSelectBright = false;
	ViewportDesc.Width = (_float)(fInitWidth / pow(m_fSamplerRatio,2));
	ViewportDesc.Height = (_float)(fInitHeight / pow(m_fSamplerRatio, 2));

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_DownSample0"), "g_DownSampleTexture")))
		return E_FAIL;

	if (FAILED(m_pBloomShader->Bind_RawValue("g_isSelectBright", &isSelectBright, sizeof(_bool))))
		return E_FAIL;

	m_pBloomShader->Begin(1);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	// 다운 샘플 3번
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_DownSample2"), m_pDownSampleDepthStencilView2)))
		return E_FAIL;

	ViewportDesc.Width = (_float)(fInitWidth / pow(m_fSamplerRatio, 3));
	ViewportDesc.Height = (_float)(fInitHeight / pow(m_fSamplerRatio, 3));

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_DownSample1"), "g_DownSampleTexture")))
		return E_FAIL;

	m_pBloomShader->Begin(1);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region 블러 업샘플링1
	// 블러X
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_BlurX0"), m_pDownSampleDepthStencilView2)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_DownSample2"), "g_DownSampleTexture")))
		return E_FAIL;

	_Vec2 vScreenSize = _Vec2((_float)(fInitWidth / pow(m_fSamplerRatio, 3)), ViewportDesc.Height = (_float)(fInitHeight / pow(m_fSamplerRatio, 3)));
	if (FAILED(m_pBloomShader->Bind_RawValue("g_vScreenSize", &vScreenSize, sizeof(_Vec2))))
		return E_FAIL;
	
	if (FAILED((m_pBloomShader->Begin(2))))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	// 블러Y
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_BlurXY0"), m_pDownSampleDepthStencilView2)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_BlurX0"), "g_DownSampleTexture")))
		return E_FAIL;

	m_pBloomShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	// 업 샘플
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_UpSample0"), m_pDownSampleDepthStencilView1)))
		return E_FAIL;

	ViewportDesc.Width = (_float)(fInitWidth / pow(m_fSamplerRatio, 2));
	ViewportDesc.Height = (_float)(fInitHeight / pow(m_fSamplerRatio, 2));

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_BlurXY0"), "g_BlurTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_DownSample2"), "g_DownSampleTexture")))
		return E_FAIL;

	m_pBloomShader->Begin(4);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region 블러 업샘플링2
	// 블러X
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_BlurX1"), m_pDownSampleDepthStencilView1)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_UpSample0"), "g_DownSampleTexture")))
		return E_FAIL;

	vScreenSize = _Vec2((_float)(fInitWidth / pow(m_fSamplerRatio, 2)), ViewportDesc.Height = (_float)(fInitHeight / pow(m_fSamplerRatio, 2)));
	if (FAILED(m_pBloomShader->Bind_RawValue("g_vScreenSize", &vScreenSize, sizeof(_Vec2))))
		return E_FAIL;

	if (FAILED((m_pBloomShader->Begin(2))))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	// 블러Y
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_BlurXY1"), m_pDownSampleDepthStencilView1)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_BlurX1"), "g_DownSampleTexture")))
		return E_FAIL;

	m_pBloomShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	// 업 샘플
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_UpSample1"), m_pDownSampleDepthStencilView0)))
		return E_FAIL;

	ViewportDesc.Width = (_float)(fInitWidth / m_fSamplerRatio);
	ViewportDesc.Height = (_float)(fInitHeight / m_fSamplerRatio);

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_BlurXY1"), "g_BlurTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_UpSample0"), "g_DownSampleTexture")))
		return E_FAIL;

	m_pBloomShader->Begin(4);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region 마지막 블러 이후 원본이랑 섞기
	// 블러X
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_BlurX2"), m_pDownSampleDepthStencilView0)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_UpSample1"), "g_DownSampleTexture")))
		return E_FAIL;

	vScreenSize = _Vec2((_float)(fInitWidth / m_fSamplerRatio), ViewportDesc.Height = (_float)(fInitHeight / m_fSamplerRatio));
	if (FAILED(m_pBloomShader->Bind_RawValue("g_vScreenSize", &vScreenSize, sizeof(_Vec2))))
		return E_FAIL;

	if (FAILED((m_pBloomShader->Begin(2))))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	// 블러Y
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_BlurXY2"), m_pDownSampleDepthStencilView0)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_BlurX2"), "g_DownSampleTexture")))
		return E_FAIL;

	m_pBloomShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

	/* 원래대로 돌리기 */
	ViewportDesc.Width = fInitWidth;
	ViewportDesc.Height = fInitHeight;

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 20.f, -15.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Priority"), "g_PriorityTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture"))) // 아마 없어도 될거같긴한데 확인해보기
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_LightDepth"), "g_LightDepthTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Bloom_BlurXY2"), "g_BloomTexture")))
		return E_FAIL;

	m_pShader->Begin(3);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_SRV("g_BackTexture", m_pGameInstance->Get_BackBuffer_SRV())))
		return E_FAIL;

	m_pShader->Begin(4);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blur()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Final"), "g_FinalTexture")))
		return E_FAIL;

	m_pShader->Begin(5);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Blur_X"), "g_BlurXTexture")))
		return E_FAIL;

	m_pShader->Begin(6);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Final"), "g_FinalTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Blur_Y"), "g_BlurYTexture")))
		return E_FAIL;

	m_pShader->Begin(7);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Picking()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Picking"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_PICKING])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_Picking();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_PICKING].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonLights()
{
	for (auto& pGameObject : m_RenderObjects[RG_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_NONLIGHT].clear();

	return S_OK;
}

//_bool Compare(CGameObject* pSour, CGameObject* pDest)
//{
//	return dynamic_cast<CBlendObject*>(pSour)->Get_ViewZ() > dynamic_cast<CBlendObject*>(pDest)->Get_ViewZ();
//}

HRESULT CRenderer::Render_Blend()
{
	/*WeightBlend*/

	_Vec3 vCamPosition = m_pGameInstance->Get_CamPosition_Vec3();
	m_RenderObjects[RG_BLEND].sort([&](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			_float fDistSour, fDistDest;
			fDistSour = XMVectorGetX(XMVector3Length(pSour->Get_Transform()->Get_State(CTransform::STATE_POSITION) - vCamPosition));
			fDistDest = XMVectorGetX(XMVector3Length(pDest->Get_Transform()->Get_State(CTransform::STATE_POSITION) - vCamPosition));

			return fDistSour > fDistDest;
		});

	for (auto& pGameObject : m_RenderObjects[RG_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	/*m_RenderObjects[RG_UI].sort();*/

	for (auto& pGameObject : m_RenderObjects[RG_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Ready_LightDepthStencilView()
{
	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iSizeX;
	TextureDesc.Height = g_iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL	/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	/* Bloom */
	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = (_uint)(ViewportDesc.Width / m_fSamplerRatio);
	TextureDesc.Height = (_uint)(ViewportDesc.Height / m_fSamplerRatio);
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL	/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDownSampleDepthStencilView0)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);
	

	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = (_uint)(ViewportDesc.Width / pow(m_fSamplerRatio, 2));
	TextureDesc.Height = (_uint)(ViewportDesc.Height / pow(m_fSamplerRatio, 2));
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL	/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDownSampleDepthStencilView1)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);


	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = (_uint)(ViewportDesc.Width / pow(m_fSamplerRatio, 3));
	TextureDesc.Height = (_uint)(ViewportDesc.Height / pow(m_fSamplerRatio, 3));
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL	/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDownSampleDepthStencilView2)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
	if (!m_isDebugRender)
		return S_OK;

	for (auto& pDebugCom : m_DebugObjects)
	{
			pDebugCom->Render();

		Safe_Release(pDebugCom);
	}

	m_DebugObjects.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer); 
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Bloom_DownSample0"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Bloom_BlurXY0"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Bloom_BlurXY1"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Bloom_BlurXY2"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Picking"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_ShadowObj"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Final"), m_pShader, m_pVIBuffer);
	//m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Blur_X"), m_pShader, m_pVIBuffer);

	return S_OK;
}

#endif

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pGameObject : RenderObjects)
			Safe_Release(pGameObject);

		RenderObjects.clear();
	}

	Safe_Release(m_pDownSampleDepthStencilView2);
	Safe_Release(m_pDownSampleDepthStencilView1);
	Safe_Release(m_pDownSampleDepthStencilView0);
	Safe_Release(m_pLightDepthStencilView);
	Safe_Release(m_pBloomShader);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
