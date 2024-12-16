#include "stdafx.h"
#include "..\Public\UIPage_Telepot.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"
#include "Stargazer.h"
#include "Layer.h"

#include "State_Player_Teleport.h"

CUIPage_Telepot::CUIPage_Telepot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Telepot::CUIPage_Telepot(const CUIPage_Telepot& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Telepot::Initialize_Prototype()
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

	m_fTopPartMove = -1.f;

	return S_OK;
}

HRESULT CUIPage_Telepot::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Telepot::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Telepot::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_pScroll_Telepot == nullptr)
	{
		m_pScroll_Telepot = new SCROLL;
		m_pScroll_Telepot->Initialize_Scroll(m_vecPart[_int(PART_GROUP::TELEPOT_List_Area)], m_vecPart[_int(PART_GROUP::TELEPOT_Scroll_Bar)], SCROLL_AREA::SCROLL_TELEPOT, true, false);
		Page_Setting();
	}
	
}

void CUIPage_Telepot::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	Update_Telepot_Cell(fTimeDelta);
}

HRESULT CUIPage_Telepot::Render()
{
	return S_OK;
}

void CUIPage_Telepot::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Telepot::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Telepot::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	if (m_pScroll_Telepot->bIsActive_X)
		Action_Scroll(fTimeDelta);
	Action_Telepot(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Telepot::Ready_UIPart_Group_Control()
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

	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;

	m_bRender = false;

	return S_OK;
}

void CUIPage_Telepot::Set_Now_Interact_Stargezer(_int iNaviIndex)
{
	for (_int i = 0; i < m_vecTelepot_Dest.size(); ++i)
	{
		if (m_vecTelepot_Dest[i]->iDestination_Navi_Index == iNaviIndex)
			m_vecTelepot_Dest[i]->bIsNow = true;
		else 
			m_vecTelepot_Dest[i]->bIsNow = false;
	}
}

void CUIPage_Telepot::Page_Setting()
{
	_float fInterval = m_vecPart[_int(PART_GROUP::TELEPOT_List_Area)]->fSize.x * 0.5f;
	fInterval += m_vecPart[_int(PART_GROUP::TELEPOT_Dest_Frame)]->fAdjust.x;
	_float fDataSizeX = m_iDest_Num * (fInterval * 2.f);
	m_pScroll_Telepot->Activate_Scroll(0.f, fDataSizeX);

	_Vec2 vFirst = m_vecPart[_int(PART_GROUP::TELEPOT_Dest_Frame)]->fPosition;

	for (_int i = 0; i < m_iDest_Num; ++i)
	{
		DEST_INFO* pNewDest = new DEST_INFO;
		pNewDest->vUICell_Pos = vFirst;
		vFirst.x += fInterval * 2.f;
		m_vecTelepot_Dest.push_back(pNewDest);
	}

	m_vecTelepot_Dest[0]->iDestination_Navi_Index = 747; // 모나스트리 입구
	m_vecTelepot_Dest[1]->iDestination_Navi_Index = 280; // 락사시아 보스전 
	m_vecTelepot_Dest[2]->iDestination_Navi_Index = 1008; // 원형계단
	m_vecTelepot_Dest[3]->iDestination_Navi_Index = 227; // 마누스 보스전

	m_vecTelepot_Dest[0]->strDest_Name = TEXT("아르케 대수도원 입구"); 
	m_vecTelepot_Dest[1]->strDest_Name = TEXT("결전 : 락사시아");
	m_vecTelepot_Dest[2]->strDest_Name = TEXT("대수도원 원형계단");
	m_vecTelepot_Dest[3]->strDest_Name = TEXT("결전 : 마누스");
}

void CUIPage_Telepot::Action_Scroll(_float fTimeDelta)
{
	if (m_pScroll_Telepot->bIsBarMoving_X == false)
	{
		if (KEY_TAP(KEY::LBUTTON))
		{
			_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::TELEPOT_Scroll_Bar)]->fPosition, _Vec2{ m_vecPart[_int(PART_GROUP::TELEPOT_Scroll_Bar)]->fSize.y,m_vecPart[_int(PART_GROUP::TELEPOT_Scroll_Bar)]->fSize.x });
			if (vMouse.x != -1.f)
			{
				m_pScroll_Telepot->Start_Bar_Moving_X(vMouse.x);
			}
		}
	}
	else if (m_pScroll_Telepot->bIsBarMoving_X == true)
	{
		if (KEY_HOLD(KEY::LBUTTON))
		{
			POINT			ptMouse{};
			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);

			_float fNow_X = _float(ptMouse.x);

			m_pScroll_Telepot->Bar_Moving_X(fNow_X);
		}
		else
			m_pScroll_Telepot->End_Bar_Moving_X();
	}

	m_vecPart[_int(PART_GROUP::TELEPOT_Scroll_Bar)]->fRatio = m_pScroll_Telepot->fScroll_Ratio_X;
	m_vec_Group_Ctrl[_int(PART_GROUP::TELEPOT_Scroll_Bar)]->fRatio = m_pScroll_Telepot->fScroll_Ratio_X;
}

void CUIPage_Telepot::Action_Telepot(_float fTimeDelta)
{
	if (m_iNowActive == -1)
	{
		for (_int i = 0; i < m_iDest_Num; ++i)
		{
			_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(m_vecTelepot_Dest[i]->vUICell_Pos - _Vec2{ m_pScroll_Telepot->fData_Offset_X, 0.f }, m_vecPart[_int(PART_GROUP::TELEPOT_Dest_Frame)]->fSize);
			if (vMouse.x != -1.f)
			{
				m_iNowFocus = i;
				if (KEY_TAP(KEY::LBUTTON))
				{
					if (m_vecTelepot_Dest[i]->bIsNow == true)
					{
						m_iNowActive = 4;
						GET_GAMEINTERFACE->Show_Popup(TEXT("위치 이동 불가"), TEXT("선택한 위치는 현재 위치입니다."));
					}
					else if (m_vecTelepot_Dest[i]->bIsInactive == true)
					{
						m_iNowActive = 4;
						GET_GAMEINTERFACE->Show_Popup(TEXT("위치 이동 불가"), TEXT("해당 위치의 별바라기가 활성화되지 않았습니다."));
					}
					else
					{
						_wstring strDest = TEXT("이동 위치 -> ");
						strDest += m_vecTelepot_Dest[i]->strDest_Name;
						m_iNowActive = i;
						m_iIsStartTelepot = 0;
						GET_GAMEINTERFACE->Show_TrueFalsePopup(TEXT("다른 위치로 이동"), strDest, &m_iIsStartTelepot);
					}
				}
			}
		}
	}
	else if (m_iNowActive == 4)
	{
		if (GET_GAMEINTERFACE->IsPopupOn() == false)
		{
			m_iNowActive = -1;
			m_iIsStartTelepot = 0;
		}
	}
	else if (m_iNowActive != -1)
	{
		if (m_iIsStartTelepot != 0)
		{
			if (m_iIsStartTelepot == 1) // 텔레포트 진행 
			{
				// 텔레포트 기능 구현 
				CState_Player_Teleport::TELEPORT_DESC TeleportDesc{};
				TeleportDesc.iCellNum = m_vecTelepot_Dest[m_iNowActive]->iDestination_Navi_Index;
				GET_GAMEINTERFACE->Get_Player()->Change_State(CPlayer::TELEPORT, &TeleportDesc);	
				GET_GAMEINTERFACE->SwicthPage(UIPAGE::PAGE_TELEPOT, UIPAGE::PAGE_PLAY);
			}

			m_iNowActive = -1;
			m_iIsStartTelepot = 0;
		}
	}

	if (GET_GAMEINTERFACE->IsPopupOn() == false)
	{
		m_iNowActive = -1;
		m_iIsStartTelepot = 0;
	}
}

void CUIPage_Telepot::Update_Telepot_Cell(_float fTimeDelta)
{
	// Common
	for (_int i = _int(PART_GROUP::TELEPOT_Back); i <= _int(PART_GROUP::TELEPOT_ESC_Text); ++i)
	{
		Input_Render_Info(*m_vecPart[i]);
	}

	// Scroll
	if (m_pScroll_Telepot->bIsActive_X == true)
	{
		for (_int i = _int(PART_GROUP::TELEPOT_Scroll_Line); i <= _int(PART_GROUP::TELEPOT_Scroll_Bar); ++i)
		{
			UpdatePart_ByIndex(i, fTimeDelta);
			Input_Render_Info(*m_vecPart[i]);
		}
	}

	// Cell
	CLayer* pStargzzerLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Stargazer"));
	vector<class CGameObject*> Stargazers = pStargzzerLayer->Get_ObjectList();

	for (auto& iter : Stargazers)
	{
		_int iNowIndex = static_cast<CStargazer*>(iter)->Get_CellNum();

		for (auto& iterUI : m_vecTelepot_Dest)
			if (iterUI->iDestination_Navi_Index == iNowIndex)
				iterUI->bIsInactive = !static_cast<CStargazer*>(iter)->Get_IsOpend();
	}

	for (_int i = 0; i < m_iDest_Num; ++i)
	{
		m_vecPart[_int(PART_GROUP::TELEPOT_Dest_Frame)]->fPosition = m_vecTelepot_Dest[i]->vUICell_Pos - _Vec2{ m_pScroll_Telepot->fData_Offset_X, 0.f };
		Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_TELEPOT);

		_int iPicture = _int(PART_GROUP::TELEPOT_Dest_Picture_Start) + i;
		_int iNowIndex = m_vecTelepot_Dest[i]->iDestination_Navi_Index;
				
		for (_int j = _int(PART_GROUP::TELEPOT_Dest_Deco_LT); j <= _int(PART_GROUP::TELEPOT_Dest_X); ++j)
		{
			UpdatePart_ByIndex(j, fTimeDelta);

			if (j == _int(PART_GROUP::TELEPOT_Dest_Fx_Select))
			{
				if (m_vecTelepot_Dest[i]->bIsNow)
				{
					m_vecPart[j]->fTextureColor.x = 0.4f;
					m_vecPart[j]->fTextureColor.y = 0.6f;
					m_vecPart[j]->fTextureColor.z = 0.4f;
					Input_Render_Info(*m_vecPart[j], SCROLL_AREA::SCROLL_TELEPOT);
				}
				else if (m_vecTelepot_Dest[i]->bIsInactive)
				{
					m_vecPart[j]->fTextureColor.x = 0.6f;
					m_vecPart[j]->fTextureColor.y = 0.4f;
					m_vecPart[j]->fTextureColor.z = 0.4f;
					Input_Render_Info(*m_vecPart[j], SCROLL_AREA::SCROLL_TELEPOT);
				}
				else if (i == m_iNowFocus)
				{
					m_vecPart[j]->fTextureColor.x = 0.4f;
					m_vecPart[j]->fTextureColor.y = 0.4f;
					m_vecPart[j]->fTextureColor.z = 0.4f;
					Input_Render_Info(*m_vecPart[j], SCROLL_AREA::SCROLL_TELEPOT);
				}
			}
			else if ((j >= _int(PART_GROUP::TELEPOT_Dest_Picture_Start)) && (j <= _int(PART_GROUP::TELEPOT_Dest_Picture_Manus)))
			{
				if (j == iPicture)
					Input_Render_Info(*m_vecPart[j], SCROLL_AREA::SCROLL_TELEPOT);
			}
			else if (j == _int(PART_GROUP::TELEPOT_Dest_Text_Now))
			{
				if (m_vecTelepot_Dest[i]->bIsNow)
					Input_Render_Info(*m_vecPart[j], SCROLL_AREA::SCROLL_TELEPOT);
			}
			else if (j == _int(PART_GROUP::TELEPOT_Dest_Text_Inactive))
			{
				if ((m_vecTelepot_Dest[i]->bIsNow == false) && (m_vecTelepot_Dest[i]->bIsInactive))
					Input_Render_Info(*m_vecPart[j], SCROLL_AREA::SCROLL_TELEPOT);
			}
			else if (j == _int(PART_GROUP::TELEPOT_Dest_Text_Name))
			{
				m_vecPart[j]->strText = m_vecTelepot_Dest[i]->strDest_Name;
				Input_Render_Info(*m_vecPart[j], SCROLL_AREA::SCROLL_TELEPOT);
			}
			else if ((j == _int(PART_GROUP::TELEPOT_Dest_Black_Cover)) || (j == _int(PART_GROUP::TELEPOT_Dest_X)))
			{
				if ((m_vecTelepot_Dest[i]->bIsNow == false) && (m_vecTelepot_Dest[i]->bIsInactive))
					Input_Render_Info(*m_vecPart[j], SCROLL_AREA::SCROLL_TELEPOT);
			}
			else
				Input_Render_Info(*m_vecPart[j], SCROLL_AREA::SCROLL_TELEPOT);
		}
	}
}

CUIPage_Telepot* CUIPage_Telepot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Telepot* pInstance = new CUIPage_Telepot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Telepot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Telepot::Clone(void* pArg)
{
	CUIPage_Telepot* pInstance = new CUIPage_Telepot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Telepot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Telepot::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}
	m_vecPart.clear();

	for (auto& iter : m_vecTelepot_Dest)
	{
		Safe_Delete(iter);
	}
	m_vecTelepot_Dest.clear();

	Safe_Delete(m_pScroll_Telepot);
}
