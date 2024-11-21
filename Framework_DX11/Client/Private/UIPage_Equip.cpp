#include "stdafx.h"
#include "..\Public\UIPage_Equip.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Equip::CUIPage_Equip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Equip::CUIPage_Equip(const CUIPage_Equip& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Equip::Initialize_Prototype()
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

	m_fTopPartMove_Excel = 100.f;

	return S_OK;
}

HRESULT CUIPage_Equip::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Equip::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Equip::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Equip::Late_Update(_float fTimeDelta)
{
	Update_Equip_Page(fTimeDelta);
	Update_ToolTip(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);

	Update_Tap_Button(fTimeDelta);
	Update_Item_Cell(fTimeDelta);

	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Equip::Render()
{
	return S_OK;
}

void CUIPage_Equip::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Equip::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Equip::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	Action_Equip_Page(fTimeDelta);
	Action_Focus(fTimeDelta);


	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Equip::Ready_UIPart_Group_Control()
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

void CUIPage_Equip::Action_Equip_Page(_float fTimeDelta)
{
	for (_int i = 0; i < 3; ++i)
	{
		_Vec2 fPos = Check_Mouse_By_Part(*m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_TOP_TAP_BUTTON_0) + i)]);

		if (fPos.x != -1.f)
			m_iNow_Page = i;
	}
		
	if (KEY_TAP(KEY::Q))
	{
		if (m_iNow_Page > 0)
			--m_iNow_Page;
	}
	else if (KEY_TAP(KEY::E))
	{
		if (m_iNow_Page < 2)
			++m_iNow_Page;
	}
}

void CUIPage_Equip::Action_Focus(_float fTimeDelta)
{






}

void CUIPage_Equip::Update_Equip_Page(_float fTimeDelta)
{
	if (m_iNow_Page == 0)
	{
		__super::Array_Control(_int(PART_GROUP::GROUP_PAGE_0_FRAME), _int(PART_GROUP::GROUP_PAGE_0_DEFENCE_3), CTRL_COMMAND::COM_RENDER, true);
		__super::Array_Control(_int(PART_GROUP::GROUP_PAGE_1_FRAME), _int(PART_GROUP::GROUP_PAGE_1_BAG_3), CTRL_COMMAND::COM_RENDER, false);
		__super::Array_Control(_int(PART_GROUP::GROUP_PAGE_2_FRAME), _int(PART_GROUP::GROUP_PAGE_2_COSTUME), CTRL_COMMAND::COM_RENDER, false);
		m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOP_TAP_HIGHLIGHT_LINE)]->fRatio = 0.f;
	}
	else if (m_iNow_Page == 1)
	{
		__super::Array_Control(_int(PART_GROUP::GROUP_PAGE_0_FRAME), _int(PART_GROUP::GROUP_PAGE_0_DEFENCE_3), CTRL_COMMAND::COM_RENDER, false);
		__super::Array_Control(_int(PART_GROUP::GROUP_PAGE_1_FRAME), _int(PART_GROUP::GROUP_PAGE_1_BAG_3), CTRL_COMMAND::COM_RENDER, true);
		__super::Array_Control(_int(PART_GROUP::GROUP_PAGE_2_FRAME), _int(PART_GROUP::GROUP_PAGE_2_COSTUME), CTRL_COMMAND::COM_RENDER, false);
		m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOP_TAP_HIGHLIGHT_LINE)]->fRatio = 0.5f;
	}
	else if (m_iNow_Page == 2)
	{
		__super::Array_Control(_int(PART_GROUP::GROUP_PAGE_0_FRAME), _int(PART_GROUP::GROUP_PAGE_0_DEFENCE_3), CTRL_COMMAND::COM_RENDER, false);
		__super::Array_Control(_int(PART_GROUP::GROUP_PAGE_1_FRAME), _int(PART_GROUP::GROUP_PAGE_1_BAG_3), CTRL_COMMAND::COM_RENDER, false);
		__super::Array_Control(_int(PART_GROUP::GROUP_PAGE_2_FRAME), _int(PART_GROUP::GROUP_PAGE_2_COSTUME), CTRL_COMMAND::COM_RENDER, true);
		m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOP_TAP_HIGHLIGHT_LINE)]->fRatio = 1.f;
	}

	Update_Item_Cell(fTimeDelta);
}

void CUIPage_Equip::Update_Tap_Button(_float fTimeDelta)
{
	for (_int i = 0; i < 3; ++i)
	{
		list<_int>::iterator iter = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOP_TAP_BUTTON_0) + i]->PartIndexlist.begin();

		if (i == m_iNow_Page)
		{
			++iter;
			m_vecPart[*iter]->bRender = true;
			++iter;
			m_vecPart[*iter]->Set_RedText();
		}
		else
		{
			++iter;
			m_vecPart[*iter]->bRender = false;
			++iter;
			m_vecPart[*iter]->Set_WhiteText();
		}
	}
}

void CUIPage_Equip::Update_ToolTip(_float fTimeDelta)
{
}

void CUIPage_Equip::Update_Item_Cell(_float fTimeDelta)
{
	if (m_iNow_Page == 0)
	{
		Update_Weapon_Cell();
		Update_Arm_Cell();
	}

	Update_Normal_Cell();
}

void CUIPage_Equip::Update_Weapon_Cell()
{
	for (_int i = 0; i < 2; ++i)
	{
		_int iEquip_Slot_Index = _int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0) + (i * 2);

		const CItem_Manager::ITEM* pItemBlade = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(iEquip_Slot_Index));
		const CItem_Manager::ITEM* pItemHandle = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(iEquip_Slot_Index + 1));

		_int iGroup = _int(PART_GROUP::GROUP_PAGE_0_WEAPON_FIRST) + i;
		list<_int>::iterator iter = m_vec_Group_Ctrl[iGroup]->PartIndexlist.begin();

		if ((pItemBlade == nullptr) || (pItemBlade->eType_Index == ITEM_TYPE::ITEMTYPE_END))
		{
			++iter; m_vecPart[*iter]->bRender = m_eFocus_Group == PART_GROUP(iGroup) ? true : false;
			++iter; m_vecPart[*iter]->bRender = true;
			++iter; m_vecPart[*iter]->bRender = false;
			++iter; m_vecPart[*iter]->bRender = false;
			++iter; m_vecPart[*iter]->bRender = false;
			++iter; m_vecPart[*iter]->bRender = false;
			++iter; m_vecPart[*iter]->bRender = false;
		}
		else if (pItemBlade->bModule_Weapon)
		{
			++iter; m_vecPart[*iter]->bRender = m_eFocus_Group == PART_GROUP(iGroup) ? true : false;
			++iter; m_vecPart[*iter]->bRender = false;
			++iter; m_vecPart[*iter]->bRender = true;
			++iter; m_vecPart[*iter]->bRender = false;
			++iter; m_vecPart[*iter]->bRender = true; m_vecPart[*iter]->iTexture_Index = pItemBlade->iTexture_Index;
			++iter; m_vecPart[*iter]->bRender = true; m_vecPart[*iter]->iTexture_Index = pItemHandle->iTexture_Index;
			++iter; m_vecPart[*iter]->bRender = true;
		}
		else
		{
			++iter; m_vecPart[*iter]->bRender = m_eFocus_Group == PART_GROUP(iGroup) ? true : false;
			++iter; m_vecPart[*iter]->bRender = false;
			++iter; m_vecPart[*iter]->bRender = false;
			++iter; m_vecPart[*iter]->bRender = true; m_vecPart[*iter]->iTexture_Index = pItemBlade->iTexture_Index;
			++iter; m_vecPart[*iter]->bRender = false;
			++iter; m_vecPart[*iter]->bRender = false;
			++iter; m_vecPart[*iter]->bRender = true;
		}
	}
}

void CUIPage_Equip::Update_Arm_Cell()
{
	const CItem_Manager::ITEM* pItem = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_RESION_ARM);
	list<_int>::iterator iter = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_PAGE_0_ARM)]->PartIndexlist.begin();

	if((pItem == nullptr) || (pItem->eType_Index == ITEM_TYPE::ITEMTYPE_END))
	{
		++iter; m_vecPart[*iter]->bRender = m_eFocus_Group == PART_GROUP::GROUP_PAGE_0_ARM ? true : false;
		++iter; 
		++iter; m_vecPart[*iter]->bRender = false;
	}
	else
	{
		++iter; m_vecPart[*iter]->bRender = m_eFocus_Group == PART_GROUP::GROUP_PAGE_0_ARM ? true : false;
		++iter;
		++iter; m_vecPart[*iter]->bRender = false; m_vecPart[*iter]->iTexture_Index = pItem->iTexture_Index;
	}
}

void CUIPage_Equip::Update_Normal_Cell()
{
	_int iStart_Index = 0;
	_int iEnd_Index = 0;
	EQUIP_SLOT eStart_Slot = EQUIP_SLOT::EQUIP_END;

	if (m_iNow_Page == 0)
	{
		iStart_Index = _int(PART_GROUP::GROUP_PAGE_0_AMULET_FIRST);
		iEnd_Index = _int(PART_GROUP::GROUP_PAGE_0_DEFENCE_3);
		eStart_Slot = EQUIP_SLOT::EQUIP_AMULET_0;
	}
	else if (m_iNow_Page == 1)
	{
		iStart_Index = _int(PART_GROUP::GROUP_PAGE_1_TOP_0);
		iEnd_Index = _int(PART_GROUP::GROUP_PAGE_1_BAG_3);
		eStart_Slot = EQUIP_SLOT::EQUIP_USING_TOP_0;
	}
	else if (m_iNow_Page == 2)
	{
		iStart_Index = _int(PART_GROUP::GROUP_PAGE_2_HEAD_0);
		iEnd_Index = _int(PART_GROUP::GROUP_PAGE_2_COSTUME);
		eStart_Slot = EQUIP_SLOT::EQUIP_HEAD_0;
	}

	for (_int i = iStart_Index; i <= iEnd_Index; ++i)
	{
		list<_int>::iterator iter = m_vec_Group_Ctrl[i]->PartIndexlist.begin();

		++iter;
		if (_int(m_eFocus_Group) == i)
			m_vecPart[*iter]->bRender = true;
		else
			m_vecPart[*iter]->bRender = false;

		const CItem_Manager::ITEM* pItem = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(_int(eStart_Slot) + (i - iStart_Index)));

		if ((pItem == nullptr) || (pItem->eType_Index == ITEM_TYPE::ITEMTYPE_END)) 
		{
			++iter;
			m_vecPart[*iter]->bRender = true;
			++iter;
			m_vecPart[*iter]->bRender = false;

			if (m_iNow_Page == 1)
			{
				++iter;
				m_vecPart[*iter]->bRender = false;
			}
		}
		else
		{
			++iter;
			m_vecPart[*iter]->bRender = false;
			++iter;
			m_vecPart[*iter]->bRender = true;
			m_vecPart[*iter]->iTexture_Index = pItem->iTexture_Index;

			if (m_iNow_Page == 1)
			{
				++iter;
				m_vecPart[*iter]->bRender = true;
				m_vecPart[*iter]->strText = to_wstring(pItem->iCount);
			}
		}
	}
}

CUIPage_Equip* CUIPage_Equip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Equip* pInstance = new CUIPage_Equip(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Equip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Equip::Clone(void* pArg)
{
	CUIPage_Equip* pInstance = new CUIPage_Equip(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Equip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Equip::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
