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

	Focus_Update(fTimeDelta);

	

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);

	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Menu::Render()
{
	return S_OK;
}

void CUIPage_Menu::OpenAction()
{
	__super::OpenAction();

	GET_GAMEINTERFACE->SetIngame(true);

	if ((m_eFocus_Group < PART_GROUP::GROUP_MENU_EQUIP) || (m_eFocus_Group > PART_GROUP::GROUP_MENU_OPTION))
		m_eFocus_Group = PART_GROUP::GROUP_MENU_EQUIP;

	__super::Array_Control(_int(PART_GROUP::GROUP_DESC_BACK), _int(PART_GROUP::GROUP_DESC_MOUSE), CTRL_COMMAND::COM_RENDER, false);
	__super::Array_Control(_int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0), _int(PART_GROUP::GROUP_ITEM_DESC_FUNC_1), CTRL_COMMAND::COM_RENDER, false);
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_MARK)]->bRender = false;


}

void CUIPage_Menu::CloseAction()
{
	__super::CloseAction();

	__super::Array_Control(_int(PART_GROUP::GROUP_DESC_BACK), _int(PART_GROUP::GROUP_DESC_MOUSE), CTRL_COMMAND::COM_RENDER, false);
	__super::Array_Control(_int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0), _int(PART_GROUP::GROUP_ITEM_DESC_FUNC_1), CTRL_COMMAND::COM_RENDER, false);
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_MARK)]->bRender = false;
}

CHECK_MOUSE CUIPage_Menu::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	Focus_Update(fTimeDelta);
	if (m_eFocus_Group != PART_GROUP::GROUP_END)
		Desc_Update(fTimeDelta);

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
		_float2 fMouse = __super::Check_Mouse_By_Part(*__super::Get_Front_Part_In_Control(i));
		if (fMouse.x != -1.f)
		{
			m_eFocus_Group = PART_GROUP(i);
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

		pSelect->iParentPart_Index = (_int)m_eFocus_Group;
		pSelect->fSize = pGroup->fSize;
	}


}

void CUIPage_Menu::Desc_Update(_float fTimeDelta)
{
	__super::Array_Control(_int(PART_GROUP::GROUP_DESC_BACK), _int(PART_GROUP::GROUP_DESC_MOUSE), CTRL_COMMAND::COM_RENDER, false);
	__super::Array_Control(_int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0), _int(PART_GROUP::GROUP_ITEM_DESC_FUNC_1), CTRL_COMMAND::COM_RENDER, false);

	if ((m_eFocus_Group >= PART_GROUP::GROUP_MENU_EQUIP) && (m_eFocus_Group <= PART_GROUP::GROUP_MENU_OPTION))
	{
		__super::Array_Control(_int(PART_GROUP::GROUP_DESC_BACK), _int(PART_GROUP::GROUP_DESC_MOUSE), CTRL_COMMAND::COM_RENDER, true);
		_float2 fMouse = __super::Check_Mouse_By_Part(*__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DESC_MOUSE)));
		if (fMouse.x != -1.f)
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_DESC_FX)]->bRender = true;
		else
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_DESC_FX)]->bRender = false;
	}
	else
	{
		__super::Array_Control(_int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0), _int(PART_GROUP::GROUP_ITEM_DESC_FUNC_1), CTRL_COMMAND::COM_RENDER, true);

		for (_int i = _int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0); i <= _int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_1); ++i)
		{
			_float2 fMouse = __super::Check_Mouse_By_Part(*__super::Get_Front_Part_In_Control(i));
			if (fMouse.x != -1.f)
				m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0) + i]->bRender = true;
			else
				m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ITEM_DESC_MOUSE_0) + i]->bRender = false;
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
