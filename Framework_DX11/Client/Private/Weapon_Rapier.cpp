#include "stdafx.h"
#include "Weapon_Rapier.h"

#include "Player.h"

#include "GameInstance.h"
#include "Effect_Manager.h"

CWeapon_Rapier::CWeapon_Rapier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Rapier::CWeapon_Rapier(const CWeapon_Rapier& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_Rapier::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Rapier::Initialize(void* pArg)
{
	PLAYER_WAPON_DESC* pDesc = static_cast<PLAYER_WAPON_DESC*>(pArg);
	m_pPlayer = pDesc->pPlayer;
	if (nullptr == m_pPlayer)
		return E_FAIL;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_strObjectTag = TEXT("PlayerWeapon");
	m_pColliderCom->Set_Owner(this);
	m_fDamageAmount = 5.f;

	m_pColliderCom->IsActive(false);

	return S_OK;
}

void CWeapon_Rapier::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

}

void CWeapon_Rapier::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Update(fTimeDelta);

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_Rapier::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
}

HRESULT CWeapon_Rapier::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Rapier::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

void CWeapon_Rapier::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Monster"))
	{
		_bool bOverlapCheck = false;
		for (auto& pObj : m_DamagedObjects)
		{
			if (pObj == pOther)
			{
				bOverlapCheck = true;
				break;
			}
		}

		if (!bOverlapCheck)
		{
			m_DamagedObjects.push_back(pOther);
			pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio);
		}
	}
}

void CWeapon_Rapier::OnCollisionStay(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Monster"))
	{

	}
}

void CWeapon_Rapier::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Monster"))
	{

	}
}

HRESULT CWeapon_Rapier::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rapier"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.1f, 0.1f, 0.53f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, -0.5f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CWeapon_Rapier* CWeapon_Rapier::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Rapier* pInstance = new CWeapon_Rapier(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Rapier"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Rapier::Clone(void* pArg)
{
	CWeapon_Rapier* pInstance = new CWeapon_Rapier(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Rapier"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Rapier::Free()
{
	__super::Free();
}
