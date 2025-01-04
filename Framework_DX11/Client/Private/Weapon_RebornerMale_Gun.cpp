#include "stdafx.h"
#include "Weapon_RebornerMale_Gun.h"

#include "Monster.h"

#include "GameInstance.h"

CWeapon_RebornerMale_Gun::CWeapon_RebornerMale_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_RebornerMale_Gun::CWeapon_RebornerMale_Gun(const CWeapon_RebornerMale_Gun& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_RebornerMale_Gun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_RebornerMale_Gun::Initialize(void* pArg)
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

void CWeapon_RebornerMale_Gun::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

}

void CWeapon_RebornerMale_Gun::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;



	__super::Update(fTimeDelta);


}

void CWeapon_RebornerMale_Gun::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CWeapon_RebornerMale_Gun::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_RebornerMale_Gun::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

void CWeapon_RebornerMale_Gun::OnCollisionEnter(CGameObject* pOther)
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
			pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, vPos, HIT_METAL, m_eAttackStrength);
		}
	}
}

void CWeapon_RebornerMale_Gun::OnCollisionStay(CGameObject* pOther)
{
}

void CWeapon_RebornerMale_Gun::OnCollisionExit(CGameObject* pOther)
{
}

void CWeapon_RebornerMale_Gun::ChangeAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration, _int iStartFrame, _bool bEitherChange, _bool bSameChange)
{
}

_bool CWeapon_RebornerMale_Gun::is_EndAnim(_int iAnimIndex)
{
	return m_pModelCom->Get_IsEndAnimArray()[iAnimIndex];
}

HRESULT CWeapon_RebornerMale_Gun::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_RebornerMale_Gun"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CWeapon_RebornerMale_Gun* CWeapon_RebornerMale_Gun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_RebornerMale_Gun* pInstance = new CWeapon_RebornerMale_Gun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_RebornerMale_Gun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_RebornerMale_Gun::Clone(void* pArg)
{
	CWeapon_RebornerMale_Gun* pInstance = new CWeapon_RebornerMale_Gun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_RebornerMale_Gun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_RebornerMale_Gun::Free()
{
	__super::Free();
}
