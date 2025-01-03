#include "stdafx.h"
#include "..\Public\UIPage_Ortho.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"
#include "UIPage_Ortho_Include.h"

CUIPage_Ortho::CUIPage_Ortho(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Ortho::CUIPage_Ortho(const CUIPage_Ortho& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Ortho::Initialize_Prototype()
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

	Initialize_Ortho_Info();

	m_bRender = false;
	m_fTopPartMove = 1.f;

	return S_OK;
}

HRESULT CUIPage_Ortho::Initialize(void* pArg)
{
	

	return S_OK;
}

void CUIPage_Ortho::Priority_Update(_float fTimeDelta)
{
	
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Ortho::Update(_float fTimeDelta)
{
	
	__super::Update(fTimeDelta);
}

void CUIPage_Ortho::Late_Update(_float fTimeDelta)
{
	
	m_fTimeDelta = fTimeDelta;
	CheckHost(fTimeDelta);
	__super::Late_Update(fTimeDelta);

	Render_Ortho_UI();


}

HRESULT CUIPage_Ortho::Render()
{
	return S_OK;
}

void CUIPage_Ortho::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Ortho::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Ortho::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Ortho::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	for (_int i = 0; i < _int(PART_GROUP::GROUP_END); ++i)
	{
		UG_CTRL* pNew = new UG_CTRL;
		m_vec_Group_Ctrl.push_back(pNew);
	}

	for (_int i = 0; i < m_vecPart.size(); ++i)
	{
		_int iGroup = m_vecPart[i]->iGroupIndex;

		if ((iGroup >= 0) && (iGroup < _int(PART_GROUP::GROUP_END)))
			m_vec_Group_Ctrl[iGroup]->PartIndexlist.push_back(i);
	}

	m_vecPart[7]->iTexture_Index = -1;

	return S_OK;
}

void CUIPage_Ortho::Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE eType, void* pObj)
{
	ORTHO_HOST* pNew = new ORTHO_HOST;
	pNew->eType = eType;
	pNew->pHost = static_cast<CGameObject*>(pObj);
	m_Ortho_Host_list.push_back(pNew);

	if (eType == UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_ATTACK)
		GET_GAMEINTERFACE->Input_TrainingMonsterPointer_Attack(pNew->pHost);
	else if (eType == UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_NORMAL)
		GET_GAMEINTERFACE->Input_TrainingMonsterPointer_Normal(pNew->pHost);
}

HRESULT CUIPage_Ortho::Render_Ortho_UI()
{
	// 여기서 렌더 해야 함
	
	while (!m_queue_Ortho_Render_Ctrl.empty())
	{
		OR_RENDER* pRender = m_queue_Ortho_Render_Ctrl.top();
		m_queue_Ortho_Render_Ctrl.pop();
		PART_GROUP eType = pRender->eType;
		m_vec_Group_Ctrl[_int(eType)]->fRatio = pRender->fRatio;
		m_fX = pRender->fPosition.x;
		m_fY = pRender->fPosition.y;

		if (eType == PART_GROUP::GROUP_HP_COUNT)
			m_vecPart[m_vec_Group_Ctrl[_int(eType)]->PartIndexlist.front()]->strText = pRender->strText;

		if (eType == PART_GROUP::GROUP_HP_FILL)
		{
			m_vecPart[m_vec_Group_Ctrl[_int(eType)]->PartIndexlist.front()]->fRatio = pRender->fRatio;
			if (pRender->bIsWeakness)
				m_vecPart[m_vec_Group_Ctrl[_int(eType)]->PartIndexlist.back()]->bRender = true;
			else 
				m_vecPart[m_vec_Group_Ctrl[_int(eType)]->PartIndexlist.back()]->bRender = false;
		}

		for (auto& iter : m_vec_Group_Ctrl[_int(eType)]->PartIndexlist)
		{
			if ((eType == PART_GROUP::GROUP_HP_COUNT) || (eType == PART_GROUP::GROUP_HP_FILL))
				m_vecPart[0]->fPosition = { m_fX, m_fY };

			__super::UpdatePart_ByIndex(iter, m_fTimeDelta);

			/*if (eType != PART_GROUP::GROUP_HP_COUNT)
				m_vecPart[iter]->fPosition = { m_vecPart[iter]->fPosition.x + m_fViewWidth * 0.5f, m_vecPart[iter]->fPosition.y - m_fViewHeight * 0.5f };*/

			__super::Input_Render_Info(*m_vecPart[iter]);
		}

		Safe_Delete(pRender);
	}

	return S_OK;
}

void CUIPage_Ortho::Set_OnOff_OrthoUI(_bool bIsOn, void* pObj)
{
	for (auto& iter : m_Ortho_Host_list)
		if (iter->pHost == pObj)
		{
			iter->bIsActive = bIsOn;

			if (bIsOn == false)
			{
				if (static_cast<CPawn*>(pObj)->Get_Status()->fHp <= 0.f)
				{
					if (iter->eType == UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_RAXASIA)
						GET_GAMEINTERFACE->Input_Achievment_Data(8, 1);
					else if (iter->eType == UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_SIMON)
						GET_GAMEINTERFACE->Input_Achievment_Data(9, 1);
					else if ((iter->eType == UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_ATTACK) || (iter->eType == UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_NORMAL))
						return;
					else 
						GET_GAMEINTERFACE->Input_Achievment_Data(6, 1);
				}
			}
		}
}

void CUIPage_Ortho::Initialize_Ortho_Info()
{
	// 직교 UI 별 보정치 세팅 
	m_vecOrtho_Adjust.resize(_int(UI_ORTHO_OBJ_TYPE::ORTHO_END));

	for(auto& iter : m_vecOrtho_Adjust)
		iter.resize(_int(PART_GROUP::GROUP_END));

	// 일반 몬스터 
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_NORMAL_MONSTER)][_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,4.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_NORMAL_MONSTER)][_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,4.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_NORMAL_MONSTER)][_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,4.f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_NORMAL_MONSTER)][_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,2.f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_NORMAL_MONSTER)][_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,2.f,0.f }; // 몬스터 특수공격

	// 락사시아
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_RAXASIA)][_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,3.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_RAXASIA)][_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,3.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_RAXASIA)][_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,3.f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_RAXASIA)][_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,1.f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_RAXASIA)][_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,1.f,0.f }; // 몬스터 특수공격

	// 시몬 마누스
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_SIMON)][_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,3.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_SIMON)][_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,3.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_SIMON)][_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,3.f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_SIMON)][_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,1.55f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_SIMON)][_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,1.55f,0.f }; // 몬스터 특수공격

	// 더미 (공격)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_ATTACK)][_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,2.35f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_ATTACK)][_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,2.35f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_ATTACK)][_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,2.35f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_ATTACK)][_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,1.3f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_ATTACK)][_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,1.3f,0.f }; // 몬스터 특수공격

	// 더미 (일반)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_NORMAL)][_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,2.35f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_NORMAL)][_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,2.35f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_NORMAL)][_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,2.35f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_NORMAL)][_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,1.3f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_NORMAL)][_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,1.3f,0.f }; // 몬스터 특수공격

	// Carcass_Big
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_BIG)][_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,4.3f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_BIG)][_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,4.3f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_BIG)][_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,4.3f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_BIG)][_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,2.15f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_BIG)][_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,2.15f,0.f }; // 몬스터 특수공격

	// Carcass_Tail
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_TAIL)][_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,1.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_TAIL)][_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,1.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_TAIL)][_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,1.f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_TAIL)][_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,0.5f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_TAIL)][_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,0.5f,0.f }; // 몬스터 특수공격

	// Carcass_Normal
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_NORMAL)][_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,2.5f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_NORMAL)][_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,2.5f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_NORMAL)][_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,2.5f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_NORMAL)][_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,1.1f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_NORMAL)][_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,1.1f,0.f }; // 몬스터 특수공격

	// Currupted_Puppet
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CURRUPTED_PUPPET)][_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,2.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CURRUPTED_PUPPET)][_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,2.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CURRUPTED_PUPPET)][_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,2.f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CURRUPTED_PUPPET)][_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,1.5f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_CURRUPTED_PUPPET)][_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,1.5f,0.f }; // 몬스터 특수공격

	// Reboner_Big
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_BIG)][_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,2.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_BIG)][_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,2.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_BIG)][_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,2.f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_BIG)][_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,1.5f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_BIG)][_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,1.5f,0.f }; // 몬스터 특수공격

	// Reboner_Male
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_MALE)][_int(PART_GROUP::GROUP_HP_FRAME)] = { 0.f,2.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_MALE)][_int(PART_GROUP::GROUP_HP_FILL)] = { 0.f,2.f,0.f }; // 몬스터 체력바 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_MALE)][_int(PART_GROUP::GROUP_HP_COUNT)] = { 0.f,2.f,0.f }; // 몬스터 데미지 (프레임, Fill 모두 적용됨)
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_MALE)][_int(PART_GROUP::GROUP_FOCUS)] = { 0.f,1.5f,0.f }; // 몬스터 포커싱
	m_vecOrtho_Adjust[_int(UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_MALE)][_int(PART_GROUP::GROUP_SPECIAL_HIT)] = { 0.f,1.5f,0.f }; // 몬스터 특수공격
}

void CUIPage_Ortho::CheckHost(_float fTimeDelta)
{
	for (list<CUIPage_Ortho::OR_HOST*>::iterator iter = m_Ortho_Host_list.begin(); iter!= m_Ortho_Host_list.end();)
	{
		// Host 정보 제거 논리 
		/*
		1. host가 nullptr이거나 dead이다 
		
		
		*/


		if (((*iter)->pHost == nullptr) || ((*iter)->pHost->Get_Dead()))
		{
			// iter->pHost는 얕은 복사로 가져왔으며 addref 하지 않았음
			Safe_Delete(*iter);
			Set_OnOff_OrthoUI(false, (*iter)->pHost);
			iter = m_Ortho_Host_list.erase(iter);
			continue;
		}

		if (!(*iter)->bIsActive)
		{
			++iter;
			continue;
		}

		else if ((*iter)->eType == UI_ORTHO_OBJ_TYPE::ORTHO_NORMAL_MONSTER)
		{
			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			Make_Monster_HP_Bar((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			++iter;
		}
		else if ((*iter)->eType == UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_SIMON)
		{
			const CPawn::PAWN_STATUS* pStat = static_cast<CPawn*>((*iter)->pHost)->Get_Status();

			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			GET_GAMEINTERFACE->Activate_Boss_Hp_Bar(true);
			GET_GAMEINTERFACE->Set_Boss_Hp_Bar_Info(pStat->strName, pStat->fHp, pStat->fMaxHp);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			++iter;
		}
		else if ((*iter)->eType == UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_RAXASIA)
		{
			const CPawn::PAWN_STATUS* pStat = static_cast<CPawn*>((*iter)->pHost)->Get_Status();

			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			GET_GAMEINTERFACE->Activate_Boss_Hp_Bar(true);
			GET_GAMEINTERFACE->Set_Boss_Hp_Bar_Info(pStat->strName, pStat->fHp, pStat->fMaxHp);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			++iter;
		}
		else if ((*iter)->eType == UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_ATTACK)
		{
			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			Make_Monster_HP_Bar((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			++iter;
		}
		else if ((*iter)->eType == UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_NORMAL)
		{
			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			Make_Monster_HP_Bar((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			++iter;
		}
		else if ((*iter)->eType == UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_BIG)
		{
			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			Make_Monster_HP_Bar((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			++iter;
		}
		else if ((*iter)->eType == UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_TAIL)
		{
			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			Make_Monster_HP_Bar((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			++iter;
		}
		else if ((*iter)->eType == UI_ORTHO_OBJ_TYPE::ORTHO_CARCASS_NORMAL)
		{
			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			Make_Monster_HP_Bar((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			++iter;
		}
		else if ((*iter)->eType == UI_ORTHO_OBJ_TYPE::ORTHO_CURRUPTED_PUPPET)
		{
			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			Make_Monster_HP_Bar((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			++iter;
		}
		else if ((*iter)->eType == UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_BIG)
		{
			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			Make_Monster_HP_Bar((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			++iter;
		}
		else if ((*iter)->eType == UI_ORTHO_OBJ_TYPE::ORTHO_REBORNER_MALE)
		{
			_float fDistnace = Check_Distance_From_Cam((*iter)->pHost);

			Make_Monster_HP_Bar((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_Focusing((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			Make_Monster_SpecialHit((*iter)->pHost, fTimeDelta, fDistnace, (*iter)->eType);
			++iter;
		}
	}
}

void CUIPage_Ortho::Make_Monster_HP_Bar(CGameObject* pHost, _float fTimeDelta, _float fDistance, UI_ORTHO_OBJ_TYPE eType)
{
	// 여기에 HP 바를 띄우는 논리가 필요함 

	// 테스트 수치 
	const CPawn::PAWN_STATUS* pStat = static_cast<CPawn*>(pHost)->Get_Status();

	_float fRatio = pStat->fHp / pStat->fMaxHp;
	_float fDamege = pStat->fAtkDmg;

	_Vec2 fPosition = { 0.f,0.f };
	if (!Make_OrthoGraphy_Position(pHost, PART_GROUP::GROUP_HP_FRAME, &fPosition, eType))
		return;

	OR_RENDER* pRender_HP_Frame = new OR_RENDER;
	*pRender_HP_Frame = { fDistance ,fPosition,  PART_GROUP::GROUP_HP_FRAME, fRatio, {}, -1 };
	OR_RENDER* pRender_HP_Fill = new OR_RENDER;
	*pRender_HP_Fill = { fDistance ,fPosition,  PART_GROUP::GROUP_HP_FILL, fRatio, {}, -1 };
	pRender_HP_Fill->bIsWeakness = pStat->bWeakness;

	m_queue_Ortho_Render_Ctrl.push(pRender_HP_Frame);
	m_queue_Ortho_Render_Ctrl.push(pRender_HP_Fill);

	if (fDamege > 0.f)
	{
		OR_RENDER* pRender_HP_Demege = new OR_RENDER;
		*pRender_HP_Demege = { fDistance ,fPosition,  PART_GROUP::GROUP_HP_COUNT, fRatio, to_wstring(_int(fDamege)), -1 };
		m_queue_Ortho_Render_Ctrl.push(pRender_HP_Demege);
	}
}

void CUIPage_Ortho::Make_Monster_Focusing(CGameObject* pHost, _float fTimeDelta, _float fDistance, UI_ORTHO_OBJ_TYPE eType)
{
	CPawn* pTarget = GET_GAMEINTERFACE->Get_Player()->Get_TargetMonster();

	if (pTarget == nullptr)
		return;

	if (pTarget != pHost)
		return;

	if (KEY_HOLD(KEY::ALT))
	{
		if (KEY_HOLD(KEY::UP))
		{
			m_vecOrtho_Adjust[_int(eType)][_int(PART_GROUP::GROUP_HP_FRAME)].y += 0.05f;
			m_vecOrtho_Adjust[_int(eType)][_int(PART_GROUP::GROUP_HP_FILL)].y += 0.05f;
			m_vecOrtho_Adjust[_int(eType)][_int(PART_GROUP::GROUP_HP_COUNT)].y += 0.05f;
		}
		else if (KEY_HOLD(KEY::DOWN))
		{
			m_vecOrtho_Adjust[_int(eType)][_int(PART_GROUP::GROUP_HP_FRAME)].y -= 0.05f;
			m_vecOrtho_Adjust[_int(eType)][_int(PART_GROUP::GROUP_HP_FILL)].y -= 0.05f;
			m_vecOrtho_Adjust[_int(eType)][_int(PART_GROUP::GROUP_HP_COUNT)].y -= 0.05f;
		}
	}
	else if (KEY_HOLD(KEY::LSHIFT))
	{
		if (KEY_HOLD(KEY::UP))
		{
			m_vecOrtho_Adjust[_int(eType)][_int(PART_GROUP::GROUP_FOCUS)].y += 0.05f;
			m_vecOrtho_Adjust[_int(eType)][_int(PART_GROUP::GROUP_SPECIAL_HIT)].y += 0.05f;
		}
		else if (KEY_HOLD(KEY::DOWN))
		{
			m_vecOrtho_Adjust[_int(eType)][_int(PART_GROUP::GROUP_FOCUS)].y -= 0.05f;
			m_vecOrtho_Adjust[_int(eType)][_int(PART_GROUP::GROUP_SPECIAL_HIT)].y -= 0.05f;
		}
	}

	_Vec2 fPosition = { 0.f,0.f };
	if (!Make_OrthoGraphy_Position(pHost, PART_GROUP::GROUP_FOCUS, &fPosition, eType))
		return;

	OR_RENDER* pRender_Focusing = new OR_RENDER;
	*pRender_Focusing = { fDistance ,fPosition,  PART_GROUP::GROUP_FOCUS,1.f, {}, -1 };

	m_queue_Ortho_Render_Ctrl.push(pRender_Focusing);
}

void CUIPage_Ortho::Make_Monster_SpecialHit(CGameObject* pHost, _float fTimeDelta, _float fDistance, UI_ORTHO_OBJ_TYPE eType)
{
	if (!static_cast<CPawn*>(pHost)->Get_Status()->bFatalAttack)
		return;

	_Vec2 fPosition = { 0.f,0.f };
	if (!Make_OrthoGraphy_Position(pHost, PART_GROUP::GROUP_SPECIAL_HIT, &fPosition, eType))
		return;

	OR_RENDER* pRender_Special = new OR_RENDER;
	*pRender_Special = { fDistance ,fPosition,  PART_GROUP::GROUP_SPECIAL_HIT,1.f, {}, -1 };

	m_queue_Ortho_Render_Ctrl.push(pRender_Special);
}


_bool CUIPage_Ortho::Make_OrthoGraphy_Position(CGameObject* pHost, PART_GROUP eGroup, _Vec2* fPosition, UI_ORTHO_OBJ_TYPE eType)
{
	const _Matrix mMat = pHost->Get_Transform()->Get_WorldMatrix();
	const _Matrix mProj = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
	const _Matrix mView = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	const _Vec4 vSize = pHost->Get_Transform()->Get_Scaled();

	_Vec3 fAdj = m_vecOrtho_Adjust[_int(eType)][_int(eGroup)];
	fAdj.x *= vSize.x;
	fAdj.y *= vSize.y;
	fAdj.z *= vSize.z;

	_Vec4 vResult = { fAdj.x, fAdj.y, fAdj.z, 0.f };

	// 투영 좌표 계산
	vResult = XMVector3Transform(vResult, mMat);
	vResult = XMVector3Transform(vResult, mView);
	vResult = XMVector3Transform(vResult, mProj);

	// W나누기
	_Vec4 fResult{};
	XMStoreFloat4(&fResult, vResult);

	if (fResult.w < 0.f)
		return false;

	fPosition->x = fResult.x / fResult.w;
	fPosition->y = fResult.y / fResult.w;

	// 스크린 좌표로 변환
	fPosition->x = ((fPosition->x + 1.f) * 0.5f) * 1280.f;
	fPosition->y = ((1.f - fPosition->y) * 0.5f) * 720.f;

	return true;
}

_float CUIPage_Ortho::Check_Distance_From_Cam(CGameObject* pHost)
{
	_Vec4 vDistance = pHost->Get_Transform()->Get_State(CTransform::STATE_POSITION) - m_pGameInstance->Get_CamPosition_Vec4();

	return _float(sqrt(pow(vDistance.x, 2) + pow(vDistance.y, 2) + pow(vDistance.z, 2)));
}


CUIPage_Ortho* CUIPage_Ortho::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Ortho* pInstance = new CUIPage_Ortho(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Ortho"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Ortho::Clone(void* pArg)
{
	CUIPage_Ortho* pInstance = new CUIPage_Ortho(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Ortho"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Ortho::Free()
{
	__super::Free();

	for (auto& iter : m_Ortho_Host_list)
		Safe_Delete(iter);

	m_Ortho_Host_list.clear();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();

	for (auto& iter : m_vecOrtho_Adjust)
		iter.clear();

	m_vecOrtho_Adjust.clear();

	while (!m_queue_Ortho_Render_Ctrl.empty())
	{
		m_queue_Ortho_Render_Ctrl.pop();
	}
	
}
