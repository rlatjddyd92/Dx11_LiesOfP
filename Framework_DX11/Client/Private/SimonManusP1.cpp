#include "stdafx.h"
#include "..\Public\SimonManusP1.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

//전부 수정하기
#include "State_SimonManusP1_Idle.h"
#include "State_SimonManusP1_Die.h"
#include "State_SimonManusP1_StingAttack.h"
#include "State_SimonManusP1_UpperAttack.h"
//#include "State_SimonManus_UpperAttack.h"
//#include "State_SimonManus_UpperAttack.h"
//#include "State_SimonManus_UpperAttack.h"

#include "State_SimonManusP1_Grogy.h"
#include "State_SimonManusP1_HitFatal.h"

#include "State_SimonManusP1_Walk.h"
#include "State_SimonManusP1_RUN.h"

CSimonManusP1::CSimonManusP1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster{ pDevice, pContext }
{
}

CSimonManusP1::CSimonManusP1(const CSimonManusP1& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CSimonManusP1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSimonManusP1::Initialize(void* pArg)
{
  	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(rand() % 20, true);

	if (FAILED(Ready_FSM()))
		return E_FAIL;


	return S_OK;
}

void CSimonManusP1::Priority_Update(_float fTimeDelta)
{

	
}

void CSimonManusP1::Update(_float fTimeDelta)
{


	{
		//애니메이션 재생 및 루트 본 움직임 제어
		m_vCurRootMove = m_pModelCom->Play_Animation(fTimeDelta, &m_bEndAnim, nullptr);

		//진행한 애니메이션 정도를 통해서 상태 제어(애니메이션이 끝나면 넘어가도록)
		//m_pFSMCom->Update(fTimeDelta);

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (m_bEndAnim == true && m_bResetRootMove)//조건을 애니메이션이 끝났을때 or 변경 되었을때로
		{
			m_vCurRootMove = m_vRootMoveStack = XMVectorSet(0, 0, 0, 1);
		}
		else
		{
			m_vCurRootMove = XMVector3TransformNormal(m_vCurRootMove, m_pTransformCom->Get_WorldMatrix());

			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + m_vCurRootMove - m_vRootMoveStack);
			m_vRootMoveStack = m_vCurRootMove;
		}
	}

	for (auto& pColliderObj : m_pColliderObject)
		pColliderObj->Update(fTimeDelta);

	//for (auto& pCollider : m_pColliderCom)
	//	pCollider->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CSimonManusP1::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	for (_uint i = 0; i < TYPE_END; ++i)
	{
		if (m_bColliderCtrs[i] != true)
		{
			m_pColliderObject[i]->Late_Update(fTimeDelta);
		}
	}

}

HRESULT CSimonManusP1::Render()
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
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", DIFFUSE, (_uint)i)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}


#ifdef _DEBUG
	//m_pColliderCom[1]->Render();
	//for (auto& pColliderObj : m_pColliderObject)
	//	pColliderObj->Render();
#endif
	return S_OK;
}

HRESULT CSimonManusP1::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CarcassBigA"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderAABBDesc{};
	ColliderAABBDesc.vExtents = _float3(0.5f, 1.0f, 0.5f);
	ColliderAABBDesc.vCenter = _float3(0.f, ColliderAABBDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom[CCollider::TYPE_AABB]), &ColliderAABBDesc)))
		return E_FAIL;

	/* FOR.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc{};
	ColliderOBBDesc.vExtents = _float3(0.7f, 0.7f, 0.7f);
	ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vExtents.y, 0.f);
	ColliderOBBDesc.vAngles = _float3(0.f, m_pGameInstance->Get_Random(XMConvertToRadians(0.f), XMConvertToRadians(360.f)), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom[CCollider::TYPE_OBB]), &ColliderOBBDesc)))
		return E_FAIL;

	/* FOR.Com_Collider_Sphere */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderSphereDesc{};
	ColliderSphereDesc.fRadius = 1.2f;
	ColliderSphereDesc.vCenter = _float3(0.f, ColliderSphereDesc.fRadius, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom[CCollider::TYPE_SPHERE]), &ColliderSphereDesc)))
		return E_FAIL;


	/* FOR.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc_Obj{};

	//ColliderAABBDesc_Obj.vExtents = _float3(0.5f, 1.0f, 0.5f);
	//ColliderAABBDesc_Obj.vCenter = _float3(0.f, ColliderAABBDesc.vExtents.y, 0.f);

	ColliderOBBDesc_Obj.vAngles = _float3(0.0f, 0.0f, 0.0f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.2f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vExtents = _float3(0.7f, 0.35f, 0.35f);

	CColliderObject::COLIDEROBJECT_DESC Desc{};

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pCombinedBoneTransformMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT));
	Desc.pParentTransformComMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	;
	m_pColliderObject[COLLIDERTYPE::TYPE_LEFTHAND] = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc);


	/* FOR.Com_Collider_OBB */
	ColliderOBBDesc_Obj.vAngles = _float3(0.0f, 0.0f, 0.0f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.2f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vExtents = _float3(0.5f, 0.3f, 0.3f);
	
	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pCombinedBoneTransformMatrix = (_Matrix*)m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT));
	Desc.pParentTransformComMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	
	m_pColliderObject[COLLIDERTYPE::TYPE_RIGHTHAND] = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc);



	return S_OK;
}

HRESULT CSimonManusP1::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSMSTATE_DESC Desc{};
	
	Desc.pIsEndAnim = &m_bEndAnim;
	Desc.pIsResetRootMove =&m_bResetRootMove;
	Desc.pColliderCtrs = m_bColliderCtrs;
	//


	//m_pFSMCom->Add_State(CState_SimonManusP1_Idle::Create(m_pFSMCom, this, IDLE, &Desc));
	//m_pFSMCom->Add_State(CState_SimonManusP1_Walk::Create(m_pFSMCom, this, WALK, &Desc));
	//m_pFSMCom->Add_State(CState_SimonManusP1_Run::Create(m_pFSMCom, this, RUN, &Desc));
	//m_pFSMCom->Add_State(CState_SimonManusP1_Grogy::Create(m_pFSMCom, this, GROGY, &Desc));
	//m_pFSMCom->Add_State(CState_SimonManusP1_HitFatal::Create(m_pFSMCom, this, HITFATAL, &Desc));
	//m_pFSMCom->Add_State(CState_SimonManusP1_Die::Create(m_pFSMCom, this, DIE, &Desc));
	//
	//m_pFSMCom->Add_State(CState_SimonManusP1_StingAttack::Create(m_pFSMCom, this, ATK_STING, &Desc));
	//m_pFSMCom->Add_State(CState_SimonManusP1_UpperAttack::Create(m_pFSMCom, this, ATK_UPPER, &Desc));


	//m_pFSMCom->Set_State(IDLE);

	return S_OK;



}

CSimonManusP1* CSimonManusP1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSimonManusP1* pInstance = new CSimonManusP1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSimonManusP1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSimonManusP1::Clone(void* pArg)
{
	CSimonManusP1* pInstance = new CSimonManusP1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSimonManusP1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSimonManusP1::Free()
{
	for (_uint i = 0; i < TYPE_END; ++i)
	{
		Safe_Release(m_pColliderObject[i]);
	}

	__super::Free();

}