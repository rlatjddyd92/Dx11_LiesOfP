#include "stdafx.h"

#include "GameInstance.h"
#include "Client_Defines.h"
#include "ColliderObject.h"

CColliderObject::CColliderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CColliderObject::CColliderObject(const CColliderObject& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CColliderObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CColliderObject::Initialize(void* pArg)
{

	COLIDEROBJECT_DESC* pDesc = static_cast<COLIDEROBJECT_DESC*>(pArg);
	m_eType = pDesc->eType;
	m_pOwner = pDesc->pOWner;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
	
	if(FAILED(Ready_Components(pDesc->pBoundingDesc, pDesc->eType)))
		return E_FAIL;

	m_fDamageAmount = pDesc->fDamageAmount;
	m_pColliderCom->IsActive(false);
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

void CColliderObject::Priority_Update(_float fTimeDelta)
{
}

void CColliderObject::Update(_float fTimeDelta)
{
	
}

void CColliderObject::Late_Update(_float fTimeDelta)
{
	//if (!m_pColliderCom->IsActive())
	//	return;

	XMStoreFloat4x4(&m_WorldMatrix, (*m_pSocketMatrix) * (*m_pParentMatrix));

	m_pColliderCom->Update(&m_WorldMatrix);

	if (nullptr != m_pColliderCom)
		m_pGameInstance->Add_ColliderList(m_pColliderCom);


#ifdef _DEBUG
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
#endif
}

HRESULT CColliderObject::Render()
{
	
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CColliderObject::OnCollisionEnter(CGameObject* pOther)
{
}

void CColliderObject::OnCollisionStay(CGameObject* pOther)
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
			pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, m_WorldMatrix.Translation(), m_eHitType, m_eAttackStrength, (CGameObject*)m_pOwner);
		}
	}
}

void CColliderObject::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CColliderObject::Ready_Components(CBounding::BOUNDING_DESC* pBoundingDesc, CCollider::TYPE eType)
{

	if (eType == CCollider::TYPE_AABB)
	{
		CBounding_AABB::BOUNDING_AABB_DESC* pColliderAABBDesc = static_cast<CBounding_AABB::BOUNDING_AABB_DESC*>(pBoundingDesc);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), pColliderAABBDesc)))
			return E_FAIL;
	}
	else if (eType == CCollider::TYPE_OBB)
	{
		CBounding_OBB::BOUNDING_OBB_DESC* pColliderOBBDesc = static_cast<CBounding_OBB::BOUNDING_OBB_DESC*>(pBoundingDesc);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), pColliderOBBDesc)))
			return E_FAIL;
	}
	else if (eType == CCollider::TYPE_SPHERE)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC* pColliderSphereDesc = static_cast<CBounding_Sphere::BOUNDING_SPHERE_DESC*>(pBoundingDesc);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), pColliderSphereDesc)))
			return E_FAIL;
	}
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CColliderObject* CColliderObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CColliderObject* pInstance = new CColliderObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CColliderObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CColliderObject::Clone(void* pArg)
{
	CColliderObject* pInstance = new CColliderObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CColliderObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColliderObject::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
