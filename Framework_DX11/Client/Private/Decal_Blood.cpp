#include "stdafx.h"
#include "Decal_Blood.h"

#include "GameInstance.h"

CDecal_Blood::CDecal_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDecal_Blood::CDecal_Blood(const CDecal_Blood& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CDecal_Blood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal_Blood::Initialize(void* pArg)
{
	BLOOD_DECAL_DESC* pDesc = static_cast<BLOOD_DECAL_DESC*>(pArg);

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_isNormal = true;
	m_bUseWorldColor = false;

	_uint iType = (_uint)m_pGameInstance->Get_Random(0.f, 3.f);

	if (FAILED(Ready_Components(iType)))
		return E_FAIL;

	_float fSize = m_pGameInstance->Get_Random(0.6f, 1.f);
	m_pTransformCom->Set_Scaled(fSize, fSize, fSize);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

	return S_OK;
}

void CDecal_Blood::Priority_Update(_float fTimeDelta)
{
}

void CDecal_Blood::Update(_float fTimeDelta)
{
}

void CDecal_Blood::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_DECAL, this);
}

HRESULT CDecal_Blood::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;

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

	_bool bFalse = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("bARM", &bFalse, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("bUseWorldColor", &m_bUseWorldColor, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("vColor", &m_vColor, sizeof(_Vec3))))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Normal->Bind_ShadeResource(m_pShaderCom, "g_DeacalNormalTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_ARM"), "g_ARMTexture")))
		return E_FAIL;

	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CDecal_Blood::Ready_Components(_uint iType)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_SSD"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (iType == TYPE1)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_01_C_LGS"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_01_N_LGS"),
			TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom_Normal))))
			return E_FAIL;
	}
	else if (iType == TYPE2)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_02_C_LGS"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_02_N_LGS"),
			TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom_Normal))))
			return E_FAIL;
	}
	else if (iType == TYPE3)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_03_C_LGS"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_03_N_LGS"),
			TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom_Normal))))
			return E_FAIL;
	}
	else if (iType == TYPE4)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_04_C_LGS"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
			return E_FAIL;

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_T_BloodDrop_04_N_LGS"),
			TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom_Normal))))
			return E_FAIL;
	}
	
	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CDecal_Blood* CDecal_Blood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal_Blood* pInstance = new CDecal_Blood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDecal_Blood"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecal_Blood::Clone(void* pArg)
{
	CDecal_Blood* pInstance = new CDecal_Blood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDecal_Blood"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal_Blood::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom_Diffuse);
	Safe_Release(m_pTextureCom_Normal);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
