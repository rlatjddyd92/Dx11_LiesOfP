#include "stdafx.h"
#include "..\Public\UIPage_Chest.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Chest::CUIPage_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Chest::CUIPage_Chest(const CUIPage_Chest& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Chest::Initialize_Prototype()
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

	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = false;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;

	return S_OK;
}

HRESULT CUIPage_Chest::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Chest::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Chest::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if ((m_pScroll_InvenInfo == nullptr) && (m_pScroll_Chest == nullptr))
	{
		SCROLL* pScroll_InvenInfo = new SCROLL;
		pScroll_InvenInfo->Initialize_Scroll(__super::Get_Front_Part_In_Control(_int(PART_GROUP::CHEST_Inven_Data_Area)), __super::Get_Front_Part_In_Control(_int(PART_GROUP::CHEST_Inven_Bar)), SCROLL_AREA::SCROLL_CHEST_INVEN);
		m_pScroll_InvenInfo = pScroll_InvenInfo;

		SCROLL* pScroll_DestInfo = new SCROLL;
		pScroll_DestInfo->Initialize_Scroll(__super::Get_Front_Part_In_Control(_int(PART_GROUP::CHEST_Dest_Data_Area)), __super::Get_Front_Part_In_Control(_int(PART_GROUP::CHEST_Dest_Bar)), SCROLL_AREA::SCROLL_CHEST_DEST);
		m_pScroll_Chest = pScroll_DestInfo;
	}
}

void CUIPage_Chest::Late_Update(_float fTimeDelta)
{
	if ((m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)]) && (!m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)]))
	{
		m_fTopPartMove -= m_fTopPartMove_Excel * fTimeDelta;
		if (m_fTopPartMove < 0.f)
		{
			m_fTopPartMove = 0.f;
			m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)] = false;
			m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;
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
	
	Update_Top(fTimeDelta);
	Update_Scroll(fTimeDelta);
	Update_Cell(fTimeDelta);
	Update_Cell_Dest(fTimeDelta);
	Update_Focus(fTimeDelta);
	Update_BagInfo(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
}

HRESULT CUIPage_Chest::Render()
{
	return S_OK;
}

void CUIPage_Chest::OpenAction()
{
	__super::OpenAction();
	Setting_Cell_Inven();
	Setting_Cell_Dest();
	Reset_PopupInfo();
}

void CUIPage_Chest::CloseAction()
{
	__super::CloseAction();
	GET_GAMEINTERFACE->Set_Show_NewMark_Off();
	GET_GAMEINTERFACE->Off_Focus();
}

CHECK_MOUSE CUIPage_Chest::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	Action_Scroll(fTimeDelta);

	if (m_bIsItem_Popup)
		Action_Popup(fTimeDelta);
	else
		Action_Cell(fTimeDelta);
	

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Chest::Ready_UIPart_Group_Control()
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

	return S_OK;
}

void CUIPage_Chest::Action_Scroll(_float fTimeDelta)
{
	if (m_pScroll_InvenInfo->bIsActive_Y)
	{
		if (m_pScroll_InvenInfo->bIsBarMoving_Y)
		{
			if (KEY_HOLD(KEY::LBUTTON))
			{
				POINT			ptMouse{};
				GetCursorPos(&ptMouse);
				ScreenToClient(g_hWnd, &ptMouse);
				m_pScroll_InvenInfo->Bar_Moving_Y(_float(ptMouse.y));
			}
			else
				m_pScroll_InvenInfo->End_Bar_Moving_Y();
		}
		else
		{
			_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(__super::Get_Front_Part_In_Control(_int(PART_GROUP::CHEST_Inven_Bar))->fPosition, __super::Get_Front_Part_In_Control(_int(PART_GROUP::CHEST_Inven_Bar))->fSize);

			if (vMouse.x != -1.f)
				if (KEY_TAP(KEY::LBUTTON))
				{
					m_pScroll_InvenInfo->Start_Bar_Moving_Y(vMouse.y);
				}
		}

		__super::Get_Front_Part_In_Control(_int(PART_GROUP::CHEST_Inven_Bar))->fRatio = m_pScroll_InvenInfo->fScroll_Ratio_Y;
		__super::UpdatePart_ByIndex(__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::CHEST_Inven_Bar)), 1.f);
	}
	
	if (m_pScroll_Chest->bIsActive_Y)
	{
		if (m_pScroll_Chest->bIsBarMoving_Y)
		{
			if (KEY_HOLD(KEY::LBUTTON))
			{
				POINT			ptMouse{};
				GetCursorPos(&ptMouse);
				ScreenToClient(g_hWnd, &ptMouse);
				m_pScroll_Chest->Bar_Moving_Y(_float(ptMouse.y));
			}
			else
				m_pScroll_Chest->End_Bar_Moving_Y();
		}
		else
		{
			_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(__super::Get_Front_Part_In_Control(_int(PART_GROUP::CHEST_Dest_Bar))->fPosition, __super::Get_Front_Part_In_Control(_int(PART_GROUP::CHEST_Dest_Bar))->fSize);

			if (vMouse.x != -1.f)
				if (KEY_TAP(KEY::LBUTTON))
				{
					m_pScroll_Chest->Start_Bar_Moving_Y(vMouse.y);
				}
		}

		__super::Get_Front_Part_In_Control(_int(PART_GROUP::CHEST_Dest_Bar))->fRatio = m_pScroll_Chest->fScroll_Ratio_Y;
		__super::UpdatePart_ByIndex(__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::CHEST_Dest_Bar)), 1.f);
	}
}

void CUIPage_Chest::Action_Cell(_float fTimeDelta)
{
	_bool bIsClick = KEY_TAP(KEY::RBUTTON);

	for (_int i = 0; i < m_vecInven_RenderInfo.size(); ++i)
	{
		_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(m_vecInven_RenderInfo[i]->vPos - _Vec2{ 0.f, m_pScroll_InvenInfo->fData_Offset_Y }, m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Frame)]->fSize);

		if (vMouse.x != -1.f)
		{
			m_bIsInven = true;
			m_iNowFocus = i;

			if ((bIsClick) && (m_vecInven_RenderInfo[i]->pInven->eSlot == EQUIP_SLOT::EQUIP_END))
			{
				m_iNowCount = 0;
				GET_GAMEINTERFACE->Show_ItemPopup(TEXT("보관함에 넣기"), m_vecInven_RenderInfo[i]->pInven->strName, 1, &m_iNowCount, m_vecInven_RenderInfo[i]->pInven->iCount);
				m_bIsItem_Popup = true;
			}

			return;
		}
	}

	for (_int i = 0; i < m_vecDest_RenderInfo.size(); ++i)
	{
		_Vec2 vMouse = GET_GAMEINTERFACE->CheckMouse(m_vecDest_RenderInfo[i]->vPos - _Vec2{ 0.f, m_pScroll_Chest->fData_Offset_Y }, m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Frame)]->fSize);

		if (vMouse.x != -1.f)
		{
			m_bIsInven = false;
			m_iNowFocus = i;

			if (bIsClick)
			{
				m_iNowCount = 0;
				GET_GAMEINTERFACE->Show_ItemPopup(TEXT("보관함에서 꺼내기"), m_vecDest_RenderInfo[i]->pInven->strName, 1, &m_iNowCount, m_vecDest_RenderInfo[i]->pInven->iCount);
				m_bIsItem_Popup = true;
			}

			return;
		}
	}
}

void CUIPage_Chest::Action_Popup(_float fTimeDelta)
{
	if (m_bIsItem_Popup == true)
	{
		if (m_iNowCount > 0)
		{
			if (m_bIsInven == true)
			{
				GET_GAMEINTERFACE->InvenItem_To_Chest(m_vecInven_RenderInfo[m_iNowFocus]->eArray, m_vecInven_RenderInfo[m_iNowFocus]->iInven_Index, m_iNowCount);
			}
			else
			{
				GET_GAMEINTERFACE->ChestItem_To_Inven(m_vecDest_RenderInfo[m_iNowFocus]->iIndexChest, m_iNowCount);
			}
			Reset_PopupInfo();
			Setting_Cell_Inven();
			Setting_Cell_Dest();
		}
		else if (GET_GAMEINTERFACE->IsPopupOn() == false)
			Reset_PopupInfo();
	}
}

void CUIPage_Chest::Update_Top(_float fTimeDelta)
{
	for (_int i = _int(PART_GROUP::CHEST_Back); i <= _int(PART_GROUP::CHEST_Weight_Ratio); ++i)
		__super::Input_Render_Info(*m_vecPart[i]);

	__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Arrow)]);

	__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Inven_Data_Area)]);
	__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Dest_Data_Area)]);
}

void CUIPage_Chest::Update_Scroll(_float fTimeDelta)
{
	if (m_pScroll_InvenInfo->bIsActive_Y)
	{
		m_vecPart[_int(PART_GROUP::CHEST_Inven_Bar)]->fRatio = m_pScroll_InvenInfo->fScroll_Ratio_Y;
		__super::UpdatePart_ByIndex(_int(PART_GROUP::CHEST_Inven_Bar), 1.f);

		__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Inven_Scroll_Line)]);
		__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Inven_Bar)]);
	}
	
	if (m_pScroll_InvenInfo->bIsActive_Y)
	{
		m_vecPart[_int(PART_GROUP::CHEST_Dest_Bar)]->fRatio = m_pScroll_Chest->fScroll_Ratio_Y;
		__super::UpdatePart_ByIndex(_int(PART_GROUP::CHEST_Dest_Bar), 1.f);

		__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Dest_Scroll_Line)]);
		__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Dest_Bar)]);
	}
}

void CUIPage_Chest::Update_Cell(_float fTimeDelta)
{
	__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Inven_Text)]);
	__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Inven_Line)]);

	_int iNow = 0;

	for (auto& iter : m_vecInven_RenderInfo)
	{
		const CItem_Manager::ITEM* pNow = iter->pInven;

		if (pNow == nullptr)
			continue;
		
		m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Frame)]->fPosition = iter->vPos - _Vec2{ 0.f, m_pScroll_InvenInfo->fData_Offset_Y };
		__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Frame)], SCROLL_AREA::SCROLL_CHEST_INVEN);
		
		m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Fx)]->bRender = false;
		m_vecPart[_int(PART_GROUP::CHEST_Inven_X)]->bRender = false;
		m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Equip_Symbol)]->bRender = false;
		m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Count)]->bRender = false;

		if (pNow->eSlot != EQUIP_SLOT::EQUIP_END)
		{
			m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Fx)]->fTextureColor.y = 0.3f;
			m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Fx)]->fTextureColor.z = 0.3f;
			m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Fx)]->bRender = true;

			m_vecPart[_int(PART_GROUP::CHEST_Inven_X)]->bRender = true;
			m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Equip_Symbol)]->bRender = true;
		}
		else if ((m_bIsInven == true) && (m_iNowFocus == iNow))
		{
			m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Fx)]->fTextureColor.y = 0.8f;
			m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Fx)]->fTextureColor.z = 0.8f;
			m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Fx)]->bRender = true;
		}
		
		if (pNow->bStack == true)
		{
			m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Count)]->bRender = true;
			m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Count)]->strText = to_wstring(pNow->iCount);
		}
			
		m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Item)]->iTexture_Index = pNow->iTexture_Index;

		for (_int i = _int(PART_GROUP::CHEST_Inven_Array_Cell0_Fx); i <= _int(PART_GROUP::CHEST_Inven_Array_Cell0_Equip_Symbol); ++i)
		{
			__super::UpdatePart_ByIndex(i, 1.f);

			if (m_vecPart[i]->bRender == true)
				__super::Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_CHEST_INVEN);
		}

		++iNow;
	}
}

void CUIPage_Chest::Update_Cell_Dest(_float fTimeDelta)
{
	__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Dest_Text)]);
	__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Dest_Line)]);

	_int iNow = 0;

	for (auto& iter : m_vecDest_RenderInfo)
	{
		const CItem_Manager::ITEM* pNow = iter->pInven;

		if (pNow == nullptr)
			continue;

		m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Frame)]->fPosition = iter->vPos - _Vec2{ 0.f, m_pScroll_Chest->fData_Offset_Y };
		__super::Input_Render_Info(*m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Frame)], SCROLL_AREA::SCROLL_CHEST_DEST);

		m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Fx)]->bRender = false;
		m_vecPart[_int(PART_GROUP::CHEST_Dest_X)]->bRender = false;
		m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Equip_Symbol)]->bRender = false;
		m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Count)]->bRender = false;

		if ((m_bIsInven == false) && (m_iNowFocus == iNow))
		{
			m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Fx)]->fTextureColor.y = 0.8f;
			m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Fx)]->fTextureColor.z = 0.8f;
			m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Fx)]->bRender = true;
		}

		if (pNow->bStack == true)
		{
			m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Count)]->bRender = true;
			m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Count)]->strText = to_wstring(pNow->iCount);
		}

		m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Item)]->iTexture_Index = pNow->iTexture_Index;

		for (_int i = _int(PART_GROUP::CHEST_Dest_Cell0_Fx); i <= _int(PART_GROUP::CHEST_Dest_Cell0_Equip_Symbol); ++i)
		{
			__super::UpdatePart_ByIndex(i, 1.f);

			if (m_vecPart[i]->bRender == true)
				__super::Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_CHEST_DEST);
		}

		++iNow;
	}
}

void CUIPage_Chest::Update_Focus(_float fTimeDelta)
{
	_Vec2 vPos = {};

	if ((m_bIsInven == true) && (!m_vecInven_RenderInfo.empty()))
		vPos = m_vecInven_RenderInfo[m_iNowFocus]->vPos;
	else if (!m_vecDest_RenderInfo.empty())
		vPos = m_vecDest_RenderInfo[m_iNowFocus]->vPos;
	else
	{
		GET_GAMEINTERFACE->Off_Focus();
		return;
	}
		
	GET_GAMEINTERFACE->Show_Focus(vPos, m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Frame)]->fSize);
}

void CUIPage_Chest::Update_BagInfo(_float fTimeDelta)
{
	for (_int i = _int(PART_GROUP::CHEST_BoxInfo_Frame); i <= _int(PART_GROUP::CHEST_BoxInfo_Bag_Count1); ++i)
		__super::Input_Render_Info(*m_vecPart[i]);
}

void CUIPage_Chest::Setting_Cell_Inven()
{
	for (auto& iter : m_vecInven_RenderInfo)
	{
		Safe_Delete(iter);
	}

	m_iNowFocus = 0;
	m_vecInven_RenderInfo.clear();

	_int iArray = 0;

	UpdatePart_ByIndex(_int(PART_GROUP::CHEST_Inven_Array), 1.f);

	for (_int i = _int(INVEN_ARRAY_TYPE::TYPE_USING_BASIC); i < _int(INVEN_ARRAY_TYPE::TYPE_END); ++i)
	{
		_wstring strName = {};
		_int iIndex = 0;
		GET_GAMEINTERFACE->Get_Array_Info(INVEN_ARRAY_TYPE(i), &strName, &iIndex);

		for (_int j = 0; j < iIndex; ++j)
		{
			const CItem_Manager::ITEM* pItem = GET_GAMEINTERFACE->Get_Array_Item_Info(INVEN_ARRAY_TYPE(i), j);

			if (pItem == nullptr)
				continue;

			if (pItem->bIsAvailable_Chest == false)
				continue;

			CELL_RENDER* pNew = new CELL_RENDER;

			pNew->bIsInven = true;
			pNew->eArray = INVEN_ARRAY_TYPE(i);
			pNew->iInven_Index = j;
			pNew->pInven = pItem;

			if (m_vecInven_RenderInfo.size() / 7 > iArray)
			{
				m_vecPart[_int(PART_GROUP::CHEST_Inven_Array)]->fPosition.y += m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Frame)]->fSize.y * 1.1f;
				++iArray;
			}

			m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Frame)]->fRatio = _float(m_vecInven_RenderInfo.size() % 7) / 6.f;

			UpdatePart_ByIndex(_int(PART_GROUP::CHEST_Inven_Array_Cell0_Frame), 1.f);

			pNew->vPos = m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Frame)]->fPosition;
				
			m_vecInven_RenderInfo.push_back(pNew);
		}
	}
	m_pScroll_InvenInfo->Activate_Scroll(_float(iArray + 1.f) * m_vecPart[_int(PART_GROUP::CHEST_Inven_Array_Cell0_Frame)]->fSize.y * 1.1f, 0.f);
}

void CUIPage_Chest::Setting_Cell_Dest()
{
	for (auto& iter : m_vecDest_RenderInfo)
	{
		Safe_Delete(iter);
	}

	m_iNowFocus = 0;
	m_vecDest_RenderInfo.clear();

	_int iArray = 0;

	UpdatePart_ByIndex(_int(PART_GROUP::CHEST_Dest_Array), 1.f);

	map<_int, CItem_Manager::ITEM*> ChestData = GET_GAMEINTERFACE->Get_ChestData();

	for (auto& iter : ChestData)
	{
		_wstring strName = {};
		_int iIndex = 0;

		if (iter.second == nullptr)
			continue;

		CELL_RENDER* pNew = new CELL_RENDER;

		pNew->bIsInven = false;
		pNew->iIndexChest = (iter).first;
		pNew->pInven = (iter).second;

		if (m_vecDest_RenderInfo.size() / 7 > iArray)
		{
			m_vecPart[_int(PART_GROUP::CHEST_Dest_Array)]->fPosition.y += m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Frame)]->fSize.y * 1.1f;
			++iArray;
		}

		m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Frame)]->fRatio = _float(m_vecDest_RenderInfo.size() % 7) / 6.f;

		UpdatePart_ByIndex(_int(PART_GROUP::CHEST_Dest_Cell0_Frame), 1.f);

		pNew->vPos = m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Frame)]->fPosition;

		m_vecDest_RenderInfo.push_back(pNew);
	}
	m_pScroll_Chest->Activate_Scroll(_float(iArray + 1.f) * m_vecPart[_int(PART_GROUP::CHEST_Dest_Cell0_Frame)]->fSize.y * 1.1f, 0.f);
}

void CUIPage_Chest::Reset_PopupInfo()
{
	m_iNowCount = 0;
	m_bIsItem_Popup = false;
}

CUIPage_Chest* CUIPage_Chest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Chest* pInstance = new CUIPage_Chest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Chest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Chest::Clone(void* pArg)
{
	CUIPage_Chest* pInstance = new CUIPage_Chest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Chest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Chest::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();

	for (auto& iter : m_vecInven_RenderInfo)
	{
		Safe_Delete(iter);
	}

	m_vecInven_RenderInfo.clear();

	for (auto& iter : m_vecDest_RenderInfo)
	{
		Safe_Delete(iter);
	}

	m_vecDest_RenderInfo.clear();

	Safe_Delete(m_pScroll_InvenInfo);
	Safe_Delete(m_pScroll_Chest);
}
