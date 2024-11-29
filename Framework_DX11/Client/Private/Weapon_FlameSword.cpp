#include "stdafx.h"
#include "Weapon_FlameSword.h"

#include "Player.h"

#include "GameInstance.h"

CWeapon_FlameSword::CWeapon_FlameSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_FlameSword::CWeapon_FlameSword(const CWeapon_FlameSword& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_FlameSword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_FlameSword::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_strObjectTag = TEXT("PlayerWeapon");
	m_fDamageAmount = 10.f;

	m_isActive = false;
	m_pColliderCom->IsActive(false);

	return S_OK;
}

void CWeapon_FlameSword::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

}

void CWeapon_FlameSword::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Update(fTimeDelta);

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_FlameSword::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);

#endif
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
}

HRESULT CWeapon_FlameSword::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_FlameSword::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

void CWeapon_FlameSword::OnCollisionEnter(CGameObject* pOther)
{
}

void CWeapon_FlameSword::OnCollisionStay(CGameObject* pOther)
{
}

void CWeapon_FlameSword::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CWeapon_FlameSword::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FlameSword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.15f, 0.15f, 0.75f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, -0.7f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);


	return S_OK;
}

CWeapon_FlameSword* CWeapon_FlameSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_FlameSword* pInstance = new CWeapon_FlameSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Scissor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_FlameSword::Clone(void* pArg)
{
	CWeapon_FlameSword* pInstance = new CWeapon_FlameSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Scissor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_FlameSword::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
