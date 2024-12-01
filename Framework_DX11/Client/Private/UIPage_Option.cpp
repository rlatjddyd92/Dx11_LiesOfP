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
	Update_Scroll(fTimeDelta);
	Update_Line(fTimeDelta);
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

		pNewFunc->strVariable_Key = vecBuffer_Spec[i][4];
		m_mapVariables.insert({ pNewFunc->strVariable_Key , 0.f });

		if (pNewLine->eFunc == OPTION_FUNC::FUNC_BOOL)
		{
			pNewFunc->strLeft = vecBuffer_Spec[i][5];
			pNewFunc->strRight = vecBuffer_Spec[i][6];
		}
		else if (pNewLine->eFunc == OPTION_FUNC::FUNC_SLIDE)
		{
			pNewFunc->bIsShow_IntNum = stoi(vecBuffer_Spec[i][7]);
			pNewFunc->fMin = stof(vecBuffer_Spec[i][8]);
			pNewFunc->fMax = stof(vecBuffer_Spec[i][9]);
			pNewFunc->fInterval = stof(vecBuffer_Spec[i][10]);
			pNewFunc->fInterval_Ratio = (pNewFunc->fInterval / (pNewFunc->fMax - pNewFunc->fMin));
			pNewFunc->fInterval_X = (pNewFunc->fMax - pNewFunc->fMin) * pNewFunc->fInterval_Ratio;
			
		}
		else if (pNewLine->eFunc == OPTION_FUNC::FUNC_DROPBOX)
		{
			pNewFunc->iSize_Select_Button = stoi(vecBuffer_Spec[i][11]);
			for (_int j = 0; j <= pNewFunc->iSize_Select_Button; ++j)
				pNewFunc->vecSelect_Name.push_back(vecBuffer_Spec[i][12 + j]);
		}

		pNewLine->pFunction = pNewFunc;
		m_vecOption_TabInfo.back()->vecOption_Line.push_back(pNewLine);
	}

	

	
}

void CUIPage_Option::Update_Tab(_float fTimeDelta)
{
	m_vecPart[_int(PART_GROUP::OPTION_Highlight_Line)]->bRender = false;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fRatio = 0.f;
	__super::UpdatePart_ByIndex(_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button), fTimeDelta);

	// 배경 렌더 요청 

	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Back)]);
	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Back_Grid)]);
	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Back_Fx)]);

	// 탭 버튼 위치 잡기 -> 마우스 확인 -> 렌더 요청 
	_int iMoveTap = 0;
	_int iNowCheckTap = 0;
	_bool bMouseLButton = KEY_TAP(KEY::LBUTTON);

	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Tap_Q_Button)]);
	_Vec2 vPos = GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_Tap_Q_Button)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_Tap_Q_Button)]->fSize);

	_int iTab_Origin = m_iNow_Tab;
	m_bChange_Tab = false;

	
	_int iNowMouse = -1;

	// 마우스 
	for (auto& iter : m_vecOption_TabInfo)
	{
		vPos = GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fSize);

		if (vPos.x != -1.f)
		{
			iNowMouse = iNowCheckTap;
			if (bMouseLButton)
				m_iNow_Tab = iNowCheckTap;
		}

		m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fPosition.x += m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fSize.x;
		++iNowCheckTap;
	}

	m_vecPart[_int(PART_GROUP::OPTION_Tap_E_Button)]->fPosition.x = m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fPosition.x + m_vecPart[_int(PART_GROUP::OPTION_Tap_Mouse_Area)]->fAdjust.x;
	Input_Render_Info(*m_vecPart[_int(PART_GROUP::OPTION_Tap_E_Button)]);

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

void CUIPage_Option::Update_Scroll(_float fTimeDelta)
{
	/*OPTION_Main_Scroll_Area,
		OPTION_Main_Scroll_Bar_Line,
		OPTION_Main_Scroll_Bar,*/


}

void CUIPage_Option::Update_Line(_float fTimeDelta)
{
	_float fStartY = m_vecPart[_int(PART_GROUP::OPTION_Main_Scroll_Area)]->fPosition.y - m_vecPart[_int(PART_GROUP::OPTION_Main_Scroll_Area)]->fSize.y * 0.5f;
	_float fAdjustY = 0.f;
	
	_float fHeight_Title = m_vecPart[_int(PART_GROUP::OPTION_TITLE_Area)]->fSize.y;
	_float fHeight_Line = m_vecPart[_int(PART_GROUP::OPTION_LINE_Area)]->fSize.y;

	
	
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
			if (m_pScroll_Option->Check_Is_Render_Y(fStartY + fAdjustY, m_vecPart[_int(PART_GROUP::OPTION_LINE_Area)]->fSize.y))
			{
				m_vecPart[_int(PART_GROUP::OPTION_LINE_Area)]->fPosition.y = fStartY + fAdjustY;
				_Vec2 vPos = GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::OPTION_LINE_Area)]->fPosition, m_vecPart[_int(PART_GROUP::OPTION_LINE_Area)]->fSize);
				if (vPos.x != -1.f)
					bMouse = true;

				m_vecPart[_int(PART_GROUP::OPTION_LINE_Text)]->strText = iter->strName;

				for (_int i = _int(PART_GROUP::OPTION_LINE_Focus_Pos); i <= _int(PART_GROUP::OPTION_LINE_Underline); ++i)
				{
					UpdatePart_ByIndex(i, fTimeDelta);
					Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_OPTION);
				}

				m_vecPart[_int(PART_GROUP::OPTION_Focus)]->fAdjust_End = m_vecPart[_int(PART_GROUP::OPTION_LINE_Focus_Pos)]->fPosition;
			}
			
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
				if (KEY_TAP(KEY::LBUTTON))
				{
					if (m_mapVariables.find(iter->pFunction->strVariable_Key)->second == 0.f)
						m_mapVariables.find(iter->pFunction->strVariable_Key)->second = 1.f;
					else if (m_mapVariables.find(iter->pFunction->strVariable_Key)->second == 1.f)
						m_mapVariables.find(iter->pFunction->strVariable_Key)->second = 0.f;
				}
			}
			fAdjustY += fHeight_Line * 0.5f;
		}
	}

	if (m_bChange_Tab)
		m_pScroll_Option->Activate_Scroll(fAdjustY);
}

void CUIPage_Option::Update_BoolButton(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse)
{
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Left_Text)]->strText = NowFunction.strLeft;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Right_Text)]->strText = NowFunction.strRight;

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Bool_Frame); i <= _int(PART_GROUP::OPTION_FUNC_Bool_Right_UnderLine); ++i)
		__super::UpdatePart_ByIndex(i, fTimeDelta);

	_int iFocus = 0;

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

	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Left_Focus_Pos)]->bRender = (iFocus == -1) ? true : false;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Right_Focus_Pos)]->bRender = (iFocus == 1) ? true : false;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Left_Area)]->bRender = (NowFunction.bIsSelect_Left == true) ? true : false;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Left_UnderLine)]->bRender = (NowFunction.bIsSelect_Left == true) ? true : false;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Right_Area)]->bRender = (NowFunction.bIsSelect_Left == false) ? true : false;
	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Bool_Right_UnderLine)]->bRender = (NowFunction.bIsSelect_Left == false) ? true : false;

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Bool_Frame); i <= _int(PART_GROUP::OPTION_FUNC_Bool_Right_UnderLine); ++i)
		if (m_vecPart[i]->bRender)
			Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_OPTION);

	m_mapVariables.find(NowFunction.strVariable_Key)->second = _float(NowFunction.bIsSelect_Left);
}

void CUIPage_Option::Update_Slide(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse)
{
	_int iArrowMoving = 0;
	_bool bIsClick = KEY_TAP(KEY::LBUTTON);

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Slide_Area); i <= _int(PART_GROUP::OPTION_FUNC_Slide_Right_Arrow); ++i)
		__super::UpdatePart_ByIndex(i, fTimeDelta);

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

	Update_Slide_Button(NowFunction, fTimeDelta, bMouse);

	if (NowFunction.bIsShow_IntNum)
		m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Num)]->strText = to_wstring(_int(NowFunction.fNow));
	else 
		m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Num)]->strText = to_wstring(NowFunction.fNow);

	for (_int i = _int(PART_GROUP::OPTION_FUNC_Slide_Area); i <= _int(PART_GROUP::OPTION_FUNC_Slide_Right_Arrow); ++i)
		Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_OPTION);
	
	m_mapVariables.find(NowFunction.strVariable_Key)->second = _float(NowFunction.fNow);
}

void CUIPage_Option::Update_Slide_Button(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse)
{
	_float fOriginX = m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fPosition.x;

	m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fRatio = NowFunction.fNow / NowFunction.fMax;
	__super::UpdatePart_ByIndex(_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button), fTimeDelta);

	_float fStartX = m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fAdjust_Start.x;
	_float fEndX = m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fAdjust_End.x;

	if (NowFunction.bIsButton_Moving)
	{
		if (KEY_HOLD(KEY::LBUTTON))
		{
			

			POINT			ptMouse{};
			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);

			_float fNowX = _float(ptMouse.x);

			if (fNowX >= fOriginX + fEndX)
			{
				m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fRatio = 1.f;
				NowFunction.fNow = NowFunction.fMax;
			}
			else if (fNowX <= fOriginX + fStartX)
			{
				m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fRatio = 0.f;
				NowFunction.fNow = NowFunction.fMin;
			}
			else
			{
				fNowX -= fOriginX;

				_float fRatio = fNowX / fEndX;

				_int iNowIndex = (_int)(fRatio / NowFunction.fInterval_Ratio);
				fRatio = NowFunction.fInterval_Ratio * iNowIndex;
				m_vecPart[_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button)]->fRatio = fRatio;
				__super::UpdatePart_ByIndex(_int(PART_GROUP::OPTION_FUNC_Slide_Bar_Button), fTimeDelta);
				NowFunction.fNow = NowFunction.fInterval * iNowIndex;
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

void CUIPage_Option::Update_Dropbox(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse)
{

	/*for (_int i = _int(PART_GROUP::OPTION_FUNC_Dropbox_Area); i <= _int(PART_GROUP::OPTION_FUNC_Dropbox_Select_Text); ++i)
	{
		__super::UpdatePart_ByIndex(i, fTimeDelta);
		


		Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_OPTION);
	}

*/













	/*OPTION_FUNC_Dropbox_Area,
		OPTION_FUNC_Dropbox_Selected_Text,
		OPTION_FUNC_Dropbox_Arrow,
		*/
}

void CUIPage_Option::Open_Dropbox_SelectBox(_float fTimeDelta)
{
	/*OPTION_FUNC_Dropbox_Box_Header,
		OPTION_FUNC_Dropbox_Box_Area,
		OPTION_FUNC_Dropbox_Select_Area,
		OPTION_FUNC_Dropbox_Select_Focus_Pos,
		OPTION_FUNC_Dropbox_Select_Text,
		OPTION_FUNC_Dropbox_Scroll_Line,
		OPTION_FUNC_Dropbox_Scroll_Bar,*/

	

}


void CUIPage_Option::Close_Dropbox_SelectBox(_float fTimeDelta)
{
}

void CUIPage_Option::Activate_OptionScroll(_float fTimeDelta)
{
}

void CUIPage_Option::DeActivate_OptionScroll(_float fTimeDelta)
{
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

	m_mapVariables.clear();
}
