#include "stdafx.h"
#include "..\Public\UIPage_ItemInfo.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_ItemInfo::CUIPage_ItemInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_ItemInfo::CUIPage_ItemInfo(const CUIPage_ItemInfo& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_ItemInfo::Initialize_Prototype()
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

HRESULT CUIPage_ItemInfo::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_ItemInfo::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_ItemInfo::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_ItemInfo::Late_Update(_float fTimeDelta)
{
	if (m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] == false)
		return;

	SCROLL_AREA eArea = SCROLL_AREA::SCROLL_NONE;
	if (m_eNowPage == UIPAGE::PAGE_INVEN)
		eArea = SCROLL_AREA::SCROLL_INVEN;

	if (m_eNowPage == UIPAGE::PAGE_TELEPOT)
		eArea = SCROLL_AREA::SCROLL_TELEPOT;

	// focus 
	if (m_bFocus)
	{
		if (m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->bRender)
		{
			m_fFocus_Fire_Move_Ratio += fTimeDelta * 5.f;
			m_fFocus_Fire_Move_Ratio = min(m_fFocus_Fire_Move_Ratio, 1.f);

			m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->MakeDirec();
			m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fPosition =
			{
				m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fAdjust_Start.x + (m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fDirec.x * m_fFocus_Fire_Move_Ratio),
				m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fAdjust_Start.y + (m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fDirec.y * m_fFocus_Fire_Move_Ratio)
			};
		}

		Input_Render_Info(*m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Rect)], eArea);
		Input_Render_Info(*m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)], eArea);
	}

	// New
	m_fNewmarkGrow = min(m_fNewmarkGrow + (fTimeDelta * 120.f), 360.f);
	if (m_fNewmarkGrow == 360.f) m_fNewmarkGrow = 0.f;

	_float fAdjustAlpha = _float(sin(XMConvertToRadians(m_fNewmarkGrow)));
	m_vecPart[_int(PART_GROUP::ITEMINFO_NEW)]->fTextureColor.w *= abs(fAdjustAlpha) + 0.5f;

	while (!m_queueNewMarkPos.empty())
	{
		_Vec2 vPos = m_queueNewMarkPos.front();
		m_queueNewMarkPos.pop();

		m_vecPart[_int(PART_GROUP::ITEMINFO_NEW)]->fPosition = vPos;

		Input_Render_Info(*m_vecPart[_int(PART_GROUP::ITEMINFO_NEW)], eArea);
	}

	m_vecPart[_int(PART_GROUP::ITEMINFO_NEW)]->fTextureColor.w /= abs(fAdjustAlpha) + 0.5f;

	// Action
	if (m_bIsActive_Func)
	for (_int i = _int(PART_GROUP::ITEMINFO_ACTION_Back); i <= _int(PART_GROUP::ITEMINFO_ACTION_Text_3); ++i)
	{
		m_vecPart[i]->MovePart(m_vecPart[m_vecPart[i]->iParentPart_Index]->fPosition, fTimeDelta);

		if (m_vecPart[i]->bRender)
			Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_NONE);
	}

	// ToolTip
	{
		for (_int i = _int(PART_GROUP::TOOLTIP_Back); i <= _int(PART_GROUP::TOOLTIP_Item_ChestInfo_Desc); ++i)
		{
			if (i == _int(PART_GROUP::TOOLTIP_Back))
				m_vecPart[i]->MovePart(_Vec2{0.f,0.f}, fTimeDelta);
			else 
				m_vecPart[i]->MovePart(m_vecPart[m_vecPart[i]->iParentPart_Index]->fPosition, fTimeDelta);

			if (m_vecPart[i]->bRender)
				Input_Render_Info(*m_vecPart[i], SCROLL_AREA::SCROLL_NONE);
		}
	}

	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = false;
	__super::Array_Control(_int(PART_GROUP::TOOLTIP_Back), _int(PART_GROUP::TOOLTIP_Item_ChestInfo_Desc), CTRL_COMMAND::COM_RENDER, false);
}

HRESULT CUIPage_ItemInfo::Render()
{
	return S_OK;
}

void CUIPage_ItemInfo::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_ItemInfo::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_ItemInfo::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	if (m_bIsActive_Func)
		Action_ItemAction(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_ItemInfo::Ready_UIPart_Group_Control()
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

	__super::Array_Control(_int(PART_GROUP::ITEMINFO_TOOLTIP_Frame), _int(PART_GROUP::ITEMINFO_ACTION_Text_3), CTRL_COMMAND::COM_RENDER, false);

	m_strFuncName[_int(ITEM_FUNC::FUNC_USING)] = TEXT("사용하기");
	m_strFuncName[_int(ITEM_FUNC::FUNC_TO_INVEN)] = TEXT("인벤토리로 이동");
	m_strFuncName[_int(ITEM_FUNC::FUNC_TO_EQUIP)] = TEXT("장비창으로 이동");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP)] = TEXT("착용하기");
	m_strFuncName[_int(ITEM_FUNC::FUNC_UNEQUIP)] = TEXT("해제하기");
	m_strFuncName[_int(ITEM_FUNC::FUNC_DELETE)] = TEXT("버리기");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_TOP)] = TEXT("착용하기 : 위 벨트");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_BOTTOM)] = TEXT("착용하기 : 아래 벨트");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_BAG)] = TEXT("착용하기 : 보조가방");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_WEAPON_FIRST)] = TEXT("착용하기 : 무기 1번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_WEAPON_SECOND)] = TEXT("착용하기 : 무기 2번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_TOP_0)] = TEXT("착용하기 : 위 벨트 1번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_TOP_1)] = TEXT("착용하기 : 위 벨트 2번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_TOP_2)] = TEXT("착용하기 : 위 벨트 3번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_BOTTOM_0)] = TEXT("착용하기 : 아래 벨트 1번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_BOTTOM_1)] = TEXT("착용하기 : 아래 벨트 2번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_BOTTOM_2)] = TEXT("착용하기 : 아래 벨트 3번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_BAG_0)] = TEXT("착용하기 : 보조가방 1번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_BAG_1)] = TEXT("착용하기 : 보조가방 2번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_BAG_2)] = TEXT("착용하기 : 보조가방 3번");
	m_strFuncName[_int(ITEM_FUNC::FUNC_EQUIP_BAG_3)] = TEXT("착용하기 : 보조가방 4번");


	Off_ItemAction();

	m_bRender = false;

	return S_OK;
}

void CUIPage_ItemInfo::Show_Focus(_Vec2 vItemCellPos, _Vec2 vItemCellSize)
{
	m_bFocus = true;

	_float fSize_Adjust = 1.1f;
	_float fFire_Adjust = 0.35f;

	if (m_eNowPage == UIPAGE::PAGE_INVEN)
	{
		fSize_Adjust = 1.2f;
		fFire_Adjust = 0.45f;
	}
		

	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Rect)]->fPosition = vItemCellPos;
	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Rect)]->fSize = vItemCellSize * fSize_Adjust;

	if (!m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Rect)]->bRender)
	{
		//m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fAdjust_Start = vItemCellPos + _Vec2{ -(vItemCellSize.x * 0.4f), vItemCellSize.y * 0.4f };
		m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fAdjust_End = vItemCellPos + _Vec2{ -(vItemCellSize.x * fFire_Adjust), vItemCellSize.y * fFire_Adjust };
	}
	else
	{
		m_fFocus_Fire_Move_Ratio = 0.f;
		m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fAdjust_Start = m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fPosition;
		m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->fAdjust_End = vItemCellPos + _Vec2{ -(vItemCellSize.x * fFire_Adjust), vItemCellSize.y * fFire_Adjust };
	}

	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Rect)]->bRender = true;
	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->bRender = true;
}

void CUIPage_ItemInfo::Off_Focus()
{
	m_bFocus = false;

	m_fFocus_Fire_Move_Ratio = 0.f;
	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Rect)]->bRender = false;
	m_vecPart[_int(PART_GROUP::ITEMINFO_SELECT_Fire)]->bRender = false;
}

void CUIPage_ItemInfo::Show_NewMark(_Vec2 vItemCellPos, _Vec2 vItemCellSize)
{
	_Vec2 vPos = vItemCellPos + _Vec2(vItemCellSize.x * 0.5f, -vItemCellSize.y * 0.5f);
	m_queueNewMarkPos.push(vPos);
}

void CUIPage_ItemInfo::Show_ItemAction(_Vec2 vItemCellPos, _Vec2 vItemCellSize, ITEM_FUNC eFunc0, ITEM_FUNC eFunc1, ITEM_FUNC eFunc2, ITEM_FUNC eFunc3)
{
	if (m_bIsActive_Func == false)
		m_pSoundCom->Play2D(TEXT("SE_UI_OpenWindow_01.wav"), &g_fUIVolume);

	m_bIsActive_Func = true;
	m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Header)]->fPosition = vItemCellPos + (m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Header)]->fSize * 0.5f) + (vItemCellSize * 0.5f);
	m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Back)]->bRender = true;
	m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Deco)]->bRender = true;

	m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Back)]->fRatio = 0.f;

	m_eActive_Func[0] = eFunc0;
	m_eActive_Func[1] = eFunc1;
	m_eActive_Func[2] = eFunc2;
	m_eActive_Func[3] = eFunc3;

	for (_int i = 0; i < 4; ++i)
	{
		if (m_eActive_Func[i] != ITEM_FUNC::FUNC_END)
		{
			m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Back)]->fRatio += 0.25f;
			m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Text_0) + (i * 3)]->strText = m_strFuncName[_int(m_eActive_Func[i])];
			m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Text_0) + (i * 3)]->bRender = true;
		}
		else
		{
			m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Text_0) + (i * 3)]->strText = {};
			m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Text_0) + (i * 3)]->bRender = false;
		}
	}
}

void CUIPage_ItemInfo::Off_ItemAction()
{
	m_bIsActive_Func = false;

	__super::Array_Control(_int(PART_GROUP::ITEMINFO_ACTION_Header), _int(PART_GROUP::ITEMINFO_ACTION_Text_3), CTRL_COMMAND::COM_RENDER, false);
	for (_int i = 0; i < 4; ++i)
		m_eActive_Func[i] = ITEM_FUNC::FUNC_END;
}

void CUIPage_ItemInfo::Action_ItemAction(_float fTimeDelta)
{
	m_iNow_Func = -1;

	for (_int i = 0; i < 4; ++i)
	{
		if (m_eActive_Func[i] != ITEM_FUNC::FUNC_END)
			if (GET_GAMEINTERFACE->CheckMouse(m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Mouse_0) + (i * 3)]->fPosition, m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Mouse_0) + (i * 3)]->fSize).x != -1.f)
			{
				m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Fx_0) + (i * 3)]->bRender = true;
				if (KEY_TAP(KEY::LBUTTON))
					m_iNow_Func = i;
				continue;
			}
		m_vecPart[_int(PART_GROUP::ITEMINFO_ACTION_Fx_0) + (i * 3)]->bRender = false;
	}
}

void CUIPage_ItemInfo::Show_Tooltip_Shop(_int iShopIndex)
{
	const CItem_Manager::ITEM* pItem = GET_GAMEINTERFACE->Get_Item_Origin_Spec(GET_GAMEINTERFACE->Get_ShopData()[iShopIndex]->iIndex);

	Make_TooltipPage(pItem);
}

void CUIPage_ItemInfo::Show_Tooltip_Chest(_int iChestIndex)
{
	const CItem_Manager::ITEM* pItem = GET_GAMEINTERFACE->Get_ChestData().find(iChestIndex)->second;

	Make_TooltipPage(pItem);
}

void CUIPage_ItemInfo::Show_Tooltip(INVEN_ARRAY_TYPE eType, _int iIndex)
{
	if ((_int(eType) >= _int(INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE)) && (_int(eType) <= _int(INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_HANDLE)))
	{
		if (eType == INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_HANDLE)
			eType = INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE;

		if (eType == INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_HANDLE)
			eType = INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_BLADE;

		const CItem_Manager::ITEM* pNowBlade = GET_GAMEINTERFACE->Get_Array_Item_Info(eType, iIndex);
		const CItem_Manager::ITEM* pNowHandle = GET_GAMEINTERFACE->Get_Array_Item_Info(INVEN_ARRAY_TYPE(_int(eType) + 1), iIndex);

		Make_TooltipPage(pNowBlade, true, pNowHandle);
	}
	else
	{
		const CItem_Manager::ITEM* pItem = GET_GAMEINTERFACE->Get_Array_Item_Info(eType, iIndex);

		Make_TooltipPage(pItem);
	}
}

void CUIPage_ItemInfo::Show_Tooltip(EQUIP_SLOT eSlot)
{
	if ((_int(eSlot) >= _int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0)) && (_int(eSlot) <= _int(EQUIP_SLOT::EQUIP_WEAPON_HANDLE_1)))
	{
		if (eSlot == EQUIP_SLOT::EQUIP_WEAPON_HANDLE_0)
			eSlot = EQUIP_SLOT::EQUIP_WEAPON_BLADE_0;

		if (eSlot == EQUIP_SLOT::EQUIP_WEAPON_HANDLE_1)
			eSlot = EQUIP_SLOT::EQUIP_WEAPON_BLADE_1;

		const CItem_Manager::ITEM* pNowBlade = GET_GAMEINTERFACE->Get_Equip_Item_Info(eSlot);
		const CItem_Manager::ITEM* pNowHandle = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(_int(eSlot) + 1));

		Make_TooltipPage(pNowBlade, true, pNowHandle);
	}
	else
	{
		const CItem_Manager::ITEM* pItem = GET_GAMEINTERFACE->Get_Equip_Item_Info(eSlot);

		Make_TooltipPage(pItem);
	}
}

void CUIPage_ItemInfo::Make_TooltipPage(const CItem_Manager::ITEM* Item, _bool bIsWeapon, const CItem_Manager::ITEM* ItemHandle)
{
	for (_int i = _int(PART_GROUP::TOOLTIP_Back); i <= _int(PART_GROUP::TOOLTIP_Item_ChestInfo_Desc); ++i)
	{
		m_vecPart[i]->bRender = false;
	}

	if (bIsWeapon == true)
	{
		_bool bIsNormal = true;

		if (!Item)
			return;
		else if ((Item) && (!ItemHandle))
			bIsNormal = false;
		else if (Item->bModule_Weapon == false)
			bIsNormal = false;

		for (_int i = _int(PART_GROUP::TOOLTIP_Back); i <= _int(PART_GROUP::TOOLTIP_Item_Fx1); ++i)
			m_vecPart[i]->bRender = true;

		m_vecPart[_int(PART_GROUP::TOOLTIP_Back)]->fAdjust = m_vToolTip_Pos;

		if (bIsNormal)
		{
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Blade)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Cross)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Handle)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Blade)]->iTexture_Index = Item->iTexture_Index;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Handle)]->iTexture_Index = ItemHandle->iTexture_Index;

			_wstring strName{};
			strName += Item->strName;
			strName += TEXT(" / ");
			strName += ItemHandle->strName;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Name)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Desc)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Desc)]->strText = Item->strItem_Desc;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Name)]->strText = strName;
		}
		else
		{
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Heroic)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Heroic)]->iTexture_Index = Item->iTexture_Index;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Name)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Desc)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Name)]->strText = Item->strName;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Desc)]->strText = Item->strItem_Desc;
		}

		m_vecPart[_int(PART_GROUP::TOOLTIP_Item_ShopInfo_Count)]->strText = Item->bIsAvailable_Shop == false ? TEXT("판매 불가") : to_wstring(Item->iPrice);
		m_vecPart[_int(PART_GROUP::TOOLTIP_Item_ChestInfo_Desc)]->strText = Item->bIsAvailable_Chest == false ? TEXT("보관함 사용 불가") : TEXT("보관함 사용 가능");

		for (_int i = _int(PART_GROUP::TOOLTIP_Item_ShopInfo_Frame); i <= _int(PART_GROUP::TOOLTIP_Item_ChestInfo_Desc); ++i)
			m_vecPart[i]->bRender = true;
	}
	else
	{
		const CItem_Manager::ITEM* pItem = Item;

		if (pItem)
		{
			for (_int i = _int(PART_GROUP::TOOLTIP_Back); i <= _int(PART_GROUP::TOOLTIP_Item_Fx1); ++i)
				m_vecPart[i]->bRender = true;

			m_vecPart[_int(PART_GROUP::TOOLTIP_Back)]->fAdjust = m_vToolTip_Pos;

			m_vecPart[_int(PART_GROUP::TOOLTIP_Back)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Frame)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Normal)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Normal)]->iTexture_Index = pItem->iTexture_Index;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Name)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Desc)]->bRender = true;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Name)]->strText = pItem->strName;
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_Desc)]->strText = pItem->strItem_Desc;

			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_ShopInfo_Count)]->strText = pItem->bIsAvailable_Shop == false ? TEXT("판매 불가") : to_wstring(pItem->iPrice);
			m_vecPart[_int(PART_GROUP::TOOLTIP_Item_ChestInfo_Desc)]->strText = pItem->bIsAvailable_Chest == false ? TEXT("보관함 사용 불가") : TEXT("보관함 사용 가능");

			for (_int i = _int(PART_GROUP::TOOLTIP_Item_ShopInfo_Frame); i <= _int(PART_GROUP::TOOLTIP_Item_ChestInfo_Desc); ++i)
				m_vecPart[i]->bRender = true;
		}
	}
}


CUIPage_ItemInfo* CUIPage_ItemInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_ItemInfo* pInstance = new CUIPage_ItemInfo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_ToolTip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_ItemInfo::Clone(void* pArg)
{
	CUIPage_ItemInfo* pInstance = new CUIPage_ItemInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_ToolTip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_ItemInfo::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}