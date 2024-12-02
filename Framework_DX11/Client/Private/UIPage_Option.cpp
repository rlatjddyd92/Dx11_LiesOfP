#include "stdafx.h"
#include "..\Public\UIPage_Option.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Option::CUIPage_Option(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Option::CUIPage_Option(const CUIPage_Option& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Option::Initialize_Prototype()
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

HRESULT CUIPage_Option::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Option::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Option::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pScroll_Option == nullptr)
	{
		SCROLL* pScroll_Option = new SCROLL;
		pScroll_Option->Initialize_Scroll(m_vecPart[_int(PART_GROUP::OPTION_Main_Scroll_Area)], m_vecPart[_int(PART_GROUP::OPTION_Main_Scroll_Bar)], SCROLL_AREA::SCROLL_OPTION);
		m_pScroll_Option = pScroll_Option;
	}

	if (m_pScroll_DropBox == nullptr)
	{
		SCROLL* pScroll_Dropbox = new SCROLL;
		pScroll_Dropbox->Initialize_Scroll(m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)], m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Scroll_Bar)], SCROLL_AREA::SCROLL_OPTION_DROPBOX);
		m_pScroll_DropBox = pScroll_Dropbox;
	}


	



}

void CUIPage_Option::Late_Update(_float fTimeDelta)
{
	if (!m_bRender)
		return;

	__super::Late_Update(fTimeDelta);
	m_vecPart[_int(PART_GROUP::OPTION_Focus)]->bRender = false;
	m_vecPart[_int(PART_GROUP::OPTION_Highlight_Line)]->bRender = false;

	Update_Tab(fTimeDelta);

	if (m_pScroll_Option->bIsActive)
		Action_Scroll(fTimeDelta);

	Update_Line(fTimeDelta);
	Update_Right_Side(fTimeDelta);
	Update_Focus_Highlight(fTimeDelta);

	Update_Variable();
}

HRESULT CUIPage_Option::Render()
{
	return S_OK;
}

void CUIPage_Option::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Option::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Option::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Option::Ready_UIPart_Group_Control()
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

	Initialize_Option();

	return S_OK;
}

_bool CUIPage_Option::Input_Variable_Pointer_Into_Option(_int iKey, _bool* pVar)
{
	map<_int, _bool*>::iterator iter = m_mapVariables_Bool.find(iKey);
	if (iter == m_mapVariables_Bool.end())
		return false;

	iter->second = pVar;

	return true;
}

_bool CUIPage_Option::Input_Variable_Pointer_Into_Option(_int iKey, _float* pVar)
{
	map<_int, _float*>::iterator iter = m_mapVariables_Slide.find(iKey);
	if (iter == m_mapVariables_Slide.end())
		return false;

	iter->second = pVar;

	return true;
}

_bool CUIPage_Option::Input_Variable_Pointer_Into_Option(_int iKey, _int* pVar)
{
	map<_int, _int*>::iterator iter = m_mapVariables_DropBox.find(iKey);
	if (iter == m_mapVariables_DropBox.end())
		return false;

	iter->second = pVar;

	return true;
}


HRESULT CUIPage_Option::Initialize_Option()
{
	vector<vector<_wstring>> vecBuffer_Spec;
	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Option_Info.csv", &vecBuffer_Spec)))
		return E_FAIL;

	_int iNowTabNum = -1;

	for (_int i = 1; i < vecBuffer_Spec.size(); ++i)
	{
		if (iNowTabNum < stoi(vecBuffer_Spec[i][0]))
		{
			TAB* pNewTab = new TAB;
			pNewTab->strName = vecBuffer_Spec[i][1];
			m_vecOption_TabInfo.push_back(pNewTab);
			++iNowTabNum;
		}

		LINE* pNewLine = new LINE;

		pNewLine->eFunc = OPTION_FUNC(stoi(vecBuffer_Spec[i][2]));
		pNewLine->strName = vecBuffer_Spec[i][3];

		FUNCTION* pNewFunc = new FUNCTION;

		pNewFunc->iVariable_Key = stoi(vecBuffer_Spec[i][4]);

		if (pNewLine->eFunc == OPTION_FUNC::FUNC_BOOL)
		{
			pNewFunc->strLeft = vecBuffer_Spec[i][5];
			pNewFunc->strRight = vecBuffer_Spec[i][6];

			m_mapVariables_Bool.insert({ pNewFunc->iVariable_Key , nullptr });
		}
		else if (pNewLine->eFunc == OPTION_FUNC::FUNC_SLIDE)
		{
			pNewFunc->bIsShow_IntNum = stoi(vecBuffer_Spec[i][7]);
			pNewFunc->fMin = stof(vecBuffer_Spec[i][8]);
			pNewFunc->fNow = stof(vecBuffer_Spec[i][9]);
			pNewFunc->fMax = stof(vecBuffer_Spec[i][10]);

			pNewFunc->fNow = max(pNewFunc->fNow, pNewFunc->fMin);
			pNewFunc->fNow = min(pNewFunc->fNow, pNewFunc->fMax);

			pNewFunc->fInterval = stof(vecBuffer_Spec[i][11]);
			pNewFunc->fInterval_Ratio = (pNewFunc->fInterval / (pNewFunc->fMax - pNewFunc->fMin));
			pNewFunc->fInterval_X = (pNewFunc->fMax - pNewFunc->fMin) * pNewFunc->fInterval_Ratio;

			m_mapVariables_Slide.insert({ pNewFunc->iVariable_Key , nullptr });
		}
		else if (pNewLine->eFunc == OPTION_FUNC::FUNC_DROPBOX)
		{
			pNewFunc->iSize_Select_Button = stoi(vecBuffer_Spec[i][12]);
			for (_int j = 0; j <= pNewFunc->iSize_Select_Button; ++j)
				pNewFunc->vecSelect_Name.push_back(vecBuffer_Spec[i][13 + j]);

			m_mapVariables_DropBox.insert({ pNewFunc->iVariable_Key , nullptr });
		}
		pNewFunc->strDescription = vecBuffer_Spec[i][28];
		pNewLine->pFunction = pNewFunc;
		m_vecOption_TabInfo.back()->vecOption_Line.push_back(pNewLine);
	}

	// 그래픽 변수 
	//_bool* pGraTitle = new _bool;
	//Input_Variable_Pointer_Into_Option(11100100, pGraTitle);
	Input_Variable_Pointer_Into_Option(1, &m_bGraphic[0]);
	Input_Variable_Pointer_Into_Option(2, &m_bGraphic[1]);
	Input_Variable_Pointer_Into_Option(3, &m_bGraphic[2]);
	Input_Variable_Pointer_Into_Option(4, &m_bGraphic[3]);
	Input_Variable_Pointer_Into_Option(5, &m_bGraphic[4]);
	Input_Variable_Pointer_Into_Option(6, &m_bGraphic[5]);

	// 사운드 변수 - 전역 
	//_bool* pSoundTitle = new _bool;
	//Input_Variable_Pointer_Into_Option(11100101, pSoundTitle);
	Input_Variable_Pointer_Into_Option(8, &g_fBGMVolume);
	Input_Variable_Pointer_Into_Option(9, &g_fEffectVolume);
	Input_Variable_Pointer_Into_Option(10, &g_fVoiceVolume);
	Input_Variable_Pointer_Into_Option(11, &g_fEnvVolume);
	Input_Variable_Pointer_Into_Option(12, &g_fUIVolume);


	for (_int i = 0; i < m_vecOption_TabInfo.size(); ++i)
	{
		for (_int j = 0; j < m_vecOption_TabInfo[i]->vecOption_Line.size(); ++j)
		{
			LINE* pNow = m_vecOption_TabInfo[i]->vecOption_Line[j];
			// 테스트 코드
			if (i >= 2)
			{
				if (pNow->eFunc == OPTION_FUNC::FUNC_BOOL)
				{
					_bool* pNew = new _bool;
					Input_Variable_Pointer_Into_Option(pNow->pFunction->iVariable_Key, pNew);
				}
				else if (pNow->eFunc == OPTION_FUNC::FUNC_SLIDE)
				{
					_float* pNew = new _float;
					Input_Variable_Pointer_Into_Option(pNow->pFunction->iVariable_Key, pNew);
				}
				else if (pNow->eFunc == OPTION_FUNC::FUNC_DROPBOX)
				{
					_int* pNew = new _int;
					Input_Variable_Pointer_Into_Option(pNow->pFunction->iVariable_Key, pNew);
				}
			}

			if (pNow->eFunc == OPTION_FUNC::FUNC_BOOL)
				pNow->pFunction->bIsSelect_Left = *m_mapVariables_Bool.find(pNow->pFunction->iVariable_Key)->second;
			else if (pNow->eFunc == OPTION_FUNC::FUNC_SLIDE)
				pNow->pFunction->fNow = *m_mapVariables_Slide.find(pNow->pFunction->iVariable_Key)->second;
			else if (pNow->eFunc == OPTION_FUNC::FUNC_DROPBOX)
				pNow->pFunction->iSelected_Num = *m_mapVariables_DropBox.find(pNow->pFunction->iVariable_Key)->second;
		}
	}


	m_vecPart[_int(PART_GROUP::OPTION_Calibration_Frame)]->bRender = false;
	m_vecPart[_int(PART_GROUP::OPTION_Calibration_Image)]->bRender = false;
	m_vecPart[_int(PART_GROUP::OPTION_Calibration_Text)]->bRender = false;
}

void CUIPage_Option::Update_Tab(_float fTimeDelta)
{
	m_vecPart[_int(PART_GROUP::OPTION_Highlight_Line)]->bRender = false;


	// 배경 렌더 요청 

	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Back)]);
	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Back_Grid)]);
	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Back_Fx)]);

	// 탭 버튼 위치 잡기 -> 마우스 확인 -> 렌더 요청 
	_int iMoveTap = 0;
	_int iNowCheckTap = 0;
	_bool bMouseLButton = KEY_TAP(KEY::LBUTTON);

	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Tap_Q_Button)]);
	_int iTab_Origin = m_iNow_Tab;
	_int iNowMouse = -1;
	
	
	_Vec2 vPos = GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_Tap_Q_Button)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_Tap_Q_Button)]->fSize);
	m_bChange_Tab = false;

	for (auto& iter : m_vecOption_TabInfo)
	{
		if (m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)])
		{
			vPos = GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fSize);

			if (vPos.x != -1.f)
			{
				iNowMouse = iNowCheckTap;
				if (bMouseLButton)
					m_iNow_Tab = iNowCheckTap;
			}
		}

		m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fPosition.x += m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fSize.x;
		++iNowCheckTap;
	}

		
	
	
	m_vecPart[_int(PART_GROUP::OPTION_Tap_E_Button)]->fPosition.x = m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fPosition.x + m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fAdjust.x;

	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Tap_E_Button)]);

	if (m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)])
	{
		if ((KEY_TAP(KEY::Q)) || (iMoveTap == -1))
			--m_iNow_Tab;
		else if ((KEY_TAP(KEY::E)) || (iMoveTap == 1))
			++m_iNow_Tab;
		else if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_Tap_E_Button)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_Tap_E_Button)]->fSize).x != -1.f)
		{
			if (bMouseLButton)
				++m_iNow_Tab;
		}
		else if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_Tap_Q_Button)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_Tap_Q_Button)]->fSize).x != -1.f)
		{
			if (bMouseLButton)
				--m_iNow_Tab;
		}

		m_iNow_Tab = max(m_iNow_Tab, 0);
		m_iNow_Tab = min(m_iNow_Tab, m_vecOption_TabInfo.size() - 1);
	}

	m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fPosition.x -= (m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fSize.x * (m_vecOption_TabInfo.size()));
	iNowCheckTap = 0;

	// 그리기 
	for (auto& iter : m_vecOption_TabInfo)
	{
		if (iNowCheckTap == m_iNow_Tab)
		{
			m_vecPart[_int(PART_GROUP::OPTION_Highlight_Line)]->fPosition = m_vecPart[_int(PART_GROUP::OPTION_Tap_Highlight_Line_Pos)]->fPosition;
			m_vecPart[_int(PART_GROUP::OPTION_Highlight_Line)]->bRender = true;
			m_vecPart[_int(PART_GROUP::OPTION_Tap_Text)]->Set_RedText();
		}
		else
		{
			m_vecPart[_int(PART_GROUP::OPTION_Tap_Text)]->Set_WhiteText();
		}

		if (iNowCheckTap == iNowMouse)
			Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Tap_Text_Fx)]);

		m_vecPart[_int(PART_GROUP::OPTION_Tap_Text)]->strText = m_vecOption_TabInfo[iNowCheckTap]->strName;
		Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Tap_Text)]);

		m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fPosition.x += m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fSize.x;
		for (_int i = _int(PART_GROUP::OPTION_Tap_Text_Fx); i <= _int(PART_GROUP::OPTION_Tap_Highlight_Line_Pos); ++i)
			UpdatePart_ByIndex(i, fTimeDelta);

		++iNowCheckTap;
	}

	m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fPosition.x -= (m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fSize.x * (m_vecOption_TabInfo.size()));

	if (iTab_Origin != m_iNow_Tab)
		m_bChange_Tab = true;
}

void CUIPage_Option::Action_Scroll(_float fTimeDelta)
{
	if (!m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)])
	{
		if (m_pScroll_Option->bIsBarMoving)
		{
			if (KEY_HOLD(KEY::LBUTTON))
			{
				POINT			ptMouse{};
				GetCursorPos(&ptMouse);
				ScreenToClient(g_hWnd, &ptMouse);
				m_pScroll_Option->Bar_Moving(_float(ptMouse.y));
				m_bIsCloseAction_DropBox = true;
			}
			else
				m_pScroll_Option->End_Bar_Moving();
		}
		else
		{
			_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_Main_Scroll_Bar)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_Main_Scroll_Bar)]->fSize);

			if (vMouse.x != -1.f)
				if (KEY_TAP(KEY::LBUTTON))
				{
					m_pScroll_Option->Start_Bar_Moving(vMouse.y);
					m_bIsCloseAction_DropBox = true;
				}

		}
	}

	

	m_vecPart[_int(PART_GROUP::OPTION_Main_Scroll_Bar)]->fRatio = m_pScroll_Option->fScroll_Ratio;

	for (_int i = _int(PART_GROUP::OPTION_Main_Scroll_Bar_Line); i <= _int(PART_GROUP::OPTION_Main_Scroll_Bar); ++i)
	{
		__super::UpdatePart_ByIndex(i, fTimeDelta);
		Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_NONE);
	}
}

void CUIPage_Option::Update_Line(_float fTimeDelta)
{
	_float fStartY = m_vecPart[_int(PART_GROUP::OPTION_Main_Scroll_Area)]->fPosition.y - m_vecPart[_int(PART_GROUP::OPTION_Main_Scroll_Area)]->fSize.y * 0.5f - m_pScroll_Option->fData_Offset_Y;
	_float fAdjustY = 0.f;

	_float fHeight_Title = m_vecPart[_int(PART_GROUP::OPTION_TITLE_Area)]->fSize.y;
	_float fHeight_Line = m_vecPart[_int(PART_GROUP::OPTION_LINE_Area)]->fSize.y;

	_int iLineIndex = 0;

	for (auto& iter : m_vecOption_TabInfo[m_iNow_Tab]->vecOption_Line)
	{
		OPTION_FUNC eFunc = iter->eFunc;

		if (eFunc == OPTION_FUNC::FUNC_TITLE)
		{
			fAdjustY += fHeight_Title * 0.5f;
			if (m_pScroll_Option->Check_Is_Render_Y(fStartY + fAdjustY, m_vecPart[_int(PART_GROUP::OPTION_TITLE_Area)]->fSize.y))
			{
				m_vecPart[_int(PART_GROUP::OPTION_TITLE_Area)]->fPosition.y = fStartY + fAdjustY;

				m_vecPart[_int(PART_GROUP::OPTION_TITLE_Text)]->strText = iter->strName;
				for (_int i = _int(PART_GROUP::OPTION_TITLE_Text); i <= _int(PART_GROUP::OPTION_TITLE_Underline); ++i)
				{
					UpdatePart_ByIndex(i, fTimeDelta);
					Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_OPTION);
				}
			}
			fAdjustY += fHeight_Title * 0.5f;
		}
		else
		{
			_bool bMouse = false;
			fAdjustY += fHeight_Line * 0.5f;
			_bool bFuncion_Render = false;
			if (m_pScroll_Option->Check_Is_Render_Y(fStartY + fAdjustY, m_vecPart[_int(PART_GROUP::OPTION_LINE_Area)]->fSize.y))
			{
				bFuncion_Render = true;
				m_vecPart[_int(PART_GROUP::OPTION_LINE_Area)]->fPosition.y = fStartY + fAdjustY;
				if (m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)])
				{
					_Vec2 vPos = GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_LINE_Area)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_LINE_Area)]->fSize);
					if (vPos.x != -1.f)
						bMouse = true;
				}
				

				m_vecPart[_int(PART_GROUP::OPTION_LINE_Text)]->strText = iter->strName;

				for (_int i = _int(PART_GROUP::OPTION_LINE_Focus_Pos); i <= _int(PART_GROUP::OPTION_LINE_Underline); ++i)
				{
					UpdatePart_ByIndex(i, fTimeDelta);
					if (i > _int(PART_GROUP::OPTION_LINE_Focus_Pos))
						Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_OPTION);
				}
				Change_Focus_Mark_Destination(m_vecPart[_int(PART_GROUP::OPTION_LINE_Focus_Pos)]->fPosition);
			}

			if (bFuncion_Render)
			{
				m_iNow_Line = iLineIndex;

				if (eFunc == OPTION_FUNC::FUNC_BOOL)
				{
					Update_BoolButton(*iter->pFunction, fTimeDelta, bMouse);
				}
				else if (eFunc == OPTION_FUNC::FUNC_SLIDE)
				{
					Update_Slide(*iter->pFunction, fTimeDelta, bMouse);
				}
				else if (eFunc == OPTION_FUNC::FUNC_DROPBOX)
				{
					Update_Dropbox(*iter->pFunction, fTimeDelta, bMouse);
				}
				else if (eFunc == OPTION_FUNC::FUNC_BUTTON)
				{
					
				}
			}

			fAdjustY += fHeight_Line * 0.5f;
		}

		++iLineIndex;
	}

	m_bIsCloseAction_DropBox = false;

	if (m_bChange_Tab)
	{
		m_iNow_Line = 0;
		m_bIsCloseAction_DropBox = true;
		m_pScroll_Option->Activate_Scroll(fAdjustY);
	}
}

void CUIPage_Option::Update_BoolButton(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse)
{
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Left_Text)]->strText = NowFunction.strLeft;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Right_Text)]->strText = NowFunction.strRight;

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Bool_Frame); i <= _int(PART_GROUP::OPTION_FUNC_Bool_Right_UnderLine); ++i)
		__super::UpdatePart_ByIndex(i, fTimeDelta);

	_int iFocus = 0;

	if (m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)])
	{
		if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Left_Area)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Left_Area)]->fSize).x != -1.f)
		{
			m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fAdjust_End = m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Left_Focus_Pos)]->fPosition;
			iFocus = -1;

			if (KEY_TAP(KEY::LBUTTON))
			{
				NowFunction.bIsSelect_Left = true;
			}

		}
		else if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Right_Area)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Right_Area)]->fSize).x != -1.f)
		{
			m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fAdjust_End = m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Right_Focus_Pos)]->fPosition;
			iFocus = 1;

			if (KEY_TAP(KEY::LBUTTON))
			{
				NowFunction.bIsSelect_Left = false;
			}
		}
	}

	

	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Left_Focus_Pos)]->bRender = (iFocus == -1) ? true : false;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Right_Focus_Pos)]->bRender = (iFocus == 1) ? true : false;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Left_Area)]->bRender = (NowFunction.bIsSelect_Left == true) ? true : false;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Left_UnderLine)]->bRender = (NowFunction.bIsSelect_Left == true) ? true : false;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Right_Area)]->bRender = (NowFunction.bIsSelect_Left == false) ? true : false;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Right_UnderLine)]->bRender = (NowFunction.bIsSelect_Left == false) ? true : false;

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Bool_Frame); i <= _int(PART_GROUP::OPTION_FUNC_Bool_Right_UnderLine); ++i)
		if (m_vecPart[i]->bRender)
			Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_OPTION);

	if (NowFunction.bIsSelect_Left)
		*m_mapVariables_Bool.find(NowFunction.iVariable_Key)->second = true;
	else 
		*m_mapVariables_Bool.find(NowFunction.iVariable_Key)->second = false;
}

void CUIPage_Option::Update_Slide(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse)
{
	_int iArrowMoving = 0;
	_bool bIsClick = KEY_TAP(KEY::LBUTTON);

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Slide_Area); i <= _int(PART_GROUP::OPTION_FUNC_Slide_Right_Arrow); ++i)
		__super::UpdatePart_ByIndex(i, fTimeDelta);


	if (m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)])
	{
		if (bIsClick)
			if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Left_Arrow)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Left_Arrow)]->fSize).x != -1.f)
				iArrowMoving = -1;

		if (bIsClick)
			if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Right_Arrow)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Right_Arrow)]->fSize).x != -1.f)
				iArrowMoving = 1;

		if (!NowFunction.bIsButton_Moving)
			NowFunction.fNow += NowFunction.fInterval * iArrowMoving;

		NowFunction.fNow = min(NowFunction.fNow, NowFunction.fMax);
		NowFunction.fNow = max(NowFunction.fNow, 0.f);
	}
	

	Update_Slide_Button(NowFunction, fTimeDelta, bMouse);

	if (NowFunction.bIsShow_IntNum)
		m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Num)]->strText = to_wstring(_int(NowFunction.fNow));
	else
		m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Num)]->strText = to_wstring(NowFunction.fNow);

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Slide_Area); i <= _int(PART_GROUP::OPTION_FUNC_Slide_Right_Arrow); ++i)
		Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_OPTION);

	*m_mapVariables_Slide.find(NowFunction.iVariable_Key)->second = _float(NowFunction.fNow);
}

void CUIPage_Option::Update_Slide_Button(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse)
{
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fRatio = (NowFunction.fNow - NowFunction.fMin) / (NowFunction.fMax - NowFunction.fMin);
	__super::UpdatePart_ByIndex(_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button), fTimeDelta);

	if (m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)])
	{
		if (NowFunction.bIsButton_Moving)
		{
			if (KEY_HOLD(KEY::LBUTTON))
			{
				m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fRatio = 0.f;
				__super::UpdatePart_ByIndex(_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button), fTimeDelta);
				_float fOriginX = m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fPosition.x;

				_float fEndX = m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fAdjust_End.x * 2.f;

				POINT			ptMouse{};
				GetCursorPos(&ptMouse);
				ScreenToClient(g_hWnd, &ptMouse);

				_float fNowX = _float(ptMouse.x);

				if (fNowX >= fOriginX + fEndX)
				{
					m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fRatio = 1.f;
					NowFunction.fNow = NowFunction.fMax;
					__super::UpdatePart_ByIndex(_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button), fTimeDelta);
				}
				else if (fNowX <= fOriginX)
				{
					m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fRatio = 0.f;
					NowFunction.fNow = NowFunction.fMin;
					__super::UpdatePart_ByIndex(_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button), fTimeDelta);
				}
				else
				{
					fNowX -= fOriginX;

					_float fRatio = fNowX / fEndX;

					_int iNowIndex = (_int)(fRatio / NowFunction.fInterval_Ratio);
					fRatio = NowFunction.fInterval_Ratio * iNowIndex;
					m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fRatio = fRatio;
					__super::UpdatePart_ByIndex(_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button), fTimeDelta);
					NowFunction.fNow = NowFunction.fMin + NowFunction.fInterval * iNowIndex;
				}
			}
			else
				NowFunction.bIsButton_Moving = false;
		}
		else
		{
			if (KEY_TAP(KEY::LBUTTON))
				if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fSize).x != -1.f)
					NowFunction.bIsButton_Moving = true;
		}
	}
	else 
		NowFunction.bIsButton_Moving = false;

}

void CUIPage_Option::Update_Dropbox(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse)
{
	// 안전 코드
	return;

	_bool bIsClick = KEY_TAP(KEY::LBUTTON);

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Dropbox_Area); i <= _int(PART_GROUP::OPTION_FUNC_Dropbox_Arrow); ++i)
		__super::UpdatePart_ByIndex(i, fTimeDelta);

	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Selected_Text)]->strText = NowFunction.vecSelect_Name[NowFunction.iSelected_Num];

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Dropbox_Area); i <= _int(PART_GROUP::OPTION_FUNC_Dropbox_Arrow); ++i)
		Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_OPTION);

	if (m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)])
	{
		if ((!NowFunction.bDropBox_On) && (!m_bIsCloseAction_DropBox))
		{
			if (bIsClick)
				if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Area)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Area)]->fSize).x != -1.f)
				{

					Open_Dropbox_SelectBox(NowFunction, fTimeDelta);
				}
		}

		if (NowFunction.bDropBox_On)
		{
			if (m_bIsCloseAction_DropBox)
				Close_Dropbox_SelectBox(NowFunction, fTimeDelta);
		}

		Update_Dropbox_SelectBox(NowFunction, fTimeDelta);
	}

	
}

void CUIPage_Option::Open_Dropbox_SelectBox(FUNCTION& NowFunction, _float fTimeDelta)
{
	_bool bIsClick = KEY_TAP(KEY::LBUTTON);

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Dropbox_Box_Header); i <= _int(PART_GROUP::OPTION_FUNC_Dropbox_Scroll_Bar); ++i)
		__super::UpdatePart_ByIndex(i, fTimeDelta);

	_float fScroll_Size = m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fSize.y * NowFunction.iSize_Select_Button;

	m_pScroll_DropBox->Activate_Scroll(fScroll_Size, m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Box_Area)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Box_Area)]->fSize);
	NowFunction.bDropBox_On = true;
	m_bIsCloseAction_DropBox = false;
}


void CUIPage_Option::Close_Dropbox_SelectBox(FUNCTION& NowFunction, _float fTimeDelta)
{
	m_pScroll_DropBox->DeActivate_Scroll();
	NowFunction.bDropBox_On = false;
}

void CUIPage_Option::Update_Dropbox_SelectBox(FUNCTION& NowFunction, _float fTimeDelta)
{
	_bool bIsClick = KEY_TAP(KEY::LBUTTON);

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Dropbox_Box_Header); i <= _int(PART_GROUP::OPTION_FUNC_Dropbox_Scroll_Bar); ++i)
		__super::UpdatePart_ByIndex(i, fTimeDelta);

	_bool bIsBoxClick = false;
	_int iMouseFocus = -1;
	_float fOffset = 0.f;
	SCROLL_AREA eArea = SCROLL_AREA::SCROLL_END;

	if (m_pScroll_DropBox->bIsActive)
	{
		eArea = SCROLL_AREA::SCROLL_OPTION_DROPBOX;

		if (m_pScroll_DropBox->bIsBarMoving)
		{
			if (KEY_HOLD(KEY::LBUTTON))
			{
				POINT			ptMouse{};
				GetCursorPos(&ptMouse);
				ScreenToClient(g_hWnd, &ptMouse);

				m_pScroll_DropBox->Bar_Moving(_float(ptMouse.y));
				bIsBoxClick = true;
			}
			m_pScroll_DropBox->End_Bar_Moving();
		}
		else if (bIsClick)
		{
			_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Scroll_Bar)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Scroll_Bar)]->fSize);

			if (vMouse.y != -1.f)
			{
				bIsBoxClick = true;
				m_pScroll_DropBox->Start_Bar_Moving(vMouse.y);
			}
		}

		m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Scroll_Bar)]->fRatio = m_pScroll_Option->fScroll_Ratio;
		fOffset = m_pScroll_Option->fData_Offset_Y;

		for (_int i = _int(PART_GROUP::OPTION_FUNC_Dropbox_Scroll_Line); i <= _int(PART_GROUP::OPTION_FUNC_Dropbox_Scroll_Bar); ++i)
		{
			__super::UpdatePart_ByIndex(i, fTimeDelta);
			Input_Render_Info(*m_vecPart[i], eArea);
		}
	}

	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fPosition -= _Vec2{ 0.f, fOffset };
	_Vec2 vStart = m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fPosition;

	for (_int i = 0; i < NowFunction.iSize_Select_Button; ++i)
	{
		if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fSize).x != -1.f)
		{
			iMouseFocus = i;

			if (bIsClick)
			{
				NowFunction.iSelected_Num = i;
				bIsBoxClick = true;
			}
		}

		m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fPosition.y += m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fSize.y;
	}

	if ((bIsClick) && (!bIsBoxClick))
	{
		Close_Dropbox_SelectBox(NowFunction, fTimeDelta);
		return;
	}

	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fPosition = vStart;

	for (_int i = 0; i < NowFunction.iSize_Select_Button; ++i)
	{
		__super::UpdatePart_ByIndex(_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area), fTimeDelta);
		__super::UpdatePart_ByIndex(_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Focus_Pos), fTimeDelta);
		__super::UpdatePart_ByIndex(_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Text), fTimeDelta);
		m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Text)]->strText = NowFunction.vecSelect_Name[i];

		if (i == iMouseFocus)
		{
			m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fAdjust_End = m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Focus_Pos)]->fPosition;
			m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fTextureColor = _Vec4{ 0.1f,0.1f,0.1f, 1.f };
			m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Text)]->Set_WhiteText();
		}

		if (i == NowFunction.iSelected_Num)
		{
			m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fTextureColor = _Vec4{ 0.4f,0.4f,0.4f, 1.f };
			m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Text)]->Set_RedText();
		}

		Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)], eArea);
		Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Text)], eArea);

		m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fPosition.y += m_vecPart[_int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Area)]->fSize.y;
	}

	*m_mapVariables_DropBox.find(NowFunction.iVariable_Key)->second = _float(NowFunction.iSelected_Num);
}

void CUIPage_Option::Update_Right_Side(_float fTimeDelta)
{
	m_vecPart[_int(PART_GROUP::OPTION_DESC_Text)]->strText = m_vecOption_TabInfo[m_iNow_Tab]->vecOption_Line[m_iNow_Line]->pFunction->strDescription;

	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_DESC_Head)], SCROLL_AREA::SCROLL_NONE);
	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_DESC_Deco)], SCROLL_AREA::SCROLL_NONE);
	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_DESC_Text)], SCROLL_AREA::SCROLL_NONE);
	
}

void CUIPage_Option::Update_Focus_Highlight(_float fTimeDelta)
{
	// Focus
	_Vec2 vDirec = m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fAdjust_End - m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fAdjust_Start;
	m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fPosition = m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fAdjust_Start;
	m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fPosition += vDirec * m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fRatio;
	m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fRatio += fTimeDelta * 10.f;
	m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fRatio = min(m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fRatio, 1.f);

	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Focus)], SCROLL_AREA::SCROLL_NONE);

	// Highlight
	if (m_vecPart[_int(PART_GROUP::OPTION_Highlight_Line)]->bRender)
		Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Focus)], SCROLL_AREA::SCROLL_NONE);
}

void CUIPage_Option::Change_Focus_Mark_Destination(_Vec2 vPos)
{
	if (m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fAdjust_End != vPos)
	{
		m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fAdjust_End = vPos;
		m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fAdjust_Start = m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fPosition;
		m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fRatio = 0.f;
	}
}

void CUIPage_Option::Change_Highlight_Pos(_Vec2 vPos)
{
	m_vecPart[_int(PART_GROUP::OPTION_Highlight_Line)]->bRender = true;
	m_vecPart[_int(PART_GROUP::OPTION_Highlight_Line)]->fPosition = vPos;
}

void CUIPage_Option::Update_Variable()
{
	m_pGameInstance->Set_SSAODesc(m_bGraphic[0]);
	m_pGameInstance->Set_HDRDesc(m_bGraphic[1]);
	m_pGameInstance->Set_BloomDesc(m_bGraphic[2]);
	m_pGameInstance->Set_DOFDesc(m_bGraphic[3]);

	if (m_pGameInstance->Get_IsOnPBR() != m_bGraphic[4])
		m_pGameInstance->Toggle_PBR();

	if (m_pGameInstance->Get_IsOnShadow() != m_bGraphic[5])
		m_pGameInstance->Toggle_Shadow();
}


CUIPage_Option* CUIPage_Option::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Option* pInstance = new CUIPage_Option(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Option"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Option::Clone(void* pArg)
{
	CUIPage_Option* pInstance = new CUIPage_Option(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Option"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Option::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();

	Safe_Delete(m_pScroll_Option);
	Safe_Delete(m_pScroll_DropBox);

	for (auto& iter : m_vecOption_TabInfo)
	{
		Safe_Delete(iter);
	}

	m_vecOption_TabInfo.clear();

	for (auto& iter : m_mapVariables_Bool)
	{
		if (iter.first > 100000)
			Safe_Delete(iter.second);
	}
	m_mapVariables_Bool.clear();

	for (auto& iter : m_mapVariables_Slide)
	{
		if (iter.first > 100000)
		Safe_Delete(iter.second);
	}
	m_mapVariables_Slide.clear();
	for (auto& iter : m_mapVariables_DropBox)
	{
		if (iter.first > 100000)
		Safe_Delete(iter.second);
	}
	m_mapVariables_DropBox.clear();
}
