#include "stdafx.h"
#include "CurruptedStrongArm_Puppet.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

// 24-11-26 김성용
// 게임인터페이스 접근 코드 
// 정식 코드  
#include "GameInterface_Controller.h"


#include "State_CurruptedStrongArm_Idle.h"
#include "State_CurruptedStrongArm_Die.h"
#include "State_CurruptedStrongArm_Grogy.h"
#include "State_CurruptedStrongArm_HitFatal.h"
#include "State_CurruptedStrongArm_KnockBack.h"

#include "State_CurruptedStrongArm_JumpPunch.h"
#include "State_CurruptedStrongArm_StingTwice.h"
#include "State_CurruptedStrongArm_SwipAttack.h"

CCurruptedStrongArm_Puppet::CCurruptedStrongArm_Puppet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CCurruptedStrongArm_Puppet::CCurruptedStrongArm_Puppet(const CCurruptedStrongArm_Puppet& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CCurruptedStrongArm_Puppet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCurruptedStrongArm_Puppet::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDefaultDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_eStat.fHp = 400.f;
	m_eStat.fMaxHp = 400.f;
	m_eStat.fAtk = 260.f;
	//m_eStat.fDefence = 3.f;

	m_eStat.bWeakness = false;

	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 130.f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iOriginCellNum = pDefaultDesc->iCurrentCellNum;

	m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, m_iOriginCellNum);
	m_iInitRoomNum = m_pNavigationCom->Get_Cell_AreaNum(m_iOriginCellNum);

	m_pModelCom->SetUp_Animation(26, true);

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	m_strObjectTag = TEXT("Monster");

	m_vRimLightColor = { 0.f, 0.f, 0.f, 0.f };

	m_vCenterOffset = _Vec3{ 0.f, 1.55f, 0.f };

	m_bDiscover = false;

	GET_GAMEINTERFACE->Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE::ORTHO_CURRUPTED_PUPPET, this);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
	return S_OK;
}

void CCurruptedStrongArm_Puppet::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (!m_bDieState && m_eStat.fHp <= 0.f)
	{
		GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
		m_bDieState = true;
		m_pFsmCom->Change_State(DIE);
	}
}

void CCurruptedStrongArm_Puppet::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_Player_AreaNum() == m_iInitRoomNum)
		m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta), m_pTransformCom->Get_WorldMatrix());
	else
		m_vCurRootMove = _Vec3(0.f, 0.f, 0.f);

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	m_pFsmCom->Update(fTimeDelta);
	
	Update_Collider();

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
}

void CCurruptedStrongArm_Puppet::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_Player_AreaNum() == m_iInitRoomNum)
	{
		if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 50.f))
		{
			__super::Late_Update(fTimeDelta);

			m_pRigidBodyCom->Update(fTimeDelta);
			m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

			for (_uint i = 0; i < TYPE_END; ++i)
			{
				m_pColliderObject[i]->Late_Update(fTimeDelta);
			}

			for (_int i = 0; i < CT_END - 1; ++i)
			{
				m_pGameInstance->Add_ColliderList(m_EXCollider[i]);
			}
			m_pGameInstance->Add_ColliderList(m_pColliderCom);
		}
	}
}

HRESULT CCurruptedStrongArm_Puppet::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();
	for(_int i = 0; i < CT_END - 1; ++i)
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

void CCurruptedStrongArm_Puppet::Resetting()
{
	m_vRimLightColor = { 0.f, 0.f, 0.f, 0.f };

	m_eStat.fHp = 400.f;
	m_eStat.fMaxHp = 400.f;
	m_eStat.fAtk = 260.f;

	m_eStat.bWeakness = false;

	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 130.f;

	m_bDiscover = false;
	m_bFirstMeetCheck = false;

	m_bFatalAttacked = false;

	m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, m_iOriginCellNum);
	m_iInitRoomNum = m_pNavigationCom->Get_Cell_AreaNum(m_iOriginCellNum);

	Change_State(CMonster::IDLE);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
}

void CCurruptedStrongArm_Puppet::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex, HIT_TYPE eHitType, ATTACK_STRENGTH eAtkStrength)
{
	m_pColliderObject[iCollIndex]->Active_Collider(fDamageRatio, 0, eHitType, eAtkStrength);
}

void CCurruptedStrongArm_Puppet::DeActive_CurretnWeaponCollider(_uint iCollIndex)
{
	m_pColliderObject[iCollIndex]->DeActive_Collider();
}

HRESULT CCurruptedStrongArm_Puppet::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Horesman"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */		//Body
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.55f, 0.45f, 0.8f);
	ColliderDesc.vCenter = _float3(0.3f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);
	m_pColliderBindMatrix[CT_BODY_UPPER] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(7);
	

	//LowerBody
	ColliderDesc.vExtents = _float3(0.4f, 0.4f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, -0.3f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_BodyLower"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_BODY_LOWER]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_BODY_LOWER] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(4);


	//ArmLeft
	ColliderDesc.vExtents = _float3(0.7f, 0.3f, 0.3f);
	ColliderDesc.vCenter = _float3(0.3f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_ArmLeft"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_ARM_LEFT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_ARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT) - 1);


	//ArmRight
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_ArmRight"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_ARM_RIGHT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_ARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT) - 1);


	//LegLeft
	ColliderDesc.vExtents = _float3(0.7f, 0.3f, 0.3f);
	ColliderDesc.vCenter = _float3(0.22f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LegLeft"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_LEG_LEFT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_LEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_FOOT_LEFT) - 1);


	//LegRight
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LegRight"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_LEG_RIGHT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_LEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_FOOT_RIGHT) - 1);

	for (_int i = 0; i < CT_END - 1; ++i)
	{
		m_EXCollider[i]->Set_Owner(this);
	}

	/* FOR.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc_Obj{};

	ColliderOBBDesc_Obj.vAngles = _float3(0.0f, 0.0f, 0.0f);
	ColliderOBBDesc_Obj.vCenter = _float3(-0.1f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vExtents = _float3(0.7f, 0.3f, 0.3f);

	CColliderObject::COLIDEROBJECT_DESC Desc{};

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT));
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketBoneMatrix2 = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.fDamageAmount = m_eStat.fAtk;

	m_pColliderObject[TYPE_ARM_LEFT] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	ColliderOBBDesc_Obj.vAngles = _float3(0.0f, 0.0f, 0.0f);
	ColliderOBBDesc_Obj.vCenter = _float3(-0.1f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vExtents = _float3(0.7f, 0.3f, 0.3f);

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT));
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketBoneMatrix2 = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.fDamageAmount = m_eStat.fAtk;

	m_pColliderObject[TYPE_ARM_RIGHT] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	ColliderOBBDesc_Obj.vAngles = _float3(0.0f, 0.0f, 0.0f);
	ColliderOBBDesc_Obj.vCenter = _float3(-0.1f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vExtents = _float3(1.f, 0.2f, 0.2f);

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(285);
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketBoneMatrix2 = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.fDamageAmount = m_eStat.fAtk;

	m_pColliderObject[TYPE_TENTACLE_FL] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));


	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(299);
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketBoneMatrix2 = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.fDamageAmount = m_eStat.fAtk;

	m_pColliderObject[TYPE_TENTACLE_FR] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));


	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(311);
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketBoneMatrix2 = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.fDamageAmount = m_eStat.fAtk;

	m_pColliderObject[TYPE_TENTACLE_BL] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));


	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(321);
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketBoneMatrix2 = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.fDamageAmount = m_eStat.fAtk;

	m_pColliderObject[TYPE_TENTACLE_BR] = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));

	for (_int i = 0; i < CT_END - 1; ++i)
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
	CapsuleDesc.fRadius = 0.85f;
	RigidBodyDesc.pGeometry = &CapsuleDesc;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCurruptedStrongArm_Puppet::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};



	m_pFsmCom->Add_State(CState_CurruptedStrongArm_Idle::Create(m_pFsmCom, this, IDLE, &Desc));
	m_pFsmCom->Add_State(CState_CurruptedStrongArm_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_CurruptedStrongArm_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));
	m_pFsmCom->Add_State(CState_CurruptedStrongArm_Die::Create(m_pFsmCom, this, DIE, &Desc));
	m_pFsmCom->Add_State(CState_CurruptedStrongArm_KnockBack::Create(m_pFsmCom, this, KNOCKBACK, &Desc));

	m_pFsmCom->Add_State(CState_CurruptedStrongArm_StingTwice::Create(m_pFsmCom, this, STINGTWICE, &Desc));
	m_pFsmCom->Add_State(CState_CurruptedStrongArm_SwipAttack::Create(m_pFsmCom, this, SWIPATTACK, &Desc));
	m_pFsmCom->Add_State(CState_CurruptedStrongArm_JumpPunch::Create(m_pFsmCom, this, JUMP_PUNCH, &Desc));

	m_pFsmCom->Set_State(IDLE);

	return S_OK;



}

void CCurruptedStrongArm_Puppet::Update_Collider()
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

CCurruptedStrongArm_Puppet* CCurruptedStrongArm_Puppet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCurruptedStrongArm_Puppet* pInstance = new CCurruptedStrongArm_Puppet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCurruptedStrongArm_Puppet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CPawn* CCurruptedStrongArm_Puppet::Clone(void* pArg)
{
	CCurruptedStrongArm_Puppet* pInstance = new CCurruptedStrongArm_Puppet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCurruptedStrongArm_Puppet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCurruptedStrongArm_Puppet::Free()
{
	for (_uint i = 0; i < TYPE_END; ++i)
	{
		Safe_Release(m_pColliderObject[i]);
	}

	for (_uint i = 0; i < CT_END - 1; ++i)
	{
		Safe_Release(m_EXCollider[i]);
	}

	__super::Free();

}