#include "stdafx.h"
#include "Weapon_RebornerBigA_Stick.h"

#include "Monster.h"

#include "GameInstance.h"

CWeapon_RebornerBigA_Stick::CWeapon_RebornerBigA_Stick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_RebornerBigA_Stick::CWeapon_RebornerBigA_Stick(const CWeapon_RebornerBigA_Stick& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_RebornerBigA_Stick::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_RebornerBigA_Stick::Initialize(void* pArg)
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

	*m_pParentAtk = 160.f;
	m_strObjectTag = TEXT("MonsterWeapon");

	m_fDamageAmount = 160.f;
	m_fDamageAmount = 10.f;

	return S_OK;
}

void CWeapon_RebornerBigA_Stick::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

}

void CWeapon_RebornerBigA_Stick::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;



	__super::Update(fTimeDelta);


	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_RebornerBigA_Stick::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CWeapon_RebornerBigA_Stick::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_RebornerBigA_Stick::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

void CWeapon_RebornerBigA_Stick::OnCollisionEnter(CGameObject* pOther)
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

void CWeapon_RebornerBigA_Stick::OnCollisionStay(CGameObject* pOther)
{
}

void CWeapon_RebornerBigA_Stick::OnCollisionExit(CGameObject* pOther)
{
}

void CWeapon_RebornerBigA_Stick::ChangeAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration, _int iStartFrame, _bool bEitherChange, _bool bSameChange)
{
}

_bool CWeapon_RebornerBigA_Stick::is_EndAnim(_int iAnimIndex)
{
	return m_pModelCom->Get_IsEndAnimArray()[iAnimIndex];
}

HRESULT CWeapon_RebornerBigA_Stick::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_RebornerBigA_Stick"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.8f, 0.15f, 0.15f);
	ColliderDesc.vCenter = _float3(0.6f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CWeapon_RebornerBigA_Stick* CWeapon_RebornerBigA_Stick::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_RebornerBigA_Stick* pInstance = new CWeapon_RebornerBigA_Stick(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_RebornerBigA_Stick"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_RebornerBigA_Stick::Clone(void* pArg)
{
	CWeapon_RebornerBigA_Stick* pInstance = new CWeapon_RebornerBigA_Stick(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_RebornerBigA_Stick"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_RebornerBigA_Stick::Free()
{
	__super::Free();
}
