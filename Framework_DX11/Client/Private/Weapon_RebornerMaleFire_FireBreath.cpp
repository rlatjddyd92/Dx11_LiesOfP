#include "stdafx.h"
#include "Weapon_RebornerMaleFire_FireBreath.h"

#include "Monster.h"
#include "Player.h"

#include "GameInstance.h"

CWeapon_RebornerMaleFire_FireBreath::CWeapon_RebornerMaleFire_FireBreath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_RebornerMaleFire_FireBreath::CWeapon_RebornerMaleFire_FireBreath(const CWeapon_RebornerMaleFire_FireBreath& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_RebornerMaleFire_FireBreath::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_RebornerMaleFire_FireBreath::Initialize(void* pArg)
{
	MONSTER_WAPON_DESC* pDesc = static_cast<MONSTER_WAPON_DESC*>(pArg);
	m_pMonster = pDesc->pMonster;
	if (nullptr == m_pMonster)
		return E_FAIL;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	if (m_pParentAtk == nullptr)
	{
		return E_FAIL;
	}

	*m_pParentAtk = 20.f;
	m_strObjectTag = TEXT("MonsterWeapon");

	m_fDamageAmount = 20.f;
	m_fDamageAmount = 10.f;
	m_fAttackRange = 0.f;
	return S_OK;
}

void CWeapon_RebornerMaleFire_FireBreath::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

}

void CWeapon_RebornerMaleFire_FireBreath::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Update(fTimeDelta);

	if (m_bRangeCtl)
	{
		if (m_fAttackRange < 3.5f)
		{
			m_fAttackRange += fTimeDelta * 5.f;
		}

		if (m_fResetTimer >= m_fResetTime)
		{
			m_fResetTimer -= m_fResetTime;
			Reset_OverLapCheck();
		}
		else
		{
			m_fResetTimer += fTimeDelta;
		}
	}
	//CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	//ColliderDesc.vExtents = _float3(m_fAttackRange, 1.4f, 1.2f);
	//ColliderDesc.vCenter = _float3(m_fAttackRange, 0.f, -1.2f);
	//ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
	//
	//m_pColliderCom->Change_BoundingDesc(&ColliderDesc);

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_RebornerMaleFire_FireBreath::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	//m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CWeapon_RebornerMaleFire_FireBreath::Render()
{
	//if (FAILED(__super::Render()))
	//	return E_FAIL;

#ifdef _DEBUG
	if (m_pColliderCom->IsActive())
	{
		m_pColliderCom->Render();
	}
#endif

	return S_OK;
}

HRESULT CWeapon_RebornerMaleFire_FireBreath::Render_LightDepth()
{
	//if (FAILED(__super::Render_LightDepth()))
	//	return E_FAIL;

	return S_OK;
}

void CWeapon_RebornerMaleFire_FireBreath::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Player"))
	{
		_float fTemp = m_pMonster->Calc_Distance_XZ();
		if (fTemp > m_fAttackRange * 2.f)
		{
			return;
		}

		static_cast<CPlayer*>(pOther)->Damaged(5.f);
	}
}

void CWeapon_RebornerMaleFire_FireBreath::OnCollisionStay(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Player"))
	{
		_float fTemp = m_pMonster->Calc_Distance_XZ();
		if (fTemp > m_fAttackRange * 2.f)
		{
			return;
		}

		static_cast<CPlayer*>(pOther)->Damaged(5.f);

	}
}

void CWeapon_RebornerMaleFire_FireBreath::OnCollisionExit(CGameObject* pOther)
{
}

void CWeapon_RebornerMaleFire_FireBreath::ChangeAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration, _int iStartFrame, _bool bEitherChange, _bool bSameChange)
{
}

_bool CWeapon_RebornerMaleFire_FireBreath::is_EndAnim(_int iAnimIndex)
{
	return false;
}

_bool CWeapon_RebornerMaleFire_FireBreath::Active_Collider(_float fDamageRatio, _uint iHandIndex, HIT_TYPE eHitType, ATTACK_STRENGTH eAtkStrength)
{
	if (m_pColliderCom->IsActive())
		return false;

	m_fDamageRatio = fDamageRatio;
	m_pColliderCom->IsActive(true);
	m_eHitType = eHitType;
	m_eAttackStrength = eAtkStrength;
	m_DamagedObjects.clear();
	m_bRangeCtl = true;

	return true;
}

void CWeapon_RebornerMaleFire_FireBreath::DeActive_Collider(_uint iHandIndex)
{
	if (!m_pColliderCom->IsActive())
		return;
	
	m_bRangeCtl = false;
	m_fAttackRange = 0.f;

	m_pColliderCom->IsActive(false);
}

HRESULT CWeapon_RebornerMaleFire_FireBreath::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(3.5f, 1.4f, 1.2f);
	ColliderDesc.vCenter = _float3(3.5f, 0.f, -1.2f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CWeapon_RebornerMaleFire_FireBreath* CWeapon_RebornerMaleFire_FireBreath::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_RebornerMaleFire_FireBreath* pInstance = new CWeapon_RebornerMaleFire_FireBreath(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_RebornerMaleFire_FireBreath"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_RebornerMaleFire_FireBreath::Clone(void* pArg)
{
	CWeapon_RebornerMaleFire_FireBreath* pInstance = new CWeapon_RebornerMaleFire_FireBreath(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_RebornerMaleFire_FireBreath"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_RebornerMaleFire_FireBreath::Free()
{
	__super::Free();
}
