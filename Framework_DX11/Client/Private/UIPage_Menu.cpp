#include "stdafx.h"
#include "..\Public\UIPage_Menu.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Menu::CUIPage_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Menu::CUIPage_Menu(const CUIPage_Menu& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Menu::Initialize_Prototype()
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

HRESULT CUIPage_Menu::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Menu::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Menu::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Menu::Late_Update(_float fTimeDelta)
{
	
	Desc_Update(fTimeDelta);
	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
	Item_Icon_Update(fTimeDelta);
	

	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Menu::Render()
{
	return S_OK;
}

void CUIPage_Menu::OpenAction()
{
	__super::OpenAction();

	m_pSoundCom->Play2D(TEXT("SE_UI_OpenMenu_04.wav"), &g_fUIVolume);

	if ((m_eFocus_Group < PART_GROUP::GROUP_MENU_EQUIP) || (m_eFocus_Group > PART_GROUP::GROUP_MENU_OPTION))
		m_eFocus_Group = PART_GROUP::GROUP_MENU_EQUIP;

	__super::Array_Control(_int(PART_GROUP::GROUP_DESC_BACK), _int(PART_GROUP::GROUP_DESC_MOUSE), CTRL_COMMAND::COM_RENDER, false);
	__super::Array_Control(_int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0), _int(PART_GROUP::GROUP_ITEM_DESC_FUNC_1), CTRL_COMMAND::COM_RENDER, false);
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_MARK)]->bRender = false;


}

void CUIPage_Menu::CloseAction()
{
	__super::CloseAction();

	m_pSoundCom->Play2D(TEXT("SE_UI_OpenMenu_04.wav"), &g_fUIVolume);

	__super::Array_Control(_int(PART_GROUP::GROUP_DESC_BACK), _int(PART_GROUP::GROUP_DESC_MOUSE), CTRL_COMMAND::COM_RENDER, false);
	__super::Array_Control(_int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0), _int(PART_GROUP::GROUP_ITEM_DESC_FUNC_1), CTRL_COMMAND::COM_RENDER, false);
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_MARK)]->bRender = false;
}

CHECK_MOUSE CUIPage_Menu::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	Focus_Update(fTimeDelta);
	Select_Update(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Menu::Ready_UIPart_Group_Control()
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

void CUIPage_Menu::Focus_Update(_float fTimeDelta)
{
	for (_int i = _int(PART_GROUP::GROUP_MENU_EQUIP); i <= _int(PART_GROUP::GROUP_BAG_CELL_3); ++i)
	{
		_Vec2 fMouse = __super::Check_Mouse_By_Part(*__super::Get_Front_Part_In_Control(i));
		if (fMouse.x != -1.f)
		{
			m_eFocus_Group = PART_GROUP(i);
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_MARK)]->bRender = true;

			// 여기에 마우스 액션 넣기

			if (KEY_TAP(KEY::LBUTTON))
			{
				switch (i)
				{
				case _int(PART_GROUP::GROUP_MENU_EQUIP):
					GET_GAMEINTERFACE->SwicthPage(UIPAGE::PAGE_MENU, UIPAGE::PAGE_EQUIP);
					break;
				case _int(PART_GROUP::GROUP_MENU_INVEN):
					GET_GAMEINTERFACE->SwicthPage(UIPAGE::PAGE_MENU, UIPAGE::PAGE_INVEN);
					break;
				case _int(PART_GROUP::GROUP_MENU_CHARACTOR):
					GET_GAMEINTERFACE->SwicthPage(UIPAGE::PAGE_MENU, UIPAGE::PAGE_STAT);
					break;
				/*case _int(PART_GROUP::GROUP_MENU_HEART):
					GET_GAMEINTERFACE->SwicthPage(UIPAGE::PAGE_MENU, UIPAGE::PAGE_COMMON);
					break;*/
				case _int(PART_GROUP::GROUP_MENU_OPTION):
					GET_GAMEINTERFACE->SwicthPage(UIPAGE::PAGE_MENU, UIPAGE::PAGE_OPTION);
					break;
				default:
					break;
				}



				
			}



			break;
		}
	}
}

void CUIPage_Menu::Select_Update(_float fTimeDelta)
{
	if (m_eFocus_Group != PART_GROUP::GROUP_END)
	{
		UPART* pSelect = __super::Get_Front_Part_In_Control((_int)PART_GROUP::GROUP_SELECT_MARK);
		UPART* pGroup = __super::Get_Front_Part_In_Control((_int)m_eFocus_Group);
		_int iParent = __super::Get_Front_PartIndex_In_Control((_int)m_eFocus_Group);
		
		pSelect->iParentPart_Index = iParent;
		pSelect->fSize = pGroup->fSize;
	}


}

void CUIPage_Menu::Item_Icon_Update(_float fTimeDelta)
{
	for (_int i = _int(PART_GROUP::GROUP_TOP_CELL_0); i <= _int(PART_GROUP::GROUP_BAG_CELL_3); ++i)
	{
		list<_int>::iterator iter = m_vec_Group_Ctrl[i]->PartIndexlist.begin();
		++iter;
		if (i == (_int)m_eFocus_Group)
			m_vecPart[*iter]->bRender = true;
		else
			m_vecPart[*iter]->bRender = false;

		const CItem_Manager::ITEM* pItem = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(i - _int(PART_GROUP::GROUP_TOP_CELL_0) + _int(EQUIP_SLOT::EQUIP_USING_TOP_0)));

		if ((pItem == nullptr)||(pItem->iTexture_Index == -1))
		{
			++iter;
			m_vecPart[*iter]->bRender = true;
			++iter;
			m_vecPart[*iter]->bRender = false;
			m_vecPart[*iter]->iTexture_Index = -1;
			++iter;
			m_vecPart[*iter]->bRender = false;
		}
		else
		{
			++iter;
			m_vecPart[*iter]->bRender = false;
			++iter;
			m_vecPart[*iter]->bRender = true;
			m_vecPart[*iter]->iTexture_Index = pItem->iTexture_Index;
			if (pItem->bStack)
			{
				++iter;
				m_vecPart[*iter]->bRender = true;
				m_vecPart[*iter]->strText = to_wstring(pItem->iCount);
			}
				
		}
	}
}

void CUIPage_Menu::Desc_Update(_float fTimeDelta)
{
	__super::Array_Control(_int(PART_GROUP::GROUP_DESC_BACK), _int(PART_GROUP::GROUP_DESC_MOUSE), CTRL_COMMAND::COM_RENDER, false);
	__super::Array_Control(_int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0), _int(PART_GROUP::GROUP_ITEM_DESC_FUNC_1), CTRL_COMMAND::COM_RENDER, false);

	if (m_eFocus_Group == PART_GROUP::GROUP_END)
		return;



	if ((m_eFocus_Group >= PART_GROUP::GROUP_MENU_EQUIP) && (m_eFocus_Group <= PART_GROUP::GROUP_MENU_OPTION))
	{
		__super::Array_Control(_int(PART_GROUP::GROUP_DESC_BACK), _int(PART_GROUP::GROUP_DESC_MOUSE), CTRL_COMMAND::COM_RENDER, true);
		_Vec2 fMouse = __super::Check_Mouse_By_Part(*__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DESC_MOUSE)));
		if (fMouse.x != -1.f)
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_DESC_FX)]->bRender = true;
		else
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_DESC_FX)]->bRender = false;

		_wstring strTitle = {};
		_wstring strDesc = {};

		switch (m_eFocus_Group)
		{
		case Client::CUIPage_Menu::PART_GROUP::GROUP_MENU_EQUIP:
			strTitle = TEXT("장비");
			strDesc = TEXT("장착하는 장비를 변경할 수 있습니다.");
			break;
		case Client::CUIPage_Menu::PART_GROUP::GROUP_MENU_INVEN:
			strTitle = TEXT("가방");
			strDesc = TEXT("보유한 아이템을 관리할 수 있습니다.");
			break;
		case Client::CUIPage_Menu::PART_GROUP::GROUP_MENU_CHARACTOR:
			strTitle = TEXT("캐릭터 정보");
			strDesc = TEXT("상태를 확인할 수 있습니다.");
			break;
		case Client::CUIPage_Menu::PART_GROUP::GROUP_MENU_HEART:
			strTitle = TEXT("P기관");
			strDesc = TEXT("업그레이드 및 기능을 선택할 수 있습니다.");
			break;
		case Client::CUIPage_Menu::PART_GROUP::GROUP_MENU_OPTION:
			strTitle = TEXT("설정");
			strDesc = TEXT("게임 내 설정을 변경할 수 있습니다.");
			break;
		default:
			break;
		}

		__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DESC_TITLE))->strText = strTitle;
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DESC_TEXT))->strText = strDesc;
	
	}
	else
	{
		const CItem_Manager::ITEM* pItem = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(_int(m_eFocus_Group) - _int(PART_GROUP::GROUP_TOP_CELL_0) + _int(EQUIP_SLOT::EQUIP_USING_TOP_0)));

		if (pItem != nullptr)
		{
			__super::Array_Control(_int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0), _int(PART_GROUP::GROUP_ITEM_DESC_FUNC_1), CTRL_COMMAND::COM_RENDER, true);
			__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ITEM_DESC_NAME))->strText = pItem->strName;

			for (_int i = _int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0); i <= _int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_1); ++i)
			{
				_Vec2 fMouse = __super::Check_Mouse_By_Part(*__super::Get_Front_Part_In_Control(i));
				if (fMouse.x != -1.f)
					m_vec_Group_Ctrl[i]->bRender = true;
				else
					m_vec_Group_Ctrl[i]->bRender = false;
			}
		}
			
	}

}

CUIPage_Menu* CUIPage_Menu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Menu* pInstance = new CUIPage_Menu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Menu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Menu::Clone(void* pArg)
{
	CUIPage_Menu* pInstance = new CUIPage_Menu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Menu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Menu::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
