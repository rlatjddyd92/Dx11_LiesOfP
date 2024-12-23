#include "stdafx.h"
#include "Decal.h"

#include "GameInstance.h"

CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDecal::CDecal(const CDecal& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CDecal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_isARM = pDesc->bARM;
	m_isNormal = pDesc->bNormal;
	m_bUseWorldColor = pDesc->bUseWorldColor;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPosition);

	return S_OK;
}

void CDecal::Priority_Update(_float fTimeDelta)
{
}

void CDecal::Update(_float fTimeDelta)
{
}

void CDecal::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_DECAL, this);
}

HRESULT CDecal::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fHashColor", &m_vHashColor, sizeof(_float4))))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_vDecalWorldInverse", &m_pTransformCom->Get_WorldMatrix_Inverse())))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Diffuse->Bind_ShadeResource(m_pShaderCom, "g_DeacalDiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("bNormal", &m_isNormal, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("bARM", &m_isARM, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("bUseWorldColor", &m_bUseWorldColor, sizeof(_bool))))
		return E_FAIL;

	if (m_isNormal)
	{
		if (FAILED(m_pTextureCom_Normal->Bind_ShadeResource(m_pShaderCom, "g_DeacalNormalTexture", 0)))
			return E_FAIL;
	}
	if (m_isARM)
	{
		if (FAILED(m_pTextureCom_ARM->Bind_ShadeResource(m_pShaderCom, "g_DeacalARMTexture", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_ARM"), "g_ARMTexture")))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CDecal::Ready_Components(OBJECT_DEFAULT_DESC* pObjDesc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_SSD"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	wcscpy_s(m_szTextureTag_Diffuse, pObjDesc->szTextureTag_Diffuse);

	//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pObjDesc->szTextureTag_Diffuse, MAX_PATH, m_szTextureTag_Diffuse, MAX_PATH);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureTag_Diffuse,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
		return E_FAIL;

	if (m_isNormal)
	{
		wcscpy_s(m_szTextureTag_Normal, pObjDesc->szTextureTag_Normal);
		//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pObjDesc->szTextureTag_Normal, MAX_PATH, m_szTextureTag_Normal, MAX_PATH);
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureTag_Normal,
			TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom_Normal))))
			return E_FAIL;
	}
	if (m_isARM)
	{
		wcscpy_s(m_szTextureTag_ARM, pObjDesc->szTextureTag_ARM);
		//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pObjDesc->szTextureTag_ARM, MAX_PATH, m_szTextureTag_ARM, MAX_PATH);
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_szTextureTag_ARM,
			TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom_ARM))))
			return E_FAIL;
	}
	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CDecal* CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal* pInstance = new CDecal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDecal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecal::Clone(void* pArg)
{
	CDecal* pInstance = new CDecal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDecal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom_Diffuse);
	Safe_Release(m_pTextureCom_Normal);
	Safe_Release(m_pTextureCom_ARM);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
