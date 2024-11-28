#include "stdafx.h"
#include "Lift_Floor.h"
#include "GameInstance.h"
#include "Lift_Door.h"

CLift_Floor::CLift_Floor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLift_Floor::CLift_Floor(const CLift_Floor& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CLift_Floor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLift_Floor::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);
	_Vec3 vPos = { pDesc->vPosition.x, -97.8f, pDesc->vPosition.z};
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_bShadow = pDesc->bShadow;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	m_vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_strObjectTag = TEXT("Lift_Floor");

	return S_OK;
}

void CLift_Floor::Priority_Update(_float fTimeDelta)
{

	if (m_pDoors[0] == nullptr || m_pDoors[1] == nullptr)
	{
		for (int i = 0; i < 2; ++i)
		{
			CGameObject* pObj = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_LiftDoor"), i);

			if (pObj == nullptr)
				return;

			if (pObj->Get_Transform()->Get_State(CTransform::STATE_POSITION).y > -100.f)
				m_pDoors[UP] = static_cast<CLift_Door*>(pObj);
			else
				m_pDoors[DOWN] = static_cast<CLift_Door*>(pObj);
		}
	}
}

void CLift_Floor::Update(_float fTimeDelta)
{
	if (m_bMove)
	{
		m_isMoving = true;
		_Vec3 vVel = _Vec3(0.f, 3.f, 0.f);

		m_pRigidBodyCom->Set_Velocity(vVel * m_fMoveDir);
	}

	if (m_bMove && Calculate_Arrive_TargetPos())
	{
		m_bMove = false;
		m_isMoving = false;
		m_bArrive = true;
	}

	if (m_bArrive)
	{
		Open_Door();
	}

	if (m_bCollid)
		m_fCollisonTimer += fTimeDelta;

	if (m_fCollisonTimer > 2.f)
	{
		Close_Door();
		m_bArrive = false;
		m_fCollisonTimer = 0.f;
		m_bCollid = false;
	}

	m_pRigidBodyCom->Update(fTimeDelta);

	if(m_pColliderCom != nullptr)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CLift_Floor::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 15.f))
	{
		__super::Late_Update(fTimeDelta);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
		m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
	if (m_pColliderCom != nullptr)
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
	}
}

HRESULT CLift_Floor::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::NORMALS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", NORMALS, (_uint)i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;

	}
	return S_OK;

}

void CLift_Floor::OnCollisionEnter(CGameObject* pOther)
{
}

void CLift_Floor::OnCollisionStay(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Player"))
	{
		m_bCollid = true;
	}
}

void CLift_Floor::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Player"))
	{
		m_bCollid = false;
		m_fCollisonTimer = 0.f;
	}
}

void CLift_Floor::Set_Move_Dir(_int iDir)
{
	switch (iDir)
	{
	case UP:
		m_fMoveDir = 1.f;
		m_vTargetPos.y = m_vTargetPosList[UP];
		break;
	case DOWN:
		m_fMoveDir = -1.f;
		m_vTargetPos.y = m_vTargetPosList[DOWN];
		break;
	}

	if (m_iPreDir != iDir)
	{
		m_iPreDir = iDir;
		m_bMove = true;
		m_isMoving = true;
	}
}

HRESULT CLift_Floor::Ready_Components(OBJECT_DEFAULT_DESC* pDesc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("SM_Monastery_Lift_01_Bottom"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(2.f, 1.f, 2.f);
	ColliderDesc.vAngles = _float3(0.f, XMConvertToRadians(pDesc->vRotation.y),0.f);

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
		PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;

	physX::GeometryBox BoxDesc;
	BoxDesc.vSize = _Vec3(1.f, 0.05f, 1.f);
	RigidBodyDesc.pGeometry = &BoxDesc;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;

	return S_OK;
}

_bool CLift_Floor::Calculate_Arrive_TargetPos()
{
	_Vec4 vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (fabs(vCurPos.y - m_vTargetPos.y) < 0.1f)
	{
		_Vec3 vZero = _Vec3(0.f, 0.f, 0.f);
		m_pRigidBodyCom->Set_Velocity(vZero);

		m_pRigidBodyCom->Set_GloblePose((_Vec3)m_vTargetPos);

		return true;
	}
	else
		return false;
}

void CLift_Floor::Open_Door()
{
	switch (m_iPreDir)
	{
	case UP:
		m_pDoors[UP]->Open();
		break;
	case DOWN:
		m_pDoors[DOWN]->Open();
		break;
	default:
		break;
	}
}

void CLift_Floor::Close_Door()
{
	switch (m_iPreDir)
	{
	case UP:
		m_pDoors[UP]->Close();
		break;
	case DOWN:
		m_pDoors[DOWN]->Close();
		break;
	default:
		break;
	}

	_int iNewDir = m_iPreDir == DOWN ? UP : DOWN;
	Set_Move_Dir(iNewDir);
}

CLift_Floor* CLift_Floor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLift_Floor* pInstance = new CLift_Floor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLift_Floor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLift_Floor::Clone(void* pArg)
{
	CLift_Floor* pInstance = new CLift_Floor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLift_Floor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLift_Floor::Free()
{
	__super::Free();

	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
