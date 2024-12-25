#include "stdafx.h"
#include "..\Public\UIPage_Achievment.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

#include "Achievment_DataLine.h"
#include "Statistic_Data.h"

CUIPage_Achievment::CUIPage_Achievment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Achievment::CUIPage_Achievment(const CUIPage_Achievment& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Achievment::Initialize_Prototype()
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

HRESULT CUIPage_Achievment::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Achievment::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Achievment::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pScroll == nullptr)
	{
		SCROLL* pScroll = new SCROLL;

		_int iBar = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_GOAL_FRAME)]->PartIndexlist.back();

		pScroll->Initialize_Scroll(__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_GOAL_FRAME)), m_vecPart[iBar], SCROLL_AREA::SCROLL_ACHIEVMENT);
		m_pScroll = pScroll;

		_float fInterval = m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_GOAL_LINE)]->PartIndexlist.front() + 1]->fSize.y;
		
		fInterval *= (1.3f * m_vecAcievment.size());
		m_pScroll->Activate_Scroll(fInterval, 0.f);
	}
}

void CUIPage_Achievment::Late_Update(_float fTimeDelta)
{
	if ((m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)]) && (!m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)]))
	{
		m_fTopPartMove -= m_fTopPartMove_Excel * fTimeDelta;
		if (m_fTopPartMove < 0.f)
		{
			m_fTopPartMove = 0.f;
			m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)] = false;
			//m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;
			m_bRender = false;
		}
	}
	else if ((!m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)]) && (m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)]))
	{
		m_fTopPartMove += m_fTopPartMove_Excel * fTimeDelta;
		if (m_fTopPartMove > 1.f)
		{
			m_fTopPartMove = 1.f;
			m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)] = false;
			m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
		}
	}

	Check_Data(fTimeDelta);

	if (m_bRender)
	{
		Update_Static(fTimeDelta);
		Update_Statistic(fTimeDelta);
		Update_Achievment(fTimeDelta);
	}

	if (!m_Popup_Indexlist.empty())
		Update_Popup(fTimeDelta);
}

HRESULT CUIPage_Achievment::Render()
{
	return S_OK;
}

void CUIPage_Achievment::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Achievment::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Achievment::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);
	Action_Slide(fTimeDelta);
	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Achievment::Ready_UIPart_Group_Control()
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

	for (auto& iter : m_vecPart)
		iter->bRender = false;

	//m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;

	m_bRender = false;

	Make_Achievment_DataLine();
	Make_Statistic_DataLine();

	return S_OK;
}

void CUIPage_Achievment::Input_Achievment_Data(_int iIndex, _int Data)
{
	if ((iIndex < 0) || (iIndex > m_vecAcievment.size()))
		return;

	if (m_vecAcievment[iIndex]->Is_Complete() == true)
		return;
	
	m_vecAcievment[iIndex]->Add_Stat(Data);

	if (m_vecAcievment[iIndex]->Is_Complete() == true)
	{
		m_Popup_Indexlist.push_back(iIndex);
		m_Popup_Ratiolist.push_back(1.f);

		if (m_Popup_Indexlist.size() > m_iMax_Popup)
		{
			m_Popup_Indexlist.pop_front();
			m_Popup_Ratiolist.pop_front();
		}
	}
}

void CUIPage_Achievment::Make_Achievment_DataLine()
{
	vector<vector<_wstring>> vecBuffer;
	m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Achievment_Data.csv", &vecBuffer);

	for (_int i = 1; i < vecBuffer.size(); ++i)
	{
		CAchievment_DataLine* pNew = CAchievment_DataLine::Create(m_pDevice, m_pContext);
		pNew->Initialize_Data(m_vecPart);
		pNew->Input_Data(vecBuffer[i]);
		m_vecAcievment.push_back(pNew);
	}
}

void CUIPage_Achievment::Make_Statistic_DataLine()
{
	for (_int i = 0; i < 4; ++i)
	{
		CStatistic_Data* pNew = CStatistic_Data::Create(m_pDevice, m_pContext);
		pNew->Set_Statistic_Data(m_vecPart, i);
		m_vecStatistic.push_back(pNew);
	}
}

void CUIPage_Achievment::Action_Slide(_float fTimeDelta)
{
	if (m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)])
	{
		_int iBar = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_GOAL_FRAME)]->PartIndexlist.back();

		if (m_pScroll->bIsBarMoving_Y)
		{
			if (KEY_HOLD(KEY::LBUTTON))
			{
				POINT			ptMouse{};
				GetCursorPos(&ptMouse);
				ScreenToClient(g_hWnd, &ptMouse);
				m_pScroll->Bar_Moving_Y(_float(ptMouse.y));
			}
			else
				m_pScroll->End_Bar_Moving_Y();
		}
		else
		{
			_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(m_vecPart[iBar]->fPosition, m_vecPart[iBar]->fSize);

			if (vMouse.x != -1.f)
				if (KEY_TAP(KEY::LBUTTON))
				{
					m_pScroll->Start_Bar_Moving_Y(vMouse.y);
				}
		}

		m_vecPart[iBar]->fRatio = m_pScroll->fScroll_Ratio_Y;
	}
}

void CUIPage_Achievment::Check_Data(_float fTimeDelta)
{
	if (GET_GAMEINTERFACE->IsGamePause() == false)
		m_vecStatistic[0]->Input_Data_Playtime(fTimeDelta);
}

void CUIPage_Achievment::Update_Static(_float fTimeDelta)
{
	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_FRAME)]->PartIndexlist)
	{
		__super::UpdatePart_ByIndex(iter, fTimeDelta);
		Input_Render_Info(*m_vecPart[iter], SCROLL_AREA::SCROLL_NONE);
	}
	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TITLE)]->PartIndexlist)
	{
		__super::UpdatePart_ByIndex(iter, fTimeDelta);
		Input_Render_Info(*m_vecPart[iter], SCROLL_AREA::SCROLL_NONE);
	}
	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_STATISTIC_FRAME)]->PartIndexlist)
	{
		__super::UpdatePart_ByIndex(iter, fTimeDelta);
		Input_Render_Info(*m_vecPart[iter], SCROLL_AREA::SCROLL_NONE);
	}
	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_GOAL_FRAME)]->PartIndexlist)
	{
		__super::UpdatePart_ByIndex(iter, fTimeDelta);
		Input_Render_Info(*m_vecPart[iter], SCROLL_AREA::SCROLL_NONE);
	}
}

void CUIPage_Achievment::Update_Statistic(_float fTimeDelta)
{
	for (auto& iter : m_vecStatistic)
		iter->Update_Statistic_Line(m_fTopPartMove);
}

void CUIPage_Achievment::Update_Achievment(_float fTimeDelta)
{
	for (_int i = 0; i < m_vecAcievment.size(); ++i)
	{
		m_vecAcievment[i]->Update_Line(fTimeDelta, m_pScroll->fData_Offset_Y, m_fTopPartMove, m_bRender);
	}
}

void CUIPage_Achievment::Update_Popup(_float fTimeDelta)
{
	_float fAdjust = 0.f;
	_int iIndex = 0;

	for (auto& iter : m_Popup_Ratiolist)
	{
		fAdjust = _float(iIndex) / _float(m_iMax_Popup - 1);

		if (iter > fAdjust)
			iter -= fTimeDelta;

		if (iter < fAdjust)
			iter = fAdjust;

		++iIndex;
	}

	list<_float>::iterator iterRatio = m_Popup_Ratiolist.begin();
	list<_int>::iterator iterIndex = m_Popup_Indexlist.begin();

	while (!m_Popup_Indexlist.empty())
	{
		if ((m_vecAcievment[m_Popup_Indexlist.front()]->Is_Complete() == true) && (m_vecAcievment[m_Popup_Indexlist.front()]->Is_PopupOn() == false))
		{
			if (!m_Popup_Indexlist.empty())
				m_Popup_Indexlist.pop_front();
			if (!m_Popup_Ratiolist.empty())
				m_Popup_Ratiolist.pop_front();
		}
		else
			break;
	}

	iterRatio = m_Popup_Ratiolist.begin();
	iterIndex = m_Popup_Indexlist.begin();

	while (iterIndex != m_Popup_Indexlist.end())
	{
		m_vecAcievment[(*iterIndex)]->Update_PopUp(fTimeDelta, (*iterRatio));
		++iterRatio;
		++iterIndex;
	}
}

CUIPage_Achievment* CUIPage_Achievment::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Achievment* pInstance = new CUIPage_Achievment(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Achievment"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Achievment::Clone(void* pArg)
{
	CUIPage_Achievment* pInstance = new CUIPage_Achievment(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Achievment"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Achievment::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
	
	for (auto& iter : m_vecStatistic)
	{
		Safe_Release(iter);
	}

	m_vecStatistic.clear();

	for (auto& iter : m_vecAcievment)
	{
		Safe_Release(iter);
	}

	m_vecAcievment.clear();
	m_Popup_Indexlist.clear();
	m_Popup_Ratiolist.clear();

	Safe_Delete(m_pScroll);
}
