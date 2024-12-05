#include "stdafx.h"
#include "..\Public\CarcassTail.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

#include "State_CarcassTail_Idle.h"
#include "State_CarcassTail_Die.h"
#include "State_CarcassTail_Grogy.h"
#include "State_CarcassTail_HitFatal.h"
#include "State_CarcassTail_Run.h"
#include "State_CarcassTail_Walk.h"

#include "State_CarcassTail_Heading.h"
#include "State_CarcassTail_Leap.h"
#include "State_CarcassTail_MultyHittingDown.h"
#include "State_CarcassTail_Scratching.h"
#include "State_CarcassTail_TailSwingDown.h"
#include "State_CarcassTail_TailSwip.h"
#include "State_CarcassTail_TailSwipMultiple.h"

#include "State_CarcassTail_HeadingMultiple.h"
#include "State_CarcassTail_LeapAttack.h"
#include "State_CarcassTail_LeapToAttack.h"
#include "State_CarcassTail_ScratchingMultiple.h"
#include "State_CarcassTail_ScratchingToSwip.h"

CCarcassTail::CCarcassTail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CCarcassTail::CCarcassTail(const CCarcassTail& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CCarcassTail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCarcassTail::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDefaultDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, pDefaultDesc->iCurrentCellNum);

	m_pModelCom->SetUp_Animation(rand() % 20, true);

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	m_strObjectTag = TEXT("Monster");

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });


	m_vRimLightColor = { 0.f, 0.f, 0.f, 0.f };

	m_eStat.fHp = 500;
	m_eStat.fAtk = 4.f;
	m_eStat.fDefence = 2.f;
	m_eStat.fStemina = 30.f;


	return S_OK;
}

void CCarcassTail::Priority_Update(_float fTimeDelta)
{
	__super::Set_UpTargetPos();

	if (m_eStat.fHp <= 0.f)
	{
		m_pFsmCom->Set_State(DIE);
	}
}

void CCarcassTail::Update(_float fTimeDelta)
{
	m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta), m_pTransformCom->Get_WorldMatrix());

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	m_pFsmCom->Update(fTimeDelta);

	_float4x4 WorldMat{};
	XMStoreFloat4x4(&WorldMat , m_pModelCom->Get_BoneCombindTransformationMatrix(7) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));

	m_pColliderCom->Update(&WorldMat);
	m_pGameInstance->Add_ColliderList(m_pColliderCom);
}

void CCarcassTail::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRigidBodyCom->Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	for (_uint i = 0; i < TYPE_END; ++i)
	{
		m_pColliderObject[i]->Late_Update(fTimeDelta);
	}

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
}

HRESULT CCarcassTail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();

	for (_uint i = 0; i < TYPE_END; ++i)
	{
		m_pColliderObject[i]->Render();
	}
#endif
	return S_OK;
}

void CCarcassTail::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex)
{
	m_pColliderObject[iCollIndex]->Active_Collider(fDamageRatio);
}

void CCarcassTail::DeActive_CurretnWeaponCollider(_uint iCollIndex)
{
	m_pColliderObject[iCollIndex]->DeActive_Collider();
}

HRESULT CCarcassTail::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CarcassTail"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */		//Body
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(1.7f, 0.6f, 1.3f);
	ColliderDesc.vCenter = _float3(0.3f, -0.1f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	/* FOR.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc_Obj{};

	ColliderOBBDesc_Obj.vExtents = _float3(0.5f, 0.25f, 0.25f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vAngles = _float3(0.f, 0.f, 0.f);

	CColliderObject::COLIDEROBJECT_DESC Desc{};

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT));
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketBoneMatrix2 = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.fDamageAmount = 2.f;

	m_pColliderObject[TYPE_LEFTHAND] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	//
	//ColliderOBBDesc_Obj.vExtents = _float3(0.5f, 0.5f, 0.5f);
	//ColliderOBBDesc_Obj.vCenter = _float3(0.f, 0.f, 0.f);
	//ColliderOBBDesc_Obj.vAngles = _float3(0.f, 0.f, 0.f);

	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT));

	m_pColliderObject[TYPE_RIGHTHAND] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	//
	ColliderOBBDesc_Obj.vExtents = _float3(1.4f, 0.3f, 0.3f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.4f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vAngles = _float3(0.f, 0.f, 0.f);

	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_WEAPON));
	Desc.fDamageAmount = 3.f;

	m_pColliderObject[TYPE_TAIL] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	//
	ColliderOBBDesc_Obj.vExtents = _float3(0.5f, 0.7f, 1.4f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.8f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vAngles = _float3(0.f, 0.f, 0.f);

	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(8);
	Desc.fDamageAmount = 3.f;

	m_pColliderObject[TYPE_IMPACT] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	

	for (_int i = 0; i < TYPE_END; ++i)
	{
		m_pColliderObject[i]->DeActive_Collider();
		//m_pColliderObject[i]->;
	}

	// 항상 마지막에 생성하기
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc{};
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerTransform = m_pTransformCom;
	RigidBodyDesc.pOwnerNavigation = m_pNavigationCom;
	RigidBodyDesc.isCapsule = true;

	RigidBodyDesc.pOwner = this;
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestituion = 0.0f;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	physX::GeometryCapsule CapsuleDesc;
	CapsuleDesc.fHeight = 1.5f;
	CapsuleDesc.fRadius = 0.25f;
	RigidBodyDesc.pGeometry = &CapsuleDesc;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CCarcassTail::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};
	

	m_pFsmCom->Add_State(CState_CarcassTail_Idle::Create(m_pFsmCom, this, IDLE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_Die::Create(m_pFsmCom, this, DIE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_Run::Create(m_pFsmCom, this, RUN, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_Walk::Create(m_pFsmCom, this, WALK, &Desc));

	m_pFsmCom->Add_State(CState_CarcassTail_Heading::Create(m_pFsmCom, this, HEADING, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_Leap::Create(m_pFsmCom, this, LEAP, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_MultyHittingDown::Create(m_pFsmCom, this, MULTYHITTINGDOWN, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_Scratching::Create(m_pFsmCom, this, SCRATCHING, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_TailSwingDown::Create(m_pFsmCom, this, TAILSWINGDOWN, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_TailSwip::Create(m_pFsmCom, this, TAILSWIP, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_TailSwipMultiple::Create(m_pFsmCom, this, TAILSWIPMULTIPLE, &Desc));

	m_pFsmCom->Add_State(CState_CarcassTail_HeadingMultiple::Create(m_pFsmCom, this, HEADINGMULTIPLE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_LeapAttack::Create(m_pFsmCom, this, LEAPATTACK, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_LeapToAttack::Create(m_pFsmCom, this, LEAPTOATTACK, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_ScratchingMultiple::Create(m_pFsmCom, this, SCRATCHINGMULTIPLE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassTail_ScratchingToSwip::Create(m_pFsmCom, this, SCRATCHINGTOWIP, &Desc));
	
	
	m_pFsmCom->Set_State(IDLE);


	return S_OK;

}

CCarcassTail* CCarcassTail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarcassTail* pInstance = new CCarcassTail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCarcassTail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CPawn* CCarcassTail::Clone(void* pArg)
{
	CCarcassTail* pInstance = new CCarcassTail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCarcassTail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCarcassTail::Free()
{
	for (_uint i = 0; i < TYPE_END; ++i)
	{
		Safe_Release(m_pColliderObject[i]);
	}

	__super::Free();

}