#include "stdafx.h"
#include "..\Public\UIPage_Popup.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Popup::CUIPage_Popup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Popup::CUIPage_Popup(const CUIPage_Popup& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Popup::Initialize_Prototype()
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

HRESULT CUIPage_Popup::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Popup::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Popup::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Popup::Late_Update(_float fTimeDelta)
{
	if (m_pItemPopup_Info->bIsActive == true)
	{
		if (KEY_TAP(KEY::LBUTTON))
		{
			_int iAdd = 0;

			if (m_pItemPopup_Info->pNow_Input != nullptr)
			{
				if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[int(PART_GROUP::POPUP_Input_Count_Decrease)]->fPosition, m_vecPart[int(PART_GROUP::POPUP_Input_Count_Decrease)]->fSize).x != -1.f)
				{
					if (m_pItemPopup_Info->iNow > m_pItemPopup_Info->iMin)
					{
						--m_pItemPopup_Info->iNow;
						iAdd = -1;
					}
				}
				else if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[int(PART_GROUP::POPUP_Input_Count_Increase)]->fPosition, m_vecPart[int(PART_GROUP::POPUP_Input_Count_Increase)]->fSize).x != -1.f)
				{
					if (m_pItemPopup_Info->iNow < m_pItemPopup_Info->iMax)
					{
						++m_pItemPopup_Info->iNow;
						iAdd = 1;
					}
				}

				m_vecPart[int(PART_GROUP::POPUP_Input_Count_Num)]->strText = to_wstring(m_pItemPopup_Info->iNow);
			}

			if (m_pItemPopup_Info->pNow_Count != nullptr)
			{
				m_pItemPopup_Info->iNow_Count += m_pItemPopup_Info->iInterval * iAdd;
				m_vecPart[int(PART_GROUP::POPUP_Count_Num)]->strText = to_wstring(m_pItemPopup_Info->iNow_Count);
			}
			
			if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[int(PART_GROUP::POPUP_Mouse_0)]->fPosition, m_vecPart[int(PART_GROUP::POPUP_Mouse_0)]->fSize).x != -1.f)
			{
				if (m_pItemPopup_Info->pNow_Input != nullptr)
					*m_pItemPopup_Info->pNow_Input = m_pItemPopup_Info->iNow;

				if (m_pItemPopup_Info->pNow_Count != nullptr)
					*m_pItemPopup_Info->pNow_Count = m_pItemPopup_Info->iNow_Count;

				Off_Popup();
			}
			else if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[int(PART_GROUP::POPUP_Mouse_1)]->fPosition, m_vecPart[int(PART_GROUP::POPUP_Mouse_1)]->fSize).x != -1.f)
			{
				if (m_pItemPopup_Info->pNow_Input != nullptr)
					*m_pItemPopup_Info->pNow_Input = -1;

				if (m_pItemPopup_Info->pNow_Count != nullptr)
					*m_pItemPopup_Info->pNow_Count = -1;

				Off_Popup();
			}
		}
	}
	else if (m_bItemUsePopUp == false)
	{
		if (m_vecPart[int(PART_GROUP::POPUP_Mouse_0)]->bRender)
			if (KEY_TAP(KEY::LBUTTON))
			{
				if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[int(PART_GROUP::POPUP_Mouse_0)]->fPosition, m_vecPart[int(PART_GROUP::POPUP_Mouse_0)]->fSize).x != -1.f)
					Off_Popup();
			}
	}
	else if (m_bItemUsePopUp == true)
	{
			if (KEY_TAP(KEY::LBUTTON))
			{
				if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[int(PART_GROUP::POPUP_Mouse_1)]->fPosition, m_vecPart[int(PART_GROUP::POPUP_Mouse_1)]->fSize).x != -1.f)
					Off_Popup();
				else if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[int(PART_GROUP::POPUP_Mouse_0)]->fPosition, m_vecPart[int(PART_GROUP::POPUP_Mouse_0)]->fSize).x != -1.f)
				{
					if (m_bTopBelt)
						GET_GAMEINTERFACE->Use_Potion_Slot();
					else 
						GET_GAMEINTERFACE->Use_Tool_Slot();
					Off_Popup();
				}
			}
	}
	m_fTopPartMove = -1;
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Popup::Render()
{
	return S_OK;
}

void CUIPage_Popup::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Popup::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Popup::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

void CUIPage_Popup::Show_Popup(_wstring strTitle, _wstring strDescA, _wstring strDescB)
{
	m_pSoundCom->Play2D(TEXT("SE_UI_OpenWindow_01.wav"), &g_fUIVolume);
	Off_Popup();
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;

	for (_int i = _int(PART_GROUP::POPUP_Top); i <= _int(PART_GROUP::POPUP_Text_0); ++i)
	{
		m_vecPart[i]->bRender = true;
		m_vecPart[i]->fTextColor = { 1.f,1.f,1.f,1.f };
	}

	m_vecPart[int(PART_GROUP::POPUP_Title)]->strText = strTitle;
	m_vecPart[int(PART_GROUP::POPUP_Desc_0)]->strText = strDescA;
	m_vecPart[int(PART_GROUP::POPUP_Mouse_0)]->fRatio = 0.5f;

	if (strDescB == TEXT("none"))
		m_vecPart[int(PART_GROUP::POPUP_Desc_1)]->bRender = false;
	else
		m_vecPart[int(PART_GROUP::POPUP_Desc_1)]->strText = strDescB;

	for (_int i = _int(PART_GROUP::POPUP_Top); i <= _int(PART_GROUP::POPUP_Text_0); ++i)
		__super::UpdatePart_ByIndex(i, 1.f);
}

void CUIPage_Popup::Off_Popup()
{
	for (_int i = _int(PART_GROUP::POPUP_Top); i <= _int(PART_GROUP::POPUP_Count_Num); ++i)
		m_vecPart[i]->bRender = false;

	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = false;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;

	m_pItemPopup_Info->bIsActive = false;
	m_pItemPopup_Info->iMax = 0;
	m_pItemPopup_Info->iMin = 0;
	m_pItemPopup_Info->iNow = 0;
	m_pItemPopup_Info->iNow_Count = 0;
	m_pItemPopup_Info->iInterval = 0;
	m_pItemPopup_Info->pNow_Count = nullptr;
	m_pItemPopup_Info->pNow_Input = nullptr;
	m_bItemUsePopUp = false;
	m_bTopBelt = false;
}

void CUIPage_Popup::Show_ItemPopup(_wstring strTitle, _wstring strInputTitle, _int iMin, _int* pNow_Input, _int iMax, _wstring strCountTitle, _int iInterval, _int* pNow_Count)
{

	if ((pNow_Input == nullptr) && (pNow_Count == nullptr))
		return;
	m_pSoundCom->Play2D(TEXT("SE_UI_OpenWindow_01.wav"), &g_fUIVolume);
	Off_Popup();

	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;

	m_pItemPopup_Info->bIsActive = true;

	for (_int i = _int(PART_GROUP::POPUP_Top); i <= _int(PART_GROUP::POPUP_Count_Num); ++i)
	{
		m_vecPart[i]->bRender = true;
		m_vecPart[i]->fTextColor = { 1.f,1.f,1.f,1.f };
	}

	m_vecPart[int(PART_GROUP::POPUP_Desc_0)]->bRender = false;
	m_vecPart[int(PART_GROUP::POPUP_Desc_1)]->bRender = false;
	
	m_vecPart[int(PART_GROUP::POPUP_Title)]->strText = strTitle;
	m_vecPart[int(PART_GROUP::POPUP_Text_0)]->strText = TEXT("확인");
	m_vecPart[int(PART_GROUP::POPUP_Text_1)]->strText = TEXT("취소");
	m_vecPart[int(PART_GROUP::POPUP_Mouse_0)]->fRatio = 0.f;
	m_vecPart[int(PART_GROUP::POPUP_Mouse_1)]->fRatio = 1.f;
	

	
	if (pNow_Input != nullptr)
	{
		m_pItemPopup_Info->iMin = iMin;
		m_pItemPopup_Info->iNow = iMin;
		m_pItemPopup_Info->pNow_Input = pNow_Input;
		m_pItemPopup_Info->iInterval = iInterval;
		m_pItemPopup_Info->iMax = iMax;

		for (_int i = _int(PART_GROUP::POPUP_Input_Count_Back); i <= _int(PART_GROUP::POPUP_Input_Count_Increase); ++i)
			m_vecPart[i]->bRender = true;

		m_vecPart[int(PART_GROUP::POPUP_Input_Count_Text)]->strText = strInputTitle;
		m_vecPart[int(PART_GROUP::POPUP_Input_Count_Num)]->strText = to_wstring(m_pItemPopup_Info->iNow);
	}
		
	if (pNow_Count != nullptr)
	{
		m_pItemPopup_Info->iNow_Count = iInterval * iMin;
		m_pItemPopup_Info->pNow_Count = pNow_Count;
		m_pItemPopup_Info->iInterval = iInterval;

		for (_int i = _int(PART_GROUP::POPUP_Count_Back); i <= _int(PART_GROUP::POPUP_Count_Num); ++i)
			m_vecPart[i]->bRender = true;

		m_vecPart[int(PART_GROUP::POPUP_Count_Text)]->strText = strCountTitle;
		m_vecPart[int(PART_GROUP::POPUP_Count_Num)]->strText = to_wstring(m_pItemPopup_Info->iNow_Count);
	}
	
	if ((pNow_Input != nullptr) && (pNow_Count != nullptr))
	{
		m_vecPart[int(PART_GROUP::POPUP_Input_Count_Back)]->fRatio = 0.f;
		m_vecPart[int(PART_GROUP::POPUP_Count_Back)]->fRatio = 1.f;
	}
	else if (pNow_Input != nullptr)
	{
		m_vecPart[int(PART_GROUP::POPUP_Input_Count_Back)]->fRatio = 0.5f;
	}
	else if (pNow_Count != nullptr)
	{
		m_vecPart[int(PART_GROUP::POPUP_Count_Back)]->fRatio = 0.5f;
	}

	for (_int i = _int(PART_GROUP::POPUP_Top); i <= _int(PART_GROUP::POPUP_Text_0); ++i)
		__super::UpdatePart_ByIndex(i, 1.f);
}

void CUIPage_Popup::Show_ItemUsePopup(_wstring strTitle, _wstring strDescA, _bool bIsTop)
{
	Off_Popup();
	m_pSoundCom->Play2D(TEXT("SE_UI_OpenWindow_01.wav"), &g_fUIVolume);
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;
	for (_int i = _int(PART_GROUP::POPUP_Top); i <= _int(PART_GROUP::POPUP_Text_1); ++i)
	{
		m_vecPart[i]->bRender = true;
		m_vecPart[i]->fTextColor = { 1.f,1.f,1.f,1.f };
	}

	m_vecPart[int(PART_GROUP::POPUP_Title)]->strText = strTitle;
	m_vecPart[int(PART_GROUP::POPUP_Desc_0)]->strText = strDescA;
	m_vecPart[int(PART_GROUP::POPUP_Desc_1)]->strText = {};

	m_vecPart[int(PART_GROUP::POPUP_Mouse_0)]->fRatio = 0.f;
	m_vecPart[int(PART_GROUP::POPUP_Mouse_1)]->fRatio = 1.f;

	m_vecPart[int(PART_GROUP::POPUP_Text_0)]->strText = TEXT("사용");
	m_vecPart[int(PART_GROUP::POPUP_Text_1)]->strText = TEXT("취소");

	m_bItemUsePopUp = true;
	m_bTopBelt = bIsTop;

	

	for (_int i = _int(PART_GROUP::POPUP_Top); i <= _int(PART_GROUP::POPUP_Text_1); ++i)
		__super::UpdatePart_ByIndex(i, 1.f);
}

HRESULT CUIPage_Popup::Ready_UIPart_Group_Control()
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

	for (_int i = _int(PART_GROUP::POPUP_Top); i <= _int(PART_GROUP::POPUP_Count_Num); ++i)
		m_vecPart[i]->bRender = false;

	__super::Array_Control(_int(PART_GROUP::POPUP_Top), _int(PART_GROUP::POPUP_Text_0), CTRL_COMMAND::COM_RENDER, true);

	m_bRender = true;

	m_pItemPopup_Info = new ITEMPOPUP_INFO;

	return S_OK;
}

CUIPage_Popup* CUIPage_Popup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Popup* pInstance = new CUIPage_Popup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Popup"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Popup::Clone(void* pArg)
{
	CUIPage_Popup* pInstance = new CUIPage_Popup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Popup"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Popup::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();

	Safe_Delete(m_pItemPopup_Info);
}
