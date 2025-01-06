#include "stdafx.h"
#include "RebornerBigA.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

// 24-11-26 김성용
// 게임인터페이스 접근 코드 
// 정식 코드  
#include "GameInterface_Controller.h"


#include "State_RebornerBigA_Idle.h"
#include "State_RebornerBigA_Die.h"
#include "State_RebornerBigA_Grogy.h"
#include "State_RebornerBigA_HitFatal.h"
#include "State_RebornerBigA_KnockBack.h"

#include "State_RebornerBigA_SlashJump.h"
#include "State_RebornerBigA_GuardSting.h"
#include "State_RebornerBigA_RushSting.h"
#include "State_RebornerBigA_SlashTwice.h"
#include "State_RebornerBigA_SwingMultiple.h"

#include "Effect_Manager.h"
#include "Effect_Container.h"

#include "Dissolve_PowerAttack.h"

CRebornerBigA::CRebornerBigA(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CRebornerBigA::CRebornerBigA(const CRebornerBigA& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CRebornerBigA::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRebornerBigA::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDefaultDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_eStat.fHp = 600.f;
	m_eStat.fMaxHp = 600.f;
	m_eStat.fAtk = 180.f;
	//m_eStat.fDefence = 3.f;

	m_eStat.bWeakness = false;

	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 160.f;

	m_iErgoPoint = 200;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	m_iOriginCellNum = pDefaultDesc->iCurrentCellNum;

	m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, m_iOriginCellNum);
	m_iInitRoomNum = m_pNavigationCom->Get_Cell_AreaNum(m_iOriginCellNum);

	m_pModelCom->SetUp_Animation(51, true);

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	m_strObjectTag = TEXT("Monster");

	m_vRimLightColor = { 0.f, 0.f, 0.f, 0.f };

	m_vCenterOffset = _Vec3{ 0.f, 1.78f, 0.f };
	
	m_bDiscover = false;

	m_pSoundCom[CPawn::PAWN_SOUND_VOICE]->Play2D_Repeat(TEXT("SE_NPC_Carcass_Horseman_MT_Tentacle_Movement_02"), &g_fVoiceVolume);
	GET_GAMEINTERFACE->Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_BIG, this);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
	return S_OK;
}

void CRebornerBigA::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	m_pWeapon->Priority_Update(fTimeDelta);

	if (!m_pSwingEffect->Get_Dead())
	{
		m_pSwingEffect->Priority_Update(fTimeDelta);
	}
	m_pDissolveEffect->Priority_Update(fTimeDelta);

}

void CRebornerBigA::Update(_float fTimeDelta)
{
	if (KEY_TAP(KEY::R))
	{
		CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Impact"),
			_Vec3{ Calc_CenterPos() }, _Vec3{ 0, 0, 1 });
	}
	if (m_pGameInstance->Get_Player_AreaNum() == m_iInitRoomNum)
		m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta), m_pTransformCom->Get_WorldMatrix());
	else
		m_vCurRootMove = _Vec3(0.f, 0.f, 0.f);

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	m_pFsmCom->Update(fTimeDelta);

	Update_Collider();

	for (_uint i = 0; i < PAWN_SOUND_END; ++i)
	{
		m_pSoundCom[i]->Update(fTimeDelta);
	}

	if (!m_pSwingEffect->Get_Dead())
	{
		m_pSwingEffect->Update(fTimeDelta);
	}
	m_pDissolveEffect->Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_pWeapon->Update(fTimeDelta);
}

void CRebornerBigA::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_Player_AreaNum() == m_iInitRoomNum)
	{
		if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 50.f))
		{
			__super::Late_Update(fTimeDelta);

			m_pRigidBodyCom->Update(fTimeDelta);
			m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

			if (!m_pSwingEffect->Get_Dead())
			{
				m_pSwingEffect->Late_Update(fTimeDelta);
			}
			m_pDissolveEffect->Late_Update(fTimeDelta);

			m_pGameInstance->Add_ColliderList(m_pColliderCom);
			for (_int i = 0; i < CT_END - 1; ++i)
			{
				m_pGameInstance->Add_ColliderList(m_EXCollider[i]);
			}
			m_pWeapon->Late_Update(fTimeDelta);
		}
	}
}

HRESULT CRebornerBigA::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();
	for (_int i = 0; i < CT_END - 1; ++i)
	{
		m_EXCollider[i]->Render();
	}

#endif
	return S_OK;
}

void CRebornerBigA::Resetting()
{
	m_vRimLightColor = { 0.f, 0.f, 0.f, 0.f };

	m_eStat.fHp = 600.f;
	m_eStat.fMaxHp = 600.f;
	m_eStat.fAtk = 180.f;

	m_eStat.bWeakness = false;
	m_eStat.bFatalAttack = false;

	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 160.f;

	m_bDieState = false;

	m_bDiscover = false;
	m_bFirstMeetCheck = false;

	m_bFatalAttacked = false;

	m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, m_iOriginCellNum);
	m_iInitRoomNum = m_pNavigationCom->Get_Cell_AreaNum(m_iOriginCellNum);

	Change_State(CMonster::IDLE);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
}

void CRebornerBigA::On_PowerAttack(_bool bOn)
{
	if (bOn != m_pDissolveEffect->Get_On())
		m_pDissolveEffect->Set_On(bOn);
}

void CRebornerBigA::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex, HIT_TYPE eHitType, ATTACK_STRENGTH eAtkStrength)
{
	m_pWeapon->Active_Collider(fDamageRatio, iCollIndex, eHitType, eAtkStrength);
}

void CRebornerBigA::DeActive_CurrentWeaponCollider(_uint iCollIndex)
{
	m_pWeapon->DeActive_Collider();
}

void CRebornerBigA::Active_Effect(const _uint eType, _bool isLoop)
{
	m_pSwingEffect->Set_Loop(true);
}

void CRebornerBigA::DeActive_Effect(const _uint eType)
{
	m_pSwingEffect->Set_Loop(false);
}

HRESULT CRebornerBigA::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RebornerBigA"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */		//Body
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.4f, 0.3f, 0.35f);
	ColliderDesc.vCenter = _float3(0.3f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);
	m_pColliderBindMatrix[CT_BODY_UPPER] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(8);


	//LowerBody
	ColliderDesc.vExtents = _float3(0.4f, 0.3f, 0.3f);
	ColliderDesc.vCenter = _float3(0.1f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_BodyLower"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_BODY_LOWER]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_BODY_LOWER] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(5);

	//UPPERArmLeft
	ColliderDesc.vExtents = _float3(0.35f, 0.15f, 0.15f);
	ColliderDesc.vCenter = _float3(0.2f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_UpperArmLeft"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_UPPERARM_LEFT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_UPPERARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT) - 2);

	//UPPERArmRight
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_UpperArmRight"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_UPPERARM_RIGHT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_UPPERARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT) - 2);


	//LOWERArmLeft
	ColliderDesc.vExtents = _float3(0.4f, 0.15f, 0.15f);
	ColliderDesc.vCenter = _float3(0.4f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LowerArmLeft"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_LOWERARM_LEFT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_LOWERARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_LEFT) - 1);


	//LOWERArmRight
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LowerArmRight"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_LOWERARM_RIGHT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_LOWERARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT) - 1);



	//UPPERLegLeft
	ColliderDesc.vExtents = _float3(0.35f, 0.15f, 0.15f);
	ColliderDesc.vCenter = _float3(0.3f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_UpperLegLeft"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_UPPERLEG_LEFT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_UPPERLEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_FOOT_LEFT) - 2);


	//UPPERLegRight
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_UpperLegRight"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_UPPERLEG_RIGHT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_UPPERLEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_FOOT_RIGHT) - 2);


	//LOWERLegLeft
	ColliderDesc.vExtents = _float3(0.45f, 0.15f, 0.15f);
	ColliderDesc.vCenter = _float3(0.4f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LowerLegLeft"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_LOWERLEG_LEFT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_LOWERLEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_FOOT_LEFT) - 1);


	//LOWERLegRight
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LowerLegRight"), reinterpret_cast<CComponent**>(&m_EXCollider[CT_LOWERLEG_RIGHT]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderBindMatrix[CT_LOWERLEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_FOOT_RIGHT) - 1);

	for (_int i = 0; i < CT_END - 1; ++i)
	{
		m_EXCollider[i]->Set_Owner(this);
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
	CapsuleDesc.fRadius = 0.6f;
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

HRESULT CRebornerBigA::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};


	m_pFsmCom->Add_State(CState_RebornerBigA_Idle::Create(m_pFsmCom, this, IDLE, &Desc));
	m_pFsmCom->Add_State(CState_RebornerBigA_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_RebornerBigA_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));
	m_pFsmCom->Add_State(CState_RebornerBigA_Die::Create(m_pFsmCom, this, DIE, &Desc));
	m_pFsmCom->Add_State(CState_RebornerBigA_KnockBack::Create(m_pFsmCom, this, KNOCKBACK, &Desc));

	m_pFsmCom->Add_State(CState_RebornerBigA_GuardSting::Create(m_pFsmCom, this, GUARDSTING, &Desc));
	m_pFsmCom->Add_State(CState_RebornerBigA_RushSting::Create(m_pFsmCom, this, RUSHSTING, &Desc));
	m_pFsmCom->Add_State(CState_RebornerBigA_SlashTwice::Create(m_pFsmCom, this, SLASHTWICE, &Desc));
	m_pFsmCom->Add_State(CState_RebornerBigA_SlashJump::Create(m_pFsmCom, this, SLASHJUMP, &Desc));
	m_pFsmCom->Add_State(CState_RebornerBigA_SwingMultiple::Create(m_pFsmCom, this, SWINGMULTIPLE, &Desc));

	m_pFsmCom->Set_State(IDLE);

	return S_OK;



}

HRESULT CRebornerBigA::Ready_Weapon()
{
	CWeapon::MONSTER_WAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_WEAPON));	//Weapon_R

	WeaponDesc.pParentAtk = &m_eStat.fAtk;

	WeaponDesc.pMonster = this;

	m_pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_RebornerBigA_Stick"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon->Appear();

	m_pWeapon->DeActive_Collider();

	return S_OK;
}

HRESULT CRebornerBigA::Ready_Effect()
{
	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_WEAPON));


	m_pSwingEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Monster_Stick_Swing"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_pSwingEffect->Set_Loop(false);

	CDissolve_Effect::DISSOLVE_EFFECT_DESC DissolveDesc = {};
	DissolveDesc.fRotationPerSec = XMConvertToRadians(90.f);
	DissolveDesc.fSpeedPerSec = 1.f;
	DissolveDesc.iLevelIndex = LEVEL_GAMEPLAY;
	DissolveDesc.pTarget_ModelCom = m_pModelCom;
	DissolveDesc.pTarget_TransformCom = m_pTransformCom;
	DissolveDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_RebornerBigA_PowerAttack");

	m_pDissolveEffect = static_cast<CDissolve_PowerAttack*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_PowerAttack"), &DissolveDesc));
	if (nullptr == m_pDissolveEffect)
		return E_FAIL;

	return S_OK;
}

void CRebornerBigA::Update_Collider()
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
		, *(m_pColliderBindMatrix[CT_UPPERARM_LEFT]) * WorldMat);
	m_EXCollider[CT_UPPERARM_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERARM_RIGHT]) * WorldMat);
	m_EXCollider[CT_UPPERARM_RIGHT]->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERARM_LEFT]) * WorldMat);
	m_EXCollider[CT_LOWERARM_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERARM_RIGHT]) * WorldMat);
	m_EXCollider[CT_LOWERARM_RIGHT]->Update(&UpdateMat);

	
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERLEG_LEFT]) * WorldMat);
	m_EXCollider[CT_UPPERLEG_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERLEG_RIGHT]) * WorldMat);
	m_EXCollider[CT_UPPERLEG_RIGHT]->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERLEG_LEFT]) * WorldMat);
	m_EXCollider[CT_LOWERLEG_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERLEG_RIGHT]) * WorldMat);
	m_EXCollider[CT_LOWERLEG_RIGHT]->Update(&UpdateMat);
}

CRebornerBigA* CRebornerBigA::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRebornerBigA* pInstance = new CRebornerBigA(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRebornerBigA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CPawn* CRebornerBigA::Clone(void* pArg)
{
	CRebornerBigA* pInstance = new CRebornerBigA(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRebornerBigA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRebornerBigA::Free()
{
	for (_uint i = 0; i < CT_END - 1; ++i)
	{
		Safe_Release(m_EXCollider[i]);
	}

	if (true == m_isCloned)
	{
		m_pSwingEffect->Set_Cloned(false);
		Safe_Release(m_pSwingEffect);
	}

	__super::Free();

	Safe_Release(m_pWeapon);
	Safe_Release(m_pDissolveEffect);
}