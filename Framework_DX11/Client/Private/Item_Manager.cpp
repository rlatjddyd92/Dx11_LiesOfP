#include "stdafx.h"
#include "..\Public\Item_Manager.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"


CItem_Manager::CItem_Manager(CGameInstance* pGameInstance)
{
	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGameInstance);
}

void CItem_Manager::Update_Item(_float fDeltatime)
{
	m_bIsChange = false;
	m_iNow_Using = -1;
	m_LastFrame_UsingItem.clear();

	if ((!m_bSpec_Setting) && (GET_GAMEINTERFACE->Get_Player() != nullptr))
	{
		Adjust_Spec();
		m_bSpec_Setting = false;
	}
		
	if (m_bAmulet_Achievment_Complete == false)
	{
		if (m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_AMULET_0)]->eType != INVEN_ARRAY_TYPE::TYPE_END)
			if (m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_AMULET_1)]->eType != INVEN_ARRAY_TYPE::TYPE_END)
			{
				GET_GAMEINTERFACE->Input_Achievment_Data(5, 1);
				m_bAmulet_Achievment_Complete = true;
			}
	}
	
	if (m_bDefence_Achievment_Complete == false)
	{
		_int iCountDefence = 0;
		iCountDefence += m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_DEFENCE_FRAME)]->eType != INVEN_ARRAY_TYPE::TYPE_END;
		iCountDefence += m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_DEFENCE_CARTRIGE)]->eType != INVEN_ARRAY_TYPE::TYPE_END;
		iCountDefence += m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_DEFENCE_CONVERTOR)]->eType != INVEN_ARRAY_TYPE::TYPE_END;
		iCountDefence += m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_DEFENCE_RAINER)]->eType != INVEN_ARRAY_TYPE::TYPE_END;

		if (iCountDefence == 4)
		{
			GET_GAMEINTERFACE->Input_Achievment_Data(13, 1);
			m_bDefence_Achievment_Complete = true;
		}
	}
	
}

ITEM_RESULT CItem_Manager::AddNewItem_Inven(_uint iItemIndex, _uint iCount)
{
	ITEM* NewItem = m_vecItem_BasicSpec[iItemIndex];

	if (NewItem == nullptr)
		return ITEM_RESULT::RESULT_INVALID;

	_uint iInvenSlotIndex = m_vecItem_InvenSlotIndex[_uint(NewItem->eType_Index)];

	if (_uint(NewItem->eType_Index) <= _uint(ITEM_TYPE::ITEMTYPE_POLE))
	{
		if (!NewItem->bModule_Weapon) iInvenSlotIndex += m_iAdd_Heroic;
		else iInvenSlotIndex += m_iAdd_Separate;
		if (NewItem->bIsHandele) iInvenSlotIndex += m_iAdd_Handle;
	}


	m_vecArray_Inven[iInvenSlotIndex]->Input_Item(NewItem, iCount);

	GET_GAMEINTERFACE->Input_Drop_Item_Info(iItemIndex, iCount);

	m_bIsChange = true;
	return ITEM_RESULT::RESULT_SUCCESS;
}

ITEM_RESULT CItem_Manager::InputItem_Inven(ITEM* pItem, _uint iCount)
{
	_uint iInvenSlotIndex = m_vecItem_InvenSlotIndex[_uint(pItem->eType_Index)];
	if (!pItem->bModule_Weapon) iInvenSlotIndex += m_iAdd_Heroic;
	else iInvenSlotIndex += m_iAdd_Separate;
	if (!pItem->bIsHandele) iInvenSlotIndex += m_iAdd_Handle;

	m_vecArray_Inven[iInvenSlotIndex]->Input_Item(pItem, iCount);

	m_bIsChange = true;
	return ITEM_RESULT::RESULT_SUCCESS;
}

ITEM_RESULT CItem_Manager::Assemble_Blade_Handle(_int iBladeIndex, _int iHandleIndex)
{
	ITEM* pBlade = m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_BLADE_PART)]->Get_Item_Info(iBladeIndex);
	ITEM* pHandle = m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_HANDEL_PART)]->Get_Item_Info(iHandleIndex);

	if ((pBlade == nullptr) || (pHandle == nullptr))
		return ITEM_RESULT::RESULT_INVALID;

	if (m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE)]->iNextIndex != m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_HANDLE)]->iNextIndex)
		return ITEM_RESULT::RESULT_INVALID;

	if (m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE)]->Input_Item(pBlade, 1) == ITEM_RESULT::RESULT_INVALID)
		return ITEM_RESULT::RESULT_INVALID;
	if (m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_HANDLE)]->Input_Item(pHandle, 1) == ITEM_RESULT::RESULT_INVALID)
		return ITEM_RESULT::RESULT_INVALID;

	m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_BLADE_PART)]->Remove_Item(iBladeIndex);
	m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_HANDEL_PART)]->Remove_Item(iHandleIndex);

	m_bIsChange = true;
	return ITEM_RESULT::RESULT_SUCCESS;
}

ITEM_RESULT CItem_Manager::EquipItem_Inven(INVEN_ARRAY_TYPE eIndex, EQUIP_SLOT eSlot, _uint iIndex)
{
	if ((eIndex == INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE) || (eIndex == INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_HANDLE))
		return(EquipWeapon_Inven(eIndex, eSlot, iIndex));
	else if ((eIndex == INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_BLADE) || (eIndex == INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_HANDLE))
		return(EquipWeapon_Inven(eIndex, eSlot, iIndex));
	else
	{
		ITEM& NewItem = *(m_vecArray_Inven[_uint(eIndex)]->vecItemInfo[iIndex]);

		if (NewItem.eType_Index == ITEM_TYPE::ITEMTYPE_END)
			return ITEM_RESULT::RESULT_INVALID;

		if (m_vecEquip_ItemInfo[_uint(eSlot)]->vecValid_InvenArray[_uint(eIndex)])
		{
			UnEquipItem_Inven(eSlot);
			m_vecEquip_ItemInfo[_uint(eSlot)]->eType = eIndex;
			m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex = iIndex;
			NewItem.eSlot = eSlot;
			m_bIsChange = true;
			return ITEM_RESULT::RESULT_SUCCESS;
		}

		return ITEM_RESULT::RESULT_INVALID;
	}
}

ITEM_RESULT CItem_Manager::EquipWeapon_Inven(INVEN_ARRAY_TYPE eIndex, EQUIP_SLOT eSlot, _uint iIndex)
{
	if ((eIndex == INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE) || (eIndex == INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_HANDLE))
		eIndex = INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE;
	if ((eIndex == INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_BLADE) || (eIndex == INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_HANDLE))
		eIndex = INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_BLADE;

	// 안전장치 
	// 지금 들고 있는 무기 조작 금지
	if ((eSlot == EQUIP_SLOT::EQUIP_WEAPON_BLADE_0) && (m_iWeapon_Select == 0))
	{
		GET_GAMEINTERFACE->Show_Popup(TEXT("변경 불가"), TEXT("현재 사용 중인 무기는 교체 불가."));
		return ITEM_RESULT::RESULT_INVALID;
	}
		
	if ((eSlot == EQUIP_SLOT::EQUIP_WEAPON_BLADE_1) && (m_iWeapon_Select == 1))
	{
		GET_GAMEINTERFACE->Show_Popup(TEXT("변경 불가"), TEXT("현재 사용 중인 무기는 교체 불가."));
		return ITEM_RESULT::RESULT_INVALID;
	}
		

	ITEM& Blade = *(m_vecArray_Inven[_uint(eIndex)]->vecItemInfo[iIndex]);
	ITEM& Handle = *(m_vecArray_Inven[_uint(eIndex) + 1]->vecItemInfo[iIndex]);

	if (Blade.eType_Index == ITEM_TYPE::ITEMTYPE_END)
		return ITEM_RESULT::RESULT_INVALID;

	if ((_int(eSlot) >= _int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0)) && (_int(eSlot) <= _int(EQUIP_SLOT::EQUIP_WEAPON_HANDLE_1)))
	{
		UnEquipItem_Inven(eSlot);
		UnEquipItem_Inven(EQUIP_SLOT(_int(eSlot) + 1));

		m_vecEquip_ItemInfo[_uint(eSlot)]->eType = eIndex;
		m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex = iIndex;
		Blade.eSlot = eSlot;

		if (Handle.eType_Index != ITEM_TYPE::ITEMTYPE_END)
		{
			m_vecEquip_ItemInfo[_uint(_int(eSlot) + 1)]->eType = INVEN_ARRAY_TYPE( _int(eIndex) + 1);
			m_vecEquip_ItemInfo[_uint(_int(eSlot) + 1)]->iIndex = iIndex;
			Handle.eSlot = EQUIP_SLOT(_int(eSlot) + 1);
		}

		m_bIsChange = true;
		
		if (GET_GAMEINTERFACE->IsGamePause())
			GET_GAMEINTERFACE->Input_Achievment_Data(4, 1);

		return ITEM_RESULT::RESULT_SUCCESS;
	}



	return ITEM_RESULT::RESULT_INVALID;
}




ITEM_RESULT CItem_Manager::UnEquipItem_Inven(EQUIP_SLOT eSlot)
{
	if (m_vecEquip_ItemInfo[_uint(eSlot)]->eType == INVEN_ARRAY_TYPE::TYPE_END)
		return ITEM_RESULT::RESULT_INVALID;

	if ((_int(eSlot) >= _int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0)) && (_int(eSlot) <= _int(EQUIP_SLOT::EQUIP_WEAPON_HANDLE_1)))
		return(UnEquipWeapon_Inven(eSlot));
	else
	{
		IsValid_Inven(m_vecEquip_ItemInfo[_uint(eSlot)]->eType, m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex);

		ITEM& NewItem = *(m_vecArray_Inven[_uint(m_vecEquip_ItemInfo[_uint(eSlot)]->eType)]->vecItemInfo[m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex]);

		if (NewItem.eType_Index == ITEM_TYPE::ITEMTYPE_END)
			return ITEM_RESULT::RESULT_INVALID;

		NewItem.eSlot = EQUIP_SLOT::EQUIP_END;

		m_vecEquip_ItemInfo[_uint(eSlot)]->eType = INVEN_ARRAY_TYPE::TYPE_END;
		m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex = -1;

		return ITEM_RESULT::RESULT_SUCCESS;
	}
}

ITEM_RESULT CItem_Manager::UnEquipWeapon_Inven(EQUIP_SLOT eSlot)
{
	if ((_int(eSlot) >= _int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0)) && (_int(eSlot) <= _int(EQUIP_SLOT::EQUIP_WEAPON_HANDLE_0)))
		eSlot = EQUIP_SLOT::EQUIP_WEAPON_BLADE_0;
	if ((_int(eSlot) >= _int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_1)) && (_int(eSlot) <= _int(EQUIP_SLOT::EQUIP_WEAPON_HANDLE_1)))
		eSlot = EQUIP_SLOT::EQUIP_WEAPON_BLADE_1;

	// 안전장치 
	// 지금 들고 있는 무기 조작 금지
	if ((eSlot == EQUIP_SLOT::EQUIP_WEAPON_BLADE_0) && (m_iWeapon_Select == 0))
	{
		GET_GAMEINTERFACE->Show_Popup(TEXT("변경 불가"), TEXT("현재 사용 중인 무기는 해제 불가."));
		return ITEM_RESULT::RESULT_INVALID;
	}
		
	if ((eSlot == EQUIP_SLOT::EQUIP_WEAPON_BLADE_1) && (m_iWeapon_Select == 1))
	{
		GET_GAMEINTERFACE->Show_Popup(TEXT("변경 불가"), TEXT("현재 사용 중인 무기는 해제 불가."));
		return ITEM_RESULT::RESULT_INVALID;
	}

	if (!IsValid_Inven(m_vecEquip_ItemInfo[_uint(eSlot)]->eType, m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex))
		return ITEM_RESULT::RESULT_INVALID;
	else
	{
		ITEM& Blade = *(m_vecArray_Inven[_uint(m_vecEquip_ItemInfo[_uint(eSlot)]->eType)]->vecItemInfo[m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex]);

		if (Blade.eType_Index == ITEM_TYPE::ITEMTYPE_END)
			return ITEM_RESULT::RESULT_INVALID;

		Blade.eSlot = EQUIP_SLOT::EQUIP_END;

		m_vecEquip_ItemInfo[_uint(eSlot)]->eType = INVEN_ARRAY_TYPE::TYPE_END;
		m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex = -1;
	}
	if (IsValid_Inven(m_vecEquip_ItemInfo[_uint(eSlot) + 1]->eType, m_vecEquip_ItemInfo[_uint(eSlot) + 1]->iIndex))
	{
		ITEM& Handle = *(m_vecArray_Inven[_uint(m_vecEquip_ItemInfo[_uint(eSlot) + 1]->eType)]->vecItemInfo[m_vecEquip_ItemInfo[_uint(eSlot) + 1]->iIndex]);

		if (Handle.eType_Index != ITEM_TYPE::ITEMTYPE_END)
		{
			Handle.eSlot = EQUIP_SLOT::EQUIP_END;

			m_vecEquip_ItemInfo[_uint(eSlot) + 1]->eType = INVEN_ARRAY_TYPE::TYPE_END;
			m_vecEquip_ItemInfo[_uint(eSlot) + 1]->iIndex = -1;
		}
	}

	return ITEM_RESULT::RESULT_SUCCESS;
}

ITEM_RESULT CItem_Manager::Move_EquipItem(EQUIP_SLOT eBefore, EQUIP_SLOT eDest)
{
	if ((_int(eBefore) >= _int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0)) && (_int(eBefore) <= _int(EQUIP_SLOT::EQUIP_WEAPON_HANDLE_0)))
		eBefore = EQUIP_SLOT::EQUIP_WEAPON_BLADE_0;
	if ((_int(eBefore) >= _int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_1)) && (_int(eBefore) <= _int(EQUIP_SLOT::EQUIP_WEAPON_HANDLE_1)))
		eBefore = EQUIP_SLOT::EQUIP_WEAPON_BLADE_1;

	if ((_int(eDest) >= _int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0)) && (_int(eDest) <= _int(EQUIP_SLOT::EQUIP_WEAPON_HANDLE_0)))
		eDest = EQUIP_SLOT::EQUIP_WEAPON_BLADE_0;
	if ((_int(eDest) >= _int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_1)) && (_int(eDest) <= _int(EQUIP_SLOT::EQUIP_WEAPON_HANDLE_1)))
		eDest = EQUIP_SLOT::EQUIP_WEAPON_BLADE_1;

	if ((IsValid_Equip(eBefore)) && (IsValid_Equip(eDest)))
	{
		INVEN_ARRAY_TYPE eDest_Type = m_vecEquip_ItemInfo[_uint(eBefore)]->eType;
		_int iDest_Index = m_vecEquip_ItemInfo[_uint(eBefore)]->iIndex;
		if (!IsValid_Inven(eDest_Type, iDest_Index))
			return ITEM_RESULT::RESULT_INVALID;

		UnEquipItem_Inven(eBefore);
		EquipItem_Inven(eDest_Type, eDest, iDest_Index);
	}

	return ITEM_RESULT();
}

ITEM_RESULT CItem_Manager::UseItem_Equip(EQUIP_SLOT eSlot, _uint iCount)
{
	INVEN_ARRAY_TYPE eArray = m_vecEquip_ItemInfo[_uint(eSlot)]->eType;
	_uint iIndex = m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex;
		
	if (!IsValid_Inven(eArray, iIndex))
		return ITEM_RESULT::RESULT_INVALID;

	if (m_vecArray_Inven[_uint(eArray)]->Get_Item_Info(iIndex)->iItem_Index == _int(SPECIAL_ITEM::SP_PULSE_BATTERY))
	{
		if (Use_Potion())
		{
			m_bIsChange = true;
			Set_Item_Funtion(m_vecArray_Inven[_uint(eArray)]->Get_Item_Info(iIndex)->iItem_Index);
			return ITEM_RESULT::RESULT_SUCCESS;
		}
		else
			return ITEM_RESULT::RESULT_INVALID;
	}
	else
	{
		Set_Item_Funtion(m_vecArray_Inven[_uint(eArray)]->Get_Item_Info(iIndex)->iItem_Index);
		return m_vecArray_Inven[_uint(eArray)]->Use_Item(iIndex, iCount);
	}
		
}

ITEM_RESULT CItem_Manager::UseItem_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex, _uint iCount)
{
	if (!IsValid_Inven(eIndex, iIndex))
		return ITEM_RESULT::RESULT_INVALID;

	if (m_vecArray_Inven[_uint(eIndex)]->Get_Item_Info(iIndex)->iItem_Index == _int(SPECIAL_ITEM::SP_PULSE_BATTERY))
	{
		if (Use_Potion())
		{
			m_bIsChange = true;
			Set_Item_Funtion(m_vecArray_Inven[_uint(eIndex)]->Get_Item_Info(iIndex)->iItem_Index);
			return ITEM_RESULT::RESULT_SUCCESS;
		}
		else
			return ITEM_RESULT::RESULT_INVALID;
	}
	else
	{
		Set_Item_Funtion(m_vecArray_Inven[_uint(eIndex)]->Get_Item_Info(iIndex)->iItem_Index);
		return m_vecArray_Inven[_uint(eIndex)]->Use_Item(iIndex, iCount);
	}
}

ITEM_RESULT CItem_Manager::Remove_Item_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex)
{
	if (!IsValid_Inven(eIndex, iIndex))
		return ITEM_RESULT::RESULT_INVALID;

	m_bIsChange = true;
	return m_vecArray_Inven[_uint(eIndex)]->Remove_Item(iIndex);
}

CPlayer::WEAPON_TYPE CItem_Manager::Get_Weapon_Model_Index()
{
	INVEN_ARRAY_TYPE eArray = m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0) + (m_iWeapon_Select * 2)]->eType;
	_int iIndex = m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0) + (m_iWeapon_Select * 2)]->iIndex;

	if (!IsValid_Inven(eArray, iIndex))
		return CPlayer::WEAPON_TYPE::WEP_END;

	const ITEM* pItem = m_vecArray_Inven[_int(eArray)]->Get_Item_Info(iIndex);

	if (pItem == nullptr)
		return CPlayer::WEAPON_TYPE::WEP_END;
	else if (pItem->iItem_Index == 1)
		return CPlayer::WEAPON_TYPE::WEP_RAPIER;
	else if (pItem->iItem_Index == 35)
		return CPlayer::WEAPON_TYPE::WEP_FLAME;
	else if (pItem->iItem_Index == 56)
		return CPlayer::WEAPON_TYPE::WEP_SCISSOR;

	return CPlayer::WEAPON_TYPE::WEP_END;
}

list<SPECIAL_ITEM>& CItem_Manager::Get_LastFrame_UsingItem_Info()
{
	return m_LastFrame_UsingItem;
}

SPECIAL_ITEM CItem_Manager::Check_Item_Special_Enum(_int iIndex)
{
	// 이건 하드코딩이 나을 듯 
	if (iIndex == _int(SPECIAL_ITEM::SP_PULSE_BATTERY))
		return SPECIAL_ITEM::SP_PULSE_BATTERY;
	else if (iIndex == _int(SPECIAL_ITEM::SP_DUSTCAT))
		return SPECIAL_ITEM::SP_DUSTCAT;
	else if (iIndex == _int(SPECIAL_ITEM::SP_GRINDER))
		return SPECIAL_ITEM::SP_GRINDER;
	else if (iIndex == _int(SPECIAL_ITEM::SP_LAMP))
		return SPECIAL_ITEM::SP_LAMP;
	else if (iIndex == _int(SPECIAL_ITEM::SP_TELEPOT))
		return SPECIAL_ITEM::SP_TELEPOT;
	else if (iIndex == _int(SPECIAL_ITEM::SP_RESISTANCE))
		return SPECIAL_ITEM::SP_RESISTANCE;
	else if (iIndex == _int(SPECIAL_ITEM::SP_PURIFICATION))
		return SPECIAL_ITEM::SP_PURIFICATION;
	else if (iIndex == _int(SPECIAL_ITEM::SP_DEAD))
		return SPECIAL_ITEM::SP_DEAD;
	else if (iIndex == _int(SPECIAL_ITEM::SP_GRANADE))
		return SPECIAL_ITEM::SP_GRANADE;
	else if (iIndex == _int(SPECIAL_ITEM::SP_THERMITE))
		return SPECIAL_ITEM::SP_THERMITE;
	else if (iIndex == _int(SPECIAL_ITEM::SP_THROW_BATTERY))
		return SPECIAL_ITEM::SP_THROW_BATTERY;

	return SPECIAL_ITEM::SP_END;
}

void CItem_Manager::Set_Item_Funtion(_int iItem_Index)
{
	SPECIAL_ITEM eNow = Check_Item_Special_Enum(iItem_Index);
	if (eNow != SPECIAL_ITEM::SP_END)
		m_LastFrame_UsingItem.push_back(eNow);
}

_bool CItem_Manager::Get_CanSwitch_Weapon()
{
	if (m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0)]->eType == INVEN_ARRAY_TYPE::TYPE_END)
		return false;
	if (m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_1)]->eType == INVEN_ARRAY_TYPE::TYPE_END)
		return false;

	return true;
}


_bool CItem_Manager::Use_Potion()
{
	if (m_iNow_Potion_Count <= 0)
		return false;

	m_bIsChange = true;
	--m_iNow_Potion_Count;
	for (_int i = 0; i < 5; ++i)
		if (m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_USING_BASIC)]->Get_Item_Info(i)->iItem_Index == _int(SPECIAL_ITEM::SP_PULSE_BATTERY))
			m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_USING_BASIC)]->Get_Item_Info(i)->iCount = m_iNow_Potion_Count;

	return true;
}

void CItem_Manager::Set_ItemAction(EQUIP_SLOT eSlot, _Vec2 vPos, _Vec2 vSize, _bool bMainTainPage)
{
	if (eSlot == EQUIP_SLOT::EQUIP_WEAPON_HANDLE_0)
		eSlot = EQUIP_SLOT::EQUIP_WEAPON_BLADE_0;
	else if (eSlot == EQUIP_SLOT::EQUIP_WEAPON_HANDLE_1)
		eSlot = EQUIP_SLOT::EQUIP_WEAPON_BLADE_1;

	_int iArray = _int(eSlot);

	if (!bMainTainPage)
		Reset_ItemAction();

	m_bItemAction_Active = true;
	m_eNow_ActionSlot = eSlot;
	m_eNow_ActionArray = INVEN_ARRAY_TYPE::TYPE_END;
	m_iArray_Index = -1;

	vector<ITEM_FUNC> vecFunc;

	if ((iArray >= _int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0)) && (iArray <= _int(EQUIP_SLOT::EQUIP_DEFENCE_RAINER)))
	{
		if (m_vecEquip_ItemInfo[iArray]->eType == INVEN_ARRAY_TYPE::TYPE_END)
			vecFunc.push_back(ITEM_FUNC::FUNC_TO_INVEN);
		else
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
	}
	else if (m_iActionPopup_Page == 2)
	{
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_TOP_0);
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_TOP_1);
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_TOP_2);
	}
	else if (m_iActionPopup_Page == 3)
	{
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BOTTOM_0);
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BOTTOM_1);
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BOTTOM_2);
	}
	else if (m_iActionPopup_Page == 4)
	{
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BAG_0);
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BAG_1);
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BAG_2);
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BAG_3);
	}
	else if ((iArray >= _int(EQUIP_SLOT::EQUIP_USING_TOP_0)) && (iArray <= _int(EQUIP_SLOT::EQUIP_USING_TOP_2)))
	{
		if (m_vecEquip_ItemInfo[iArray]->eType == INVEN_ARRAY_TYPE::TYPE_END)
			vecFunc.push_back(ITEM_FUNC::FUNC_TO_INVEN);
		else
		{
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BOTTOM);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BAG);
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
		}
	}
	else if ((iArray >= _int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0)) && (iArray <= _int(EQUIP_SLOT::EQUIP_USING_BOTTOM_2)))
	{
		if (m_vecEquip_ItemInfo[iArray]->eType == INVEN_ARRAY_TYPE::TYPE_END)
			vecFunc.push_back(ITEM_FUNC::FUNC_TO_INVEN);
		else
		{
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_TOP);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BAG);
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
		}
	}
	else if ((iArray >= _int(EQUIP_SLOT::EQUIP_USING_BAG_0)) && (iArray <= _int(EQUIP_SLOT::EQUIP_USING_BAG_3)))
	{
		if (m_vecEquip_ItemInfo[iArray]->eType == INVEN_ARRAY_TYPE::TYPE_END)
			vecFunc.push_back(ITEM_FUNC::FUNC_TO_INVEN);
		else
		{
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_TOP);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BOTTOM);
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
		}
	}
	else if ((iArray >= _int(EQUIP_SLOT::EQUIP_HEAD_0)) && (iArray <= _int(EQUIP_SLOT::EQUIP_CLOTHES)))
	{
		if (m_vecEquip_ItemInfo[iArray]->iIndex == -1)
			vecFunc.push_back(ITEM_FUNC::FUNC_TO_INVEN);
		else
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
	}

	for (_int i = (_int)vecFunc.size(); i < 4; ++i)
		vecFunc.push_back(ITEM_FUNC::FUNC_END);

	GET_GAMEINTERFACE->Show_ItemAction(vPos, vSize, vecFunc[0], vecFunc[1], vecFunc[2], vecFunc[3]);

	vecFunc.clear();
}

void CItem_Manager::Set_ItemAction(INVEN_ARRAY_TYPE eType, _int iIndex, _Vec2 vPos, _Vec2 vSize, _bool bMainTainPage)
{
	_int iArray = _int(eType);

	if (m_vecArray_Inven[iArray]->vecItemInfo[iIndex]->iItem_Index == -1)
		return;

	if (!bMainTainPage)
		Reset_ItemAction();

	m_bItemAction_Active = true;
	m_eNow_ActionSlot = EQUIP_SLOT::EQUIP_END;
	m_eNow_ActionArray = eType;
	if (eType == INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_HANDLE)
		eType = INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE;
	else if (eType == INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_HANDLE)
		eType = INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_BLADE;

	m_iArray_Index = iIndex;

	vector<ITEM_FUNC> vecFunc;

	if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_USING_BASIC)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_USING_RARE_ERGO)))
	{
		if (m_iActionPopup_Page == 0)
		{
			if (iArray != _int(INVEN_ARRAY_TYPE::TYPE_USING_THROW))
				vecFunc.push_back(ITEM_FUNC::FUNC_USING);
			
			if (iArray < _int(INVEN_ARRAY_TYPE::TYPE_USING_ERGO))
			{
				
				if (m_vecArray_Inven[iArray]->vecItemInfo[iIndex]->eSlot != EQUIP_SLOT::EQUIP_END)
					vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
				else 
					vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP);
			}

			vecFunc.push_back(ITEM_FUNC::FUNC_TO_EQUIP);

			vecFunc.push_back(ITEM_FUNC::FUNC_DELETE);
		}
		if (m_iActionPopup_Page == 1)
		{
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_TOP);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BOTTOM);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BAG);
			if (m_vecArray_Inven[iArray]->vecItemInfo[iIndex]->eSlot != EQUIP_SLOT::EQUIP_END)
				vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
		}
		if (m_iActionPopup_Page == 2)
		{
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_TOP_0);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_TOP_1);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_TOP_2);
		}
		if (m_iActionPopup_Page == 3)
		{
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BOTTOM_0);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BOTTOM_1);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BOTTOM_2);
		}
		if (m_iActionPopup_Page == 4)
		{
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BAG_0);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BAG_1);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BAG_2);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_BAG_3);
		}
	}
	else if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_INGRADIENT_GROW)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_FUNTIONAL_COMMON)))
	{
		vecFunc.push_back(ITEM_FUNC::FUNC_USING);
		vecFunc.push_back(ITEM_FUNC::FUNC_DELETE);
	}
	else if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_HANDLE)))
	{
		if (m_vecArray_Inven[iArray]->vecItemInfo[iIndex]->eSlot != EQUIP_SLOT::EQUIP_END)
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
		else
		{
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_WEAPON_FIRST);
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP_WEAPON_SECOND);
		}
		vecFunc.push_back(ITEM_FUNC::FUNC_TO_EQUIP);
	}
	else if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_BLADE_PART)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_HANDEL_PART)))
	{
		vecFunc.push_back(ITEM_FUNC::FUNC_USING);
		vecFunc.push_back(ITEM_FUNC::FUNC_DELETE);
	}
	else if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_REASON_ARM)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_REASON_ARM)))
	{

		if (m_vecArray_Inven[iArray]->vecItemInfo[iIndex]->eSlot != EQUIP_SLOT::EQUIP_END)
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
		else
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP);
		vecFunc.push_back(ITEM_FUNC::FUNC_TO_EQUIP);
	}
	else if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_DEFENCE_FRAME)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_DEFENCE_RAINER)))
	{

		if (m_vecArray_Inven[iArray]->vecItemInfo[iIndex]->eSlot != EQUIP_SLOT::EQUIP_END)
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
		else
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP);
		vecFunc.push_back(ITEM_FUNC::FUNC_TO_EQUIP);
	}
	else if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_AMULET_COMMON)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_AMULET_COMMON)))
	{

		if (m_vecArray_Inven[iArray]->vecItemInfo[iIndex]->eSlot != EQUIP_SLOT::EQUIP_END)
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
		else
			vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP);
		vecFunc.push_back(ITEM_FUNC::FUNC_TO_EQUIP);
	}
	else if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_COUSTUME_CLOTHES)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_COUSTUME_ACC)))
	{
		// 이건 나중에 
	}
	else if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_GEUSTURE_COMMON)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_GEUSTURE_COMMON)))
	{
		// 이건 나중에 
	}
	else if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_COLLECTION_NEWS)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_COLLECTION_MEMO)))
	{
		// 이건 나중에 
	}

	for (_int i = (_int)vecFunc.size(); i < 4; ++i)
		vecFunc.push_back(ITEM_FUNC::FUNC_END);

	GET_GAMEINTERFACE->Show_ItemAction(vPos, vSize, vecFunc[0], vecFunc[1], vecFunc[2], vecFunc[3]);

	vecFunc.clear();
}

void CItem_Manager::Reset_ItemAction()
{
	m_bItemAction_Active = false;
	m_eNow_ActionSlot = EQUIP_SLOT::EQUIP_END;
	m_eNow_ActionArray = INVEN_ARRAY_TYPE::TYPE_END;
	m_iArray_Index = -1;
	m_iActionPopup_Page = 0;
}

_bool CItem_Manager::IsReset_ItemAction()
{
	if (m_iActionPopup_Page == 0)
		return true;


	return false;
}

ITEM_RESULT CItem_Manager::Operate_ItemAction(ITEM_FUNC eFunc, _Vec2 vPos, _Vec2 vSize)
{
	_int iFunc = _int(eFunc);

	if (iFunc == _int(ITEM_FUNC::FUNC_USING))
	{
		if (m_eNow_ActionArray != INVEN_ARRAY_TYPE::TYPE_END)
			UseItem_Inven(m_eNow_ActionArray, m_iArray_Index, 1);
		else
			UseItem_Equip(m_eNow_ActionSlot, 1);
	}
	else if ((iFunc >= _int(ITEM_FUNC::FUNC_TO_INVEN)) && (iFunc <= _int(ITEM_FUNC::FUNC_TO_EQUIP)))
	{
		GET_GAMEINTERFACE->SwicthPage(UIPAGE::PAGE_INVEN, UIPAGE::PAGE_EQUIP);
		Reset_ItemAction();
	}
	else if (iFunc == _int(ITEM_FUNC::FUNC_EQUIP))
	{
		Operate_EquipAction(vPos, vSize);
	}
	else if (iFunc == _int(ITEM_FUNC::FUNC_UNEQUIP))
	{
		if (m_eNow_ActionSlot != EQUIP_SLOT::EQUIP_END)
			UnEquipItem_Inven(m_eNow_ActionSlot);
		else 
			UnEquipItem_Inven(m_vecArray_Inven[_int(m_eNow_ActionArray)]->vecItemInfo[m_iArray_Index]->eSlot);
		Reset_ItemAction();
	}
	else if (iFunc == _int(ITEM_FUNC::FUNC_DELETE))
	{
		if (m_eNow_ActionArray != INVEN_ARRAY_TYPE::TYPE_END)
			Remove_Item_Inven(m_eNow_ActionArray, m_iArray_Index);
		else 
			Remove_Item_Inven(m_vecEquip_ItemInfo[_int(m_eNow_ActionSlot)]->eType, m_vecEquip_ItemInfo[_int(m_eNow_ActionSlot)]->iIndex);
	}
	else if (iFunc == _int(ITEM_FUNC::FUNC_EQUIP_TOP))
	{
		m_iActionPopup_Page = 2;
		if (m_eNow_ActionArray != INVEN_ARRAY_TYPE::TYPE_END)
			Set_ItemAction(m_eNow_ActionArray, m_iArray_Index, vPos, vSize, true);
		else 
			Set_ItemAction(m_eNow_ActionSlot, vPos, vSize, true);
	}
	else if (iFunc == _int(ITEM_FUNC::FUNC_EQUIP_BOTTOM))
	{
		m_iActionPopup_Page = 3;
		if (m_eNow_ActionArray != INVEN_ARRAY_TYPE::TYPE_END)
			Set_ItemAction(m_eNow_ActionArray, m_iArray_Index, vPos, vSize, true);
		else
			Set_ItemAction(m_eNow_ActionSlot, vPos, vSize, true);
	}
	else if (iFunc == _int(ITEM_FUNC::FUNC_EQUIP_BAG))
	{
		m_iActionPopup_Page = 4;
		if (m_eNow_ActionArray != INVEN_ARRAY_TYPE::TYPE_END)
			Set_ItemAction(m_eNow_ActionArray, m_iArray_Index, vPos, vSize, true);
		else
			Set_ItemAction(m_eNow_ActionSlot, vPos, vSize, true);
	}
	else if (iFunc == _int(ITEM_FUNC::FUNC_EQUIP_WEAPON_FIRST))
	{
		if (m_eNow_ActionArray != INVEN_ARRAY_TYPE::TYPE_END)
			EquipItem_Inven(m_eNow_ActionArray, EQUIP_SLOT::EQUIP_WEAPON_BLADE_0, m_iArray_Index);
		else
			Move_EquipItem(m_eNow_ActionSlot, EQUIP_SLOT::EQUIP_WEAPON_BLADE_0);
	}
	else if (iFunc == _int(ITEM_FUNC::FUNC_EQUIP_WEAPON_SECOND))
	{
		if (m_eNow_ActionArray != INVEN_ARRAY_TYPE::TYPE_END)
			EquipItem_Inven(m_eNow_ActionArray, EQUIP_SLOT::EQUIP_WEAPON_BLADE_1, m_iArray_Index);
		else
			Move_EquipItem(m_eNow_ActionSlot, EQUIP_SLOT::EQUIP_WEAPON_BLADE_1);
	}
	else if ((iFunc >= _int(ITEM_FUNC::FUNC_EQUIP_TOP_0)) && (iFunc <= _int(ITEM_FUNC::FUNC_EQUIP_TOP_2)))
	{
		if (m_eNow_ActionArray != INVEN_ARRAY_TYPE::TYPE_END)
			EquipItem_Inven(m_eNow_ActionArray, EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_TOP_0) + (iFunc - _int(ITEM_FUNC::FUNC_EQUIP_TOP_0))), m_iArray_Index);
		else
		{
			EQUIP_SLOT eDest = EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_TOP_0) + (iFunc - _int(ITEM_FUNC::FUNC_EQUIP_TOP_0)));
			Move_EquipItem(m_eNow_ActionSlot, eDest);
		}
			
		
		Reset_ItemAction();
	}
	else if ((iFunc >= _int(ITEM_FUNC::FUNC_EQUIP_BOTTOM_0)) && (iFunc <= _int(ITEM_FUNC::FUNC_EQUIP_BOTTOM_2)))
	{
		if (m_eNow_ActionArray != INVEN_ARRAY_TYPE::TYPE_END)
			EquipItem_Inven(m_eNow_ActionArray, EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0) + (iFunc - _int(ITEM_FUNC::FUNC_EQUIP_BOTTOM_0))), m_iArray_Index);
		else
		{
			EQUIP_SLOT eDest = EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0) + (iFunc - _int(ITEM_FUNC::FUNC_EQUIP_BOTTOM_0)));
			Move_EquipItem(m_eNow_ActionSlot, eDest);
		}
		Reset_ItemAction();
	}
	else if ((iFunc >= _int(ITEM_FUNC::FUNC_EQUIP_BAG_0)) && (iFunc <= _int(ITEM_FUNC::FUNC_EQUIP_BAG_3)))
	{
		if (m_eNow_ActionArray != INVEN_ARRAY_TYPE::TYPE_END)
			EquipItem_Inven(m_eNow_ActionArray, EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_BAG_0) + (iFunc - _int(ITEM_FUNC::FUNC_EQUIP_BAG_0))), m_iArray_Index);
		else
		{
			EQUIP_SLOT eDest = EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_BAG_0) + (iFunc - _int(ITEM_FUNC::FUNC_EQUIP_BAG_0)));
			Move_EquipItem(m_eNow_ActionSlot, eDest);
		}
		Reset_ItemAction();
	}


	return ITEM_RESULT::RESULT_SUCCESS;
}

ITEM_RESULT CItem_Manager::Operate_EquipAction(_Vec2 vPos, _Vec2 vSize)
{
	if (_int(m_eNow_ActionArray) <= _int(INVEN_ARRAY_TYPE::TYPE_USING_THROW))
	{
		m_iActionPopup_Page = 1;
		Set_ItemAction(m_eNow_ActionArray, m_iArray_Index, vPos, vSize, true);
		return ITEM_RESULT();
	}
	else if (m_eNow_ActionArray == INVEN_ARRAY_TYPE::TYPE_REASON_ARM)
		EquipItem_Inven(INVEN_ARRAY_TYPE::TYPE_REASON_ARM, EQUIP_SLOT::EQUIP_RESION_ARM, m_iArray_Index);
	else if (m_eNow_ActionArray == INVEN_ARRAY_TYPE::TYPE_DEFENCE_FRAME)
		EquipItem_Inven(INVEN_ARRAY_TYPE::TYPE_DEFENCE_FRAME, EQUIP_SLOT::EQUIP_DEFENCE_FRAME, m_iArray_Index);
	else if (m_eNow_ActionArray == INVEN_ARRAY_TYPE::TYPE_DEFENCE_CONVERTOR)
		EquipItem_Inven(INVEN_ARRAY_TYPE::TYPE_DEFENCE_CONVERTOR, EQUIP_SLOT::EQUIP_DEFENCE_CONVERTOR, m_iArray_Index);
	else if (m_eNow_ActionArray == INVEN_ARRAY_TYPE::TYPE_DEFENCE_CARTRIGE)
		EquipItem_Inven(INVEN_ARRAY_TYPE::TYPE_DEFENCE_CARTRIGE, EQUIP_SLOT::EQUIP_DEFENCE_CARTRIGE, m_iArray_Index);
	else if (m_eNow_ActionArray == INVEN_ARRAY_TYPE::TYPE_DEFENCE_RAINER)
		EquipItem_Inven(INVEN_ARRAY_TYPE::TYPE_DEFENCE_RAINER, EQUIP_SLOT::EQUIP_DEFENCE_RAINER, m_iArray_Index);
	else if (m_eNow_ActionArray == INVEN_ARRAY_TYPE::TYPE_AMULET_COMMON)
	{
		if (Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_AMULET_0) == nullptr)
			EquipItem_Inven(INVEN_ARRAY_TYPE::TYPE_AMULET_COMMON, EQUIP_SLOT::EQUIP_AMULET_0, m_iArray_Index);
		else if (Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_AMULET_1) == nullptr)
			EquipItem_Inven(INVEN_ARRAY_TYPE::TYPE_AMULET_COMMON, EQUIP_SLOT::EQUIP_AMULET_1, m_iArray_Index);
		else 
			EquipItem_Inven(INVEN_ARRAY_TYPE::TYPE_AMULET_COMMON, EQUIP_SLOT::EQUIP_AMULET_0, m_iArray_Index);
	}
	else if (m_eNow_ActionArray == INVEN_ARRAY_TYPE::TYPE_COUSTUME_CLOTHES)
	{
		// 이건 나중에 
	}
	else if (m_eNow_ActionArray == INVEN_ARRAY_TYPE::TYPE_COUSTUME_MASK)
	{
		// 이건 나중에 
	}
	else if (m_eNow_ActionArray == INVEN_ARRAY_TYPE::TYPE_COUSTUME_ACC)
	{
		// 이건 나중에 
	}

	Reset_ItemAction();

	return ITEM_RESULT();
}

void CItem_Manager::Adjust_Spec()
{
	CPlayer::STAT_INFO* pAdjust = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust();

	_int iAmulet[2] = { 0, };
	_float fDefence[4] = { 0.f, };

	if (Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_AMULET_0) != nullptr)
		iAmulet[0] = Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_AMULET_0)->iItem_Index;

	if (Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_AMULET_1) != nullptr)
		iAmulet[1] = Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_AMULET_1)->iItem_Index;

	if (Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_DEFENCE_FRAME) != nullptr)
		fDefence[0] = Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_DEFENCE_FRAME)->vecDamege[0];

	if (Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_DEFENCE_CONVERTOR) != nullptr)
		fDefence[1] = Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_DEFENCE_CONVERTOR)->vecDefence[0];

	if (Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_DEFENCE_CARTRIGE) != nullptr)
		fDefence[2] = Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_DEFENCE_CARTRIGE)->vecDefence[1];

	if (Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_DEFENCE_RAINER) != nullptr)
		fDefence[3] = Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_DEFENCE_RAINER)->vecDefence[2];

	if ((iAmulet[0] == 87) || (iAmulet[1] == 87)) // 꿰뚫는 증오의 아뮬렛 
		pAdjust->bDebuff_Fire_Ignore = true;
	else 
		pAdjust->bDebuff_Fire_Ignore = false;

	if ((iAmulet[0] == 88) || (iAmulet[1] == 88))
		pAdjust->vGauge_Stamina.z = 200.f;
	else
		pAdjust->vGauge_Stamina.z = 0.f;

	if ((iAmulet[0] == 90) || (iAmulet[1] == 90))
		pAdjust->bDebuff_Electric_Ignore = true;
	else
		pAdjust->bDebuff_Electric_Ignore = false;


	if ((iAmulet[0] == 94) || (iAmulet[1] == 94))
		pAdjust->bDebuff_Acid_Ignore = true;
	else
		pAdjust->bDebuff_Acid_Ignore = false;

	if ((iAmulet[0] == 101) || (iAmulet[1] == 101))
		pAdjust->vGauge_Hp.z = 200.f;
	else
		pAdjust->vGauge_Hp.z = 0.f;

	if ((iAmulet[0] == 111) || (iAmulet[1] == 111))
		pAdjust->fHeal = 10.f;
	else
		pAdjust->fHeal = 0.f;

	if ((iAmulet[0] == 113) || (iAmulet[1] == 113))
		pAdjust->iStat_Defence = 30;
	else
		pAdjust->iStat_Defence = 0;

	pAdjust->iStat_Defence += _int(fDefence[0]);
	pAdjust->fResist_Fire = fDefence[1];
	pAdjust->bDebuff_Electric_Ignore = fDefence[2];
	pAdjust->bDebuff_Acid_Ignore = fDefence[3];
}

void CItem_Manager::Buy_ShopItem(_int iIndex, _int iCount)
{
	if (m_vecItem_BasicSpec[iIndex]->bIsAvailable_Shop == false)
		return;

	if (iCount <= 0)
		return;

	_int iNowShop = -1;

	for (auto& iter : m_vecShop_Item)
	{
		++iNowShop;
		if (iter->iIndex == iIndex)
			break;
	}

	if ((iNowShop == -1) || (iNowShop >= m_vecShop_Item.size()))
		return;

	if (m_vecShop_Item[iNowShop]->iCount < iCount)
		return;

	_int iErgo = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat().iErgo;
	iErgo += GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust()->iErgo;

	if (iErgo < m_vecItem_BasicSpec[iIndex]->iPrice * iCount)
		return;

	m_vecShop_Item[iNowShop]->iCount -= iCount;
	AddNewItem_Inven(iIndex, iCount);

	_wstring strItem = m_vecItem_BasicSpec[iIndex]->strName;
	strItem += TEXT(" -> 구입수량 : ");
	strItem += to_wstring(iCount);

	GET_GAMEINTERFACE->Input_Achievment_Data(0, m_vecItem_BasicSpec[iIndex]->iPrice * iCount);
	GET_GAMEINTERFACE->Show_Popup(TEXT("구매 성공"), strItem);
	GET_GAMEINTERFACE->Input_Achievment_Data(1, 1);
	GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust()->iErgo -= m_vecItem_BasicSpec[iIndex]->iPrice * iCount;
}

void CItem_Manager::Sell_ShopItem(INVEN_ARRAY_TYPE eType, _int iIndex, _int iCount)
{
	if (!IsValid_Inven(eType, iIndex))
		return;

	if (m_vecArray_Inven[_int(eType)]->vecItemInfo[iIndex]->bIsAvailable_Shop == false)
		return;

	_int iCountNow = m_vecArray_Inven[_int(eType)]->vecItemInfo[iIndex]->iCount;

	if (iCount > iCountNow)
		return;

	GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust()->iErgo += m_vecArray_Inven[_int(eType)]->vecItemInfo[iIndex]->iPrice * iCount;

	m_vecArray_Inven[_int(eType)]->Use_Item(iIndex, iCount);

	_wstring strItem = m_vecArray_Inven[_int(eType)]->vecItemInfo[iIndex]->strName;
	strItem += TEXT(" -> 판매수량 : ");
	strItem += to_wstring(iCount);
	GET_GAMEINTERFACE->Input_Achievment_Data(2, 1);
	GET_GAMEINTERFACE->Show_Popup(TEXT("판매 성공"), strItem);
}

void CItem_Manager::ChestItem_To_Inven(_int iIndex, _int iCount)
{
	if (m_mapChest_Item.find(iIndex) == m_mapChest_Item.end())
		return;

	ITEM* pNow = m_mapChest_Item.find(iIndex)->second;

	if (pNow == nullptr)
		return;

	if (pNow->iCount < iCount)
		return;

	pNow->iCount -= iCount;
	AddNewItem_Inven(iIndex, iCount);

	_wstring strItem = pNow->strName;
	strItem += TEXT(" -> 꺼낸 수량 : ");
	strItem += to_wstring(iCount);

	GET_GAMEINTERFACE->Show_Popup(TEXT("보관 아이템 꺼내기"), strItem);

	if (pNow->iCount <= 0)
	{
		Safe_Delete(pNow);
		m_mapChest_Item.erase(m_mapChest_Item.find(iIndex));
	}

}

_int CItem_Manager::InvenItem_To_Chest(INVEN_ARRAY_TYPE eType, _int iIndex, _int iCount)
{
	if (!IsValid_Inven(eType, iIndex))
		return -1;

	if (m_vecArray_Inven[_int(eType)]->vecItemInfo[iIndex]->bIsAvailable_Chest == false)
		return -1;

	_int iCountNow = m_vecArray_Inven[_int(eType)]->vecItemInfo[iIndex]->iCount;

	if (iCount > iCountNow)
		return -1;

	if (m_vecArray_Inven[_int(eType)]->vecItemInfo[iIndex]->bStack == true)
	{
		_bool bInput = false;

		for (auto& pair : m_mapChest_Item)
		{
			if (pair.second->iItem_Index == m_vecArray_Inven[_int(eType)]->vecItemInfo[iIndex]->iItem_Index)
			{
				pair.second->iCount += iCount;
				bInput = true;
				break;
			}
		}

		if (bInput == false)
		{
			ITEM* pNew = new ITEM;

			*(pNew) = *(m_vecArray_Inven[_int(eType)]->vecItemInfo[iIndex]);
			(pNew)->iCount = iCount;
			m_mapChest_Item.insert({ m_iNext_Chest_Key, pNew });
			++m_iNext_Chest_Key;
		}
	}
	else
	{
		ITEM* pNew = new ITEM;

		*(pNew) = *(m_vecArray_Inven[_int(eType)]->vecItemInfo[iIndex]);
		m_mapChest_Item.insert({ m_iNext_Chest_Key, pNew });
		m_vecArray_Inven[_int(eType)]->Remove_Item(iIndex);
		++m_iNext_Chest_Key;
	}

	_wstring strItem = m_vecArray_Inven[_int(eType)]->vecItemInfo[iIndex]->strName;
	strItem += TEXT(" -> 보관 수량 : ");
	strItem += to_wstring(iCount);

	GET_GAMEINTERFACE->Show_Popup(TEXT("아이템 보관"), strItem);

	m_vecArray_Inven[_int(eType)]->Use_Item(iIndex, iCount);

	return m_iNext_Chest_Key;
}

HRESULT CItem_Manager::Initialize_Item()
{
	m_vecArray_Inven.resize(_uint(INVEN_ARRAY_TYPE::TYPE_END));
	m_vecItem_InvenSlotIndex.resize(_uint(ITEM_TYPE::ITEMTYPE_END));

	// 아이템 기본 스펙 저장 
	vector<vector<_wstring>> vecBuffer_Spec;
	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Item_Spec_Data.csv", &vecBuffer_Spec)))
		return E_FAIL;

	_uint iStartRow = 2;

	for (auto& iter : vecBuffer_Spec)
	{
		if (iStartRow > 0)
		{
			--iStartRow;
			continue;
		}

		ITEM* pNew = new ITEM;

		pNew->iItem_Index = stoi(iter[0]);
		pNew->strName = iter[1];
		pNew->iTexture_Index = stoi(iter[2]);
		pNew->eType_Index = ITEM_TYPE(stoi(iter[4]));
		pNew->bStack = stoi(iter[6]);
		pNew->bModule_Weapon = stoi(iter[7]);
		pNew->bIsHandele = stoi(iter[8]);
		pNew->iOtherPart_Index = stoi(iter[9]);
		pNew->fDurable_Now = stof(iter[10]);
		pNew->fDurable_Max = stof(iter[11]);
		pNew->fWeight = stof(iter[12]);

		for (_uint i = 0; i < _uint(DAMEGE_TYPE::DAMEGE_END); ++i)
			pNew->vecDamege[i] = stof(iter[13 + i]);

		for (_uint i = 0; i < _uint(DEFENCE_TYPE::DEFENCE_END); ++i)
			pNew->vecDefence[i] = stof(iter[17 + i]);

		pNew->strAttack_Type = iter[26];
		pNew->fType_Damege = stof(iter[27]);
		pNew->fType_Damege_Fatal_Ratio = stof(iter[28]);
		pNew->fPable_Charge = stof(iter[29]);
		pNew->fPulse_Charge = stof(iter[30]);
		pNew->fGuard_Damege_Reduce = stof(iter[31]);
		pNew->strFable_Art_Name = iter[32];
		pNew->iFable_Art_Cost = stoi(iter[33]);
		pNew->vecAblity_Grade[_uint(ABLITY_TYPE::ABLITY_POWER)] = iter[34];
		pNew->vecAblity_Grade[_uint(ABLITY_TYPE::ABLITY_SKILL)] = iter[35];
		pNew->vecAblity_Grade[_uint(ABLITY_TYPE::ABLITY_EVOUTION)] = iter[36];
		pNew->strSlash_Grade = iter[37];
		pNew->strPirce_Grade = iter[38];

		pNew->strFabel_Desc = iter[39];
		pNew->strItem_Desc = iter[40];

		m_vecItem_BasicSpec.push_back(pNew);
	}

	// 아이템 타입 별 들어갈 수 있는 인벤 슬롯
	vector<vector<_wstring>> vecBuffer_InvenSlot;
	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Item_Inven_Array_Data.csv", &vecBuffer_InvenSlot)))
		return E_FAIL;

	iStartRow = 2;

	for (_uint i = iStartRow; i < _uint(ITEM_TYPE::ITEMTYPE_END) + iStartRow; ++i)
		m_vecItem_InvenSlotIndex[i - iStartRow] = stoi(vecBuffer_InvenSlot[i][2]);

	// 장비창 타입 별 연결되는 인벤 타입 정리
	vector<vector<_wstring>> vecBuffer_EquipSlot;
	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Item_Equip_Slot_Data.csv", &vecBuffer_EquipSlot)))
		return E_FAIL;

	iStartRow = 1;

	for (_uint i = iStartRow; i < _uint(INVEN_ARRAY_TYPE::TYPE_END) + iStartRow; ++i)
	{
		m_vecArray_Inven[i - iStartRow] = new ARRAY;
		m_vecArray_Inven[i - iStartRow]->strInven_Array_Name = vecBuffer_EquipSlot[i][2];
	}



	for (_uint i = 3; i < _uint(EQUIP_SLOT::EQUIP_END) + 3; ++i)
	{
		EQUIP* pNew = new EQUIP;
		m_vecEquip_ItemInfo.push_back(pNew);

		for (_uint j = iStartRow; j < _uint(INVEN_ARRAY_TYPE::TYPE_END) + iStartRow; ++j)
			m_vecEquip_ItemInfo.back()->vecValid_InvenArray[j - iStartRow] = stoi(vecBuffer_EquipSlot[j][i]);
	}

	// 상점 초기 세팅
	vector<vector<_wstring>> vecBuffer_Shop;
	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Shop_Info.csv", &vecBuffer_Shop)))
		return E_FAIL;

	iStartRow = 1;

	for (_uint i = iStartRow; i < vecBuffer_Shop.size(); ++i)
	{
		if (stoi(vecBuffer_Shop[i][2]) == -1)
		{
			m_vecItem_BasicSpec[stoi(vecBuffer_Shop[i][0])]->bIsAvailable_Chest = true;
			continue;
		}

		SHOP* pNew = new SHOP;

		pNew->iIndex = stoi(vecBuffer_Shop[i][0]);

		m_vecItem_BasicSpec[pNew->iIndex]->bIsAvailable_Shop = true;
		m_vecItem_BasicSpec[pNew->iIndex]->bIsAvailable_Chest = true;
		
		pNew->iCount = stoi(vecBuffer_Shop[i][2]);
		pNew->iPrice_Buy = stoi(vecBuffer_Shop[i][3]);

		m_vecItem_BasicSpec[pNew->iIndex]->iPrice = pNew->iPrice_Buy;

		m_vecShop_Item.push_back(pNew);
	}

	// 플레이어 아이템 초기 세팅
	vector<vector<_wstring>> vecBuffer_ItemInitialze;
	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Item_Initialize_Data.csv", &vecBuffer_ItemInitialze)))
		return E_FAIL;

	_int iBefore = -1;
	iStartRow = 2;
	_bool bInputItem = true;
	_bool bAssembleWeapon = false;

	for (auto& iter : vecBuffer_ItemInitialze)
	{
		if (iStartRow > 0)
		{
			--iStartRow;
			continue;
		}

		if (bInputItem)
		{
			if (iBefore < stoi(iter[0]))
			{
				AddNewItem_Inven(stoi(iter[2]), stoi(iter[3]));

				if (stoi(iter[2]) == _int(SPECIAL_ITEM::SP_PULSE_BATTERY))
					m_iNow_Potion_Count = stoi(iter[3]);

				++iBefore;
				continue;
			}
			else
				bInputItem = false;
		}

		if (m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_BLADE_PART)]->vecItemInfo[0]->iItem_Index != -1)
			Assemble_Blade_Handle(0, 0);
		else if (m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_BLADE_PART)]->vecItemInfo[1]->iItem_Index != -1)
			Assemble_Blade_Handle(1, 1);

		if (stoi(iter[2]) != -1)
			EquipItem_Inven(INVEN_ARRAY_TYPE(stoi(iter[2])), EQUIP_SLOT(stoi(iter[0])), stoi(iter[3]));
	}

	m_iWeapon_Select = 0;

	return S_OK;
}

CItem_Manager* CItem_Manager::Create(CGameInstance* pGameInstance)
{
	CItem_Manager* pInstance = new CItem_Manager(pGameInstance);

	if (FAILED(pInstance->Initialize_Item()))
	{
		MSG_BOX(TEXT("Failed to Created : CItem_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Manager::Free()
{
	__super::Free();

	for (auto& iter : m_vecItem_BasicSpec)
		Safe_Delete(iter);
	for (auto& iter : m_vecArray_Inven)
		Safe_Delete(iter);
	for (auto& iter : m_vecEquip_ItemInfo)
		Safe_Delete(iter);
	for (auto& iter : m_vecShop_Item)
		Safe_Delete(iter);
	for (auto& iter : m_mapChest_Item)
		Safe_Delete(iter.second);

	m_vecItem_BasicSpec.clear();
	m_vecItem_InvenSlotIndex.clear();
	m_vecArray_Inven.clear();
	m_vecEquip_ItemInfo.clear();
	m_vecShop_Item.clear();
	m_mapChest_Item.clear();

	m_LastFrame_UsingItem.clear();

	Safe_Release(m_pGameInstance);
}
