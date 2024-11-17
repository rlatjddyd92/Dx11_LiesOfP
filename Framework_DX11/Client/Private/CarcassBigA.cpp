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

// 24-11-15 김성용
// 직교 UI 체크용 코드 
// 테스트 후 삭제할 것 
#include "GameInterface_Controller.h"

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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(rand() % 20, true);

	if (FAILED(Ready_FSM()))
		return E_FAIL;
	// 24-11-15 김성용
	// 직교 투영 UI 테스트를 위한 코드 
	// 메인 올릴 때는 제거 필요 
	/*_vector vPos = { 0.f,0.f,0.f,0.f };
	vPos.m128_f32[0] = m_pGameInstance->Get_Random(0.f, 10.f);
	vPos.m128_f32[1] = m_pGameInstance->Get_Random(0.f, 10.f);
	vPos.m128_f32[2] = m_pGameInstance->Get_Random(0.f, 10.f);
	vPos.m128_f32[3] = m_pGameInstance->Get_Random(0.f, 10.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		vPos);
	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });*/

	// 24-11-15 김성용
	// 직교 UI 체크용 코드 
	// 테스트 후 삭제할 것 
	/*GET_GAMEINTERFACE->Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE::ORTHO_NORMAL_MONSTER, this);

	return S_OK;*/

	// 원래 코드 (원복해야 함)
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });

	

	return S_OK;
}

void CCarcassBigA::Priority_Update(_float fTimeDelta)
{

	
}

void CCarcassBigA::Update(_float fTimeDelta)
{
	m_vCurRootMove = m_pModelCom->Play_Animation(fTimeDelta, nullptr);

	m_pFsmCom->Update(fTimeDelta);

	_Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	m_vCurRootMove = XMVector3TransformNormal(m_vCurRootMove, m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + m_vCurRootMove);
	

	//m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CCarcassBigA::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);



	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	for (_uint i = 0; i < TYPE_END; ++i)
	{
		if (m_bColliderCtrs[i] == true)
		{
			continue;
		}
		m_pColliderObject[i]->Late_Update(fTimeDelta);
	}

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


#ifdef _DEBUG
	//m_pColliderCom->Render();
	//for (auto& pColliderObj : m_pColliderObject)
	//	pColliderObj->Render();
#endif
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
	//CBounding_AABB::BOUNDING_AABB_DESC			ColliderAABBDesc{};
	//ColliderAABBDesc.vExtents = _float3(0.5f, 1.0f, 0.5f);
	//ColliderAABBDesc.vCenter = _float3(0.f, ColliderAABBDesc.vExtents.y, 0.f);
	//
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
	//	TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom[CCollider::TYPE_AABB]), &ColliderAABBDesc)))
	//	return E_FAIL;
	//
	///* FOR.Com_Collider_OBB */
	//CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc{};
	//ColliderOBBDesc.vExtents = _float3(0.7f, 0.7f, 0.7f);
	//ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vExtents.y, 0.f);
	//ColliderOBBDesc.vAngles = _float3(0.f, m_pGameInstance->Get_Random(XMConvertToRadians(0.f), XMConvertToRadians(360.f)), 0.f);
	//
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
	//	TEXT("Com_Collider_OBB"), reinterpret_cast<CComponent**>(&m_pColliderCom[CCollider::TYPE_OBB]), &ColliderOBBDesc)))
	//	return E_FAIL;
	//
	///* FOR.Com_Collider_Sphere */
	//CBounding_Sphere::BOUNDING_SPHERE_DESC			ColliderSphereDesc{};
	//ColliderSphereDesc.fRadius = 1.2f;
	//ColliderSphereDesc.vCenter = _float3(0.f, ColliderSphereDesc.fRadius, 0.f);
	//
	//
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
	//	TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom[CCollider::TYPE_SPHERE]), &ColliderSphereDesc)))
	//	return E_FAIL;


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

HRESULT CCarcassBigA::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};
	
	Desc.pIsEndAnim = &m_bEndAnim;
	Desc.pIsResetRootMove =&m_bResetRootMove;
	Desc.pPrevTrackPos = &m_fPrevTrackPos;
	//Desc.pColliderCtrs = m_bColliderCtrs;
	//


	m_pFsmCom->Add_State(CState_CarcassBigA_Idle::Create(m_pFsmCom, this, IDLE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Walk::Create(m_pFsmCom, this, WALK, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Run::Create(m_pFsmCom, this, RUN, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Attack::Create(m_pFsmCom, this, ATTACK, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Paralyze::Create(m_pFsmCom, this, PARALYZE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Die::Create(m_pFsmCom, this, DIE, &Desc));

	m_pFsmCom->Add_State(CState_CarcassBigA_Impact::Create(m_pFsmCom, this, IMPACT, &Desc));


	m_pFsmCom->Set_State(IDLE);

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



CPawn* CCarcassBigA::Clone(void* pArg)
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
	for (_uint i = 0; i < TYPE_END; ++i)
	{
		Safe_Release(m_pColliderObject[i]);
	}

	__super::Free();

}