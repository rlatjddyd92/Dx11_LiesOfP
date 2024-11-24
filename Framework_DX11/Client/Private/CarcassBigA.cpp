#include "stdafx.h"
#include "..\Public\CarcassBigA.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"


#include "State_CarcassBigA_Idle.h"
#include "State_CarcassBigA_Die.h"

#include "State_CarcassBigA_LTSwingRight.h"
#include "State_CarcassBigA_LOSwingRight.h"
#include "State_CarcassBigA_RageAttack.h"
#include "State_CarcassBigA_WheelWind.h"

#include "State_CarcassBigA_AttackRoute_0.h"
#include "State_CarcassBigA_AttackRoute_1.h"
#include "State_CarcassBigA_AttackRoute_2.h"
#include "State_CarcassBigA_AttackRoute_3.h"
#include "State_CarcassBigA_AttackRoute_4.h"

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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(rand() % 20, true);

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	m_strObjectTag = TEXT("Monster");
	m_pColliderCom->Set_Owner(this);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });


	m_fHp = 500.f;
	m_fAtk = 4.f;
	m_fDefence = 2.f;
	m_fStemina = 30.f;

	return S_OK;
}

void CCarcassBigA::Priority_Update(_float fTimeDelta)
{
	__super::Set_UpTargetPos();
	
}

void CCarcassBigA::Update(_float fTimeDelta)
{
	m_vCurRootMove = m_pModelCom->Play_Animation(fTimeDelta, nullptr);

	m_pFsmCom->Update(fTimeDelta);

	_Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	m_vCurRootMove = XMVector3TransformNormal(m_vCurRootMove, m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + m_vCurRootMove);
	

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CCarcassBigA::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);



	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	for (_uint i = 0; i < TYPE_END; ++i)
	{
		m_pColliderObject[i]->Late_Update(fTimeDelta);
	}

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
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
	m_pColliderCom->Render();

	for (_uint i = 0; i < TYPE_END; ++i)
	{
		m_pColliderObject[i]->Render();
	}
#endif
	return S_OK;
}

void CCarcassBigA::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex)
{
	m_pColliderObject[iCollIndex]->Active_Collider(fDamageRatio);
}

void CCarcassBigA::DeActive_CurretnWeaponCollider(_uint iCollIndex)
{
	m_pColliderObject[iCollIndex]->DeActive_Collider();
}

HRESULT CCarcassBigA::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CarcassBigA"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	/* FOR.Com_Collider */		//Body
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.5f, 2.f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 1.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
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
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT));
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketBoneMatrix2 = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.fDamageAmount = 2.f;


	m_pColliderObject[TYPE_LEFTHAND] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));


	/* FOR.Com_Collider_OBB */
	ColliderOBBDesc_Obj.vAngles = _float3(0.0f, 0.0f, 0.0f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.2f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vExtents = _float3(0.5f, 0.3f, 0.3f);
	
	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT));
	
	m_pColliderObject[TYPE_RIGHTHAND] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	ColliderOBBDesc_Obj.vAngles = _float3(0.0f, 0.0f, 0.0f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vExtents = _float3(0.7f, 0.5f, 0.5f);

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(5);
	Desc.fDamageAmount = 3.f;

	m_pColliderObject[TYPE_IMPACT] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));


	return S_OK;
}

HRESULT CCarcassBigA::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};

	Desc.pIsEndAnim = &m_bEndAnim;
	Desc.pIsResetRootMove = &m_bResetRootMove;
	Desc.pRootMoveCtr = &m_bRootMoveCtr;


	m_pFsmCom->Add_State(CState_CarcassBigA_Idle::Create(m_pFsmCom, this, IDLE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Walk::Create(m_pFsmCom, this, WALK, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Run::Create(m_pFsmCom, this, RUN, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Paralyze::Create(m_pFsmCom, this, PARALYZE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Die::Create(m_pFsmCom, this, DIE, &Desc));

	m_pFsmCom->Add_State(CState_CarcassBigA_WheelWind::Create(m_pFsmCom, this, WHEELWIND, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_LOSwingRight::Create(m_pFsmCom, this, LO_SWINGRIGHT, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_LTSwingRight::Create(m_pFsmCom, this, LT_SWINGRIGHT, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_RageAttack::Create(m_pFsmCom, this, RAGE_ATTACK, &Desc));

	m_pFsmCom->Add_State(CState_CarcassBigA_AttackRoute_0::Create(m_pFsmCom, this, ATK_ROUTE_0, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_AttackRoute_1::Create(m_pFsmCom, this, ATK_ROUTE_1, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_AttackRoute_2::Create(m_pFsmCom, this, ATK_ROUTE_2, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_AttackRoute_3::Create(m_pFsmCom, this, ATK_ROUTE_3, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_AttackRoute_4::Create(m_pFsmCom, this, ATK_ROUTE_4, &Desc));


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