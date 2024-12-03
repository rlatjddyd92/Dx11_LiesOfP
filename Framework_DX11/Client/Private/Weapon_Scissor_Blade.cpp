#include "stdafx.h"
#include "Weapon_Scissor_Blade.h"

#include "Player.h"

#include "GameInstance.h"

CWeapon_Scissor_Blade::CWeapon_Scissor_Blade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Scissor_Blade::CWeapon_Scissor_Blade(const CWeapon_Scissor_Blade& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_Scissor_Blade::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Scissor_Blade::Initialize(void* pArg)
{
	CWeapon_Scissor_Handle::SCISSOR_DESC* pDesc = static_cast<CWeapon_Scissor_Handle::SCISSOR_DESC*>(pArg);
	m_eType = pDesc->eScissorType;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isActive = true;

	return S_OK;
}

void CWeapon_Scissor_Blade::Priority_Update(_float fTimeDelta)
{
}

void CWeapon_Scissor_Blade::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Update(fTimeDelta);

}

void CWeapon_Scissor_Blade::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

}

HRESULT CWeapon_Scissor_Blade::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Scissor_Blade::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Scissor_Blade::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;


	if (m_eType == CWeapon_Scissor_Handle::SCISSOR_LEFT)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Left_Bld"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	else if (m_eType == CWeapon_Scissor_Handle::SCISSOR_RIGHT)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Right_Bld"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}

	return S_OK;
}

CWeapon_Scissor_Blade* CWeapon_Scissor_Blade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Scissor_Blade* pInstance = new CWeapon_Scissor_Blade(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Scissor_Blade"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Scissor_Blade::Clone(void* pArg)
{
	CWeapon_Scissor_Blade* pInstance = new CWeapon_Scissor_Blade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Scissor_Blade"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Scissor_Blade::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
