#include "stdafx.h"
#include "Weapon_Raxasia_P2_Sword.h"

#include "Monster.h"

#include "GameInstance.h"

CWeapon_Raxasia_P2_Sword::CWeapon_Raxasia_P2_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Raxasia_P2_Sword::CWeapon_Raxasia_P2_Sword(const CWeapon_Raxasia_P2_Sword& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_Raxasia_P2_Sword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Raxasia_P2_Sword::Initialize(void* pArg)
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

void CWeapon_Raxasia_P2_Sword::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

}

void CWeapon_Raxasia_P2_Sword::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Update(fTimeDelta);


	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_Raxasia_P2_Sword::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CWeapon_Raxasia_P2_Sword::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Raxasia_P2_Sword::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

void CWeapon_Raxasia_P2_Sword::OnCollisionEnter(CGameObject* pOther)
{
}

void CWeapon_Raxasia_P2_Sword::OnCollisionStay(CGameObject* pOther)
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

void CWeapon_Raxasia_P2_Sword::OnCollisionExit(CGameObject* pOther)
{
}

void CWeapon_Raxasia_P2_Sword::ChangeAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration, _int iStartFrame, _bool bEitherChange, _bool bSameChange)
{
}

_bool CWeapon_Raxasia_P2_Sword::is_EndAnim(_int iAnimIndex)
{
	return m_pModelCom->Get_IsEndAnimArray()[iAnimIndex];
}

HRESULT CWeapon_Raxasia_P2_Sword::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP2_Sword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.6f, 0.3f, 2.25f);
	ColliderDesc.vCenter = _float3(0.f, 0.0f, -0.95f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CWeapon_Raxasia_P2_Sword* CWeapon_Raxasia_P2_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Raxasia_P2_Sword* pInstance = new CWeapon_Raxasia_P2_Sword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Raxasia_P2_Sword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Raxasia_P2_Sword::Clone(void* pArg)
{
	CWeapon_Raxasia_P2_Sword* pInstance = new CWeapon_Raxasia_P2_Sword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Raxasia_P2_Sword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Raxasia_P2_Sword::Free()
{
	__super::Free();
}
