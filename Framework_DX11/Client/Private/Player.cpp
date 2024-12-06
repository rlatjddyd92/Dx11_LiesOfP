#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "Layer.h"

#include "CutScene.h"

#include "Camera.h"
#include "Monster.h"
#include "Weapon.h"
#include "Weapon_Scissor.h"

#include "Ladder.h"
#include "Lift_Floor.h"
#include "TreasureBox.h"
#include "Stargazer.h"
#include "SteppingStone.h"
#include "LastDoor.h"

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
#include "State_Player_Stargazer.h"
#include "State_Player_Teleport.h"
#include "State_Player_Grinder.h"
#include "State_Player_GetUp.h"

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
#include "State_Player_Rapier_Fablel.h"
#include "State_Player_Rapier_Fatal.h"
#include "State_Player_Rapier_ParryAttack.h"

#include "State_Player_Flame_LAttack00.h"
#include "State_Player_Flame_LAttack01.h"
#include "State_Player_Flame_RAttack00.h"
#include "State_Player_Flame_RAttack01.h"
#include "State_Player_Flame_Charge00.h"
#include "State_Player_Flame_Charge01.h"
#include "State_Player_Flame_Fable.h"
#include "State_Player_Flame_Fatal.h"
#include "State_Player_Flame_ParryAttack.h"

#include "State_Player_Scissor_LAttack00.h"
#include "State_Player_Scissor_LAttack01.h"
#include "State_Player_Scissor_RAttack00.h"
#include "State_Player_Scissor_RAttack01.h"
#include "State_Player_Scissor_Charge00.h"
#include "State_Player_Scissor_Charge01.h"
#include "State_Player_Scissor_Buff.h"
#include "State_Player_Scissor_Fable0.h"
#include "State_Player_Scissor_Fable1.h"
#include "State_Player_Scissor_Fable2.h"
#include "State_Player_Scissor_Fatal.h"

#include "State_Player_OpenSophiaDoor.h"
#include "State_Player_SophiaWalk.h"
#include "State_Player_SophiaHand.h"
#include "State_Player_SophiaHandEnd.h"

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

	//m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, 1030); // 계단 옆 별바라기
	//m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, 772); //긴사다리
	m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, 427); //짧은사다리
	//m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, 341); //아래엘베
	//m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, 440); //상자랑 장애물
	//m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, 1066); // 순간이동 790
	//m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, 800); // 소피아 방

	m_strObjectTag = TEXT("Player");

	// 24-11-27 김성용
	// 스탯 구조체 생성 
	m_tPlayer_Stat = new STAT_INFO;
	m_tPlayer_Stat_Adjust = new STAT_INFO;
	m_tPlayer_Ability = new ABILITY_INFO;

	m_tPlayer_Stat_Adjust->Reset_Zero();

	// 24-11-27 김성용
	// 게임 인터페이스와 연결을 위해 추가 
	GET_GAMEINTERFACE->Input_Player_Pointer(this);

	m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.f);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	// 24-12-05 김성용
	// 아이템 연동 
	
	/*
	UI 및 아이템 작동 시스템 
	1. UI, 아이템 매니저(인벤,장비) 시스템은 모든 오브젝트보다 나중에 업데이트 진행 
	2. 이번 프레임에 플레이어에서 조작한 내용에 대한 피드백은 다음 프레임에 받을 수 있음
	3. 이에 따라 Priority_Update에서 가장 먼저 지난 프레임의 내용을 받아 보도록 코드 작성 
	*/


	// ★ 아래 내용은 지난 프레임의 조작 결과임에 주의 ★
	// 지난 프레임의 아이템 사용 기록 가져오기 
	list<SPECIAL_ITEM>& Item_Type_list = GET_GAMEINTERFACE->Get_LastFrame_UsingItem_Info();
	
	_wstring strTest{};

	if (!Item_Type_list.empty())
	{
		for (auto& iter : Item_Type_list)
		{
			switch (iter)
			{
			case SPECIAL_ITEM::SP_PULSE_BATTERY: // 펄스 전지, 충전 수치 따로 존재하며 사용 시, 체력 회복 O
				strTest += TEXT("펄스 전지, ");
				break;
			case SPECIAL_ITEM::SP_DUSTCAT: // 고양이 가루, 기척 감소 
				strTest += TEXT("고양이 가루, ");
				break;
			case SPECIAL_ITEM::SP_GRINDER: // 그라인더, 무기 내구도 증가 O 
				strTest += TEXT("그라인더, ");
				break;
			case SPECIAL_ITEM::SP_LAMP: // 모나드의 등불, 주변 밝기 증가 
				strTest += TEXT("모나드의 등불, ");
				break;
			case SPECIAL_ITEM::SP_TELEPOT: // 문페이즈 회중시계, 순간이동 O
				strTest += TEXT("문페이즈 회중시계, ");
				break;
			case SPECIAL_ITEM::SP_RESISTANCE: // 속성 저항 앰플, 속성 저항 증가 O
				strTest += TEXT("속성 저항 앰플, ");
				break;
			case SPECIAL_ITEM::SP_PURIFICATION: // 속성 정화 앰플, 디버프 제거 O
				strTest += TEXT("속성 정화 앰플, ");
				break;
			case SPECIAL_ITEM::SP_DEAD: // 최후의 수단, 사망 O
				strTest += TEXT("최후의 수단, ");
				break;
			case SPECIAL_ITEM::SP_GRANADE: // 클러스터 수류탄 O
				strTest += TEXT("클러스터 수류탄, ");
				break;
			case SPECIAL_ITEM::SP_THERMITE: // 테르밋 O
				strTest += TEXT("테르밋, ");
				break;
			case SPECIAL_ITEM::SP_THROW_BATTERY: // 투척용 전지 O 
				strTest += TEXT("투척용 전지, ");
				break;
			case SPECIAL_ITEM::SP_END: // 특수 기능이 없는 아이템을 사용했거나 사용한 아이템이 없음
				break;
			default:
				break;
			}
		}

		GET_GAMEINTERFACE->Show_Script(strTest, TEXT("none"), 1.f);
	}

	STAT_INFO* pTest_Adjust = m_tPlayer_Stat_Adjust;
	ABILITY_INFO* pTest_Ability = m_tPlayer_Ability;
	

	if (m_isGuard)
	{
		m_fGuardTime += fTimeDelta;
	}

	Update_Stat(fTimeDelta);

	if (KEY_TAP(KEY::WHEELBUTTON))
		LockOnOff();

	if(nullptr != m_pWeapon[m_eWeaponType])
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
	if (m_isCollisionMonster)
	{
		m_vCurRootMove = _Vec3(0.f, 0.f, 0.f);
	}


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

	if (nullptr != m_pWeapon[m_eWeaponType])
		m_pWeapon[m_eWeaponType]->Update(fTimeDelta);

	if (KEY_TAP(KEY::L))
	{
		Change_State(RAPIER_PARRYATTACK);
		//Change_State(ITEMGET);
		//Calc_DamageGain(5.f, m_pTransformCom->Get_WorldMatrix().Forward() + m_pTransformCom->Get_WorldMatrix().Translation());
	}
	if (KEY_TAP(KEY::K))
	{
		//Change_State(RAPIER_PARRYATTACK);
		Change_State(RAPIER_FATAL);
		//Change_State(SOPHIA_WALK);
		//Calc_DamageGain(5.f, m_pTransformCom->Get_WorldMatrix().Forward() + m_pTransformCom->Get_WorldMatrix().Translation());
	}
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	if(m_isLockOn && m_pFsmCom->Get_CurrentState() != OH_SPRINT && m_pFsmCom->Get_CurrentState() != TH_SPRINT)
		m_pTransformCom->LookAt_NoHeight(m_pTargetMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION));

	if (nullptr != m_pWeapon[m_eWeaponType])
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
	if (FAILED(m_pDissloveTexture->Bind_ShadeResource(m_pShaderCom, "g_DissloveTexture", 0)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissloveRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	_float fResetDisslove = -1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &fResetDisslove, sizeof(_float))))
		return E_FAIL;

	//if (nullptr != m_pWeapon[m_eWeaponType])
	//	if (FAILED(m_pWeapon[m_eWeaponType]->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Render_LightDepth()
{
	if (!m_pGameInstance->Get_IsOnShadow())
		return S_OK;

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
	if (pOther->Get_Tag() == TEXT("Monster"))
	{
		m_isCollisionMonster = true;
	}
}

void CPlayer::OnCollisionStay(CGameObject* pOther)
{
	CollisionStay_IntercObj(pOther);
	
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
	if (pOther->Get_Tag() == TEXT("Monster"))
	{
		CMonster* pMonster = dynamic_cast<CMonster*>(pOther);

		// 페이탈 어택 여부 정하기
		
		m_isCollisionMonster = true;
	}
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
	if (pOther->Get_Tag() == TEXT("Monster"))
	{
		m_isCollisionMonster = false;
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
	// 24-12-05 김성용
	// 착용 중인 무기가 1개일 때 무기 변경 관련 기능을 모두 막기 위한 함수
	// 값이 false면 무기가 1개
	if (!GET_GAMEINTERFACE->Get_CanSwitch_Weapon())
		return;

	m_pWeapon[m_eWeaponType]->Appear();
}

_uint CPlayer::Change_WeaponType()
{
	// 24-12-05 김성용
	// 착용 중인 무기가 1개일 때 무기 변경 관련 기능을 모두 막기 위한 함수
	// 값이 false면 무기가 1개
	if (!GET_GAMEINTERFACE->Get_CanSwitch_Weapon())
		return m_eWeaponType;

	m_pWeapon[m_eWeaponType]->Disappear();

	// 24-12-05 김성용	
	// 무기 락 적용 
	GET_GAMEINTERFACE->SetWeaponLock(true);

	// 인벤과 플레이어 모델 연동 
	m_eWeaponType = WEAPON_TYPE(GET_GAMEINTERFACE->Get_Weapon_Model_Index());

	// 원래 코드 
	//m_eWeaponType = WEAPON_TYPE((m_eWeaponType + 1) % WEP_END);


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

void CPlayer::Set_WeaponStrength(ATTACK_STRENGTH eStrength)
{
	m_pWeapon[m_eWeaponType]->Set_AttackStrength(eStrength);
}

void CPlayer::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iHandIndex)
{
	m_pWeapon[m_eWeaponType]->Active_Collider(fDamageRatio, iHandIndex);
}

void CPlayer::DeActive_CurretnWeaponCollider(_uint iHandIndex)
{
	m_pWeapon[m_eWeaponType]->DeActive_Collider(iHandIndex);
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

void CPlayer::Change_CameraMode(CPlayerCamera::CAMERA_MODE eMode)
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
			_Vec3 vTargetPos = m_pTargetMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			_Vec3 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			_float fLength = (vTargetPos - vPlayerPos).Length();
			if (fLength < 20.f)
			{
				m_isLockOn = true;
			}
			else
			{
				m_pTargetMonster = nullptr;
				m_isLockOn = false;
			}
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

	vector<CGameObject*> ObjectList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"))->Get_ObjectList();

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

void CPlayer::Active_Effect(const EFFECT_TYPE& eType, _bool isLoop)
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

void CPlayer::DeActive_Effect(const EFFECT_TYPE& eType)
{
	m_Effects[eType]->Set_Loop(false);
}


_bool CPlayer::Calc_DamageGain(_float fAtkDmg, _Vec3 vHitPos, _uint iHitType, _uint iAttackStrength, CGameObject* pAttacker)
{
	if (fAtkDmg <= 0.f)
		return false;

	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");

	if (m_isInvicible) // 회피 상태
	{
		if (ATK_STRONG == iAttackStrength)
		{
			Damaged(fAtkDmg, vHitPos);
		}
	}
	else if (m_isGuard)	// 가드 상태
	{
		//퍼펙트 가드
		if (m_fGuardTime < 0.17f)
		{
			if (nullptr != pAttacker)
			{
				_wstring strObjecTag = pAttacker->Get_Tag();

				if (strObjecTag == TEXT("Monster"))
				{
					CMonster* pMonster = dynamic_cast<CMonster*>(pAttacker);
					pMonster->Increase_GroggyPoint(10.f);

				}
				else if (strObjecTag == TEXT("MonsterWeapon"))
				{
					CWeapon* pWeapon = dynamic_cast<CWeapon*>(pAttacker);
					CMonster* pMonster = pWeapon->Get_Monster();
					pMonster->Increase_GroggyPoint(10.f);
				}
			}

			m_pEffect_Manager->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_PerfectGuard"), pParetnMatrix, pSocketBoneMatrix);
			m_pGameInstance->Start_TimerLack(TEXT("Timer_60"), 0.001f, 0.6f);
		}
		else
		{
			if (ATK_STRONG == iAttackStrength)
			{
				Damaged(fAtkDmg, vHitPos);
			}
			else
			{
				m_pEffect_Manager->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Guard"), pParetnMatrix, pSocketBoneMatrix);
				m_tPlayer_Stat->vGauge_Hp.x = max(0.f, m_tPlayer_Stat->vGauge_Hp.x - fAtkDmg * 0.7f);
				if (m_tPlayer_Stat->vGauge_Hp.y - m_tPlayer_Stat->vGauge_Hp.x > 100.f)
					m_tPlayer_Stat->vGauge_Hp.y = max(m_tPlayer_Stat->vGauge_Hp.x, m_tPlayer_Stat->vGauge_Hp.y - fAtkDmg * 0.7f);
			}
		}

		if (ATK_STRONG != iAttackStrength)
		{
			if (m_eWeaponType < 2)
				m_pFsmCom->Change_State(OH_GUARDHIT, &vHitPos);
			else
				m_pFsmCom->Change_State(TH_GUARDHIT, &vHitPos);

			Choice_GuardSound(0, 0, false);
		}
	}
	else if (m_isParry)
	{
		if (m_eWeaponType == WEP_RAPIER)
		{
			Decrease_Region();
			m_pFsmCom->Change_State(RAPIER_PARRYATTACK);
		}
		else if (m_eWeaponType == WEP_FLAME)
		{
			Decrease_Region();
			m_pFsmCom->Change_State(FLAME_PARRYATTACK);
		}
	}
	else //그냥 맞음
	{
		Damaged(fAtkDmg, vHitPos);
	}

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
	m_tPlayer_Stat->vGauge_Stamina.y = m_tPlayer_Stat->vGauge_Stamina.x;

	m_fStaminaRecoveryTime = 1.3f;	// 1.3f초 후에 회복

	return true;
}

_bool CPlayer::Check_Region_Fable01()
{
	_int iRegionCount = 3;

	if (WEP_SCISSOR == m_eWeaponType)
		iRegionCount = 1;

	_float fCurretnRegion = m_tPlayer_Stat->vGauge_Region.x - m_tPlayer_Stat->fRegion_Interval * iRegionCount;

	if (fCurretnRegion < 0.f)
		return false;

	return true;
}

_bool CPlayer::Check_Region_Fable02()
{
	_int iRegionCount = 3;

	if (WEP_SCISSOR != m_eWeaponType)
		iRegionCount = 1;

	_float fCurretnRegion = m_tPlayer_Stat->vGauge_Region.x - m_tPlayer_Stat->fRegion_Interval * iRegionCount;

	if (fCurretnRegion < 0.f)
		return false;

	return true;
}

void CPlayer::Decrease_Region(_uint iRegionCount)
{
	_float fCurretnRegion = m_tPlayer_Stat->vGauge_Region.x;
	for (_uint i = 0; i < iRegionCount; ++i)
	{
		fCurretnRegion -= m_tPlayer_Stat->fRegion_Interval;

		if (fCurretnRegion < 0.f)
			return;
	}

	//m_tPlayer_Stat->vGauge_Region.x = fCurretnRegion;
	//m_tPlayer_Stat->vGauge_Region.y = m_tPlayer_Stat->vGauge_Region.x;
}

void CPlayer::Recovery_Region(_float fAmount)
{
	m_tPlayer_Stat->vGauge_Region.x = min(m_tPlayer_Stat->vGauge_Region.x + fAmount, m_tPlayer_Stat->vGauge_Region.z);
	m_tPlayer_Stat->vGauge_Region.y = m_tPlayer_Stat->vGauge_Region.x;
}


void CPlayer::Damaged(_float fAtkDmg, _Vec3 vHitPos)
{
	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");

	CState_Player_Hit::HIT_DESC HitDesc{};
	HitDesc.vHitPos = vHitPos;
	HitDesc.isDown = false;

	if (fAtkDmg >= 30.f)
		HitDesc.isDown = true;

	m_tPlayer_Stat->vGauge_Hp.x = max(0.f, m_tPlayer_Stat->vGauge_Hp.x - fAtkDmg);
	if(m_tPlayer_Stat->vGauge_Hp.y - m_tPlayer_Stat->vGauge_Hp.x > 100.f)
		m_tPlayer_Stat->vGauge_Hp.y = max(m_tPlayer_Stat->vGauge_Hp.x, m_tPlayer_Stat->vGauge_Hp.y - fAtkDmg);

	if (fAtkDmg > 0.f)
		m_pFsmCom->Change_State(HIT, &HitDesc);

	m_pEffect_Manager->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Impact"), pParetnMatrix, pSocketBoneMatrix);
}

void CPlayer::Update_Stat(_float fTimeDelta)
{
#pragma region 스테미나
	if (m_fStaminaRecoveryTime > 0.f)
	{
		m_fStaminaRecoveryTime -= fTimeDelta;
	}
	else if (m_fStaminaRecoveryTime <= 0.f)
	{
		m_tPlayer_Stat->vGauge_Stamina.x = min(m_tPlayer_Stat->vGauge_Stamina.x + 10.f * fTimeDelta, m_tPlayer_Stat->vGauge_Stamina.z);
	}
#pragma endregion


}

void CPlayer::CollisionStay_IntercObj(CGameObject* pGameObject)
{
	if (m_pFsmCom->Get_CurrentState() >= 100)
		return;

	if (pGameObject->Get_Tag() == TEXT("Ladder"))
	{
		if (GET_GAMEINTERFACE->Action_InterAction(TEXT("사다리를 탄다")))
		{
			m_pFsmCom->Change_State(LADDER, pGameObject);
		}
	}
	else if (pGameObject->Get_Tag() == TEXT("Lift_Controller"))
	{
		if (GET_GAMEINTERFACE->Action_InterAction(TEXT("엘리베이터를 동작시킨다")))
		{
			m_pFsmCom->Change_State(LIFT, pGameObject);
		}
	}
	else if (pGameObject->Get_Tag() == TEXT("Lift_Floor"))
	{
		m_pRigidBodyCom->Set_IsOnCell(false);
		m_pRigidBodyCom->Set_IsLockCell(false);

		if (dynamic_cast<CLift_Floor*>(pGameObject)->Get_isMoving())
		{
			m_pRigidBodyCom->Set_Gravity(true);
		}
		else
		{
			m_pRigidBodyCom->Set_Gravity(false);
		}
	}
	else if (pGameObject->Get_Tag() == TEXT("TreasureBox"))
	{
		CTreasureBox* pTreasureBox = dynamic_cast<CTreasureBox*>(pGameObject);

		if (!pTreasureBox->Get_IsOpen() && GET_GAMEINTERFACE->Action_InterAction(TEXT("상자를 연다")))
		{
			dynamic_cast<CTreasureBox*>(pGameObject)->Set_IsOpen(true);
			m_pFsmCom->Change_State(CHEST, pGameObject);
		}
	}
	else if (pGameObject->Get_Tag() == TEXT("Stargazer"))
	{
		CStargazer* pStargazer = dynamic_cast<CStargazer*>(pGameObject);

		if (pStargazer->Get_IsOpend())
		{
			if (GET_GAMEINTERFACE->Action_InterAction(TEXT("별바라기를 사용한다")))
			{
				/*dynamic_cast<CTreasureBox*>(pGameObject)->Set_IsOpen(true);
				m_pFsmCom->Change_State(CHEST, pGameObject);*/
			}
		}
		else
		{
			if (GET_GAMEINTERFACE->Action_InterAction(TEXT("별바라기를 복구시킨다")))
			{
				m_pFsmCom->Change_State(STARGAZER, pGameObject);
			}
		}

	}
	else if (pGameObject->Get_Tag() == TEXT("SteppingStone"))
	{
		CSteppingStone* pSteppingStone = dynamic_cast<CSteppingStone*>(pGameObject);
		if (GET_GAMEINTERFACE->Action_InterAction(TEXT("최후의 장소로...")))
		{
			m_pFsmCom->Change_State(TELEPORT, pSteppingStone);
		}
	}
	else if (pGameObject->Get_Tag() == TEXT("LastDoor"))
	{
		CLastDoor* pLastDoor = dynamic_cast<CLastDoor*>(pGameObject);
		if (GET_GAMEINTERFACE->Action_InterAction(TEXT("문을 연다.")))
		{
			pLastDoor->Set_IsOpen(true);
			dynamic_cast<CCutScene*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), SOPHIA_ENTER))->Start_Play();
			m_pFsmCom->Change_State(SOPHIA_DOOR_OPEN, pLastDoor);
		}
	}
}

void CPlayer::Choice_GuardSound(_uint iAttackStrength, _uint iHitType, _bool isPerfect)
{
	_wstring strSoundKey{};
	_wstring strWAV = TEXT(".wav");
	_tchar szBuffer[10];

	_int iRand = rand() % 3 + 1;

	if (isPerfect)	// 퍼펙트 가드
	{
		_itow_s(iRand, szBuffer, 10);
		strSoundKey = TEXT("SE_PC_SK_GetHit_Sword_PerfectGuard_Metal_Fury_0");
		strSoundKey = strSoundKey + szBuffer + strWAV;
	}
	else
	{
		if (ATK_STRONG == iAttackStrength)
		{
			switch (iHitType)
			{
			case HIT_CARCASS:
				_itow_s(iRand, szBuffer, 10);
				strSoundKey = TEXT("SE_PC_SK_GetHit_Guard_CarcassSkin_L_0");
				strSoundKey = strSoundKey + szBuffer + strWAV;
				break;

			case HIT_METAL:
				_itow_s(iRand, szBuffer, 10);
				strSoundKey = TEXT("SE_PC_SK_GetHit_Guard_Metal_L_0");
				strSoundKey = strSoundKey + szBuffer + strWAV;
				break;

			case HIT_FIRE:
				_itow_s(iRand, szBuffer, 10);
				strSoundKey = TEXT("SE_PC_SK_GetHit_M_Guard_Fire_0");
				strSoundKey = strSoundKey + szBuffer + strWAV;
				break;

			case HIT_ELECTRIC:
				_itow_s(iRand, szBuffer, 10);
				strSoundKey = TEXT("SE_PC_SK_GetHit_M_Guard_Electric_0");
				strSoundKey = strSoundKey + szBuffer + strWAV;
				break;

			default:
				break;
			}
		}
		else if (ATK_NORMAL == iAttackStrength)
		{
			switch (iHitType)
			{
			case HIT_CARCASS:
				_itow_s(iRand, szBuffer, 10);
				strSoundKey = TEXT("SE_PC_SK_GetHit_Guard_CarcassSkin_M_0");
				strSoundKey = strSoundKey + szBuffer + strWAV;
				break;

			case HIT_METAL:
				iRand = rand() % 6 + 1;
				_itow_s(iRand, szBuffer, 10);
				strSoundKey = TEXT("SE_PC_SK_GetHit_M_Guard_Metal_0");
				strSoundKey = strSoundKey + szBuffer + strWAV;
				break;

			case HIT_FIRE:
				_itow_s(iRand, szBuffer, 10);
				strSoundKey = TEXT("SE_PC_SK_GetHit_M_Guard_Fire_0");
				strSoundKey = strSoundKey + szBuffer + strWAV;
				break;

			case HIT_ELECTRIC:
				_itow_s(iRand, szBuffer, 10);
				strSoundKey = TEXT("SE_PC_SK_GetHit_M_Guard_Electric_0");
				strSoundKey = strSoundKey + szBuffer + strWAV;
				break;

			default:
				break;
			}
		}
		else if (ATK_WEAK == iAttackStrength)
		{
			switch (iHitType)
			{
			case HIT_CARCASS:
				_itow_s(iRand, szBuffer, 10);
				strSoundKey = TEXT("SE_PC_SK_GetHit_Guard_CarcassSkin_S_0");
				strSoundKey = strSoundKey + szBuffer + strWAV;
				break;

			case HIT_METAL:
				strSoundKey = TEXT("SE_PC_SK_GetHit_S_Guard_Metal_01");
				break;

			case HIT_FIRE:
				_itow_s(iRand, szBuffer, 10);
				strSoundKey = TEXT("SE_PC_SK_GetHit_M_Guard_Fire_0");
				strSoundKey = strSoundKey + szBuffer + strWAV;
				break;

			case HIT_ELECTRIC:
				_itow_s(iRand, szBuffer, 10);
				strSoundKey = TEXT("SE_PC_SK_GetHit_M_Guard_Electric_0");
				strSoundKey = strSoundKey + szBuffer + strWAV;
				break;

			default:
				break;
			}
		}
	}

	m_pSoundCom[PAWN_SOUND_EFFECT1]->Play2D(strSoundKey.c_str(), &g_fEffectVolume);
}


HRESULT CPlayer::Ready_Weapon()
{
	CWeapon::PLAYER_WAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");
	WeaponDesc.pPlayer = this;

	m_pWeapon[WEP_RAPIER] = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Rapier"), &WeaponDesc));
	if (nullptr == m_pWeapon[WEP_RAPIER])
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

	m_pDissloveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Anim/Player/T_DissolveMask_A.dds"), 1);

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
	RigidBodyDesc.isCapsule = true;

	RigidBodyDesc.pOwner = this;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestituion = 0.f;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	physX::GeometryCapsule CapsuleDesc;
	CapsuleDesc.fHeight = 1.f;
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
	m_pFsmCom->Add_State(CState_Player_Stargazer::Create(m_pFsmCom, this, STARGAZER, &Desc));
	m_pFsmCom->Add_State(CState_Player_Teleport::Create(m_pFsmCom, this, TELEPORT, &Desc));
	m_pFsmCom->Add_State(CState_Player_Grinder::Create(m_pFsmCom, this, GRINDER, &Desc));
	m_pFsmCom->Add_State(CState_Player_GetUp::Create(m_pFsmCom, this, GETUP, &Desc));

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
	m_pFsmCom->Add_State(CState_Player_Rapier_Fable::Create(m_pFsmCom, this, RAPIER_FABALE, &Desc));	// F 페이블아츠
	m_pFsmCom->Add_State(CState_Player_Rapier_Fatal::Create(m_pFsmCom, this, RAPIER_FATAL, &Desc));	// 페이탈
	m_pFsmCom->Add_State(CState_Player_Rapier_ParryAttack::Create(m_pFsmCom, this, RAPIER_PARRYATTACK, &Desc));
	// Shift + F 패리 어택

	m_pFsmCom->Add_State(CState_Player_Flame_LAttack00::Create(m_pFsmCom, this, FLAME_LATTACK0, &Desc));	// 좌클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Flame_LAttack01::Create(m_pFsmCom, this, FLAME_LATTACK1, &Desc));	// 좌클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Flame_RAttack00::Create(m_pFsmCom, this, FLAME_RATTACK0, &Desc));	// 우클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Flame_RAttack01::Create(m_pFsmCom, this, FLAME_RATTACK1, &Desc));	// 우클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Flame_Charge00::Create(m_pFsmCom, this, FLAME_CHARGE0, &Desc));	// 우클릭 차지 공격1
	m_pFsmCom->Add_State(CState_Player_Flame_Charge01::Create(m_pFsmCom, this, FLAME_CHARGE1, &Desc));	// 우클릭 차지 공격2
	m_pFsmCom->Add_State(CState_Player_Flame_Fable::Create(m_pFsmCom, this, FLAME_FABLE, &Desc)); // 페이블 아츠
	m_pFsmCom->Add_State(CState_Player_Flame_Fatal::Create(m_pFsmCom, this, FLAME_FATAL, &Desc)); //페이탈
	m_pFsmCom->Add_State(CState_Player_Flame_ParryAttack::Create(m_pFsmCom, this, FLAME_PARRYATTACK, &Desc)); 

	m_pFsmCom->Add_State(CState_Player_Scissor_LAttack00::Create(m_pFsmCom, this, SCISSOR_LATTACK0, &Desc));	// 좌클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Scissor_LAttack01::Create(m_pFsmCom, this, SCISSOR_LATTACK1, &Desc));	// 좌클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Scissor_RAttack00::Create(m_pFsmCom, this, SCISSOR_RATTACK0, &Desc));	// 우클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Scissor_RAttack01::Create(m_pFsmCom, this, SCISSOR_RATTACK1, &Desc));	// 우클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Scissor_Charge00::Create(m_pFsmCom, this, SCISSOR_CHARGE0, &Desc));	// 우클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Scissor_Charge01::Create(m_pFsmCom, this, SCISSOR_CHARGE1, &Desc));	// 우클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Scissor_Buff::Create(m_pFsmCom, this, SCISSOR_BUFF, &Desc));	// 버프
	m_pFsmCom->Add_State(CState_Player_Scissor_Fable0::Create(m_pFsmCom, this, SCISSOR_FABAL0, &Desc));	// 콤보1
	m_pFsmCom->Add_State(CState_Player_Scissor_Fable1::Create(m_pFsmCom, this, SCISSOR_FABAL1, &Desc));	// 콤보2
	m_pFsmCom->Add_State(CState_Player_Scissor_Fable2::Create(m_pFsmCom, this, SCISSOR_FABAL2, &Desc));	// 콤보3
	m_pFsmCom->Add_State(CState_Player_Scissor_Fatal::Create(m_pFsmCom, this, SCISSOR_FATAL, &Desc));	// 페이탈

	/* 소피아 컷신 */
	m_pFsmCom->Add_State(CState_Player_OpenSophiaDoor::Create(m_pFsmCom, this, SOPHIA_DOOR_OPEN, &Desc));
	m_pFsmCom->Add_State(CState_Player_SophiaWalk::Create(m_pFsmCom, this, SOPHIA_WALK, &Desc));
	m_pFsmCom->Add_State(CState_Player_SophiaHand::Create(m_pFsmCom, this, SOPHIA_HAND, &Desc));
	m_pFsmCom->Add_State(CState_Player_SophiaHandEnd::Create(m_pFsmCom, this, SOPHIA_HANDEND, &Desc));

	/* 락사시아 컷신 */

	/* 시몬 마누스 컷신 */

	m_pFsmCom->Set_State(OH_IDLE);

	return S_OK;
}

HRESULT CPlayer::Ready_Effect()
{
	m_pEffect_Manager = CEffect_Manager::Get_Instance();
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;
	Safe_AddRef(m_pEffect_Manager);

	m_Effects.resize(EFFECT_END);

	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");

	m_Effects[EFFECT_RAPIER_TRAIL_FIRST] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Attack_Rapier_StormStab_First"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_RAPIER_TRAIL_SECOND] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Attack_Rapier_StormStab_Second"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("Bn_L_ForeTwist");
	m_Effects[EFFECT_GRIND] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Grind"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.11f, -0.12f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_L");
	m_Effects[EFFECT_HEAL] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Potion"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));


	return S_OK;
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

	Safe_Release(m_pDissloveTexture);

	if(true == m_isCloned)
	{
		for (auto& pEffect : m_Effects)
		{
			pEffect->Set_Cloned(false);
			Safe_Release(pEffect);
		}
		m_Effects.clear();
		Safe_Release(m_pEffect_Manager);
	}

	for (_uint i = 0; i < WEP_END; ++i)
	{
		Safe_Release(m_pWeapon[i]);
	}

	// 24-11-27 김성용
	// 스탯 구조체 제거 
	Safe_Delete(m_tPlayer_Stat);
	Safe_Delete(m_tPlayer_Stat_Adjust);
	Safe_Delete(m_tPlayer_Ability);
	
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
}

