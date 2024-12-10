#include "stdafx.h"
#include "Weapon_PlayerArm.h"

#include "Player.h"
#include "Monster.h"

#include "GameInstance.h"
#include "Effect_Manager.h"

// 24-12-06 김성용
// 내구도 조정 함수 연결을 위한 헤더 추가 
#include "GameInterface_Controller.h"

CWeapon_PlayerArm::CWeapon_PlayerArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_PlayerArm::CWeapon_PlayerArm(const CWeapon_PlayerArm& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_PlayerArm::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_PlayerArm::Initialize(void* pArg)
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

void CWeapon_PlayerArm::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

}

void CWeapon_PlayerArm::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Update(fTimeDelta);

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_PlayerArm::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CWeapon_PlayerArm::Render()
{
	//if (FAILED(__super::Render()))
	//	return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
	return S_OK;
}

HRESULT CWeapon_PlayerArm::Render_LightDepth()
{
	//if (FAILED(__super::Render_LightDepth()))
	//	return E_FAIL;

	return S_OK;
}

void CWeapon_PlayerArm::OnCollisionEnter(CGameObject* pOther)
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
			CMonster* pMonster = dynamic_cast<CMonster*>(pOther);

			m_DamagedObjects.push_back(pOther);
			if (pMonster->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio))
			{
				// 사우드 및 각종 이펙트
			}
		}
	}
}

void CWeapon_PlayerArm::OnCollisionStay(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Monster"))
	{

	}
}

void CWeapon_PlayerArm::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Monster"))
	{

	}
}

void CWeapon_PlayerArm::Play_HitSound(HIT_TYPE eType)
{
	
}

HRESULT CWeapon_PlayerArm::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.1f, 0.1f, 0.53f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.5f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CWeapon_PlayerArm* CWeapon_PlayerArm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_PlayerArm* pInstance = new CWeapon_PlayerArm(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_PlayerArm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_PlayerArm::Clone(void* pArg)
{
	CWeapon_PlayerArm* pInstance = new CWeapon_PlayerArm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_PlayerArm"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_PlayerArm::Free()
{
	__super::Free();
}
