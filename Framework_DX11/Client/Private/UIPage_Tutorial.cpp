#include "stdafx.h"
#include "..\Public\UIPage_Tutorial.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

#include "UITutorial_Info.h"
#include "UITutorial_Guide.h"
#include "UITutorial_Timing.h"
#include "UITutorial_Result.h"
#include "UITutorial_Popup.h"

#include "Monster_Training01.h"
#include "Monster_Training02.h"

#include "Player.h"

CUIPage_Tutorial::CUIPage_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Tutorial::CUIPage_Tutorial(const CUIPage_Tutorial& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Tutorial::Initialize_Prototype()
{
	// 모든 UIPage는 클론 안 함, 여기서 모든 세팅 끝내기 

	UI_DESC			Desc{};

	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_vecPageAction.resize(_int(PAGEACTION::ACTION_END));

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPage_Tutorial::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Tutorial::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Tutorial::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Tutorial::Late_Update(_float fTimeDelta)
{
	if (m_bWaitStart)
	{
		m_fStartTime -= fTimeDelta * 0.3f;
		if (m_fStartTime < 0.f)
		{
			m_bWaitStart = false;
			CloseAction();
			GET_GAMEINTERFACE->Get_Player()->Off_LockOn();
			GET_GAMEINTERFACE->Get_Player()->Get_Navigation()->Move_to_Cell(GET_GAMEINTERFACE->Get_Player()->Get_RigidBody(), 774);
			GET_GAMEINTERFACE->Input_Achievment_Data(16, 1);
			GET_GAMEINTERFACE->Fade_In();
		}
	}
	else if (m_fStartTime < 0.f)
	{
		m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;
		return;
	}
		


	if (m_iNowChapter == -1)
		Next_Chapter();

	if (m_bWating_NewChapter == true)
		if (KEY_TAP(KEY::ENTER))
			Next_Chapter();

	Check_Mission_Complete(fTimeDelta);

	Update_Tutorial_Info(fTimeDelta);
	Update_Tutorial_Guide(fTimeDelta);
	Update_Tutorial_Timing(fTimeDelta);
	Update_Tutorial_Result(fTimeDelta);
	Update_Tutorial_Popup(fTimeDelta);
	Update_Tutorial_NowChapter(fTimeDelta);

	__super::Late_Update(fTimeDelta);

	m_bPlayer_StateChanged = false;
}

HRESULT CUIPage_Tutorial::Render()
{
	return S_OK;
}

void CUIPage_Tutorial::OpenAction()
{
	__super::OpenAction();
	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(true, m_pSharedPonter_AttackMonster);
	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(true, m_pSharedPonter_NormalMonster);
}

void CUIPage_Tutorial::CloseAction()
{
	__super::CloseAction();
	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, m_pSharedPonter_AttackMonster);
	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, m_pSharedPonter_NormalMonster);
}

CHECK_MOUSE CUIPage_Tutorial::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

void CUIPage_Tutorial::Close_Popup()
{
	m_bPopupOpen = false;
}

void CUIPage_Tutorial::Open_Popup()
{
	m_bPopupOpen = true;
}

void CUIPage_Tutorial::Update_Tutorial_Info(_float fTimeDelta)
{
	for (_int i = 0; i < 4; ++i)
	{
		if (m_iNow_Index + i >= m_vecTutorial_MissionData.size())
			break;

		if (m_vecTutorial_MissionData[m_iNow_Index + i]->iCapterIndex != m_iNowChapter)
			break;

		m_vecMission_Info[i]->Update_Info(*m_vecTutorial_MissionData[m_iNow_Index + i], fTimeDelta);
	}
}

void CUIPage_Tutorial::Update_Tutorial_Guide(_float fTimeDelta)
{
	if (m_iNowChapter >= m_vecTutorial_ChapterData.size())
		return;

	if (m_bWating_NewChapter == false)
		m_pGuide->Update_Guide(*m_vecTutorial_ChapterData[m_iNowChapter], fTimeDelta);
	else if (m_iNowChapter == m_vecTutorial_ChapterData.size() - 1)
		m_pGuide->Update_Guide_Wating(true);
	else
		m_pGuide->Update_Guide_Wating();
}

void CUIPage_Tutorial::Update_Tutorial_Timing(_float fTimeDelta)
{
	if (KEY_TAP(KEY::M))
		ShowTiming(KEY::LSHIFT, 2.f);

	if (m_pTiming->Update_Timing(fTimeDelta) == false)
	{
		for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TIMING)]->PartIndexlist)
			m_vecPart[iter]->bRender = false;
	}
}

void CUIPage_Tutorial::Update_Tutorial_Result(_float fTimeDelta)
{



}

void CUIPage_Tutorial::Update_Tutorial_Popup(_float fTimeDelta)
{
	if (m_bNewChapter == false)
		return;

	if (m_bWating_NewChapter == false)
	if (KEY_TAP(KEY::ENTER))
	{
		m_bPopupOpen = !m_bPopupOpen;
	}

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POPUP)]->PartIndexlist)
		m_vecPart[iter]->bRender = m_bPopupOpen;
	
	if (m_bPopupOpen == true) m_pPopup->Update_Popup(*m_vecTutorial_ChapterData[m_iNowChapter], fTimeDelta);

	m_bNewChapter = m_bPopupOpen;

	m_bWating_NewChapter = false;
}

void CUIPage_Tutorial::Update_Tutorial_NowChapter(_float fTimeDelta)
{
	if (m_iNowChapter >= m_vecTutorial_ChapterData.size())
		return;

	list<_int>::iterator iter = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_INFO_SCORE)]->PartIndexlist.begin();

	++iter;
	m_vecPart[*iter]->strText = to_wstring(m_iNowChapter + 1);
	++iter;
	m_vecPart[*iter]->strText = m_vecTutorial_ChapterData[m_iNowChapter]->strTitle;
}

void CUIPage_Tutorial::ShowTiming(KEY eKey, _float fTime)
{
	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TIMING)]->PartIndexlist)
		m_vecPart[iter]->bRender = true;

	m_pTiming->Start_Timing(eKey, fTime);
}

void CUIPage_Tutorial::Test_Control(_float fTimeDelta)
{
	if ((m_bNewChapter == false) && (KEY_TAP(KEY::N)))
	{
		_int iIndex = m_iNow_Index;

		while (1)
		{
			if (iIndex >= m_vecTutorial_MissionData.size())
			{
				CloseAction();
				break;
			}
			else if (m_vecTutorial_MissionData[iIndex]->iCapterIndex != m_iNowChapter)
			{
				Next_Chapter();
				break;
			}
			else if (m_vecTutorial_MissionData[iIndex]->bComplete == false)
			{
				m_vecTutorial_MissionData[iIndex]->bComplete = true;
				break;
			}
			else
				++iIndex;
		}
	}
}

void CUIPage_Tutorial::Check_Mission_Complete(_float fTimeDelta)
{
	if (GET_GAMEINTERFACE->IsGamePause() == true)
		return;

	switch (m_iNowChapter)
	{
	case 0: 
		Check_Player_Move(fTimeDelta); // 이동하기 미션
		break;
	case 1:
		Check_Player_Dash(fTimeDelta); // 회피 미션
		break;
	case 2:
		Check_Player_Focus();
		Check_Player_Lbutton_Attack(); // 일반 공격 
		break;
	case 3:
		Check_Player_RButton_Attack(fTimeDelta); // 강공격
		break;
	case 4:
		Check_Dummy_Weakness(); // 그로기 성공
		Check_Dummy_Get_FatalAttack(); // 페이탈 어택 성공
		break;
	case 5:
		Check_Player_Fable_Art(false); // 유저 페이블 아츠 사용
		break;
	case 6:
		Check_Player_Switch_Weapon(); // 무기 변경
		break;
	case 7:
		Check_Player_Guard(fTimeDelta); // 가드, 퍼펙트 가드
		break;
	case 8:
		Check_Player_Resion_Arm_Start(); // 리전 암 전개
		break;
	case 9:
		Check_Player_Resion_Arm_Skill(); // 리전 암 막기, 리전 암 공격
		break;
	case 10:
		Check_Player_Resion_Arm_Counter(); // 리전 암 카운터 
		break;
	default:
		break;
	}

	_int iIndex = m_iNow_Index;
	_bool bIsComplete = true;

	while (1)
	{
		if ((m_bNewChapter == false) && (KEY_TAP(KEY::N)))
		{
			Next_Chapter();
			break;
		}

		if (m_vecTutorial_MissionData[iIndex]->fNow >= m_vecTutorial_MissionData[iIndex]->fGoal)
		{
			m_vecTutorial_MissionData[iIndex]->fNow = m_vecTutorial_MissionData[iIndex]->fGoal;
			m_vecTutorial_MissionData[iIndex]->bComplete = true;
		}

		if (m_vecTutorial_MissionData[iIndex]->bComplete == false)
			bIsComplete = false;

		++iIndex;

		if (iIndex >= m_vecTutorial_MissionData.size())
		{
			if (bIsComplete == true) Set_Waiting_NextChapter();
			break;
		}
		else if (m_vecTutorial_MissionData[iIndex]->iCapterIndex != m_iNowChapter)
		{
			if (bIsComplete == true) Set_Waiting_NextChapter();
			break;
		}
	}
}

void CUIPage_Tutorial::Check_Player_Move(_float fTimeDelta)
{
	_int iPlayerState = GET_GAMEINTERFACE->Get_Player()->Get_Fsm()->Get_CurrentState();

	_int iWalk = 0;

	iWalk += CPlayer::PLAYER_STATE::OH_WALK == iPlayerState ? 1 : 0;
	iWalk += CPlayer::PLAYER_STATE::OH_RUN == iPlayerState ? 1 : 0;
	iWalk += CPlayer::PLAYER_STATE::OH_SPRINT == iPlayerState ? 1 : 0;
	iWalk += CPlayer::PLAYER_STATE::TH_WALK == iPlayerState ? 1 : 0;
	iWalk += CPlayer::PLAYER_STATE::TH_RUN == iPlayerState ? 1 : 0;
	iWalk += CPlayer::PLAYER_STATE::TH_SPRINT == iPlayerState ? 1 : 0;

	if (iWalk == 0)
		return;

	if (m_vecTutorial_MissionData[m_iNow_Index + 0]->bComplete == false)
		if ((KEY_TAP(KEY::W)) || (KEY_HOLD(KEY::W)))
			m_vecTutorial_MissionData[m_iNow_Index + 0]->fNow += fTimeDelta * 3.f;

	if (m_vecTutorial_MissionData[m_iNow_Index + 1]->bComplete == false)
		if ((KEY_TAP(KEY::S)) || (KEY_HOLD(KEY::S)))
			m_vecTutorial_MissionData[m_iNow_Index + 1]->fNow += fTimeDelta * 3.f;

	if (m_vecTutorial_MissionData[m_iNow_Index + 2]->bComplete == false)
		if ((KEY_TAP(KEY::A)) || (KEY_HOLD(KEY::A)))
			m_vecTutorial_MissionData[m_iNow_Index + 2]->fNow += fTimeDelta * 3.f;

	if (m_vecTutorial_MissionData[m_iNow_Index + 3]->bComplete == false)
		if ((KEY_TAP(KEY::D)) || (KEY_HOLD(KEY::D)))
			m_vecTutorial_MissionData[m_iNow_Index + 3]->fNow += fTimeDelta * 3.f;
}

void CUIPage_Tutorial::Check_Player_Dash(_float fTimeDelta)
{
	if (m_bPlayer_StateChanged == false)
		return;

	_int iPlayerState = GET_GAMEINTERFACE->Get_Player()->Get_Fsm()->Get_CurrentState();

	if ((CPlayer::PLAYER_STATE::OH_DASH != iPlayerState) && (CPlayer::PLAYER_STATE::TH_DASH != iPlayerState))
		return;
	
	if (m_vecTutorial_MissionData[m_iNow_Index + 0]->bComplete == false)
		if ((KEY_TAP(KEY::A)) || (KEY_HOLD(KEY::A)))
			m_vecTutorial_MissionData[m_iNow_Index + 0]->fNow += 1;

	if (m_vecTutorial_MissionData[m_iNow_Index + 1]->bComplete == false)
		if ((KEY_TAP(KEY::D)) || (KEY_HOLD(KEY::D)))
			m_vecTutorial_MissionData[m_iNow_Index + 1]->fNow += 1;
}

void CUIPage_Tutorial::Check_Player_Focus()
{
	if (m_vecTutorial_MissionData[m_iNow_Index + 0]->bComplete == false)
	{
		CPawn* pTarget = GET_GAMEINTERFACE->Get_Player()->Get_TargetMonster();

		if (pTarget == nullptr)
			return;

		if ((pTarget != m_pSharedPonter_AttackMonster) && (pTarget != m_pSharedPonter_NormalMonster))
			return;

		m_vecTutorial_MissionData[m_iNow_Index + 0]->fNow += 1;
	}
	else
	{
		CPawn* pTarget = GET_GAMEINTERFACE->Get_Player()->Get_TargetMonster();

		if (pTarget != nullptr)
			return;

		m_vecTutorial_MissionData[m_iNow_Index + 1]->fNow += 1;
	}
}

void CUIPage_Tutorial::Check_Player_Lbutton_Attack()
{
	if (m_bPlayer_StateChanged == false)
		return;

	if (!KEY_TAP(KEY::LBUTTON))
		return;

	_int iPlayerState = GET_GAMEINTERFACE->Get_Player()->Get_Fsm()->Get_CurrentState();

	_int iLAttack = 0;

	iLAttack += CPlayer::PLAYER_STATE::FLAME_LATTACK0 == iPlayerState ? 1 : 0;
	iLAttack += CPlayer::PLAYER_STATE::FLAME_LATTACK1 == iPlayerState ? 1 : 0;
	iLAttack += CPlayer::PLAYER_STATE::RAPIER_LATTACK0 == iPlayerState ? 1 : 0;
	iLAttack += CPlayer::PLAYER_STATE::RAPIER_LATTACK1 == iPlayerState ? 1 : 0;
	iLAttack += CPlayer::PLAYER_STATE::SCISSOR_LATTACK0 == iPlayerState ? 1 : 0;
	iLAttack += CPlayer::PLAYER_STATE::SCISSOR_LATTACK1 == iPlayerState ? 1 : 0;
	
	if (iLAttack == 0)
		return;

	if (m_vecTutorial_MissionData[m_iNow_Index + 2]->bComplete == false)
		m_vecTutorial_MissionData[m_iNow_Index + 2]->fNow += 1;
}

void CUIPage_Tutorial::Check_Player_RButton_Attack(_float fTimeDelta)
{
	_bool bIsRButton = ((KEY_TAP(KEY::RBUTTON)) || (KEY_HOLD(KEY::RBUTTON)));
	_bool bIsCharge = (m_vHoldTime_RButton.x > 0.f);

	if (bIsRButton == true)
	{
		m_vHoldTime_RButton.x = min(m_vHoldTime_RButton.x + fTimeDelta, m_vHoldTime_RButton.y);
		m_vecTutorial_MissionData[m_iNow_Index + 0]->fNow = max(m_vecTutorial_MissionData[m_iNow_Index + 0]->fNow, m_vHoldTime_RButton.x);
	}
	else if ((bIsRButton == false) && (bIsCharge == true))
	{
		if (m_vecTutorial_MissionData[m_iNow_Index + 1]->bComplete == false)
		{
			_int iPlayerState = GET_GAMEINTERFACE->Get_Player()->Get_Fsm()->Get_CurrentState();

			_int iRAttack = 0;

			iRAttack += CPlayer::PLAYER_STATE::FLAME_RATTACK0 == iPlayerState ? 1 : 0;
			iRAttack += CPlayer::PLAYER_STATE::FLAME_RATTACK1 == iPlayerState ? 1 : 0;
			iRAttack += CPlayer::PLAYER_STATE::RAPIER_RATTACK0 == iPlayerState ? 1 : 0;
			iRAttack += CPlayer::PLAYER_STATE::SCISSOR_RATTACK0 == iPlayerState ? 1 : 0;
			iRAttack += CPlayer::PLAYER_STATE::SCISSOR_RATTACK1 == iPlayerState ? 1 : 0;

			if (iRAttack == 0)
				m_vecTutorial_MissionData[m_iNow_Index + 1]->fNow += 1;
		}

		m_vHoldTime_RButton.x = 0.f;
	}
}

void CUIPage_Tutorial::Check_Player_Fable_Art(_bool bIsSecond)
{
	if (m_bPlayer_StateChanged == false)
		return;

	_int iSecond = _int(bIsSecond);

	_int iPlayerState = GET_GAMEINTERFACE->Get_Player()->Get_Fsm()->Get_CurrentState();

	if (!KEY_TAP(KEY::F))
		return;

	/*_int iRAttack = 0;

	iRAttack += CPlayer::PLAYER_STATE::FLAME_FABLE == iPlayerState ? 1 : 0;
	iRAttack += CPlayer::PLAYER_STATE::RAPIER_FABALE == iPlayerState ? 1 : 0;
	iRAttack += CPlayer::PLAYER_STATE::SCISSOR_FABAL0 == iPlayerState ? 1 : 0;
	iRAttack += CPlayer::PLAYER_STATE::SCISSOR_FABAL1 == iPlayerState ? 1 : 0;
	iRAttack += CPlayer::PLAYER_STATE::SCISSOR_FABAL2 == iPlayerState ? 1 : 0;

	if (iRAttack == 0)
		return;*/

	if ((KEY_TAP(KEY::LSHIFT)) || (KEY_HOLD(KEY::LSHIFT)))
		m_vecTutorial_MissionData[m_iNow_Index + 1 + iSecond]->fNow += 1;
	else 
		m_vecTutorial_MissionData[m_iNow_Index + 0 + iSecond]->fNow += 1;
}

void CUIPage_Tutorial::Check_Dummy_Weakness()
{
	if (m_vecTutorial_MissionData[m_iNow_Index + 0]->bComplete == false)
	{
		if (_int(m_pSharedPonter_AttackMonster->Get_Status()->bWeakness) + _int(m_pSharedPonter_NormalMonster->Get_Status()->bWeakness) == 0)
			return;

		m_vecTutorial_MissionData[m_iNow_Index + 0]->fNow += 1;
	}
}

void CUIPage_Tutorial::Check_Dummy_Get_FatalAttack()
{
	if (m_vecTutorial_MissionData[m_iNow_Index + 1]->bComplete == false)
	{
		if (_int(m_pSharedPonter_AttackMonster->Get_Fsm()->Get_CurrentState() == CMonster::MONSTER_STATE::HITFATAL)
			+ _int(m_pSharedPonter_NormalMonster->Get_Fsm()->Get_CurrentState() == CMonster::MONSTER_STATE::HITFATAL) > 0)
			m_vecTutorial_MissionData[m_iNow_Index + 1]->fNow += 1;
	}
}

void CUIPage_Tutorial::Check_Player_Switch_Weapon()
{
	if (m_bPlayer_StateChanged == false)
		return;

	if (m_iBeforeWeapon == -1)
		m_iBeforeWeapon = _int(GET_GAMEINTERFACE->Get_Player()->Get_WeaponType());
	else if (m_vecTutorial_MissionData[m_iNow_Index + 0]->bComplete == false)
	{
		if (m_iBeforeWeapon != _int(GET_GAMEINTERFACE->Get_Player()->Get_WeaponType()))
			m_vecTutorial_MissionData[m_iNow_Index + 0]->fNow += 1;
	}
	else
		Check_Player_Fable_Art(true);
}

void CUIPage_Tutorial::Check_Player_Guard(_float fTimeDelta)
{
	_int iPlayerState = GET_GAMEINTERFACE->Get_Player()->Get_Fsm()->Get_CurrentState();

	if ((iPlayerState == _int(CPlayer::PLAYER_STATE::OH_GUARD)) || (iPlayerState == _int(CPlayer::PLAYER_STATE::TH_GUARD)))
	{
		if (m_vGuardTime.x == 0.f)
			ShowTiming(KEY::LSHIFT, 0.17f); // <- 일반 막기 퍼펙트 시간

		m_vGuardTime.x += fTimeDelta;
		m_bGuard = true;
	}
	else if ((iPlayerState == _int(CPlayer::PLAYER_STATE::OH_GUARDHIT)) || (iPlayerState == _int(CPlayer::PLAYER_STATE::TH_GUARDHIT)))
	{
		m_vecTutorial_MissionData[m_iNow_Index + 0]->fNow += 1;

		if (m_vGuardTime.x <= m_vGuardTime.y)
			m_vecTutorial_MissionData[m_iNow_Index + 1]->fNow += 1;
	}
	else
	{
		m_vGuardTime.x = 0.f;
		m_bGuard = false;
	}
}

void CUIPage_Tutorial::Check_Player_Resion_Arm_Start()
{
	if (m_bPlayer_StateChanged == false)
		return;

	if (m_vecTutorial_MissionData[m_iNow_Index + 0]->bComplete == false)
	{
		_int iPlayerState = GET_GAMEINTERFACE->Get_Player()->Get_Fsm()->Get_CurrentState();

		if (iPlayerState == _int(CPlayer::PLAYER_STATE::ARM_LOOP))
			m_vecTutorial_MissionData[m_iNow_Index + 0]->fNow += 1;
	}
}

void CUIPage_Tutorial::Check_Player_Resion_Arm_Skill()
{
	/*if (m_bPlayer_StateChanged == false)
		return;*/

	_int iPlayerState = GET_GAMEINTERFACE->Get_Player()->Get_Fsm()->Get_CurrentState();

	if (m_vecTutorial_MissionData[m_iNow_Index + 0]->bComplete == false)
	{
		if ((KEY_HOLD(KEY::CTRL)) && KEY_TAP(KEY::LBUTTON))
			if ((iPlayerState == _int(CPlayer::PLAYER_STATE::ARM_SWING)) || (iPlayerState == _int(CPlayer::PLAYER_STATE::ARM_THRUST)))
				m_vecTutorial_MissionData[m_iNow_Index + 0]->fNow += 1.f;
	}

	if ((KEY_HOLD(KEY::CTRL)) && KEY_TAP(KEY::LSHIFT))
		ShowTiming(KEY::LSHIFT, 0.17f); // <- 리전 암 막기 퍼펙트 시간

	if (m_vecTutorial_MissionData[m_iNow_Index + 1]->bComplete == false)
	{
		_int iCheck_Guard = 0;

		iCheck_Guard += iPlayerState == _int(CPlayer::PLAYER_STATE::ARM_GURAD_WEAK) ? 1 : 0;
		iCheck_Guard += iPlayerState == _int(CPlayer::PLAYER_STATE::ARM_GURAD_HARD) ? 1 : 0;
		iCheck_Guard += iPlayerState == _int(CPlayer::PLAYER_STATE::ARM_GURAD_HEAVY) ? 1 : 0;

		if (iCheck_Guard > 0)
			m_vecTutorial_MissionData[m_iNow_Index + 1]->fNow += 1.f;
	}
}

void CUIPage_Tutorial::Check_Player_Resion_Arm_Counter()
{
	if (m_bPlayer_StateChanged == false)
		return;

	if (m_vecTutorial_MissionData[m_iNow_Index + 0]->bComplete == false)
	{
		_int iPlayerState = GET_GAMEINTERFACE->Get_Player()->Get_Fsm()->Get_CurrentState();

		if (iPlayerState == _int(CPlayer::PLAYER_STATE::ARM_COUNTER))
			m_vecTutorial_MissionData[m_iNow_Index + 0]->fNow += 1.f;
	}
}

void CUIPage_Tutorial::Input_TrainingMonsterPointer_Attack(CGameObject* pPoiter)
{
	m_pSharedPonter_AttackMonster = static_cast<CMonster_Training01*>(pPoiter);
}

void CUIPage_Tutorial::Input_TrainingMonsterPointer_Normal(CGameObject* pPoiter)
{
	m_pSharedPonter_NormalMonster = static_cast<CMonster_Training02*>(pPoiter);
}

HRESULT CUIPage_Tutorial::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	m_vec_Group_Ctrl.resize(_int(PART_GROUP::GROUP_END));

	for (auto& iter : m_vec_Group_Ctrl)
		iter = new UG_CTRL;

	for (_int i = 0; i < m_vecPart.size(); ++i)
	{
		if (m_vecPart[i]->iGroupIndex != -1)
			m_vec_Group_Ctrl[m_vecPart[i]->iGroupIndex]->PartIndexlist.push_back(i);
	}

	m_bRender = false;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TIMING)]->bRender = false;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_RESULT)]->bRender = false;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_INFO_FRAME)]->bRender = false;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_INFO_MISSION)]->bRender = false;

	Initialize_Tutorial();

	m_vecPart[m_vecPart.size() - 2]->iTexture_Index = 220;

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);

	return S_OK;
}

void CUIPage_Tutorial::Initialize_Tutorial()
{
	m_vecMission_Info.clear();

	for (_int i = 0; i < 4; ++i)
	{
		CUITutorial_Info* pNew = CUITutorial_Info::Create(m_pDevice, m_pContext);
		pNew->Set_Info(m_vecPart);
		m_vecMission_Info.push_back(pNew);
	}

	m_pGuide = CUITutorial_Guide::Create(m_pDevice, m_pContext);
	m_pTiming = CUITutorial_Timing::Create(m_pDevice, m_pContext);
	m_pResult = CUITutorial_Result::Create(m_pDevice, m_pContext);
	m_pPopup = CUITutorial_Popup::Create(m_pDevice, m_pContext);

	m_pGuide->Set_Guide(m_vecPart);
	m_pTiming->Set_Timing(m_vecPart);
	m_pResult->Set_Result(m_vecPart);
	m_pPopup->Set_Popup(m_vecPart);

	Data_Setting();
}

void CUIPage_Tutorial::Data_Setting()
{
	vector<vector<_wstring>> vecBuffer_Chapter;
	m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Tutorial_Chapter.csv", &vecBuffer_Chapter);

	for (_int i = 1; i < vecBuffer_Chapter.size(); ++i)
	{
		TUTO_CHAPTER* pNew = new TUTO_CHAPTER;

		pNew->iCapterIndex = stoi(vecBuffer_Chapter[i][0]);
		pNew->strTitle = vecBuffer_Chapter[i][1];
		pNew->strDescA = vecBuffer_Chapter[i][2];
		pNew->strDescB = vecBuffer_Chapter[i][3];

		pNew->iKey_Texture[0] = Check_KeyTexture_Index(vecBuffer_Chapter[i][4]);
		pNew->iKey_Texture[1] = Check_KeyTexture_Index(vecBuffer_Chapter[i][5]);
		pNew->strKey_Desc[0] = vecBuffer_Chapter[i][6];
		pNew->iKey_Texture[2] = Check_KeyTexture_Index(vecBuffer_Chapter[i][7]);
		pNew->iKey_Texture[3] = Check_KeyTexture_Index(vecBuffer_Chapter[i][8]);
		pNew->strKey_Desc[1] = vecBuffer_Chapter[i][9];
		pNew->iKey_Texture[4] = Check_KeyTexture_Index(vecBuffer_Chapter[i][10]);
		pNew->iKey_Texture[5] = Check_KeyTexture_Index(vecBuffer_Chapter[i][11]);
		pNew->strKey_Desc[2] = vecBuffer_Chapter[i][12];

		m_vecTutorial_ChapterData.push_back(pNew);
	}

	vector<vector<_wstring>> vecBuffer_Mission;
	m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Tutorial_Info.csv", &vecBuffer_Mission);

	for (_int i = 1; i < vecBuffer_Mission.size(); ++i)
	{
		TUTO_MISSION* pNew = new TUTO_MISSION;

		pNew->iCapterIndex = stoi(vecBuffer_Mission[i][0]);
		pNew->iMissionIndex = stoi(vecBuffer_Mission[i][1]);
		pNew->strTitle = vecBuffer_Mission[i][2];
		pNew->fGoal = stof(vecBuffer_Mission[i][3]);

		m_vecTutorial_MissionData.push_back(pNew);
	}
}

_int CUIPage_Tutorial::Check_KeyTexture_Index(_wstring strKeyName)
{
	_int iResult = -1;

	if(strKeyName[0] == 'W')
		iResult = 294;
	else if ((strKeyName[0] == 'S') && (strKeyName[1] == 'P'))
		iResult = 287;
	else if (strKeyName[0] == 'L')
		iResult = 302;
	else if (strKeyName[0] == 'R')
		iResult = 305;
	else if (strKeyName[0] == 'F')
		iResult = 223;
	else if (strKeyName[0] == 'T')
		iResult = 289;
	else if ((strKeyName[0] == 'S') && (strKeyName[1] == 'H'))
		iResult = 250;
	else if (strKeyName[0] == 'C')
		iResult = 249;
	else if (strKeyName[0] == 'M')
		iResult = 303;

	return iResult;
}

_bool CUIPage_Tutorial::Set_Crtl_Guide(vector<UPART*>* Part, TUTO_CHAPTER& Data, _int iIndex)
{
	for (auto& iter : (*Part))
		iter->bRender = false;

	if (Data.iKey_Texture[iIndex * 2] == -1)
		return false;

	(*Part)[0]->iTexture_Index = Data.iKey_Texture[iIndex * 2];
	(*Part)[0]->bRender = true;

	(*Part)[0]->fSize.y = (*Part)[0]->iTexture_Index == 287 ? m_fKeyHeight * 0.5f : m_fKeyHeight;
	
	if (Data.iKey_Texture[(iIndex * 2) + 1] != -1)
	{
		(*Part)[1]->bRender = true;
		(*Part)[2]->iTexture_Index = Data.iKey_Texture[(iIndex * 2) + 1];
		(*Part)[2]->bRender = true;
		(*Part)[3]->fRatio = 1.f;
		(*Part)[2]->fSize.y = (*Part)[2]->iTexture_Index == 287 ? m_fKeyHeight * 0.5f : m_fKeyHeight;
	}
	else 
		(*Part)[3]->fRatio = 0.f;

	if (Data.strKey_Desc[iIndex][0] != 'n')
	{
		(*Part)[3]->bRender = true;
		(*Part)[3]->strText = Data.strKey_Desc[iIndex];
	}

	return true;
}

void CUIPage_Tutorial::Update_Tutorial()
{






}

void CUIPage_Tutorial::Set_Waiting_NextChapter()
{
	m_bWating_NewChapter = true;
	

}

void CUIPage_Tutorial::Next_Chapter()
{
	++m_iNowChapter;

	if (m_iNowChapter < m_vecTutorial_ChapterData.size())
	{
		while (m_vecTutorial_MissionData[m_iNow_Index]->iCapterIndex != m_iNowChapter)
			++m_iNow_Index;

		m_bNewChapter = true;

		Open_Popup();
	}
	else
	{
		--m_iNowChapter;
		
		GET_GAMEINTERFACE->Fade_Out(TEXT(""), TEXT(""), {0.f,0.f,0.f}, m_fStartTime);
		m_bWaitStart = true;
		// 여기에 종료 팝업 필요
	}
}

CUIPage_Tutorial* CUIPage_Tutorial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Tutorial* pInstance = new CUIPage_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Tutorial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Tutorial::Clone(void* pArg)
{
	CUIPage_Tutorial* pInstance = new CUIPage_Tutorial(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Tutorial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Tutorial::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	for (auto& iter : m_vecMission_Info)
	{
		Safe_Release(iter);
	}

	m_vecMission_Info.clear();

	Safe_Release(m_pGuide);
	Safe_Release(m_pTiming); 
	Safe_Release(m_pResult);
	Safe_Release(m_pPopup);

	for (auto& iter : m_vecTutorial_ChapterData)
	{
		Safe_Delete(iter);
	}

	for (auto& iter : m_vecTutorial_MissionData)
	{
		Safe_Delete(iter);
	}

	m_vecTutorial_ChapterData.clear();
	m_vecTutorial_MissionData.clear();

	m_vecPart.clear();
}
