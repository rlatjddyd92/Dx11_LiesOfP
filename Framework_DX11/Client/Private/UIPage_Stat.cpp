#include "stdafx.h"
#include "..\Public\UIPage_Stat.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Stat::CUIPage_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Stat::CUIPage_Stat(const CUIPage_Stat& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Stat::Initialize_Prototype()
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

HRESULT CUIPage_Stat::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Stat::Priority_Update(_float fTimeDelta)
{


	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Stat::Update(_float fTimeDelta)
{

	__super::Update(fTimeDelta);
}

void CUIPage_Stat::Late_Update(_float fTimeDelta)
{
	Update_Point(fTimeDelta);
	Update_LevelUp(fTimeDelta);
	Update_Focus(fTimeDelta);
	Update_SpecData(fTimeDelta);
	Update_StarChart(fTimeDelta);


	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);

	__super::Late_Update(fTimeDelta);

	
	
}

HRESULT CUIPage_Stat::Render()
{
	return S_OK;
}

void CUIPage_Stat::OpenAction()
{
	__super::OpenAction();
	m_pSoundCom->Play2D(TEXT("SE_UI_OpenMenu_04.wav"), &g_fUIVolume);
	memset(m_fStarChart_LevelUp_Now, 0, sizeof(_float) * 8);
	memset(m_fStarChart_NowStat_Now, 0, sizeof(_float) * 8);
	m_iFocus_Point = 0;
}

void CUIPage_Stat::CloseAction()
{
	__super::CloseAction();
	m_pSoundCom->Play2D(TEXT("SE_UI_CloseWindow_01.wav"), &g_fUIVolume);
}

CHECK_MOUSE CUIPage_Stat::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	Action_Point(fTimeDelta);
	Action_LevelUp(fTimeDelta);
	Action_Focus(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Stat::Ready_UIPart_Group_Control()
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
	Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->iMoveType = 1;
	m_bRender = false;

	return S_OK;
}

void CUIPage_Stat::Action_Point(_float fTimeDelta)
{
	_bool bClick = KEY_TAP(KEY::LBUTTON);

	const CPlayer::PLAYER_STAT_INFO pOrigin = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat();
	CPlayer::PLAYER_STAT_INFO* pAdjust = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust();

	m_iUsing_Point_Now = 0;
	m_iUsing_Ergo_Now = 0;

	for (_int i = 0; i < 5; ++i)
	{
		m_iUsing_Point_Now += m_iLevelUp_Buffer_Point[i];
		m_iUsing_Ergo_Now += (m_iLevelUp_Buffer_Point[i] * pOrigin.iErgo_LevelUp) + (((m_iLevelUp_Buffer_Point[i] * (m_iLevelUp_Buffer_Point[i] - 1)) / 2) * 10);
	}
		

	for (_int i = 0; i < 5; ++i)
	{
		_Vec2 vMouseFront = GET_GAMEINTERFACE->CheckMouse(m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_BUTTON_0) + i]->PartIndexlist.front()]->fPosition, m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_BUTTON_0) + i]->PartIndexlist.front()]->fSize);
		_Vec2 vMouseBack = GET_GAMEINTERFACE->CheckMouse(m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_BUTTON_0) + i]->PartIndexlist.back()]->fPosition, m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_BUTTON_0) + i]->PartIndexlist.back()]->fSize);

		if (vMouseFront.x != -1.f)
		{
			m_iFocus_Point = i;
			if (bClick)
				if (m_iLevelUp_Buffer_Point[i] > 0)
				{
					m_pSoundCom->Play2D(TEXT("SE_UI_QuickSlotChange_03.wav"), &g_fUIVolume);
					--m_iLevelUp_Buffer_Point[i];
					--m_iUsing_Point_Now;

					m_iUsing_Ergo_Now -= pOrigin.iErgo_LevelUp + (m_iLevelUp_Buffer_Point[i] * 10);
				}	
		}

		if (vMouseBack.x != -1.f)
		{
			m_iFocus_Point = i;
			if (bClick)
			{
				m_pSoundCom->Play2D(TEXT("SE_UI_QuickSlotChange_03.wav"), &g_fUIVolume);
				++m_iLevelUp_Buffer_Point[i];
				++m_iUsing_Point_Now;

				m_iUsing_Ergo_Now += pOrigin.iErgo_LevelUp + ((m_iLevelUp_Buffer_Point[i] - 1) * 10);
			}
		}
	}
}

void CUIPage_Stat::Action_LevelUp(_float fTimeDelta)
{
	const CPlayer::PLAYER_STAT_INFO pOrigin = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat();
	CPlayer::PLAYER_STAT_INFO* pAdjust = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust();

	_int iErgo = pOrigin.iErgo + pAdjust->iErgo;

	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ERGO_NEED))->strText = to_wstring(m_iUsing_Ergo_Now);
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ERGO_NOW))->strText = to_wstring(iErgo);
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ERGO_DEPOSIT_NUM_0))->strText = to_wstring(iErgo);
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ERGO_DEPOSIT_NUM_1))->strText = TEXT("0");

	m_bActive_LevelUp_Button = false;

	if (m_iUsing_Point_Now > 0)
		if (iErgo >= m_iUsing_Ergo_Now)
			m_bActive_LevelUp_Button = true;

	if (m_bActive_LevelUp_Button)
	{
		if (KEY_TAP(KEY::LBUTTON))
			if (GET_GAMEINTERFACE->CheckMouse(__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_LEVELUP_BUTTON))->fPosition, __super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_LEVELUP_BUTTON))->fSize).x != -1.f)
			{
				if (iErgo < m_iUsing_Ergo_Now)
					GET_GAMEINTERFACE->Show_Popup(TEXT("레벨 업 불가"), TEXT("에르고가 부족합니다."));
				else
					Input_LevelUp_Stat();
			}
	}
}

void CUIPage_Stat::Action_Focus(_float fTimeDelta)
{
	_bool bFind = false;

	for (_int i = 0; i < 5; ++i)
	{
		for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_RECT_0) + i]->PartIndexlist)
		{
			if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[iter]->fPosition, m_vecPart[iter]->fSize).x != -1.f)
			{
				m_iFocus_Point = i;
				bFind = true;
				break;
			}
		}

		if (bFind) break;
	}

	// Rect 
	Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_RECT))->iParentPart_Index = _int(PART_GROUP::GROUP_POINT_RECT_0) + m_iFocus_Point;

	// Focus 
	if (Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->iParentPart_Index != _int(PART_GROUP::GROUP_POINT_RECT_0) + m_iFocus_Point)
	{
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->iParentPart_Index = _int(PART_GROUP::GROUP_POINT_RECT_0) + m_iFocus_Point;
		_Vec2 vPos = Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POINT_RECT_0) + m_iFocus_Point)->fPosition;
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->fAdjust_Start = Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->fPosition - vPos;
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->fAdjust_End = { 0.f,0.f };
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->fRatio = 0.f;
		m_vec_Group_Ctrl[(_int(PART_GROUP::GROUP_SELECT_FIRE))]->fRatio = 0.f;
	}

	if (Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->fRatio > 1.f)
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->fRatio = 1.f;
}

void CUIPage_Stat::Update_Point(_float fTimeDelta)
{
	const CPlayer::PLAYER_STAT_INFO pOrigin = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat();
	CPlayer::PLAYER_STAT_INFO* pAdjust = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust();

	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_NUM_0)]->PartIndexlist.front()]->strText = to_wstring((m_iLevelUp_Buffer_Point[0] + pOrigin.iPoint_HP) / 10);
	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_NUM_0)]->PartIndexlist.back()]->strText = to_wstring((m_iLevelUp_Buffer_Point[0] + pOrigin.iPoint_HP) % 10);
	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_NUM_1)]->PartIndexlist.front()]->strText = to_wstring((m_iLevelUp_Buffer_Point[1] + pOrigin.iPoint_Stamina) / 10);
	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_NUM_1)]->PartIndexlist.back()]->strText = to_wstring((m_iLevelUp_Buffer_Point[1] + pOrigin.iPoint_Stamina) % 10);
	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_NUM_2)]->PartIndexlist.front()]->strText = to_wstring((m_iLevelUp_Buffer_Point[2] + pOrigin.iPoint_Attack) / 10);
	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_NUM_2)]->PartIndexlist.back()]->strText = to_wstring((m_iLevelUp_Buffer_Point[2] + pOrigin.iPoint_Attack) % 10);
	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_NUM_3)]->PartIndexlist.front()]->strText = to_wstring((m_iLevelUp_Buffer_Point[3] + pOrigin.iPoint_Defence) / 10);
	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_NUM_3)]->PartIndexlist.back()]->strText = to_wstring((m_iLevelUp_Buffer_Point[3] + pOrigin.iPoint_Defence) % 10);
	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_NUM_4)]->PartIndexlist.front()]->strText = to_wstring((m_iLevelUp_Buffer_Point[4] + pOrigin.iPoint_Heal) / 10);
	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POINT_NUM_4)]->PartIndexlist.back()]->strText = to_wstring((m_iLevelUp_Buffer_Point[4] + pOrigin.iPoint_Heal) % 10);
}

void CUIPage_Stat::Update_LevelUp(_float fTimeDelta)
{
	if (m_bActive_LevelUp_Button == false)
	{
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_LEVELUP_FX))->bRender = false;
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_LEVELUP_TEXT))->fTextColor = { 0.5f, 0.5f, 0.5f, 1.f };
	}
	else
	{
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_LEVELUP_FX))->bRender = true;
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_LEVELUP_TEXT))->fTextColor = { 1.f, 1.f, 1.f, 1.f };
	}
}

void CUIPage_Stat::Update_Focus(_float fTimeDelta)
{
	if (m_iFocus_Point > -1)
	{

		_int iIndex = Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_POINT_RECT_0) + m_iFocus_Point);
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_RECT))->iParentPart_Index = Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_POINT_RECT_0) + m_iFocus_Point);
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->iParentPart_Index = Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_POINT_RECT_0) + m_iFocus_Point);
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_RECT))->bRender = true;
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->bRender = true;

		m_vec_Group_Ctrl[(_int(PART_GROUP::GROUP_SELECT_FIRE))]->fRatio += fTimeDelta * 10.f;
	}
	else
	{
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_RECT))->bRender = false;
		Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_SELECT_FIRE))->bRender = false;
	}
}

void CUIPage_Stat::Update_SpecData(_float fTimeDelta)
{
	const CPlayer::PLAYER_STAT_INFO pOrigin = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat();
	CPlayer::PLAYER_STAT_INFO* pAdjust = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust();

	_float* pOrigin_Stat = new _float[9];
	_float* pAdjust_Stat = new _float[9];

	pOrigin_Stat[0] = pOrigin.vGauge_Hp.z;
	pOrigin_Stat[1] = pOrigin.vGauge_Stamina.z;
	pOrigin_Stat[2] = pOrigin.iStat_Attack;
	pOrigin_Stat[3] = pOrigin.iStat_Defence;
	pOrigin_Stat[4] = pOrigin.fHeal;
	pOrigin_Stat[5] = pOrigin.vGauge_Region.z;
	pOrigin_Stat[6] = pOrigin.fResist_Fire;
	pOrigin_Stat[7] = pOrigin.fResist_Electric;
	pOrigin_Stat[8] = pOrigin.fResist_Acid;

	pAdjust_Stat[0] = pAdjust->vGauge_Hp.z;
	pAdjust_Stat[1] = pAdjust->vGauge_Stamina.z;
	pAdjust_Stat[2] = pAdjust->iStat_Attack;
	pAdjust_Stat[3] = pAdjust->iStat_Defence;
	pAdjust_Stat[4] = pAdjust->fHeal;
	pAdjust_Stat[5] = pAdjust->vGauge_Region.z;
	pAdjust_Stat[6] = pAdjust->fResist_Fire;
	pAdjust_Stat[7] = pAdjust->fResist_Electric;
	pAdjust_Stat[8] = pAdjust->fResist_Acid;

	// 곱해지는 수치가 스탯 포인트 1당 올라가는 수치
	m_iLevelUp_Buffer_Stat[0] = m_iLevelUp_Buffer_Point[0] * 50.f; // 체력 최대치
	m_iLevelUp_Buffer_Stat[1] = m_iLevelUp_Buffer_Point[1] * 50.f; // 스태미나 최대치
	m_iLevelUp_Buffer_Stat[2] = m_iLevelUp_Buffer_Point[2] * 10.f; // 공격력
	m_iLevelUp_Buffer_Stat[3] = m_iLevelUp_Buffer_Point[3] * 10.f; // 방어력
	m_iLevelUp_Buffer_Stat[4] = m_iLevelUp_Buffer_Point[4] * 10.f; // 회복력 
	m_iLevelUp_Buffer_Stat[5] = m_iLevelUp_Buffer_Point[1] * 10.f; // 리전 최대치 (페이블 아츠)
	m_iLevelUp_Buffer_Stat[6] = m_iLevelUp_Buffer_Point[3] * 10.f; // 화염 저항
	m_iLevelUp_Buffer_Stat[7] = m_iLevelUp_Buffer_Point[2] * 10.f; // 전격 저항
	m_iLevelUp_Buffer_Stat[8] = m_iLevelUp_Buffer_Point[3] * 10.f; // 산성 저항

	


	for (_int i = 0; i < 9; ++i)
	{
		pOrigin_Stat[i] *= 100.f;
		_float fRound = round(pOrigin_Stat[i]);
		pOrigin_Stat[i] /= 100.f;

		pAdjust_Stat[i] *= 100.f;
		fRound = round(pAdjust_Stat[i]);
		pAdjust_Stat[i] /= 100.f;

		m_iLevelUp_Buffer_Stat[i] *= 100.f;
		fRound = round(m_iLevelUp_Buffer_Stat[i]);
		m_iLevelUp_Buffer_Stat[i] /= 100.f;
	}

	if (pAdjust->bDebuff_Fire_Ignore == true)
		m_iLevelUp_Buffer_Stat[6] = -1.f;
	if (pAdjust->bDebuff_Electric_Ignore == true)
		m_iLevelUp_Buffer_Stat[7] = -1.f;
	if (pAdjust->bDebuff_Acid_Ignore == true)
		m_iLevelUp_Buffer_Stat[8] = -1.f;

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ABILITY_6)]->bRender = false;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ABILITY_7)]->bRender = false;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ABILITY_8)]->bRender = false;


	for (_int i = 0; i < 9; ++i)
	{
		list<_int>::iterator iter = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SPEC_0) + i]->PartIndexlist.begin();

		m_vecPart[*iter]->fTextureColor = { 0.2f,0.2f,0.2f,0.2f };


		if (m_iFocus_Point == 0)
			if (i == 0)
				m_vecPart[*iter]->fTextureColor = { 0.5f,1.f,0.9f,0.7f };

		if (m_iFocus_Point == 1)
			if ((i == 1) || (i == 5))
				m_vecPart[*iter]->fTextureColor = { 0.5f,1.f,0.9f,0.7f };

		if (m_iFocus_Point == 2)
			if ((i == 2) || (i == 7))
				m_vecPart[*iter]->fTextureColor = { 0.5f,1.f,0.9f,0.7f };
			
		if (m_iFocus_Point == 3)
		{
			if ((i == 3) || (i == 6))
				m_vecPart[*iter]->fTextureColor = { 0.5f,1.f,0.9f,0.7f };
			if (i == 8)
				m_vecPart[*iter]->fTextureColor = { 0.5f,1.f,0.9f,0.7f };
		}
			
		if (m_iFocus_Point == 4)
			if (i == 4)
				m_vecPart[*iter]->fTextureColor = { 0.5f,1.f,0.9f,0.7f };

		if (m_iLevelUp_Buffer_Stat[i] > 0.f)
		{
			++iter;
			m_vecPart[*iter]->strText = to_wstring(_int(pOrigin_Stat[i]));
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SPEC_0) + i]->fRatio = 0.f;
			m_vecPart[*iter]->fRatio = 0.f;
			m_vecPart[*iter]->fPosition = m_vecPart[*iter]->fAdjust_Start;
			++iter;
			m_vecPart[*iter]->strText = TEXT("(+");
			m_vecPart[*iter]->strText += to_wstring(_int(m_iLevelUp_Buffer_Stat[i]));
			m_vecPart[*iter]->strText += TEXT(")");
		}
		else if (m_iLevelUp_Buffer_Stat[i] == 0.f)
		{
			++iter;
			m_vecPart[*iter]->strText = to_wstring(_int(pOrigin_Stat[i]));
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SPEC_0) + i]->fRatio = 1.f;
			m_vecPart[*iter]->fRatio = 1.f;
			m_vecPart[*iter]->fPosition = m_vecPart[*iter]->fAdjust_End;
			++iter;
			m_vecPart[*iter]->strText = {};
		}
		else if (m_iLevelUp_Buffer_Stat[i] < 0.f)
		{
			for (_int j = 0; j < 4; ++j)
			{
				++iter;
				m_vecPart[*iter]->strText = {};
			}
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SPEC_0) + i]->bRender = false;
			if (i >= _int(PART_GROUP::GROUP_ABILITY_6))
				m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ABILITY_6) + i]->bRender = true;

			continue;
		}

		++iter;
		m_vecPart[*iter]->strText = to_wstring(_int(pAdjust_Stat[i]));

		++iter;
		m_vecPart[*iter]->strText = to_wstring(_int(pOrigin_Stat[i] + m_iLevelUp_Buffer_Stat[i] + pAdjust_Stat[i]));
	}

	Safe_Delete_Array(pOrigin_Stat); 
	Safe_Delete_Array(pAdjust_Stat);
}

void CUIPage_Stat::Update_StarChart(_float fTimeDelta)
{
	UPART* pOriginPart = m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_STARCHART_ORIGIN)]->PartIndexlist.front()];
	UPART* pLevelUpPart = m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_STARCHART_LEVELUP)]->PartIndexlist.front()];

	const CPlayer::PLAYER_STAT_INFO pOrigin = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat();
	CPlayer::PLAYER_STAT_INFO* pAdjust = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust();

	m_fStarChart_NowStat_Dest[0] = _float(pAdjust->iStat_Attack + pOrigin.iStat_Attack) / 2000.f;
	m_fStarChart_NowStat_Dest[1] = _float(pAdjust->iStat_Defence + pOrigin.iStat_Defence) / 2000.f;
	m_fStarChart_NowStat_Dest[2] = _float(pAdjust->fHeal + pOrigin.fHeal) / 200.f;
	m_fStarChart_NowStat_Dest[3] = (pAdjust->fResist_Fire + pOrigin.fResist_Fire) / 200.f;
	m_fStarChart_NowStat_Dest[4] = (pAdjust->fResist_Electric + pOrigin.fResist_Electric) / 200.f;
	m_fStarChart_NowStat_Dest[5] = (pAdjust->fResist_Acid + pOrigin.fResist_Acid) / 200.f;
	m_fStarChart_NowStat_Dest[6] = (pAdjust->vGauge_Hp.z + pOrigin.vGauge_Hp.z) / pOrigin.vGauge_Hp.w;
	m_fStarChart_NowStat_Dest[7] = (pAdjust->vGauge_Stamina.z + pOrigin.vGauge_Stamina.z) / pOrigin.vGauge_Stamina.w;

	m_fStarChart_LevelUp_Dest[0] = pOriginPart->fRatio_TwoDPolygon[0] + (m_iLevelUp_Buffer_Stat[2] / 2000.f);
	m_fStarChart_LevelUp_Dest[1] = pOriginPart->fRatio_TwoDPolygon[1] + (m_iLevelUp_Buffer_Stat[3] / 2000.f);
	m_fStarChart_LevelUp_Dest[2] = pOriginPart->fRatio_TwoDPolygon[2] + (m_iLevelUp_Buffer_Stat[4] / 200.f);
	m_fStarChart_LevelUp_Dest[3] = pOriginPart->fRatio_TwoDPolygon[3] + (m_iLevelUp_Buffer_Stat[6] / 200.f);
	m_fStarChart_LevelUp_Dest[4] = pOriginPart->fRatio_TwoDPolygon[4] + (m_iLevelUp_Buffer_Stat[7] / 200.f);
	m_fStarChart_LevelUp_Dest[5] = pOriginPart->fRatio_TwoDPolygon[5] + (m_iLevelUp_Buffer_Stat[8] / 200.f);
	m_fStarChart_LevelUp_Dest[6] = pOriginPart->fRatio_TwoDPolygon[6] + (m_iLevelUp_Buffer_Stat[0] / pOrigin.vGauge_Hp.w);
	m_fStarChart_LevelUp_Dest[7] = pOriginPart->fRatio_TwoDPolygon[7] + (m_iLevelUp_Buffer_Stat[1] / pOrigin.vGauge_Stamina.w);

	for (_int i = 0; i < 8; ++i)
	{
		if (m_fStarChart_NowStat_Now[i] < m_fStarChart_NowStat_Dest[i])
			m_fStarChart_NowStat_Now[i] = min(m_fStarChart_NowStat_Now[i] + fTimeDelta, m_fStarChart_NowStat_Dest[i]);
		else if (m_fStarChart_NowStat_Now[i] > m_fStarChart_NowStat_Dest[i])
			m_fStarChart_NowStat_Now[i] = max(m_fStarChart_NowStat_Now[i] - fTimeDelta, m_fStarChart_NowStat_Dest[i]);

		if (m_fStarChart_LevelUp_Now[i] < m_fStarChart_LevelUp_Dest[i])
			m_fStarChart_LevelUp_Now[i] = min(m_fStarChart_LevelUp_Now[i] + fTimeDelta, m_fStarChart_LevelUp_Dest[i]);
		else if (m_fStarChart_LevelUp_Now[i] > m_fStarChart_LevelUp_Dest[i])
			m_fStarChart_LevelUp_Now[i] = max(m_fStarChart_LevelUp_Now[i] - fTimeDelta, m_fStarChart_LevelUp_Dest[i]);
	}

	for (_int i = 0; i < 8; ++i)
	{
		pOriginPart->fRatio_TwoDPolygon[i] = m_fStarChart_NowStat_Now[i];
		pLevelUpPart->fRatio_TwoDPolygon[i] = m_fStarChart_LevelUp_Now[i];
	}
}

void CUIPage_Stat::Input_LevelUp_Stat()
{
	GET_GAMEINTERFACE->Get_Player()->Input_Level_UP_Stat(m_iLevelUp_Buffer_Point, m_iLevelUp_Buffer_Stat);
	_int iCost = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat().iErgo_LevelUp;
	GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust()->iErgo -= m_iUsing_Ergo_Now;

	for (_int i = 0; i < 5; ++i)
		m_iLevelUp_Buffer_Point[i] = 0;

	for (_int i = 0; i < 9; ++i)
		m_iLevelUp_Buffer_Stat[i] = 0.f;
	
	GET_GAMEINTERFACE->Input_Achievment_Data(0, m_iUsing_Ergo_Now);
	GET_GAMEINTERFACE->Input_Achievment_Data(10, 1);

	_float fHp = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat().vGauge_Hp.z;
	fHp += GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust()->vGauge_Hp.z;

	GET_GAMEINTERFACE->Input_Achievment_Data(3, (_int)fHp);

	GET_GAMEINTERFACE->Input_Achievment_Data(10, 1);

	m_pSoundCom->Play2D(TEXT("SE_UI_LevelUP.wav"), &g_fUIVolume);
	//_wstring strErgo_Spend_Inform = TEXT("에르고 사용량 : ");
	//strErgo_Spend_Inform += to_wstring(m_iUsing_Point_Now * iCost);
	//GET_GAMEINTERFACE->Show_Popup(TEXT("레벨 업 성공"), strErgo_Spend_Inform);

	m_iUsing_Point_Now = 0;
	m_iUsing_Ergo_Now = 0;
}

CUIPage_Stat* CUIPage_Stat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Stat* pInstance = new CUIPage_Stat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Stat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Stat::Clone(void* pArg)
{
	CUIPage_Stat* pInstance = new CUIPage_Stat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Stat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Stat::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
