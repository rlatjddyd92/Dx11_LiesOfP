#include "..\Public\Renderer.h"
#include "GameObject.h"

#include "GameInstance.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"

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

HRESULT CRenderer::Initialize()
{
	_uint			iNumViewports = { 1 };

	D3D11_VIEWPORT	ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_fSamplerRatio = 5.f; // 다운 업 샘플링 할 비율

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ARM"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Emessive"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_RimLight"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PickDepth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PickObjectDepth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Velocity"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_CascadeShadow"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Height"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BackBuffer"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget_Array(TEXT("Target_Cascade"), (_uint)2560, (_uint)1440, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f), 3)))
		return E_FAIL;

	//Decal
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DecalDiffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DecalNormal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DecalARM"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Effect */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Blur"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Effect_Final */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect_Final"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* MRT_GameObjects */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;	
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ARM"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emessive"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_RimLight"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_PickDepth"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_PickObjectDepth"))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Velocity"), TEXT("Target_PickObjectDepth"))))
		return E_FAIL;

	/* MRT_Lights */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_CascadeShadow"))))
		return E_FAIL;

	/* MRT_Height */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Height"), TEXT("Target_Height"))))
		return E_FAIL;

	/* MRT_ShadowObj*/
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowObj"), TEXT("Target_LightDepth"))))
		return E_FAIL;

	///* MRT_Picking */
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Picking"), TEXT("Target_PickObjectDepth"))))
	//	return E_FAIL;
	
	/* MRT_Decal */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Decal"), TEXT("Target_DecalDiffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Decal"), TEXT("Target_DecalNormal"))))
		return E_FAIL;	
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Decal"), TEXT("Target_DecalARM"))))
		return E_FAIL;

	/* MRT_Distortion */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;

	/* MRT_Effect */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect_Blur"))))
		return E_FAIL;

	/* MRT_Effect_Final */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect_Final"), TEXT("Target_Effect_Final"))))
		return E_FAIL;

	// 캐스캐이드 오브젝트 그리기용도
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Cascade"), TEXT("Target_Cascade"))))
		return E_FAIL;

	/* MRT_Final */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BackBuffer"), TEXT("Target_BackBuffer"))))
		return E_FAIL;

	if (FAILED(Ready_Shader()))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.f));

	if (FAILED(Ready_LightDepthStencilView()))
		return E_FAIL;

	if (FAILED(Ready_CascadeDepthStencilView()))
		return E_FAIL;

	if (FAILED(Ready_SSAO()))
		return E_FAIL;

	if (FAILED(Ready_HDR()))
		return E_FAIL;

	if (FAILED(Ready_Bloom()))
		return E_FAIL;

	if (FAILED(Ready_DOF()))
		return E_FAIL;

	if (FAILED(Ready_Desc()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Effect_Diffuse"), 100.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Effect_Blur"), 100.f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_CascadeShadow"), 100.f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Cascade"), 300.f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	
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

	/*if(m_pGameInstance->Get_Is_Picking())
	{
		if (FAILED(Render_Picking()))
			return E_FAIL;
	}*/

	if (FAILED(Render_Decal()))
		return E_FAIL;

	if (FAILED(Render_Cascade()))
		return E_FAIL;
	if (FAILED(Render_Lights()))
		return E_FAIL;

	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (FAILED(Render_SSAO()))
		return E_FAIL;

	if (FAILED(Render_HDR()))
		return E_FAIL;

	if (FAILED(Render_DOF())) // DOF 추가
		return E_FAIL;
	if (FAILED(Render_Radial()))
		return E_FAIL;

	if (FAILED(Render_NonLights()))
		return E_FAIL;

	if (FAILED(Render_Effect()))
		return E_FAIL;

	if (FAILED(Render_Bloom()))	// 다시 고치기
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;


	if (FAILED(Render_Distortion()))
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

	for (auto& pGameObject : m_RenderObjects[RG_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_PRIORITY].clear();


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

	D3D11_VIEWPORT			ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));
	ViewportDesc.TopLeftX = 0;
	ViewportDesc.TopLeftY = 0;
	ViewportDesc.Width = (_float)g_iSizeX;
	ViewportDesc.Height = (_float)g_iSizeY;
	ViewportDesc.MinDepth = 0.f;
	ViewportDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewportDesc); 

	for (auto& pGameObject : m_RenderObjects[RG_SHADOWOBJ])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_LightDepth();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_SHADOWOBJ].clear();

	m_pGameInstance->Draw_Shadow_Instance();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));
	ViewportDesc.TopLeftX = 0;
	ViewportDesc.TopLeftY = 0;
	ViewportDesc.Width = (_float)1280.f;
	ViewportDesc.Height = (_float)720.f;
	ViewportDesc.MinDepth = 0.f;
	ViewportDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewportDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_NONBLEND].clear();

	m_pGameInstance->Draw_Instance(0);
	

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}
HRESULT CRenderer::Render_Decal()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Decal"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_DECAL])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_DECAL].clear();


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

	if (FAILED(m_pShader->Bind_Matrices("g_CascadeViewMatrix", m_pGameInstance->Get_CascadeViewMatirx(), 3)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrices("g_CascadeProjMatrix", m_pGameInstance->Get_CascadeProjMatirx(), 3)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_ARM"), "g_ARMTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Cascade"), "g_CascadeTextureArr")))
		return E_FAIL;

	m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer, m_isOnPBR);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

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

	if (FAILED(m_pShader->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_isShadow", &m_isOnShadow, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture"))) // 아마 없어도 될거같긴한데 확인해보기
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_RimLight"), "g_RimLightTexture"))) // 아마 없어도 될거같긴한데 확인해보기
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_LightDepth"), "g_LightDepthTexture")))
		return E_FAIL;

	if (m_isOnShadow)
	{
		if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_CascadeShadow"), "g_CascadeShadowTexture")))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_DecalDiffuse"), "g_DecalDiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_DecalDiffuse"), "g_DecalNormalTexture")))
		return E_FAIL;

	if(m_isOnPBR)
		m_pShader->Begin(3);
	else
		m_pShader->Begin(7);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_SSAO()
{
	if (!m_tSSAO.isOnSSAO)
	{
		m_pGameInstance->Clear_MRT(TEXT("MRT_SSAO"));
		return S_OK;
	}

	if (FAILED(m_pSSAOShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Bind_Matrix("g_CameraViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pSSAOShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pSSAOShader->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pSSAOShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pSSAOShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pNoiseTexture_SSAO->Bind_ShadeResource(m_pSSAOShader, "g_NoiseTexture", 0)))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE SubResources;
	m_pContext->Map(m_pBuffer_SSAO, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResources);
	memcpy(SubResources.pData, &m_tSSAO, sizeof(m_tSSAO));
	m_pContext->Unmap(m_pBuffer_SSAO, 0);

	if (FAILED(m_pSSAOShader->Bind_CBuffer("SSAO_VALUE", m_pBuffer_SSAO)))
		return E_FAIL;

	m_pSSAOShader->Begin(0);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO_BlurX"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pSSAOShader, TEXT("Target_SSAO"), "g_SSAOTexture")))
		return E_FAIL;

	m_pSSAOShader->Begin(1);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO_BlurXY"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pSSAOShader, TEXT("Target_SSAO_BlurX"), "g_SSAOTexture")))
		return E_FAIL;

	m_pSSAOShader->Begin(2);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	if (FAILED(Copy_BackBuffer()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pSSAOShader, TEXT("Target_SSAO_BlurX"), "g_SSAOTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pSSAOShader, TEXT("Target_BackBuffer"), "g_BackTexture")))
		return E_FAIL;

	m_pSSAOShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_HDR()
{
	if (!m_tHDR.isOnHDR)
		return S_OK;
	Copy_BackBuffer();

	// 화면 해상도의 1/4
	_uint vRes[2] = { _uint(1280.f / 4), _uint(720.f / 4) };
	m_pHDRShader->Bind_RawValue("g_vRes", vRes, sizeof(_uint) * 2);

	_uint iDomain = vRes[0] * vRes[1];
	if (FAILED(m_pHDRShader->Bind_RawValue("g_iDomain", &iDomain, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pHDRShader, TEXT("Target_BackBuffer"), "g_InputTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->BInd_RT_UnorderedView(m_pHDRShader, TEXT("Target_HDR0"), "g_OutputAvgLum")))
		return E_FAIL;

	if (FAILED(m_pHDRShader->Begin(0)))
		return E_FAIL;
	m_pContext->Dispatch((UINT)ceil((1280.f / 4 * 720.f / 4) / 1024.f), 1, 1);

	// 다음 계산에 영향을 주지 않기 위해 클리어시킴 최대 128개 8개
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 8, m_pClearUAV, nullptr);

	// 평균 휘도 값을 추출해낼거임
	_uint iGroupSize = (UINT)ceil((_float)(1280.f / 4.f * 720.f / 4.f) / 1024.f);
	if (FAILED(m_pHDRShader->Bind_RawValue("g_iGroupSize", &iGroupSize, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pHDRShader, TEXT("Target_HDR0"), "g_InputAvgLum")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->BInd_RT_UnorderedView(m_pHDRShader, TEXT("Target_HDR1"), "g_OutputAvgLum")))
		return E_FAIL;
	m_pHDRShader->Begin(1);
	m_pContext->Flush();
	m_pContext->Dispatch(1, 1, 1);	


	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 8, m_pClearUAV, nullptr);

#pragma region Stagind 버퍼로 값 꺼내오기
	//D3D11_BUFFER_DESC bufferDesc = {};
	//bufferDesc.Usage = D3D11_USAGE_STAGING;          // Staging 버퍼 용도로 설정
	//bufferDesc.BindFlags = 0;                         // 바인드 플래그 없음
	//bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // CPU 읽기 접근 허용
	//bufferDesc.MiscFlags = 0;
	//bufferDesc.StructureByteStride = sizeof(float);   // 구조체 크기

	//// 복사할 데이터 크기 설정 (예: UAV에 저장된 float 개수만큼)
	//bufferDesc.ByteWidth = sizeof(float);

	//ID3D11Buffer* pStagingBuffer = nullptr;
	//HRESULT hr = m_pDevice->CreateBuffer(&bufferDesc, nullptr, &pStagingBuffer);
	//if (FAILED(hr)) {
	//	return hr; // 버퍼 생성 실패 시 처리
	//}

	//// GPU에서 Staging 버퍼로 데이터 복사
	//ID3D11Buffer* pBuffer = m_pGameInstance->Get_Buffer(TEXT("Target_HDR1"));
	//m_pContext->CopyResource(pStagingBuffer, pBuffer); // pBuffer는 UAV 리소스

	//// CPU에서 Staging 버퍼 데이터 읽기
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//hr = m_pContext->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);
	//if (FAILED(hr)) {
	//	pStagingBuffer->Release();
	//	return hr; // Map 실패 시 처리
	//}

	//float* pData = static_cast<float*>(mappedResource.pData);
	//// CPU에서 데이터를 처리 (예: 첫 번째 값 읽기)
	//float value = pData[0]; // 편균 휘도값

	//// Map 해제
	//m_pContext->Unmap(pStagingBuffer, 0);

	//// Staging 버퍼 해제
	//pStagingBuffer->Release();
#pragma endregion

	// 톤 매핑 할거임
	if (FAILED(m_pBackShader->Bind_RawValue("fMiddleGrey", &m_tHDR.fMiddleGrey, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pBackShader->Bind_RawValue("fLumWhiteSqr", &m_tHDR.fLumWhiteSqr, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBackShader, TEXT("Target_HDR1"), "g_HDR"))) // 휘도값
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBackShader, TEXT("Target_BackBuffer"), "g_InputBackTexture")))	// 백버퍼
		return E_FAIL;
	if (FAILED(m_pGameInstance->BInd_RT_UnorderedView(m_pBackShader, TEXT("Target_LDR"), "g_OutputBackTexture"))) // 톤 매핑 출력
		return E_FAIL;

	m_pBackShader->Begin(0);
	m_pContext->Flush(); 
	m_pContext->Dispatch(static_cast<_uint>(ceil(1280.f / 32.f)), static_cast<_uint>(ceil(720.f / 32.f)), 1);

	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 8, m_pClearUAV, nullptr);

	// 화면 출력
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_LDR"), "g_BackTexture")))
		return E_FAIL;

	m_pShader->Begin(4);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();


	return S_OK;
}

HRESULT CRenderer::Render_Bloom_Compute()
{
	if (!m_tBloom.isOnBloom)
		return S_OK;

	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	_float fInitWidth = ViewportDesc.Width;
	_float fInitHeight = ViewportDesc.Height;

	// 밝은 부분 추출
	_uint vRes[2] = { _uint(1280.f), _uint(720.f) };
	if (FAILED(m_pBloomShader->Bind_RawValue("g_vRes", vRes, sizeof(_uint) * 2)))
		return E_FAIL;

	if (FAILED(m_pBloomShader->Bind_RawValue("g_fBloomThreshold", &m_tBloom.fThreshold, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_HDR1"), "AvgLum"))) // 휘도값
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_LDR"), "vHDRDownTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->BInd_RT_UnorderedView(m_pBloomShader, TEXT("Target_Bloom0"), "BloomOutput")))
		return E_FAIL;

	m_pBloomShader->Begin(0);
	m_pContext->Dispatch((UINT)ceil((1280.f * 720.f) / 1024.f), 1, 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 8, m_pClearUAV, nullptr);


	// 다운 샘플
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBlurShader, TEXT("Target_Bloom0"), "g_InputTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->BInd_RT_UnorderedView(m_pBlurShader, TEXT("Target_Bloom1"), "g_OutputTexture")))
		return E_FAIL;

	m_pBlurShader->Begin(0);
	m_pContext->Dispatch(static_cast<_uint>(ceil(1280.f / 4.f / 32.f)), static_cast<_uint>(ceil(720.f / 4.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 8, m_pClearUAV, nullptr);

	// 가로 블러
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBlurShader, TEXT("Target_Bloom1"), "g_InputTexture"))) 
		return E_FAIL;
	if (FAILED(m_pGameInstance->BInd_RT_UnorderedView(m_pBlurShader, TEXT("Target_Bloom2"), "g_OutputTexture"))) 
		return E_FAIL;

	_float2 vWinSize = { m_vWinSize.x / 4, m_vWinSize.y / 4 };
	if (FAILED(m_pBlurShader->Bind_RawValue("g_vWinSize", &vWinSize, sizeof(_float2))))
		return E_FAIL;

	m_pBlurShader->Begin(1);
	m_pContext->Dispatch(static_cast <_uint> (ceil(1280.f / 4.f / (128.f - 12.f))), static_cast <_uint> (ceil(720.f / 4.f)), 1);
	m_pContext-> CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext-> CSSetUnorderedAccessViews(0, 8, m_pClearUAV, nullptr);

	//세로 블러
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBlurShader, TEXT("Target_Bloom2"), "g_InputTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->BInd_RT_UnorderedView(m_pBlurShader, TEXT("Target_Bloom1"), "g_OutputTexture")))
		return E_FAIL;
	m_pBlurShader->Begin(2);
	m_pContext->Dispatch(static_cast <_uint> (ceil(1280.f / 4.f)), static_cast <_uint> (ceil(720.f / 4.f / (128.f - 12.f))), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 8, m_pClearUAV, nullptr);

	// 합쳐
	m_pBackShader->Bind_RawValue("g_vRes", &m_vWinSize, sizeof(_uint) * 2);

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBackShader, TEXT("Target_Bloom1"), "g_InputBloomTexture"))) 
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBackShader, TEXT("Target_LDR"), "g_InputBackTexture")))	// 백버퍼
		return E_FAIL;
	if (FAILED(m_pGameInstance->BInd_RT_UnorderedView(m_pBackShader, TEXT("Target_Bloom0"), "g_OutputBackTexture"))) 
		return E_FAIL;

	m_pBackShader->Begin(1);
	m_pContext->Dispatch(static_cast<_uint>(ceil(1280.f / 32.f)), static_cast<_uint>(ceil(720.f / 32.f)), 1);
	m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
	m_pContext->CSSetUnorderedAccessViews(0, 8, m_pClearUAV, nullptr);


	//
	//	// 화면 출력
	//if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, TEXT("Target_Bloom2"), "g_BackTexture")))
	//	return E_FAIL;

	//m_pShader->Begin(4);

	//m_pVIBuffer->Bind_Buffers();

	//m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(Copy_BackBuffer()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_Effect_Diffuse"), "g_EffectTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_Effect_Blur"), "g_EffectBlur")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_BackBuffer"), "g_BackTexture")))
		return E_FAIL;

	m_pPostProcessShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();


	if (!m_tBloom.isOnBloom)
	{
		m_pGameInstance->Clear_MRT(TEXT("MRT_Bloom_BlurXY2"));

		return S_OK;
	}

	if (FAILED(Copy_BackBuffer()))
		return E_FAIL;

	if (FAILED(m_pBloomShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pBloomShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pBloomShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if(FAILED(m_pBloomShader->Bind_RawValue("g_fThreshold",&m_tBloom.fThreshold, sizeof(_float))))
		return E_FAIL;

	D3D11_VIEWPORT      ViewportDesc;

	_uint            iNumViewports = 1;
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
	
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Effect_Blur"), "g_DownSampleTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Emessive"), "g_EmessiveTexture")))
		return E_FAIL;

	_Vec2 vScreenSize = _Vec2((_float)(fInitWidth), ViewportDesc.Height = (_float)(fInitHeight));
	if (FAILED(m_pBloomShader->Bind_RawValue("g_vScreenSize", &vScreenSize, sizeof(_Vec2))))
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
	ViewportDesc.Width = (_float)(fInitWidth / pow(m_fSamplerRatio, 2));
	ViewportDesc.Height = (_float)(fInitHeight / pow(m_fSamplerRatio, 2));

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_DownSample0"), "g_DownSampleTexture")))
		return E_FAIL;

	vScreenSize = _Vec2((_float)(fInitWidth / m_fSamplerRatio), ViewportDesc.Height = (_float)(fInitHeight / m_fSamplerRatio));
	if (FAILED(m_pBloomShader->Bind_RawValue("g_vScreenSize", &vScreenSize, sizeof(_Vec2))))
		return E_FAIL;

	if (FAILED(m_pBloomShader->Bind_RawValue("g_isSelectBright", &isSelectBright, sizeof(_bool))))
		return E_FAIL;


	m_pBloomShader->Begin(1);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region 블러 업샘플링1
	// 블러X
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_BlurX0"), m_pDownSampleDepthStencilView1)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_DownSample1"), "g_DownSampleTexture")))
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
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_BlurXY0"), m_pDownSampleDepthStencilView1)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_BlurX0"), "g_DownSampleTexture")))
		return E_FAIL;

	m_pBloomShader->Begin(3);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	// 업 샘플
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_UpSample0"), m_pDownSampleDepthStencilView0)))
		return E_FAIL;

	ViewportDesc.Width = (_float)(fInitWidth / m_fSamplerRatio);
	ViewportDesc.Height = (_float)(fInitHeight / m_fSamplerRatio);

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_BlurXY0"), "g_BlurTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_DownSample1"), "g_DownSampleTexture")))
		return E_FAIL;

	m_pBloomShader->Begin(4);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region 마지막 블러 이후 원본이랑 섞기
	// 블러X
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_BlurX1"), m_pDownSampleDepthStencilView0)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_UpSample0"), "g_DownSampleTexture")))
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
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_BlurXY1"), m_pDownSampleDepthStencilView0)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBloomShader, TEXT("Target_Bloom_BlurX1"), "g_DownSampleTexture")))
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

	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_BackBuffer"), "g_BackTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_Bloom_BlurXY1"), "g_BloomTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;

	m_pPostProcessShader->Begin(4);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_DOF()
{	
	if (m_tDOF.isOnDOF || m_tRadial.isOnRadial)
	{
		if (FAILED(Copy_BackBuffer()))
			return E_FAIL;

		// 다운 샘플
		if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBlurShader, TEXT("Target_BackBuffer"), "g_InputTexture")))
			return E_FAIL;
		if (FAILED(m_pGameInstance->BInd_RT_UnorderedView(m_pBlurShader, TEXT("Target_DOF0"), "g_OutputTexture")))
			return E_FAIL;

		m_pBlurShader->Begin(0);
		m_pContext->Dispatch(static_cast<_uint>(ceil(1280.f / 4.f / 32.f)), static_cast<_uint>(ceil(720.f / 4.f / 32.f)), 1);
		m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
		m_pContext->CSSetUnorderedAccessViews(0, 8, m_pClearUAV, nullptr);

		// 가로 블러
		if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBlurShader, TEXT("Target_DOF0"), "g_InputTexture")))
			return E_FAIL;
		if (FAILED(m_pGameInstance->BInd_RT_UnorderedView(m_pBlurShader, TEXT("Target_DOF1"), "g_OutputTexture")))
			return E_FAIL;

		_float2 vWinSize = { m_vWinSize.x / 4, m_vWinSize.y / 4 };
		if (FAILED(m_pBlurShader->Bind_RawValue("g_vWinSize", &vWinSize, sizeof(_float2))))
			return E_FAIL;

		m_pBlurShader->Begin(1);
		m_pContext->Dispatch(static_cast <_uint> (ceil(1280.f / 4.f / (128.f - 12.f))), static_cast <_uint> (ceil(720.f / 4.f)), 1);
		m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
		m_pContext->CSSetUnorderedAccessViews(0, 8, m_pClearUAV, nullptr);

		//세로 블러
		if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pBlurShader, TEXT("Target_DOF1"), "g_InputTexture")))
			return E_FAIL;
		if (FAILED(m_pGameInstance->BInd_RT_UnorderedView(m_pBlurShader, TEXT("Target_DOF2"), "g_OutputTexture")))
			return E_FAIL;
		m_pBlurShader->Begin(2);
		m_pContext->Dispatch(static_cast <_uint> (ceil(1280.f / 4.f)), static_cast <_uint> (ceil(720.f / 4.f / (128.f - 12.f))), 1);
		m_pContext->CSSetShaderResources(0, 128, m_pClearSRV);
		m_pContext->CSSetUnorderedAccessViews(0, 8, m_pClearUAV, nullptr);
	}

	if (!m_tDOF.isOnDOF)
		return S_OK;

	//if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DOF_Final"))))
	//	return E_FAIL;

	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_BackBuffer"), "g_BackTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_DOF2"), "g_DofBlurTexture")))
		return E_FAIL;

	_float fDOF = m_tDOF.fDOF;
	if (m_tDOF.fDOF < 0.f)
	{
		fDOF -= 0.3f;
		fDOF *= -1.f;
		fDOF *= 2.f;
		m_tDOF.isInverse = true;
	}
	else
	{
		m_tDOF.isInverse = false;
	}

	m_pPostProcessShader->Bind_RawValue("g_fFocus", &fDOF, sizeof(_float));
	

	if (m_tDOF.isInverse)
	{
		m_pPostProcessShader->Begin(5);
	}
	else
	{
		m_pPostProcessShader->Begin(0);
	}
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Radial()
{
	if (!m_tRadial.isOnRadial)
		return S_OK;

	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pPostProcessShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Bind_RawValue("g_vRadialCenterPos", &m_tRadial.vRadialCenterPos, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pPostProcessShader->Bind_RawValue("g_fRadius", &m_tRadial.fRadius, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pPostProcessShader->Bind_RawValue("g_RadialPower", &m_tRadial.fRadialPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_BackBuffer"), "g_BackTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pPostProcessShader, TEXT("Target_DOF2"), "g_DofBlurTexture")))
		return E_FAIL;

	m_pPostProcessShader->Begin(2);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	
	return S_OK;
}

HRESULT CRenderer::Render_Effect()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_EFFECT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RG_EFFECT].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	if (FAILED(Copy_BackBuffer()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion"))))
		return E_FAIL;

	if (FAILED(m_pDistortionShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pDistortionShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_DISTORTION])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_DISTORTION].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pDistortionShader, TEXT("Target_BackBuffer"), "g_BackTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pDistortionShader, TEXT("Target_Distortion"), "g_DistortionTexture")))
		return E_FAIL;

	m_pDistortionShader->Begin(0);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	
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

	//옥트리 렌더
	//m_pGameInstance->World_Octree_Render();

	return S_OK;
}

HRESULT CRenderer::Render_Cascade()
{
	if (!m_isOnShadow)
	{
		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Cascade"), m_pCascadeDepthStencilViewArr)))
			return E_FAIL;

		for (auto& pGameObject : m_RenderObjects[RG_SHADOWOBJ])
		{
			Safe_Release(pGameObject);
		}

		m_pGameInstance->Draw_Shadow_Instance();

		m_RenderObjects[RG_SHADOWOBJ].clear();

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;
	}
	else
	{
		D3D11_VIEWPORT			ViewportDesc;
		ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));
		ViewportDesc.TopLeftX = 0;
		ViewportDesc.TopLeftY = 0;
		ViewportDesc.Width = 2560;
		ViewportDesc.Height = 1440;
		ViewportDesc.MinDepth = 0.f;
		ViewportDesc.MaxDepth = 1.f;

		m_pContext->RSSetViewports(1, &ViewportDesc);

		if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Cascade"), m_pCascadeDepthStencilViewArr)))
			return E_FAIL;

		for (auto& pGameObject : m_RenderObjects[RG_SHADOWOBJ])
		{
			if (nullptr != pGameObject)
				pGameObject->Render_LightDepth();

			Safe_Release(pGameObject);
		}

		m_pGameInstance->Draw_Shadow_Instance();

		m_RenderObjects[RG_SHADOWOBJ].clear();

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;

		ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));
		ViewportDesc.TopLeftX = 0;
		ViewportDesc.TopLeftY = 0;
		ViewportDesc.Width = (_float)1280.f;
		ViewportDesc.Height = (_float)720.f;
		ViewportDesc.MinDepth = 0.f;
		ViewportDesc.MaxDepth = 1.f;

		m_pContext->RSSetViewports(1, &ViewportDesc);
	}

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

HRESULT CRenderer::Copy_BackBuffer()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BackBuffer"))))
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

HRESULT CRenderer::Ready_CascadeDepthStencilView()
{
	ID3D11Texture2D* pDepthStencilArrTexture = nullptr;


	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 2560;
	TextureDesc.Height = 1440;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 3;
	TextureDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilArrTexture)))
		return E_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZeroMemory(&DepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	DepthStencilViewDesc.Flags = 0;
	DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	DepthStencilViewDesc.Texture2DArray.MipSlice = 0;
	DepthStencilViewDesc.Texture2DArray.ArraySize = 3; // 1로 해야하나?
	DepthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilArrTexture, &DepthStencilViewDesc, &m_pCascadeDepthStencilViewArr)))
		return E_FAIL;

	Safe_Release(pDepthStencilArrTexture);


	return S_OK;
}

HRESULT CRenderer::Ready_HDR()
{
	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	_uint iWidth = (_uint)ViewportDesc.Width;

	D3D11_BUFFER_DESC BufferDesc{};
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.StructureByteStride = sizeof(_float);
	BufferDesc.ByteWidth = iWidth * BufferDesc.StructureByteStride;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.NumElements = iWidth;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;;
	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.NumElements = iWidth;

	if (FAILED(m_pGameInstance->Add_RenderTarget_For_Desc(TEXT("Target_HDR0"), &BufferDesc, nullptr, &SRVDesc, &UAVDesc, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.StructureByteStride = sizeof(_float);
	BufferDesc.ByteWidth = sizeof(_float);
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.NumElements = 1;

	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.NumElements = 1;

	if (FAILED(m_pGameInstance->Add_RenderTarget_For_Desc(TEXT("Target_HDR1"), &BufferDesc, nullptr, &SRVDesc, &UAVDesc, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	D3D11_TEXTURE2D_DESC TextureDesc{};
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	TextureDesc.Width = (_uint)ViewportDesc.Width;
	TextureDesc.Height = (_uint)ViewportDesc.Height;
	TextureDesc.MipLevels = 1; 
	TextureDesc.ArraySize = 1; 
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TextureDesc.SampleDesc.Count = 1; 
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;	// 제일 중요함
	TextureDesc.CPUAccessFlags = 0; 
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pGameInstance->Add_RenderTarget_For_Desc(TEXT("Target_LDR"), nullptr, &TextureDesc, nullptr, nullptr, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LDR"), TEXT("Target_LDR"))))

	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.StructureByteStride = sizeof(_float);
	BufferDesc.ByteWidth = iWidth * BufferDesc.StructureByteStride;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.NumElements = iWidth;

	ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.NumElements = iWidth;

	if (FAILED(m_pGameInstance->Add_RenderTarget_For_Desc(TEXT("Target_Test1"), &BufferDesc, nullptr, &SRVDesc, &UAVDesc, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	m_tHDR.isOnHDR = true;
	m_tHDR.fMiddleGrey = 0.4f;
	m_tHDR.fLumWhiteSqr = 0.2f;

	return S_OK;
}

HRESULT CRenderer::Ready_SSAO()
{
	_uint			iNumViewports = { 1 };

	D3D11_VIEWPORT	ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO_BlurX"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO_BlurXY"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* MRT_SSAO */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
		return E_FAIL;

	/* MRT_SSAO_BlurX */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO_BlurX"), TEXT("Target_SSAO_BlurX"))))
		return E_FAIL;

	/* MRT_SSAO_BlurXY */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO_BlurXY"), TEXT("Target_SSAO_BlurXY"))))
		return E_FAIL;

	m_tSSAO.isOnSSAO = true;
	m_tSSAO.fRadius = 0.94f;
	m_tSSAO.fBias = 0.335f;
	m_tSSAO.fAmount = 3.f;

	return S_OK;
}

HRESULT CRenderer::Ready_Bloom()
{
	_uint			iNumViewports = { 1 };

	D3D11_VIEWPORT	ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	// Bloom
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_DownSample0"), (_uint)ViewportDesc.Width / (_uint)m_fSamplerRatio, (_uint)ViewportDesc.Height / (_uint)m_fSamplerRatio, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_DownSample1"), (_uint)ViewportDesc.Width / (_uint)pow(m_fSamplerRatio, 2), (_uint)ViewportDesc.Height / (_uint)pow(m_fSamplerRatio, 2), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_BlurX0"), (_uint)ViewportDesc.Width / (_uint)pow(m_fSamplerRatio, 2), (_uint)ViewportDesc.Height / (_uint)pow(m_fSamplerRatio, 2), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_BlurXY0"), (_uint)ViewportDesc.Width / (_uint)pow(m_fSamplerRatio, 2), (_uint)ViewportDesc.Height / (_uint)pow(m_fSamplerRatio, 2), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_BlurX1"), (_uint)ViewportDesc.Width / (_uint)m_fSamplerRatio, (_uint)ViewportDesc.Height / (_uint)m_fSamplerRatio, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_BlurXY1"), (_uint)ViewportDesc.Width / (_uint)m_fSamplerRatio, (_uint)ViewportDesc.Height / (_uint)m_fSamplerRatio, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_UpSample0"), (_uint)ViewportDesc.Width / (_uint)pow(m_fSamplerRatio, 2), (_uint)ViewportDesc.Height / (_uint)pow(m_fSamplerRatio, 2), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_UpSample0"), (_uint)ViewportDesc.Width / (_uint)m_fSamplerRatio, (_uint)ViewportDesc.Height / (_uint)m_fSamplerRatio, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_DownSample0"), TEXT("Target_Bloom_DownSample0"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_DownSample1"), TEXT("Target_Bloom_DownSample1"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_BlurX0"), TEXT("Target_Bloom_BlurX0"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_BlurXY0"), TEXT("Target_Bloom_BlurXY0"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_BlurX1"), TEXT("Target_Bloom_BlurX1"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_BlurXY1"), TEXT("Target_Bloom_BlurXY1"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_UpSample0"), TEXT("Target_Bloom_UpSample0"))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_UpSample1"), TEXT("Target_Bloom_UpSample1"))))
	//	return E_FAIL;

	m_tBloom.isOnBloom = true;
	m_tBloom.fThreshold = -0.5f;

	return S_OK;
}

HRESULT CRenderer::Ready_DOF()
{
	D3D11_VIEWPORT		ViewportDesc;

	_uint				iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	_uint iWidth = (_uint)ViewportDesc.Width;

	D3D11_TEXTURE2D_DESC TextureDesc{};
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	TextureDesc.Width = (_uint)(ViewportDesc.Width);
	TextureDesc.Height = (_uint)(ViewportDesc.Height);
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;	// 제일 중요함
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pGameInstance->Add_RenderTarget_For_Desc(TEXT("Target_DOF0"), nullptr, &TextureDesc, nullptr, nullptr, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	TextureDesc.Width = (_uint)(ViewportDesc.Width / 4);
	TextureDesc.Height = (_uint)(ViewportDesc.Height / 4);
	if (FAILED(m_pGameInstance->Add_RenderTarget_For_Desc(TEXT("Target_DOF1"), nullptr, &TextureDesc, nullptr, nullptr, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	TextureDesc.Width = (_uint)(ViewportDesc.Width / 4);
	TextureDesc.Height = (_uint)(ViewportDesc.Height / 4);
	if (FAILED(m_pGameInstance->Add_RenderTarget_For_Desc(TEXT("Target_DOF2"), nullptr, &TextureDesc, nullptr, nullptr, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	TextureDesc.Width = (_uint)(ViewportDesc.Width);
	TextureDesc.Height = (_uint)(ViewportDesc.Height);
	if (FAILED(m_pGameInstance->Add_RenderTarget_For_Desc(TEXT("Target_DOF_Final"), nullptr, &TextureDesc, nullptr, nullptr, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DOF0"), TEXT("Target_DOF0"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DOF1"), TEXT("Target_DOF1"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DOF2"), TEXT("Target_DOF2"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DOF_Final"), TEXT("Target_DOF_Final"))))
		return E_FAIL;

	m_tDOF.isOnDOF = false;
	m_tDOF.fDOF = 10.f;

	return S_OK;
}

HRESULT CRenderer::Ready_Desc()
{
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));

	//m_tSSAO.isOnSSAO = false;
	//m_tSSAO.fAmount = 0.5f;
	//m_tSSAO.fBias = 0.3f;
	//m_tSSAO.fRadius = 1.f;

	BufferDesc.Usage = D3D11_USAGE_DYNAMIC; // CPU에서 쓰기, GPU에서 읽기 가능
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.ByteWidth = sizeof(m_tSSAO); // 구조체 크기만큼 할당
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU 접근 권한 설정

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pBuffer_SSAO)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_Shader()
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pSSAOShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_SSAO.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pSSAOShader)
		return E_FAIL;
	m_pNoiseTexture_SSAO = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/T_Tile_Noise_01_C_LGS.dds"), 1);

	m_pDistortionShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Distortion.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pDistortionShader)
		return E_FAIL;

	m_pBackShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_Back.hlsl"));
	if (nullptr == m_pBackShader)
		return E_FAIL;

	m_pHDRShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_HDR.hlsl"));
	if (nullptr == m_pHDRShader)
		return E_FAIL;

	m_pBloomShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Bloom.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pBloomShader)
		return E_FAIL;

	m_pBlurShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_Blur.hlsl"));
	if (nullptr == m_pBlurShader)
		return E_FAIL;

	m_pPostProcessShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcess.hlsl"));
	if (nullptr == m_pPostProcessShader)
		return E_FAIL;
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
	
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Effect"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Distortion"), m_pShader, m_pVIBuffer);

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

	Safe_Release(m_pBuffer_SSAO);

	Safe_Release(m_pDownSampleDepthStencilView2);
	Safe_Release(m_pDownSampleDepthStencilView1);
	Safe_Release(m_pDownSampleDepthStencilView0);

	Safe_Release(m_pLightDepthStencilView);
	Safe_Release(m_pCascadeDepthStencilViewArr);

	Safe_Release(m_pPostProcessShader);
	Safe_Release(m_pBlurShader);
	Safe_Release(m_pBackShader);
	Safe_Release(m_pHDRShader);
	Safe_Release(m_pSSAOShader);
	Safe_Release(m_pNoiseTexture_SSAO);
	Safe_Release(m_pDistortionShader);

	Safe_Release(m_pBloomShader);
	Safe_Release(m_pShader);

	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}