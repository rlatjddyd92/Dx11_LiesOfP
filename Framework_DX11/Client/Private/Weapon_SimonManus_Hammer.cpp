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
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	if (m_pParentAtk == nullptr)
	{
		return E_FAIL;
	}

	*m_pParentAtk = 5.f;

	//m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);
	////m_pTransformCom->Rotation(0.0f, XMConvertToRadians(90.0f), 0.1f);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(3.f, 0.f, 3.f, 1.f));

	m_strObjectTag = TEXT("MonsterWeapon");

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

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_SimonManus_Hammer::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CWeapon_SimonManus_Hammer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

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
			pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio);
		}
	}
}

void CWeapon_SimonManus_Hammer::OnCollisionStay(CGameObject* pOther)
{
}

void CWeapon_SimonManus_Hammer::OnCollisionExit(CGameObject* pOther)
{
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
	ColliderDesc.vExtents = _float3(1.f, 1.f, 1.4f);
	ColliderDesc.vCenter = _float3(0.f, 0.0f, -1.9f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

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

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
