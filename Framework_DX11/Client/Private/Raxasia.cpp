#include "stdafx.h"
#include "..\Public\Raxasia.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

#include "GameInterface_Controller.h"

#include "Effect_Manager.h"
#include "Effect_Container.h"

#pragma region Phase1

#include "State_RaxasiaP1_Idle.h"
#include "State_RaxasiaP1_Die.h"
#include "State_RaxasiaP1_Grogy.h"
#include "State_RaxasiaP1_HitFatal.h"
#include "State_RaxasiaP1_Walk.h"
#include "State_RaxasiaP1_Run.h"

#include "State_RaxasiaP1_DashUpper.h"
#include "State_RaxasiaP1_GroundSlash.h"
#include "State_RaxasiaP1_LinkedAttack.h"
#include "State_RaxasiaP1_RepetUpperSlash.h"
#include "State_RaxasiaP1_KickSting.h"
#include "State_RaxasiaP1_TripleSting.h"

#include "State_RaxasiaP1_Discharge.h"
#include "State_RaxasiaP1_JumpAttack.h"
#include "State_RaxasiaP1_SwingDown_Multiple.h"

#pragma endregion

#pragma region Phase2

#include "State_RaxasiaP2_Idle.h"
#include "State_RaxasiaP2_Die.h"
#include "State_RaxasiaP2_Grogy.h"
#include "State_RaxasiaP2_HitFatal.h"
#include "State_RaxasiaP2_Walk.h"
#include "State_RaxasiaP2_Run.h"

#include "State_RaxasiaP2_JumpStamp.h"
#include "State_RaxasiaP2_Running.h"
#include "State_RaxasiaP2_StepJump.h"
#include "State_RaxasiaP2_Tele.h"
#include "State_RaxasiaP2_TeleportAttack.h"
#include "State_RaxasiaP2_WaveSting.h"
#include "State_RaxasiaP2_Guard.h"
#include "State_RaxasiaP2_Guard_Push.h"
#include "State_RaxasiaP2_ChargeRush.h"
#include "State_RaxasiaP2_Declare_War.h"
#include "State_RaxasiaP2_ShieldRush.h"
#include "State_RaxasiaP2_ShieldLinked.h"


#include "State_RaxasiaP2_Running_Linked.h"
#include "State_RaxasiaP2_Running_Fury.h"
#include "State_RaxasiaP2_Tele_JumpLightning.h"
#include "State_RaxasiaP2_Tele_LinkedTel.h"

#pragma endregion

#include "Weapon.h"

CRaxasia::CRaxasia(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CRaxasia::CRaxasia(const CRaxasia& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CRaxasia::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRaxasia::Initialize(void* pArg)
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

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	//if (FAILED(Ready_Effects()))
	//	return E_FAIL;

	m_strObjectTag = TEXT("Monster");
	m_pColliderCom->Set_Owner(this);

	m_eStat.strName = TEXT("Raxasia");

	m_eStat.fHp = 200.f;
	m_eStat.fMaxHp = 200.f;
	m_eStat.fAtk = 10.f;
	m_eStat.fDefence = 5.f;
	m_eStat.fStemina = 100.f;
	m_eStat.bWeakness = false;
	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 50.f;

	m_eHitType = HIT_METAL;

	m_pWeapon->DeActive_Collider();
	m_pWeaponShield->DeActive_Collider();
	//m_pModelCom->SetUp_NextAnimation(12);

	GET_GAMEINTERFACE->Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_RAXASIA, this);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);

	return S_OK;
}

void CRaxasia::Priority_Update(_float fTimeDelta)
{

	__super::Set_UpTargetPos();
	m_pWeapon->Priority_Update(fTimeDelta);
	m_pWeaponShield->Priority_Update(fTimeDelta);

	if (!m_bDieState && m_eStat.fHp <= 0.f)
	{
		m_bDieState = true;
		//m_pFsmCom->Set_State(DIE);
		if (m_isChanged)
		{
			GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
		}

	}

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Priority_Update(fTimeDelta);
	}


}

void CRaxasia::Update(_float fTimeDelta)
{
	if (KEY_TAP(KEY::B))
	{
		ChangePhase();
	}
	//if (KEY_TAP(KEY::Z))
	//{
	//	m_Effects[P2_JUMPMAGIC]->Set_Loop(true);
	//}

	if (m_isPlayAnimation)
	{
		m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta), m_pTransformCom->Get_WorldMatrix());
	}
	else
	{
		m_vCurRootMove = _Vec3(0.f, 0.f, 0.f);
	}

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	if (!m_isCutScene)
		m_pFsmCom->Update(fTimeDelta);
	//else
	//	m_pCutSceneFsmCom->Update(fTimeDelta);


	for (_uint i = 0; i < PAWN_SOUND_END; ++i)
	{
		m_pSoundCom[i]->Update(fTimeDelta);
	}

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Update(fTimeDelta);
	}

	Update_Collider();

	m_pWeapon->Update(fTimeDelta);
	m_pWeaponShield->Update(fTimeDelta);

}

void CRaxasia::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRigidBodyCom->Update(fTimeDelta);

	m_pWeapon->Late_Update(fTimeDelta);
	m_pWeaponShield->Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Late_Update(fTimeDelta);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

	//for (_uint i = 0; i < EXCOLLIDER::COLLTYPE_END; ++i)
	//{
	//	m_pGameInstance->Add_ColliderList(m_EXCollider[i]);
	//}
}

HRESULT CRaxasia::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pWeapon->Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();

	for (_int i = 0; i < COLLTYPE_END; ++i)
	{
		m_EXCollider[i]->IsActive(true);
		m_EXCollider[i]->Render();
	}
#endif

	return S_OK;
}

void CRaxasia::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex, _uint iHitType, _uint iAtkStrength)
{
	if (!m_isChanged)
	{
		if (iCollIndex == 0)
		{
			m_pWeapon->Active_Collider(fDamageRatio, iCollIndex, iHitType, iAtkStrength);
		}
		else if (iCollIndex == 1)
		{
			m_pKickCollObj->Active_Collider(fDamageRatio, iCollIndex, iHitType, iAtkStrength);
		}
	}
	else
	{
		if (iCollIndex == 0)
		{
			m_pWeapon->Active_Collider(fDamageRatio, iCollIndex, iHitType, iAtkStrength);
		}
		else if (iCollIndex == 1)
		{
			m_pWeaponShield->Active_Collider(fDamageRatio, iCollIndex, iHitType, iAtkStrength);
		}
	}
}

void CRaxasia::DeActive_CurretnWeaponCollider(_uint iCollIndex)
{
	if (!m_isChanged)
	{
		if (iCollIndex == 0)
		{
			m_pWeapon->DeActive_Collider();
		}
		else if (iCollIndex == 1)
		{
			m_pKickCollObj->DeActive_Collider();
		}
	}
	else
	{
		if (iCollIndex == 0)
		{
			m_pWeapon->DeActive_Collider();
		}
		else if (iCollIndex == 1)
		{
			m_pWeaponShield->DeActive_Collider();
		}
	}
}

void CRaxasia::Active_Effect(const _uint eType, _bool isLoop)
{
	if (isLoop)
	{
		m_Effects[eType]->Set_Loop(true);
	}
	else
	{
		m_Effects[eType]->Reset_Effects();
	}
}

void CRaxasia::DeActive_Effect(const _uint eType)
{
	m_Effects[eType]->Set_Loop(false);
}

_bool CRaxasia::Get_EffectsLoop(const _uint eType)
{
	return m_Effects[eType]->Get_Dead();
}

void CRaxasia::Reset_WeaponOverlapCheck(_uint iCollIndex)
{
	m_pWeapon->Reset_OverLapCheck();
}

void CRaxasia::Change_WeaponAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration, _int iStartFrame, _bool bEitherChange, _bool bSameChange)
{
	m_pWeapon->ChangeAnimation(iAnimIndex, isLoop, fChangeDuration, iStartFrame, bEitherChange, bSameChange);
}

_bool CRaxasia::Get_WeaponAnimEnd(_int iAnimIndex)
{
	return m_pWeapon->is_EndAnim(iAnimIndex);
}

const _Matrix* CRaxasia::Get_WeaponBoneCombinedMat(_uint iBoneIndex)
{
	return m_pWeapon->Get_BoneCombinedMatrix(iBoneIndex);
}

const _Matrix* CRaxasia::Get_WeaponWorldMat()
{
	return m_pWeapon->Get_WorldMatrix_Ptr();
}

void CRaxasia::Start_CutScene(_uint iCutSceneNum)
{
	switch (iCutSceneNum)
	{
	case CUTSCENE_MEET:
		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE1];
		//m_pCutSceneFsmCom->Set_State(STATE_MEET);
		break;
	case CUTSCENE_P2:
		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE1];
		//m_pCutSceneFsmCom->Set_State(STATE_P2);
		break;
	case CUTSCENE_DIE:
		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE2];
		break;
	}

	m_isCutScene = true;
}

void CRaxasia::End_CutScene(_uint iCutSceneNum)
{
	if (m_pCutSceneFsmCom->Get_CurrentState() == STATE_MEET)
	{
		m_pModelCom = m_pP1ModelCom;
	}
	else if (m_pCutSceneFsmCom->Get_CurrentState() == STATE_P2)
	{
		ChangePhase();
	}
	m_isCutScene = false;
}

void CRaxasia::Change_Model(_uint iModelNum)
{
	m_pModelCom = m_pCutSceneModelCom[1];
}

HRESULT CRaxasia::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP1"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pP1ModelCom))))
		return E_FAIL;

	/* FOR.Com_ExtraModel */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP2"),
		TEXT("Com_ExtraModel"), reinterpret_cast<CComponent**>(&m_pExtraModelCom))))
		return E_FAIL;

#pragma region CutScene
	/* FOR.Com_Model_CustScene01 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_CutScene_P1"),
		TEXT("Com_Model_CustScene01"), reinterpret_cast<CComponent**>(&m_pCutSceneModelCom[MODEL_PHASE1]))))
		return E_FAIL;

	/* FOR.Com_Model_CustScene02 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_CutScene_P2"),
		TEXT("Com_Model_CustScene02"), reinterpret_cast<CComponent**>(&m_pCutSceneModelCom[MODEL_PHASE2]))))
		return E_FAIL;
#pragma endregion
	m_pModelCom = m_pP1ModelCom;

	/* FOR.Com_Collider */		//U_Body
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.3f, 0.24f, 0.3f);
	ColliderDesc.vCenter = _float3(0.3f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	//L_Body
	ColliderDesc.vExtents = _float3(0.25f, 0.24f, 0.22f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LowerBody"), reinterpret_cast<CComponent**>(&m_EXCollider[LOWERBODY]), &ColliderDesc)))
		return E_FAIL;

	//Leg
	{
		//U_LegL
		ColliderDesc.vExtents = _float3(0.3f, 0.15f, 0.15f);
		ColliderDesc.vCenter = _float3(0.15f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_ULL"), reinterpret_cast<CComponent**>(&m_EXCollider[U_LEG_LEFT]), &ColliderDesc)))
			return E_FAIL;

		//U_LegR
		ColliderDesc.vExtents = _float3(0.3f, 0.15f, 0.15f);
		ColliderDesc.vCenter = _float3(0.15f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_ULR"), reinterpret_cast<CComponent**>(&m_EXCollider[U_LEG_RIGHT]), &ColliderDesc)))
			return E_FAIL;

		//L_LegL
		ColliderDesc.vExtents = _float3(0.4f, 0.13f, 0.13f);
		ColliderDesc.vCenter = _float3(0.13f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_LLL"), reinterpret_cast<CComponent**>(&m_EXCollider[L_LEG_LEFT]), &ColliderDesc)))
			return E_FAIL;

		//L_LegR
		ColliderDesc.vExtents = _float3(0.4f, 0.13f, 0.13f);
		ColliderDesc.vCenter = _float3(0.13f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_LLR"), reinterpret_cast<CComponent**>(&m_EXCollider[L_LEG_RIGHT]), &ColliderDesc)))
			return E_FAIL;
	}

	//Arm
	{
		//U_ArmL
		ColliderDesc.vExtents = _float3(0.26f, 0.11f, 0.11f);
		ColliderDesc.vCenter = _float3(0.1f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_UAL"), reinterpret_cast<CComponent**>(&m_EXCollider[U_ARM_LEFT]), &ColliderDesc)))
			return E_FAIL;

		//U_ArmR
		ColliderDesc.vExtents = _float3(0.26f, 0.11f, 0.11f);
		ColliderDesc.vCenter = _float3(0.1f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_UAR"), reinterpret_cast<CComponent**>(&m_EXCollider[U_ARM_RIGHT]), &ColliderDesc)))
			return E_FAIL;

		//L_ArmL
		ColliderDesc.vExtents = _float3(0.29f, 0.1f, 0.1f);
		ColliderDesc.vCenter = _float3(0.14f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_LAL"), reinterpret_cast<CComponent**>(&m_EXCollider[L_ARM_LEFT]), &ColliderDesc)))
			return E_FAIL;

		//L_ArmR
		ColliderDesc.vExtents = _float3(0.29f, 0.1f, 0.1f);
		ColliderDesc.vCenter = _float3(0.14f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_LAR"), reinterpret_cast<CComponent**>(&m_EXCollider[L_ARM_RIGHT]), &ColliderDesc)))
			return E_FAIL;
	}

	m_pColliderBindMatrix[CT_UPPERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(8);
	m_pColliderBindMatrix[CT_LOWERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(6);

	m_pColliderBindMatrix[CT_UPPERLEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(78);
	m_pColliderBindMatrix[CT_UPPERLEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(85);
	m_pColliderBindMatrix[CT_LOWERLEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(79);
	m_pColliderBindMatrix[CT_LOWERLEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(86);

	m_pColliderBindMatrix[CT_UPPERARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(13);
	m_pColliderBindMatrix[CT_UPPERARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(39);
	m_pColliderBindMatrix[CT_LOWERARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(14);
	m_pColliderBindMatrix[CT_LOWERARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(40);


	for (_uint i = 0; i < COLLTYPE_END; ++i)
		m_EXCollider[i]->Set_Owner(this);


	// RegidBody Initiallize is end of Ready Com
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc{};
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerTransform = m_pTransformCom;
	RigidBodyDesc.pOwnerNavigation = m_pNavigationCom;
	RigidBodyDesc.isCapsule = true;

	RigidBodyDesc.pOwner = this;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestituion = 0.f;

	physX::GeometryCapsule CapsuleDesc;
	CapsuleDesc.fHeight = 2.5f;
	CapsuleDesc.fRadius = 0.45f;
	RigidBodyDesc.pGeometry = &CapsuleDesc;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;
	return S_OK;
	m_pRigidBodyCom->Set_Mass(100000.f);

	return S_OK;
}

HRESULT CRaxasia::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};



#pragma region Phase1_Fsm
	m_pFsmCom->Add_State(CState_RaxasiaP1_Idle::Create(m_pFsmCom, this, IDLE, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_Walk::Create(m_pFsmCom, this, WALK, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_Run::Create(m_pFsmCom, this, RUN, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_Die::Create(m_pFsmCom, this, DIE, &Desc));

	m_pFsmCom->Add_State(CState_RaxasiaP1_DashUpper::Create(m_pFsmCom, this, ATK_DASHUPPER, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_GroundSlash::Create(m_pFsmCom, this, ATK_GROUNDSLASH, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_LinkedAttack::Create(m_pFsmCom, this, ATK_LINKEDATTACK, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_RepetUpperSlash::Create(m_pFsmCom, this, ATK_REPETUPPERSLASH, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_KickSting::Create(m_pFsmCom, this, ATK_KICKSTING, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_TripleSting::Create(m_pFsmCom, this, ATK_TRIPLESTING, &Desc));

	m_pFsmCom->Add_State(CState_RaxasiaP1_Discharge::Create(m_pFsmCom, this, ATK_DISCHARGE, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_JumpAttack::Create(m_pFsmCom, this, ATK_JUMPATTACK, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_SwingDown_Multiple::Create(m_pFsmCom, this, ATK_SWINGDOWN_MULTIPLE, &Desc));


	m_pFsmCom->Set_State(IDLE);
#pragma endregion

#pragma region Phase2_Fsm

	/* FOR.Com_FSM */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_ExtraFSM"), reinterpret_cast<CComponent**>(&m_pExtraFsmCom))))
		return E_FAIL;

	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Idle::Create(m_pExtraFsmCom, this, IDLE, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Walk::Create(m_pExtraFsmCom, this, WALK, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Run::Create(m_pExtraFsmCom, this, RUN, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Grogy::Create(m_pExtraFsmCom, this, GROGY, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_HitFatal::Create(m_pExtraFsmCom, this, HITFATAL, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Die::Create(m_pExtraFsmCom, this, DIE, &Desc));

	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_JumpStamp::Create(m_pExtraFsmCom, this, ATKP2_JUMPSTAMP, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_StepJump::Create(m_pExtraFsmCom, this, ATKP2_STEPJUMP, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Running::Create(m_pExtraFsmCom, this, ATKP2_RUNNING, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Tele::Create(m_pExtraFsmCom, this, ATKP2_TELE, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_TeleportAttack::Create(m_pExtraFsmCom, this, ATKP2_TELEPORTATTACK, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_WaveSting::Create(m_pExtraFsmCom, this, ATKP2_WAVESTING, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Guard::Create(m_pExtraFsmCom, this, ATKP2_GUARD, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_ChargeRush::Create(m_pExtraFsmCom, this, ATKP2_CHARGERUSH, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Declare_War::Create(m_pExtraFsmCom, this, ATKP2_DECLAREWAR, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_ShieldRush::Create(m_pExtraFsmCom, this, ATKP2_SHIELDRUSH, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_ShieldLinked::Create(m_pExtraFsmCom, this, ATKP2_SHIELDLINKED, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Guard_Push::Create(m_pExtraFsmCom, this, ATKP2_GUARD_PUSH, &Desc));

	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Running_Linked::Create(m_pExtraFsmCom, this, ATKP2_RUNNING_LINKED, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Running_Fury::Create(m_pExtraFsmCom, this, ATKP2_RUNNING_FURY, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Tele_LinkedTel::Create(m_pExtraFsmCom, this, ATKP2_TELE_LINKEDTEL, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Tele_JumpLightning::Create(m_pExtraFsmCom, this, ATKP2_TELE_JUMPLIGHTNING, &Desc));

	m_pExtraFsmCom->Set_State(IDLE);
#pragma endregion

	return S_OK;
}

HRESULT CRaxasia::Ready_Weapon()
{
	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(62);	//Weapon_R

	WeaponDesc.pParentAtk = &m_eStat.fAtk;

	m_pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Raxasia_P1_Sword"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon->Appear();


	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(36);	//

	WeaponDesc.pParentAtk = &m_eStat.fAtk;

	m_pWeaponShield = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Raxasia_P1_Shield"), &WeaponDesc));
	if (nullptr == m_pWeaponShield)
		return E_FAIL;
	
	m_pWeaponShield->Appear();


	/* FOR.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc_Obj{};

	ColliderOBBDesc_Obj.vAngles = _float3(0.4f, 0.13f, 0.13f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.13f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vExtents = _float3(0.f, 0.f, 0.f);

	CColliderObject::COLIDEROBJECT_DESC Desc{};

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(86);
	Desc.pParentWorldMatrix = WeaponDesc.pParentWorldMatrix;
	Desc.pSocketBoneMatrix2 = WeaponDesc.pParentWorldMatrix;
	Desc.fDamageAmount = 100.f;

	m_pKickCollObj = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));



	return S_OK;
}

HRESULT CRaxasia::Ready_Effects()
{
	m_Effects.resize(EFFECT_END);

	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT));


	//for (auto& pEffect : m_Effects)
	//{
	//	pEffect->Set_Loop(false);
	//	pEffect->Set_Dead(true);
	//}


	return S_OK;
}

void CRaxasia::Update_Collider()
{
	_float4x4 UpdateMat{};

	_Matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERBODY]) * WorldMat);
	m_pColliderCom->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERBODY]) * WorldMat);
	m_EXCollider[LOWERBODY]->Update(&UpdateMat);


	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERLEG_LEFT]) * WorldMat);
	m_EXCollider[U_LEG_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERLEG_RIGHT]) * WorldMat);
	m_EXCollider[U_LEG_RIGHT]->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERLEG_LEFT]) * WorldMat);
	m_EXCollider[L_LEG_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERLEG_RIGHT]) * WorldMat);
	m_EXCollider[L_LEG_RIGHT]->Update(&UpdateMat);


	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERARM_LEFT]) * WorldMat);
	m_EXCollider[U_ARM_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERARM_RIGHT]) * WorldMat);
	m_EXCollider[U_ARM_RIGHT]->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERARM_LEFT]) * WorldMat);
	m_EXCollider[L_ARM_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERARM_RIGHT]) * WorldMat);
	m_EXCollider[L_ARM_RIGHT]->Update(&UpdateMat);


}

void CRaxasia::ChangePhase()
{

	if (m_pExtraModelCom == nullptr || m_pExtraFsmCom == nullptr)
	{
		return;
	}

	m_pFsmCom->Release_States();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pFsmCom);

	m_pModelCom = m_pExtraModelCom;
	m_pFsmCom = m_pExtraFsmCom;

	m_pExtraModelCom = nullptr;
	m_pExtraFsmCom = nullptr;

	m_pModelCom->SetUp_Animation(8, true);//P2 Idle
	m_pFsmCom->Set_State(IDLE);

	m_pModelCom->Play_Animation(0);


	//P2

	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(93);	//Weapon_R

	WeaponDesc.pParentAtk = &m_eStat.fAtk;

	m_pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Raxasia_P2_Sword"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return;

	m_pWeapon->Appear();


	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(36);	//Weapon_R UFB L

	WeaponDesc.pParentAtk = &m_eStat.fAtk;

	m_pWeaponShield = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Raxasia_P2_Shield"), &WeaponDesc));
	if (nullptr == m_pWeaponShield)
		return;

	m_pWeaponShield->Appear();


	m_pColliderBindMatrix[CT_UPPERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(8);//Spine 2
	m_pColliderBindMatrix[CT_LOWERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(6);//Spine

	m_pColliderBindMatrix[CT_UPPERLEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(124);//Tight
	m_pColliderBindMatrix[CT_UPPERLEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(129);
	m_pColliderBindMatrix[CT_LOWERLEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(125);//Calf
	m_pColliderBindMatrix[CT_LOWERLEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(130);

	m_pColliderBindMatrix[CT_UPPERARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(23);//UpperArm
	m_pColliderBindMatrix[CT_UPPERARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(70);
	m_pColliderBindMatrix[CT_LOWERARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(24);//ForeArm
	m_pColliderBindMatrix[CT_LOWERARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(71);

	m_eStat.fHp = 200.f;
	m_eStat.fMaxHp = 200.f;
	m_eStat.fAtk = 15.f;
	m_eStat.fDefence = 8.f;
	m_eStat.fStemina = 100.f;
	m_eStat.fMaxGrogyPoint = 50.f;
	m_eStat.fGrogyPoint = 0.f;

	m_isDead = false;
	m_isChanged = true;
}

CRaxasia* CRaxasia::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRaxasia* pInstance = new CRaxasia(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSimonManus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CRaxasia::Clone(void* pArg)
{
	CRaxasia* pInstance = new CRaxasia(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRaxasia"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRaxasia::Free()
{
	for (_uint i = 0; i < COLLTYPE_END; ++i)
	{
		Safe_Release(m_EXCollider[i]);
	}
	Safe_Release(m_pWeapon);
	Safe_Release(m_pWeaponShield);
	Safe_Release(m_pP1ModelCom);
	Safe_Release(m_pExtraModelCom);
	Safe_Release(m_pCutSceneModelCom[0]);
	Safe_Release(m_pCutSceneModelCom[1]);
	Safe_Release(m_pKickCollObj);

	if (true == m_isCloned)
	{
		for (auto& pEffect : m_Effects)
		{
			pEffect->Set_Cloned(false);
			Safe_Release(pEffect);
		}
		m_Effects.clear();
	}

	if (m_pExtraFsmCom != nullptr)
	{
		m_pExtraFsmCom->Release_States();
	}
	Safe_Release(m_pExtraFsmCom);

	if (m_pCutSceneFsmCom != nullptr)
	{
		m_pCutSceneFsmCom->Release_States();
	}
	Safe_Release(m_pCutSceneFsmCom);

	__super::Free();

}