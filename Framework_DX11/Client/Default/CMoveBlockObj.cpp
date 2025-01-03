#include "stdafx.h"
#include "CMoveBlockObj.h"
#include "GameInstance.h"

CMoveBlockObj::CMoveBlockObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMoveBlockObj::CMoveBlockObj(const CMoveBlockObj& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CMoveBlockObj::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMoveBlockObj::Initialize(void* pArg)
{
	MOVEBLOCK_DESC* pDesc = static_cast<MOVEBLOCK_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	switch (pDesc->iTypeNum)
	{
	case 0:	//RAXASIA1
		m_pTransformCom->Rotation(0.f, 60.f, 0.f);
		break;
	case 1:	//RAXASIA2
		m_pTransformCom->Rotation(0.f, 60.f, 0.f);
		break;
	case 2:	//MANUS1
		m_pTransformCom->Rotation(0.f, 133, 0.f);
		break;
	default:
		break;
	}
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

	switch (pDesc->iTypeNum)
	{
	case 0:	//RAXASIA1
		m_pRigidBodyCom->Set_GloblePose(_Vec3(-137.541f, -97.816f, -68.452f));
		break;
	case 1:	//RAXASIA2
		m_pRigidBodyCom->Set_GloblePose(_Vec3(-39.413f, -97.809f, -68.452f));
		break;
	case 2:	//MANUS1
		m_pRigidBodyCom->Set_GloblePose(_Vec3(16.641f, 0.069f,-16.234f));
		break;
	default:
		break;
	}

	return S_OK;
}

void CMoveBlockObj::Priority_Update(_float fTimeDelta)
{
}

void CMoveBlockObj::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CMoveBlockObj::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
    if (m_pColliderCom != nullptr)
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CMoveBlockObj::Render()
{
    return S_OK;
}

void CMoveBlockObj::OnCollisionEnter(CGameObject* pOther)
{
}

void CMoveBlockObj::OnCollisionStay(CGameObject* pOther)
{
}

void CMoveBlockObj::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CMoveBlockObj::Ready_Components(MOVEBLOCK_DESC* pDesc)
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(10.f, 3.f, 0.5f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(ColliderDesc.vExtents.x * 0.5f, ColliderDesc.vExtents.y*0.5f, ColliderDesc.vExtents.z * 0.5f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	// 항상 마지막에 생성하기
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc{};
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerTransform = m_pTransformCom;
	RigidBodyDesc.pOwnerNavigation = nullptr;

	RigidBodyDesc.pOwner = this;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestituion = 0.f;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z |
		PxRigidDynamicLockFlag::eLOCK_LINEAR_X |
		PxRigidDynamicLockFlag::eLOCK_LINEAR_Y |
		PxRigidDynamicLockFlag::eLOCK_LINEAR_Z
		;
	RigidBodyDesc.vOffset = _Vec3(0.f, ColliderDesc.vExtents.y * 0.5f, 0.f);
	physX::GeometryBox BoxDesc;
	BoxDesc.vSize = _Vec3(ColliderDesc.vExtents.x, ColliderDesc.vExtents.y, ColliderDesc.vExtents.z);
	RigidBodyDesc.pGeometry = &BoxDesc;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;

	return S_OK;
}

CMoveBlockObj* CMoveBlockObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMoveBlockObj* pInstance = new CMoveBlockObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMoveBlockObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMoveBlockObj::Clone(void* pArg)
{
	CMoveBlockObj* pInstance = new CMoveBlockObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMoveBlockObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoveBlockObj::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRigidBodyCom);
}
