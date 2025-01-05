#include "stdafx.h"
#include "..\Public\CarcassBigA.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

// 24-11-26 김성용
// 게임인터페이스 접근 코드 
// 정식 코드  
#include "GameInterface_Controller.h"


#include "State_CarcassBigA_Idle.h"
#include "State_CarcassBigA_Die.h"

#include "State_CarcassBigA_LTSwingRight.h"
#include "State_CarcassBigA_LOSwingRight.h"
#include "State_CarcassBigA_RageAttack.h"
#include "State_CarcassBigA_WheelWind.h"
#include "State_CarcassBigA_Impact.h"

#include "State_CarcassBigA_AttackRoute_1.h"
#include "State_CarcassBigA_AttackRoute_2.h"
#include "State_CarcassBigA_AttackRoute_3.h"
#include "State_CarcassBigA_AttackRoute_4.h"

#include "State_CarcassBigA_Grogy.h"
#include "State_CarcassBigA_HitFatal.h"
#include "State_CarcassBigA_Paralyze.h"

#include "Dissolve_PowerAttack.h"

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
	OBJECT_DEFAULT_DESC* pDefaultDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

  	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_eStat.fHp = 1300.f;
	m_eStat.fMaxHp = 1300.f;
	m_eStat.fAtk = 210.f;
	//m_eStat.fDefence = 3.f;

	m_eStat.bWeakness = false;

	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 210.f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iOriginCellNum = pDefaultDesc->iCurrentCellNum;

	m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, m_iOriginCellNum);
	m_iInitRoomNum = m_pNavigationCom->Get_Cell_AreaNum(m_iOriginCellNum);

	m_pModelCom->SetUp_Animation(rand() % 20, true);

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	m_strObjectTag = TEXT("Monster");

	m_vRimLightColor = { 0.f, 0.f, 0.f, 0.f };

	m_vCenterOffset = _Vec3{ 0.f, 1.7f, 0.f };

	m_bDiscover = false;

	GET_GAMEINTERFACE->Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_BIG, this);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
	return S_OK;
}

void CCarcassBigA::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (!m_bDieState && m_eStat.fHp <= 0.f)
	{
		GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
		m_bDieState = true;
		m_pFsmCom->Change_State(DIE);
	}
	m_pDissolveEffect->Priority_Update(fTimeDelta);
}

void CCarcassBigA::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_Player_AreaNum() == m_iInitRoomNum)
		m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta), m_pTransformCom->Get_WorldMatrix());
	else
		m_vCurRootMove = _Vec3(0.f, 0.f, 0.f);

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	m_pFsmCom->Update(fTimeDelta);

	// 테스트 임시 코드 
	if (KEY_HOLD(KEY::ALT))
		if (KEY_TAP(KEY::NUM5))
		{
			if (m_eStat.bWeakness) m_eStat.bWeakness = false;
			else m_eStat.bWeakness = true;
		}
	Update_Collider();
	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pDissolveEffect->Update(fTimeDelta);
}

void CCarcassBigA::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_Player_AreaNum() == m_iInitRoomNum)
	{
		if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 30.f))
		{
			__super::Late_Update(fTimeDelta);

			m_pRigidBodyCom->Update(fTimeDelta);
			m_pDissolveEffect->Late_Update(fTimeDelta);

			m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
			for (_uint i = 0; i < TYPE_END; ++i)
			{
				m_pColliderObject[i]->Late_Update(fTimeDelta);
			}
			m_pGameInstance->Add_ColliderList(m_pColliderCom);
		}
	}
}

HRESULT CCarcassBigA::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();

	for (_uint i = 0; i < CT_END - 1; ++i)
	{
		m_EXCollider[i]->Render();
	}
	for (_uint i = 0; i < TYPE_END; ++i)
	{
		//m_pColliderObject[i]->Active_Collider();
		m_pColliderObject[i]->Render();
	}
#endif
	return S_OK;
}

void CCarcassBigA::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex, HIT_TYPE eHitType, ATTACK_STRENGTH eAtkStrength)
{
	m_pColliderObject[iCollIndex]->Active_Collider(fDamageRatio, iCollIndex, eHitType, eAtkStrength);
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
	ColliderDesc.vExtents = _float3(0.7f, 0.55f, 0.8f);
	ColliderDesc.vCenter = _float3(0.1f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);
	m_pColliderBindMatrix[CT_BODY_UPPER] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(8);

	//LowerBody
	ColliderDesc.vExtents = _float3(0.5f, 0.45f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_BodyMiddle"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_BODY_LOWER]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_BODY_LOWER] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(5);


	//LOWERArmLeft
	ColliderDesc.vExtents = _float3(0.8f, 0.38f, 0.38f);
	ColliderDesc.vCenter = _float3(0.6f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_ArmLeft"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_ARM_LEFT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_ARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT) - 1);

	//LOWERArmRight
	ColliderDesc.vExtents = _float3(0.7f, 0.38f, 0.38f);
	ColliderDesc.vCenter = _float3(0.4f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_ArmRight"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_ARM_RIGHT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_ARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT) - 1);



	//LegLeft
	ColliderDesc.vExtents = _float3(0.5f, 0.2f, 0.2f);
	ColliderDesc.vCenter = _float3(0.2f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_UpperLegLeft"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_LEG_LEFT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_LEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_FOOT_LEFT) - 1);

	//LegRight
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LegRight"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_LEG_RIGHT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_LEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_FOOT_RIGHT) - 1);


	//유사 웨폰
	/* FOR.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc_Obj{};

	ColliderOBBDesc_Obj.vAngles = _float3(0.0f, 0.0f, 0.0f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.1f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vExtents = _float3(1.2f, 0.45f, 0.45f);

	CColliderObject::COLIDEROBJECT_DESC Desc{};

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT) - 1);
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketBoneMatrix2 = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.fDamageAmount = m_eStat.fAtk;
	Desc.pOWner = this;

	m_pColliderObject[TYPE_LEFTHAND] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	/* FOR.Com_Collider_OBB */
	ColliderOBBDesc_Obj.vAngles = _float3(0.0f, 0.0f, 0.0f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vExtents = _float3(1.2f, 0.5f, 0.5f);
	
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT) - 1);
	
	m_pColliderObject[TYPE_RIGHTHAND] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	ColliderOBBDesc_Obj.vAngles = _float3(0.f, 0.0f, 0.f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.8f, 0.f, -1.8f);
	ColliderOBBDesc_Obj.vExtents = _float3(0.4f, 0.75f, 1.2f);

	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(2);
	Desc.fDamageAmount = m_eStat.fAtk;

	m_pColliderObject[TYPE_IMPACT] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	for (_int i = 0; i < TYPE_END; ++i)
	{
		m_pColliderObject[i]->DeActive_Collider();
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

	physX::GeometryCapsule CapsuleDesc;
	CapsuleDesc.fHeight = 1.5f;
	CapsuleDesc.fRadius = 0.7f;
	RigidBodyDesc.pGeometry = &CapsuleDesc;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;

	m_pRigidBodyCom->Set_Mass(1000.f);

	return S_OK;
}

HRESULT CCarcassBigA::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};



	m_pFsmCom->Add_State(CState_CarcassBigA_Idle::Create(m_pFsmCom, this, IDLE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Paralyze::Create(m_pFsmCom, this, PARALYZE, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Die::Create(m_pFsmCom, this, DIE, &Desc));

	m_pFsmCom->Add_State(CState_CarcassBigA_WheelWind::Create(m_pFsmCom, this, WHEELWIND, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_LOSwingRight::Create(m_pFsmCom, this, LO_SWINGRIGHT, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_LTSwingRight::Create(m_pFsmCom, this, LT_SWINGRIGHT, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_RageAttack::Create(m_pFsmCom, this, RAGE_ATTACK, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_Impact::Create(m_pFsmCom, this, ATK_IMPACT, &Desc));

	m_pFsmCom->Add_State(CState_CarcassBigA_AttackRoute_1::Create(m_pFsmCom, this, ATK_ROUTE_1, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_AttackRoute_2::Create(m_pFsmCom, this, ATK_ROUTE_2, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_AttackRoute_3::Create(m_pFsmCom, this, ATK_ROUTE_3, &Desc));
	m_pFsmCom->Add_State(CState_CarcassBigA_AttackRoute_4::Create(m_pFsmCom, this, ATK_ROUTE_4, &Desc));


	m_pFsmCom->Set_State(IDLE);

	return S_OK;



}

void CCarcassBigA::Resetting()
{
	m_vRimLightColor = { 0.f, 0.f, 0.f, 0.f };

	m_eStat.fHp = 1300.f;
	m_eStat.fMaxHp = 1300.f;
	m_eStat.fAtk = 210.f;

	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 210.f;

	m_eStat.bWeakness = false;

	m_bDiscover = false;
	m_bFirstMeetCheck = false;

	m_bFatalAttacked = false;

	m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, m_iOriginCellNum);
	m_iInitRoomNum = m_pNavigationCom->Get_Cell_AreaNum(m_iOriginCellNum);

	Change_State(CMonster::IDLE);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
}

void CCarcassBigA::On_PowerAttack(_bool bOn)
{
	if(bOn != m_pDissolveEffect->Get_On())
		m_pDissolveEffect->Set_On(bOn);
}

HRESULT CCarcassBigA::Ready_Effect()
{
	CDissolve_Effect::DISSOLVE_EFFECT_DESC DissolveDesc = {};
	DissolveDesc.fRotationPerSec = XMConvertToRadians(90.f);
	DissolveDesc.fSpeedPerSec = 1.f;
	DissolveDesc.iLevelIndex = LEVEL_GAMEPLAY;
	DissolveDesc.pTarget_ModelCom = m_pModelCom;
	DissolveDesc.pTarget_TransformCom = m_pTransformCom;
	DissolveDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_CarcassBigA_PowerAttack");

	m_pDissolveEffect = static_cast<CDissolve_PowerAttack*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_PowerAttack"), &DissolveDesc));
	if (nullptr == m_pDissolveEffect)
		return E_FAIL;

	return S_OK;
}

void CCarcassBigA::Update_Collider()
{
	_float4x4 UpdateMat{};

	_Matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_BODY_UPPER]) * WorldMat);
	m_pColliderCom->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_BODY_LOWER]) * WorldMat);
	m_EXCollider[CT_BODY_LOWER]->Update(&UpdateMat);


	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_ARM_LEFT]) * WorldMat);
	m_EXCollider[CT_ARM_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_ARM_RIGHT]) * WorldMat);
	m_EXCollider[CT_ARM_RIGHT]->Update(&UpdateMat);


	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LEG_LEFT]) * WorldMat);
	m_EXCollider[CT_LEG_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LEG_RIGHT]) * WorldMat);
	m_EXCollider[CT_LEG_RIGHT]->Update(&UpdateMat);
}

CCarcassBigA* CCarcassBigA::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCarcassBigA* pInstance = new CCarcassBigA(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCarcassBigA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CPawn* CCarcassBigA::Clone(void* pArg)
{
	CCarcassBigA* pInstance = new CCarcassBigA(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCarcassBigA"));
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

	for (_uint i = 0; i < CT_END - 1; ++i)
	{
		Safe_Release(m_EXCollider[i]);
	}
	Safe_Release(m_pDissolveEffect);

	__super::Free();

}