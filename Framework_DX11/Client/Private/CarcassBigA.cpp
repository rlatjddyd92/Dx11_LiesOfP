#include "stdafx.h"
#include "..\Public\CarcassBigA.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"


#include "State_CarcassBigA_Idle.h"
#include "State_CarcassBigA_Die.h"
#include "State_CarcassBigA_Attack.h"
#include "State_CarcassBigA_Impact.h"

#include "State_CarcassBigA_Grogy.h"
#include "State_CarcassBigA_HitFatal.h"
#include "State_CarcassBigA_Paralyze.h"

#include "State_CarcassBigA_Walk.h"
#include "State_CarcassBigA_RUN.h"

CCarcassBigA::CCarcassBigA(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster{ pDevice, pContext }
{
}

CCarcassBigA::CCarcassBigA(const CCarcassBigA& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CCarcassBigA::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarcassBigA::Initialize(void* pArg)
{
  	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(rand() % 20, true);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });

	return S_OK;
}

void CCarcassBigA::Priority_Update(_float fTimeDelta)
{

	if (KEY_TAP(KEY::Z))
	{
		Change_State(CCarcassBigA::GROGY);
	}

	if (KEY_TAP(KEY::C))
	{
		Change_State(CCarcassBigA::PARALYZE);
	}

	if (KEY_TAP(KEY::V))
	{
		Change_State(CCarcassBigA::DIE);
	}
}

void CCarcassBigA::Update(_float fTimeDelta)
{


	m_vCurRootMove = m_pModelCom->Play_Animation(fTimeDelta, &m_bEndAnim, nullptr);

	m_pFSMCom->Update(fTimeDelta);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_bEndAnim == true && m_bResetRootMove)//조건을 애니메이션이 끝났을때 or 변경 되었을때로
	{
		m_vCurRootMove = m_vRootMoveStack = XMVectorSet(0, 0, 0, 1);
	}
	else
	{
		m_vCurRootMove = XMVector3TransformNormal(m_vCurRootMove, m_pTransformCom->Get_WorldMatrix());

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + m_vCurRootMove - m_vRootMoveStack);
		m_vRootMoveStack = m_vCurRootMove;
	}


	for (auto& pColliderObj : m_pColliderObject)
		pColliderObj->Update(fTimeDelta);
	
#ifdef _DEBUG
	for (auto& pCollider : m_pColliderCom)
		pCollider->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
#endif
}

void CCarcassBigA::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	/*CComponent*		pTargetCollider = m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"));
	if (nullptr == pTargetCollider)
		return;*/

		//CComponent*		pTargetCollider = m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0, CPlayer::PART_WEAPON);
		//if (nullptr == pTargetCollider)
		//	return;

	for (size_t i = 0; i < TYPE_END; i++)
	{
		//m_pColliderCom[i]->Intersect(dynamic_cast<CCollider*>(pTargetCollider));
	}
	

	//_vector vFixPos = m_pGameInstance->Compute_Height(vPosition, XMMatrixLookAtLH(XMVectorSet(64.f, 30.f, 64.0f, 1.f), XMVectorSet(64.f, 0.f, 64.0f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f)),
	//	XMMatrixOrthographicLH(200.f, 200.f, 0.f, 50.f));

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, XMVectorGetY(vFixPos)));

	/*
	if (true == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
		for (auto& pCollider : m_pColliderCom)
			m_pGameInstance->Add_DebugObject(pCollider);
#endif
	}
	*/
	for (auto& pColliderObj : m_pColliderObject)
		pColliderObj->Late_Update(fTimeDelta);

}

HRESULT CCarcassBigA::Render()
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



	return S_OK;
}

HRESULT CCarcassBigA::Ready_Components()
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
		TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom[TYPE_AABB]), &ColliderAABBDesc)))
		return E_FAIL;

	/* FOR.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc{};
	ColliderOBBDesc.vExtents = _float3(0.7f, 0.7f, 0.7f);
	ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vExtents.y, 0.f);
	ColliderOBBDesc.vAngles = _float3(0.f, m_pGameInstance->Get_Random(XMConvertToRadians(0.f), XMConvertToRadians(360.f)), 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom[TYPE_OBB]), &ColliderOBBDesc)))
		return E_FAIL;

	/* FOR.Com_Collider_Sphere */
	CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderSphereDesc{};
	ColliderSphereDesc.fRadius = 1.2f;
	ColliderSphereDesc.vCenter = _float3(0.f, ColliderSphereDesc.fRadius, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom[TYPE_SPHERE]), &ColliderSphereDesc)))
		return E_FAIL;


	/* FOR.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc_Obj{};
	ColliderOBBDesc.vExtents = _float3(0.7f, 0.7f, 0.9f);
	ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vExtents.y, 0.f);
	ColliderOBBDesc.vAngles = _float3(0.f, m_pGameInstance->Get_Random(XMConvertToRadians(0.f), XMConvertToRadians(360.f)), 0.f);

	CColliderObject::COLIDEROBJECT_DESC Desc{};

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pCombinedTransform = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT));
	Desc.pParentTransformCom = m_pTransformCom;

	m_pColliderObject[COLLIDERTYPE::TYPE_LEFTHAND] = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc);


	/* FOR.Com_Collider_OBB */
	
	ColliderOBBDesc.vExtents = _float3(0.7f, 0.7f, 0.9f);
	ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vExtents.y, 0.f);
	ColliderOBBDesc.vAngles = _float3(0.f, m_pGameInstance->Get_Random(XMConvertToRadians(0.f), XMConvertToRadians(360.f)), 0.f);

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pCombinedTransform = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT));
	Desc.pParentTransformCom = m_pTransformCom;

	m_pColliderObject[COLLIDERTYPE::TYPE_RIGHTHAND] = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc);



	return S_OK;
}

HRESULT CCarcassBigA::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSMSTATE_DESC Desc{};
	
	Desc.pIsEndAnim = &m_bEndAnim;
	Desc.pIsResetRootMove =&m_bResetRootMove;
	Desc.pGrogyTrackPos = &m_GrogyTrackPos;
	//


	m_pFSMCom->Add_State(CState_CarcassBigA_Idle::Create(m_pFSMCom, this, IDLE, &Desc));
	m_pFSMCom->Add_State(CState_CarcassBigA_Walk::Create(m_pFSMCom, this, WALK, &Desc));
	m_pFSMCom->Add_State(CState_CarcassBigA_Run::Create(m_pFSMCom, this, RUN, &Desc));
	m_pFSMCom->Add_State(CState_CarcassBigA_Attack::Create(m_pFSMCom, this, ATTACK, &Desc));
	m_pFSMCom->Add_State(CState_CarcassBigA_Grogy::Create(m_pFSMCom, this, GROGY, &Desc));
	m_pFSMCom->Add_State(CState_CarcassBigA_HitFatal::Create(m_pFSMCom, this, HITFATAL, &Desc));
	m_pFSMCom->Add_State(CState_CarcassBigA_Paralyze::Create(m_pFSMCom, this, PARALYZE, &Desc));
	m_pFSMCom->Add_State(CState_CarcassBigA_Die::Create(m_pFSMCom, this, DIE, &Desc));

	m_pFSMCom->Add_State(CState_CarcassBigA_Impact::Create(m_pFSMCom, this, IMPACT, &Desc));


	m_pFSMCom->Set_State(IDLE);

	return S_OK;



}

CCarcassBigA* CCarcassBigA::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarcassBigA* pInstance = new CCarcassBigA(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CCarcassBigA::Clone(void* pArg)
{
	CCarcassBigA* pInstance = new CCarcassBigA(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarcassBigA::Free()
{
	__super::Free();

}