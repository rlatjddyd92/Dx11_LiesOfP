#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "Layer.h"

#include "Camera.h"
#include "Weapon.h"
#include "Weapon_Scissor.h"

#include "Ladder.h"
#include "Lift_Floor.h"

#include "Effect_Manager.h"
#include "Effect_Container.h"

#include "State_Player_Hit.h"
#include "State_Player_Parry.h"
#include "State_Player_Heal.h"
#include "State_Player_ChangeWeapon.h"
#include "State_Player_Ladder.h"
#include "State_Player_Lift.h"
#include "State_Player_Chest.h"
#include "State_Player_ItemGet.h"

#include "State_Player_OH_Idle.h"
#include "State_Player_OH_Walk.h"
#include "State_Player_OH_Run.h"
#include "State_Player_OH_Sprint.h"
#include "State_Player_OH_Guard.h"
#include "State_Player_OH_GuardHit.h"
#include "State_Player_OH_Dash.h"

#include "State_Player_TH_Idle.h"
#include "State_Player_TH_Walk.h"
#include "State_Player_TH_Run.h"
#include "State_Player_TH_Sprint.h"
#include "State_Player_TH_Guard.h"
#include "State_Player_TH_GuardHit.h"
#include "State_Player_TH_Dash.h"

#include "State_Player_Rapier_LAttack00.h"
#include "State_Player_Rapier_LAttack01.h"
#include "State_Player_Rapier_RAttack00.h"
#include "State_Player_Rapier_Charge.h"
#include "State_Player_Rapier_Fatal.h"

#include "State_Player_Flame_LAttack00.h"
#include "State_Player_Flame_LAttack01.h"
#include "State_Player_Flame_RAttack00.h"
#include "State_Player_Flame_RAttack01.h"
#include "State_Player_Flame_Charge00.h"
#include "State_Player_Flame_Charge01.h"

#include "State_Player_Scissor_LAttack00.h"
#include "State_Player_Scissor_LAttack01.h"
#include "State_Player_Scissor_RAttack00.h"
#include "State_Player_Scissor_RAttack01.h"
#include "State_Player_Scissor_Charge00.h"
#include "State_Player_Scissor_Charge01.h"
#include "State_Player_Scissor_Buff.h"
#include "State_Player_Scissor_Fatal0.h"
#include "State_Player_Scissor_Fatal1.h"
#include "State_Player_Scissor_Fatal2.h"

// 24-11-27 김성용
// 게임 인터페이스와 연결을 위해 추가 
#include "GameInterface_Controller.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPawn{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer & Prototype)
	: CPawn{ Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECT_DESC		Desc{};

	Desc.fSpeedPerSec = 10.0f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	//m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, 427);
	//m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, 341); //307

	m_strObjectTag = TEXT("Player");

	// 24-11-27 김성용
	// 스탯 구조체 생성 
	m_tPlayer_Stat = new STAT_INFO;

	// 24-11-27 김성용
	// 게임 인터페이스와 연결을 위해 추가 
	GET_GAMEINTERFACE->Input_Player_Pointer(this);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	if (m_isGuard)
	{
		m_fGuardTime += fTimeDelta;
	}

	if (KEY_TAP(KEY::WHEELBUTTON))
		LockOnOff();

	m_pWeapon[m_eWeaponType]->Priority_Update(fTimeDelta);

	for (auto& pEffect : m_Effects)
	{
		if(!pEffect->Get_Dead())
			pEffect->Priority_Update(fTimeDelta);
	}
}

void CPlayer::Update(_float fTimeDelta)
{
	m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta), m_pTransformCom->Get_WorldMatrix());

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	m_pFsmCom->Update(fTimeDelta);

	m_pRigidBodyCom->Update(fTimeDelta);

	for (_uint i = 0; i < PAWN_SOUND_END; ++i)
	{
		m_pSoundCom[i]->Update(fTimeDelta);
	}

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Update(fTimeDelta);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
	m_pGameInstance->Add_ColliderList(m_pColliderCom);

	m_pWeapon[m_eWeaponType]->Update(fTimeDelta);

	if (KEY_TAP(KEY::L))
	{
		Change_State(ITEMGET);
		//Calc_DamageGain(5.f, m_pTransformCom->Get_WorldMatrix().Forward() + m_pTransformCom->Get_WorldMatrix().Translation());
	}
	else if (KEY_TAP(KEY::K))
	{
		Change_State(CHEST);
		//Calc_DamageGain(5.f, m_pTransformCom->Get_WorldMatrix().Forward() + m_pTransformCom->Get_WorldMatrix().Translation());
	}
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	if(m_isLockOn && m_pFsmCom->Get_CurrentState() != OH_SPRINT && m_pFsmCom->Get_CurrentState() != TH_SPRINT)
		m_pTransformCom->LookAt_NoHeight(m_pTargetMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION));

	m_pWeapon[m_eWeaponType]->Late_Update(fTimeDelta);

	//업데이트에서 생성하니 업데이트 이전에 비우기
	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Late_Update(fTimeDelta);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
	m_pGameInstance->Add_DebugObject(m_pNavigationCom);
#endif

}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pWeapon[m_eWeaponType]->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Render_LightDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeViewMatrix", m_pGameInstance->Get_CascadeViewMatirx(), 3)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProjMatrix", m_pGameInstance->Get_CascadeProjMatirx(), 3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);


		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CPlayer::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Ladder"))
	{
		m_isLadderEnd = true;
	}
	/*if (pOther->Get_Tag() == TEXT("Monster"))
	{
		_Vec3 vColDir = pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vColDir.y = 0.f;
		vColDir.Normalize();
		_Vec3 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPlayerPos += (vColDir * -0.07f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
	}*/
}

void CPlayer::OnCollisionStay(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Ladder"))
	{
		if (KEY_TAP(KEY::E))
		{
			m_pFsmCom->Change_State(LADDER, pOther);
		}
	}
	if (pOther->Get_Tag() == TEXT("Lift_Controller"))
	{
		if (KEY_TAP(KEY::E))
		{
			m_pFsmCom->Change_State(LIFT, pOther);
		}
	}
	if (pOther->Get_Tag() == TEXT("Lift_Floor"))
	{
		m_pRigidBodyCom->Set_IsOnCell(false);
		m_pRigidBodyCom->Set_IsLockCell(false);
		//_Vec3 vLiftFloorPos = pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		//_Vec3 vLiftFloorPrevPos = dynamic_cast<CLift_Floor*>(pOther)->Get_PrevPos();

		if (dynamic_cast<CLift_Floor*>(pOther)->Get_isMoving())
		{
			m_pRigidBodyCom->Set_Gravity(true);
		}
		else
		{
			m_pRigidBodyCom->Set_Gravity(false);
		}
	}

	/*if (pOther->Get_Tag() == TEXT("Monster"))
	{
		_Vec3 vColNormal = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pOther->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		vColNormal.y = 0.f;
		vColNormal.Normalize();

		_Vec3 vVel = m_pRigidBodyCom->Get_Velocity();
		_Vec3 vProjected = vVel.Dot(vColNormal) * vColNormal;

		vVel = vVel = vProjected;
		m_pRigidBodyCom->Set_Velocity(vVel);
	}*/
}

void CPlayer::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Ladder"))
	{
		m_isLadderEnd = false;
	}
	if (pOther->Get_Tag() == TEXT("Lift_Floor"))
	{

		if (!dynamic_cast<CLift_Floor*>(pOther)->Get_isMoving())
		{
			m_pRigidBodyCom->Set_IsOnCell(true);
			m_pRigidBodyCom->Set_IsLockCell(true);
			m_pNavigationCom->Research_Cell((_Vec3)m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}
	}
}

void CPlayer::Move_Dir(_Vec4 vDir, _float fTimeDelta, _bool isTurn)
{
	if(isTurn)
		m_pTransformCom->LookAt_Lerp_NoHeight(vDir, 30.0f, fTimeDelta);

	m_pRigidBodyCom->Set_Velocity((_Vec3(vDir * m_fMoveSpeed)));
}

_Vec4 CPlayer::Calculate_Direction_Straight()
{
	_vector vCameraLook = m_pPlayerCamera->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK);
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::STATE_UP));
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vCameraLook));

	_vector vDir = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	return vDir;
}

_Vec4 CPlayer::Calculate_Direction_Right()
{
	_vector vCameraLook = XMVector3Normalize(m_pPlayerCamera->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::STATE_UP));

	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vCameraLook));

	return vRight;
}

void CPlayer::Change_Weapon()
{
	m_pWeapon[m_eWeaponType]->Appear();
}

_uint CPlayer::Change_WeaponType()
{
	m_pWeapon[m_eWeaponType]->Disappear();
	m_eWeaponType = WEAPON_TYPE((m_eWeaponType + 1) % WEP_END);
	return m_eWeaponType;
}

void CPlayer::Appear_Weapon()
{
	m_pWeapon[m_eWeaponType]->Appear();
}

void CPlayer::Disappear_Weapon()
{
	m_pWeapon[m_eWeaponType]->Disappear();
}

void CPlayer::Seperate_Scissor()
{
	if (WEP_SCISSOR != m_eWeaponType)
		return;

	dynamic_cast<CWeapon_Scissor*>(m_pWeapon[WEP_SCISSOR])->Change_SeperateMode();
}

void CPlayer::Combine_Scissor()
{
	if (WEP_SCISSOR != m_eWeaponType)
		return;

	dynamic_cast<CWeapon_Scissor*>(m_pWeapon[WEP_SCISSOR])->Change_CombineMode();
}

void CPlayer::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iHandIndex)
{
	m_pWeapon[m_eWeaponType]->Active_Collider(fDamageRatio, iHandIndex);
}

void CPlayer::DeActive_CurretnWeaponCollider(_uint iHandIndex)
{
	m_pWeapon[m_eWeaponType]->DeActive_Collider(iHandIndex);
}

void CPlayer::Chnage_CameraMode(CPlayerCamera::CAMERA_MODE eMode)
{
	m_pPlayerCamera->Change_Mode(eMode);
}


void CPlayer::LockOnOff()
{
	if (!m_isLockOn)
	{
		m_pTargetMonster = Find_TargetMonster();
		if (nullptr == m_pTargetMonster)
		{
			// 카메라를 플레이어 뒤로 회전
		}
		else
		{
			m_isLockOn = true;
		}
	}
	else
	{
		m_isLockOn = false;
	}
}

CPawn* CPlayer::Find_TargetMonster()
{
	if (nullptr == m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster")))
		return nullptr;

	list<CGameObject*> ObjectList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"))->Get_ObjectList();

	if (ObjectList.size() <= 0)
		return nullptr;

	CGameObject* pNearObject = { nullptr };

	_vector vDistance = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_float  fDistance = -1.f;

	for (auto& Object : ObjectList)
	{
		if (!Object->IsActive())
			continue;

		_vector vOwnerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vFindObjPos = Object->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		vDistance = (vOwnerPos - vFindObjPos);

		if (fDistance < 0.f)
		{
			pNearObject = Object;

			XMStoreFloat(&fDistance, XMVector3Length(vDistance));
			continue;
		}

		_float fNewDistance;
		XMStoreFloat(&fNewDistance, XMVector3Length(vDistance));

		if (fNewDistance < fDistance)
		{
			pNearObject = Object;
			fDistance = fNewDistance;
		}
	}

	return dynamic_cast<CPawn*>(pNearObject);
}

void CPlayer::Play_CurrentWeaponSound(const _uint iType, const TCHAR* pSoundKey, _uint iHandIndex)
{
	m_pWeapon[iType]->Play_Sound(CWeapon::WEP_SOUND_TYPE(iType), pSoundKey, iHandIndex);
}

void CPlayer::Active_Effect(const EFFECT_TYPE& eType)
{
	m_Effects[eType]->Set_Loop(true);
}
void CPlayer::DeActive_Effect(const EFFECT_TYPE& eType)
{
	m_Effects[eType]->Set_Loop(false);

}

HRESULT CPlayer::Ready_Weapon()
{
	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");

	m_pWeapon[WEP_RAPIER] = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Rapier"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon[WEP_FLAME] = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_FlameSword"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;


	WeaponDesc.pSocketBoneMatrix2 = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_L");
	m_pWeapon[WEP_SCISSOR] = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Scissor"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	Change_Weapon();

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.3f, 0.9f, 0.3f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	// 항상 마지막에 생성하기
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc{};
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerTransform = m_pTransformCom;
	RigidBodyDesc.pOwnerNavigation = m_pNavigationCom;

	RigidBodyDesc.pOwner = this;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestituion = 0.f;
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

HRESULT CPlayer::Ready_FSM()
{
	/* FOR.Com_FSM */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFsmCom))))
		return E_FAIL;

	FSM_INIT_DESC Desc{};

	Desc.pPrevTrackPos = &m_PrevTrackPos;


	m_pFsmCom->Add_State(CState_Player_Hit::Create(m_pFsmCom, this, HIT, &Desc));
	m_pFsmCom->Add_State(CState_Player_Parry::Create(m_pFsmCom, this, PARRY, &Desc));
	m_pFsmCom->Add_State(CState_Player_Heal::Create(m_pFsmCom, this, HEAL, &Desc));
	m_pFsmCom->Add_State(CState_Player_ChangeWeapon::Create(m_pFsmCom, this, CHANGEWEP, &Desc)); 
	m_pFsmCom->Add_State(CState_Player_Ladder::Create(m_pFsmCom, this, LADDER, &Desc));
	m_pFsmCom->Add_State(CState_Player_Lift::Create(m_pFsmCom, this, LIFT, &Desc));
	m_pFsmCom->Add_State(CState_Player_Chest::Create(m_pFsmCom, this, CHEST, &Desc));
	m_pFsmCom->Add_State(CState_Player_ItemGet::Create(m_pFsmCom, this, ITEMGET, &Desc));

	m_pFsmCom->Add_State(CState_Player_OH_Idle::Create(m_pFsmCom, this, OH_IDLE, &Desc));
	m_pFsmCom->Add_State(CState_Player_OH_Walk::Create(m_pFsmCom, this, OH_WALK, &Desc)); 
	m_pFsmCom->Add_State(CState_Player_OH_Run::Create(m_pFsmCom, this, OH_RUN, &Desc));
	m_pFsmCom->Add_State(CState_Player_OH_Sprint::Create(m_pFsmCom, this, OH_SPRINT, &Desc));
	m_pFsmCom->Add_State(CState_Player_OH_Guard::Create(m_pFsmCom, this, OH_GUARD, &Desc));
	m_pFsmCom->Add_State(CState_Player_OH_GuardHit::Create(m_pFsmCom, this, OH_GUARDHIT, &Desc));
	m_pFsmCom->Add_State(CState_Player_OH_Dash::Create(m_pFsmCom, this, OH_DASH, &Desc));

	m_pFsmCom->Add_State(CState_Player_TH_Idle::Create(m_pFsmCom, this, TH_IDLE, &Desc));
	m_pFsmCom->Add_State(CState_Player_TH_Walk::Create(m_pFsmCom, this, TH_WALK, &Desc));
	m_pFsmCom->Add_State(CState_Player_TH_Run::Create(m_pFsmCom, this, TH_RUN, &Desc));
	m_pFsmCom->Add_State(CState_Player_TH_Sprint::Create(m_pFsmCom, this, TH_SPRINT, &Desc));
	m_pFsmCom->Add_State(CState_Player_TH_Guard::Create(m_pFsmCom, this, TH_GUARD, &Desc));
	m_pFsmCom->Add_State(CState_Player_TH_GuardHit::Create(m_pFsmCom, this, TH_GUARDHIT, &Desc));
	m_pFsmCom->Add_State(CState_Player_TH_Dash::Create(m_pFsmCom, this, TH_DASH, &Desc));

	m_pFsmCom->Add_State(CState_Player_Rapier_LAttack00::Create(m_pFsmCom, this, RAPIER_LATTACK0, &Desc));	// 좌클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Rapier_LAttack01::Create(m_pFsmCom, this, RAPIER_LATTACK1, &Desc));	// 좌클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Rapier_RAttack00::Create(m_pFsmCom, this, RAPIER_RATTACK0, &Desc));	// 우클릭 공격
	m_pFsmCom->Add_State(CState_Player_Rapier_Charge::Create(m_pFsmCom, this, RAPIER_CHARGE, &Desc));	// 우클릭 차지공격
	m_pFsmCom->Add_State(CState_Player_Rapier_Fatal::Create(m_pFsmCom, this, RAPIER_FATAL, &Desc));	// F 페이탈아츠
	// Shift + F 패리 어택

	m_pFsmCom->Add_State(CState_Player_Flame_LAttack00::Create(m_pFsmCom, this, FLAME_LATTACK0, &Desc));	// 좌클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Flame_LAttack01::Create(m_pFsmCom, this, FLAME_LATTACK1, &Desc));	// 좌클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Flame_RAttack00::Create(m_pFsmCom, this, FLAME_RATTACK0, &Desc));	// 우클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Flame_RAttack01::Create(m_pFsmCom, this, FLAME_RATTACK1, &Desc));	// 우클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Flame_Charge00::Create(m_pFsmCom, this, FLAME_CHARGE0, &Desc));	// 우클릭 차지 공격1
	m_pFsmCom->Add_State(CState_Player_Flame_Charge01::Create(m_pFsmCom, this, FLAME_CHARGE1, &Desc));	// 우클릭 차지 공격2
	//페이탈 아츠
		// Shift + F 패리 어택

	m_pFsmCom->Add_State(CState_Player_Scissor_LAttack00::Create(m_pFsmCom, this, SCISSOR_LATTACK0, &Desc));	// 좌클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Scissor_LAttack01::Create(m_pFsmCom, this, SCISSOR_LATTACK1, &Desc));	// 좌클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Scissor_RAttack00::Create(m_pFsmCom, this, SCISSOR_RATTACK0, &Desc));	// 우클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Scissor_RAttack01::Create(m_pFsmCom, this, SCISSOR_RATTACK1, &Desc));	// 우클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Scissor_Charge00::Create(m_pFsmCom, this, SCISSOR_CHARGE0, &Desc));	// 우클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Scissor_Charge01::Create(m_pFsmCom, this, SCISSOR_CHARGE1, &Desc));	// 우클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Scissor_Buff::Create(m_pFsmCom, this, SCISSOR_BUFF, &Desc));	// 버프
	m_pFsmCom->Add_State(CState_Player_Scissor_Fatal0::Create(m_pFsmCom, this, SCISSOR_FATAL0, &Desc));	// 콤보1
	m_pFsmCom->Add_State(CState_Player_Scissor_Fatal1::Create(m_pFsmCom, this, SCISSOR_FATAL1, &Desc));	// 콤보2
	m_pFsmCom->Add_State(CState_Player_Scissor_Fatal2::Create(m_pFsmCom, this, SCISSOR_FATAL2, &Desc));	// 콤보3

	m_pFsmCom->Set_State(OH_IDLE);

	return S_OK;
}

HRESULT CPlayer::Ready_Effect()
{
	m_Effects.resize(EFFECT_END);

	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");

	m_Effects[RAPIER_TRAIL_FIRST] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Player_Attack_Rapier_StormStab_First"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 180.f, 0.f), _Vec3(1.f, 1.f, 1.f));;

	m_Effects[RAPIER_TRAIL_SECOND] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Player_Attack_Rapier_StormStab_Second"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 180.f, 0.f), _Vec3(1.f, 1.f, 1.f));;


	return S_OK;
}

_bool CPlayer::Calc_DamageGain(_float fAtkDmg, _Vec3 vHitPos)
{
	if (fAtkDmg <= 0.f)
		return false;

	if (m_isGuard)
	{
		//퍼펙트 가드
		if (m_fGuardTime < 0.15f)
		{
			int a = 0;
		}
		else
		{
			m_tPlayer_Stat->vGauge_Hp.x = max(0.f, m_tPlayer_Stat->vGauge_Hp.x - fAtkDmg * 0.7f);
		}

		if (m_eWeaponType < 2)
			m_pFsmCom->Change_State(OH_GUARDHIT, &vHitPos);
		else
			m_pFsmCom->Change_State(TH_GUARDHIT, &vHitPos);
	}
	else
	{
		m_tPlayer_Stat->vGauge_Hp.x = max(0.f, m_tPlayer_Stat->vGauge_Hp.x - fAtkDmg);
		m_tPlayer_Stat->vGauge_Hp.y = m_tPlayer_Stat->vGauge_Hp.x;
		if (fAtkDmg > 0.f)
			m_pFsmCom->Change_State(HIT, &vHitPos);
	}
	//if (m_eStat.fHp <= 0.f)
	//	m_pFsmCom->Change_State(HIT);

	return true;
}

_bool CPlayer::Decrease_Stamina(_float fAmount)
{
	m_tPlayer_Stat->vGauge_Stamina.x -= fAmount;
	if (m_tPlayer_Stat->vGauge_Stamina.x < 0.f)
	{
		m_tPlayer_Stat->vGauge_Stamina.x = 0.f;
		return false;
	}

	return true;
}

_bool CPlayer::Decrease_Region(_uint iRegionCount)
{
	_float fCurretnRegion = m_tPlayer_Stat->vGauge_Region.x;
	for (_uint i = 0; i < iRegionCount; ++i)
	{
		fCurretnRegion -= m_tPlayer_Stat->fRegion_Interval * iRegionCount;

		if (fCurretnRegion < 0.f)
			return false;
	}

	return true;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPawn* CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& pEffect : m_Effects)
	{
		Safe_Release(pEffect);
	}
	m_Effects.clear();

	for (_uint i = 0; i < WEP_END; ++i)
	{
		Safe_Release(m_pWeapon[i]);
	}

	// 24-11-27 김성용
	// 스탯 구조체 제거 
	Safe_Delete(m_tPlayer_Stat);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
}

