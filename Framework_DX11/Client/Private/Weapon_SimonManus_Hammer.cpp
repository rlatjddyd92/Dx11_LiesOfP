#include "stdafx.h"
#include "Weapon_SimonManus_Hammer.h"

#include "Player.h"

#include "GameInstance.h"

CWeapon_SimonManus_Hammer::CWeapon_SimonManus_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_SimonManus_Hammer::CWeapon_SimonManus_Hammer(const CWeapon_SimonManus_Hammer& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_SimonManus_Hammer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_SimonManus_Hammer::Initialize(void* pArg)
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

	*m_pParentAtk = 200.f;
	m_strObjectTag = TEXT("MonsterWeapon");

	m_fDamageAmount = 20.f;

	return S_OK;
}

void CWeapon_SimonManus_Hammer::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

}

void CWeapon_SimonManus_Hammer::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);

	if (!m_bStoppedSpin)
	{
		if (m_pModelCom->Get_IsEndAnimArray()[0])
		{
			m_pModelCom->SetUp_NextAnimation(1, true, 0.f, 0);
			m_bStoppedSpin = true;
		}
	}

	m_pColliderCom->Update(&m_WorldMatrix);
	m_pExtraColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_SimonManus_Hammer::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_ColliderList(m_pExtraColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CWeapon_SimonManus_Hammer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	if (nullptr != m_pExtraColliderCom)
		m_pExtraColliderCom->Render();
#endif
	return S_OK;
}

HRESULT CWeapon_SimonManus_Hammer::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

void CWeapon_SimonManus_Hammer::OnCollisionEnter(CGameObject* pOther)
{
}

void CWeapon_SimonManus_Hammer::OnCollisionStay(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Player"))
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
			_Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, vPos, HIT_METAL, m_iAtkStrength);
		}
	}
}

void CWeapon_SimonManus_Hammer::OnCollisionExit(CGameObject* pOther)
{
}

void CWeapon_SimonManus_Hammer::ChangeAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration, _int iStartFrame, _bool bEitherChange, _bool bSameChange)
{
	m_pModelCom->SetUp_NextAnimation(iAnimIndex, isLoop, fChangeDuration, iStartFrame, bEitherChange, bSameChange);
	if (iAnimIndex >= 2)
	{
		m_bStoppedSpin = false;
	}
}

_bool CWeapon_SimonManus_Hammer::is_EndAnim(_int iAnimIndex)
{
	return m_pModelCom->Get_IsEndAnimArray()[iAnimIndex];
}

_bool CWeapon_SimonManus_Hammer::Active_Collider(_float fDamageRatio, _uint iHandIndex, _uint iHitType, _uint iAtkStrength)
{
	if (m_pColliderCom->IsActive())
		return false;

	m_fDamageRatio = fDamageRatio;
	m_pColliderCom->IsActive(true);
	m_iHitType = iHitType;
	m_iAtkStrength = iAtkStrength;
	m_DamagedObjects.clear();

	if (iHandIndex == 1)
	{
		if (m_pExtraColliderCom->IsActive())
			return false;

		m_pExtraColliderCom->IsActive(true);
	}
	
	return true;
}

void CWeapon_SimonManus_Hammer::DeActive_Collider(_uint iHandIndex)
{
	if (!m_pColliderCom->IsActive())
		return;

	m_pColliderCom->IsActive(false);

	if (!m_pExtraColliderCom->IsActive())
		return;

	m_pExtraColliderCom->IsActive(false);

}

HRESULT CWeapon_SimonManus_Hammer::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_SimonManus_Hammer"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(1.f, 1.f, 4.f);
	ColliderDesc.vCenter = _float3(0.f, 0.0f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);


	//m_pExtraColliderCom	
	ColliderDesc.vExtents = _float3(3.5f, 0.8f, 0.8f);
	ColliderDesc.vCenter = _float3(-2.f, 0.0f, -0.8f);
	ColliderDesc.vAngles = _float3(0.f, -0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_ExtraCollider"), reinterpret_cast<CComponent**>(&m_pExtraColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pExtraColliderCom->Set_Owner(this);

	return S_OK;
}

CWeapon_SimonManus_Hammer* CWeapon_SimonManus_Hammer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_SimonManus_Hammer* pInstance = new CWeapon_SimonManus_Hammer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_SimonManus_Hammer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_SimonManus_Hammer::Clone(void* pArg)
{
	CWeapon_SimonManus_Hammer* pInstance = new CWeapon_SimonManus_Hammer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_SimonManus_Hammer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_SimonManus_Hammer::Free()
{
	__super::Free();
	Safe_Release(m_pExtraColliderCom);

}
