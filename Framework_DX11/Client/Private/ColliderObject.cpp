#include "stdafx.h"

#include "GameInstance.h"
#include "Client_Defines.h"
#include "ColliderObject.h"

CColliderObject::CColliderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CColliderObject::CColliderObject(const CColliderObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CColliderObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CColliderObject::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		Desc{};

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	COLIDEROBJECT_DESC* pDesc = static_cast<COLIDEROBJECT_DESC*>(pArg);
	m_eType = pDesc->eType;
	m_pParentTransformComMatrix = pDesc->pParentTransformComMatrix;
	
	m_pCombinedBoneTransformMatrix = pDesc->pCombinedBoneTransformMatrix;
	
	
	if(FAILED(Ready_Components(pDesc->pBoundingDesc, pDesc->eType)))
		return E_FAIL;

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
	__super::Late_Update(fTimeDelta);
	_float4x4 WorldMatrix;
	// 
	XMStoreFloat4x4(&WorldMatrix, XMLoadFloat4x4(m_pCombinedBoneTransformMatrix) * XMLoadFloat4x4(m_pParentTransformComMatrix));

	m_pColliderCom->Update(&WorldMatrix);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
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

HRESULT CColliderObject::Ready_Components(CBounding::BOUNDING_DESC* pBoundingDesc, CCollider::TYPE eType)
{

	if (eType == CCollider::TYPE_AABB)
	{
		CBounding_AABB::BOUNDING_AABB_DESC* pColliderAABBDesc = static_cast<CBounding_AABB::BOUNDING_AABB_DESC*>(pBoundingDesc);

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), pColliderAABBDesc)))
			return E_FAIL;
	}
	else if (eType == CCollider::TYPE_OBB)
	{
		CBounding_OBB::BOUNDING_OBB_DESC* pColliderOBBDesc = static_cast<CBounding_OBB::BOUNDING_OBB_DESC*>(pBoundingDesc);

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom), pColliderOBBDesc)))
			return E_FAIL;
	}
	else if (eType == CCollider::TYPE_SPHERE)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC* pColliderSphereDesc = static_cast<CBounding_Sphere::BOUNDING_SPHERE_DESC*>(pBoundingDesc);

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), pColliderSphereDesc)))
			return E_FAIL;
	}


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
