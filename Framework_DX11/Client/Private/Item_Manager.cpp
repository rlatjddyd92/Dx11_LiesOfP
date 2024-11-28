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
	ITEM& NewItem = *(m_vecArray_Inven[_uint(eIndex)]->vecItemInfo[iIndex]);

	if (NewItem.eType_Index == ITEM_TYPE::ITEMTYPE_END)
		return ITEM_RESULT::RESULT_INVALID;

	if (m_vecEquip_ItemInfo[_uint(eSlot)]->vecValid_InvenArray[_uint(eIndex)])
	{
		m_vecEquip_ItemInfo[_uint(eSlot)]->eType = eIndex;
		m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex = iIndex;
		NewItem.eSlot = eSlot;
		m_bIsChange = true;
		return ITEM_RESULT::RESULT_SUCCESS;
	}

	return ITEM_RESULT::RESULT_INVALID;
}

ITEM_RESULT CItem_Manager::UnEquipItem_Inven(EQUIP_SLOT eSlot)
{
	ITEM& NewItem = *(m_vecArray_Inven[_uint(m_vecEquip_ItemInfo[_uint(eSlot)]->eType)]->vecItemInfo[m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex]);

	if (NewItem.eType_Index == ITEM_TYPE::ITEMTYPE_END)
		return ITEM_RESULT::RESULT_INVALID;

	NewItem.eSlot = EQUIP_SLOT::EQUIP_END;

	m_vecEquip_ItemInfo[_uint(eSlot)]->eType = INVEN_ARRAY_TYPE::TYPE_END;
	m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex = -1;

	return ITEM_RESULT();
}

ITEM_RESULT CItem_Manager::UseItem_Equip(EQUIP_SLOT eSlot, _uint iCount)
{
	INVEN_ARRAY_TYPE eArray = m_vecEquip_ItemInfo[_uint(eSlot)]->eType;
	_uint iIndex = m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex;
	if (eArray == INVEN_ARRAY_TYPE::TYPE_END)
		return ITEM_RESULT::RESULT_INVALID;

	if (m_vecArray_Inven[_uint(eArray)]->Get_Item_Info(iIndex)->iItem_Index == _int(SPECIAL_ITEM::SP_PULSE_BATTERY))
	{
		if (Use_Potion())
		{
			m_bIsChange = true;
			return ITEM_RESULT::RESULT_SUCCESS;
		}
		else
			return ITEM_RESULT::RESULT_INVALID;
	}
	else
		return m_vecArray_Inven[_uint(eArray)]->Use_Item(iIndex, iCount);
}

ITEM_RESULT CItem_Manager::UseItem_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex, _uint iCount)
{
	if (m_vecArray_Inven[_uint(eIndex)]->Get_Item_Info(iIndex)->iItem_Index == _int(SPECIAL_ITEM::SP_PULSE_BATTERY))
	{
		if (Use_Potion())
		{
			m_bIsChange = true;
			return ITEM_RESULT::RESULT_SUCCESS;
		}
		else
			return ITEM_RESULT::RESULT_INVALID;
	}
	else
		return m_vecArray_Inven[_uint(eIndex)]->Use_Item(iIndex, iCount);
}

ITEM_RESULT CItem_Manager::Remove_Item_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex)
{
	m_bIsChange = true;
	return m_vecArray_Inven[_uint(eIndex)]->Remove_Item(iIndex);
}

CPlayer::WEAPON_TYPE CItem_Manager::Get_Weapon_Model_Index()
{
	INVEN_ARRAY_TYPE eArray = m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0) + (m_iWeapon_Select * 2)]->eType;
	_int iIndex = m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0) + (m_iWeapon_Select * 2)]->iIndex;

	const ITEM* pItem = m_vecArray_Inven[_int(eArray)]->Get_Item_Info(iIndex);

	if (pItem == nullptr)
		return CPlayer::WEAPON_TYPE::WEP_END;
	else if (pItem->strName == TEXT("겨울의 레이피어 날"))
		return CPlayer::WEAPON_TYPE::WEP_RAPIER;
	else if (pItem->strName == TEXT("흑철 절삭기 날"))
		return CPlayer::WEAPON_TYPE::WEP_FLAME;
	else if (pItem->strName == TEXT("인간성의 증거"))
		return CPlayer::WEAPON_TYPE::WEP_SCISSOR;

	return CPlayer::WEAPON_TYPE::WEP_END;
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

	//GET_GAMEINTERFACE->Add_Stat_Normal(STAT_NORMAL::STAT_GAUGE_HP, 100.f);

	return true;
}

void CItem_Manager::Set_ItemAction(EQUIP_SLOT eSlot, _Vec2 vPos, _Vec2 vSize)
{
	m_bItemAction_Active = true;
	m_eNow_ActionSlot = eSlot;
	m_eNow_ActionArray = INVEN_ARRAY_TYPE::TYPE_END;
	m_iArray_Index = -1;



}

void CItem_Manager::Set_ItemAction(INVEN_ARRAY_TYPE eType, _int iIndex, _Vec2 vPos, _Vec2 vSize)
{
	m_bItemAction_Active = true;
	m_eNow_ActionSlot = EQUIP_SLOT::EQUIP_END;
	m_eNow_ActionArray = eType;
	m_iArray_Index = iIndex;

	vector<ITEM_FUNC> vecFunc;
	_int iArray = _int(eType);


	if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_USING_BASIC)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_USING_RARE_ERGO)))
	{
		if (m_iActionPopup_Page == 0)
		{
			if (iArray != _int(INVEN_ARRAY_TYPE::TYPE_USING_THROW))
				vecFunc.push_back(ITEM_FUNC::FUNC_USING);
			
			if (iArray < _int(INVEN_ARRAY_TYPE::TYPE_USING_ERGO))
			{
				vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP);
				if (m_vecArray_Inven[iArray]->vecItemInfo[iIndex]->eSlot != EQUIP_SLOT::EQUIP_END)
					vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
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
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP);
		if (m_vecArray_Inven[iArray]->vecItemInfo[iIndex]->eSlot != EQUIP_SLOT::EQUIP_END)
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
		vecFunc.push_back(ITEM_FUNC::FUNC_TO_EQUIP);
	}
	else if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_DEFENCE_FRAME)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_DEFENCE_RAINER)))
	{
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP);
		if (m_vecArray_Inven[iArray]->vecItemInfo[iIndex]->eSlot != EQUIP_SLOT::EQUIP_END)
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
		vecFunc.push_back(ITEM_FUNC::FUNC_TO_EQUIP);
	}
	else if ((iArray >= _int(INVEN_ARRAY_TYPE::TYPE_AMULET_COMMON)) && (iArray <= _int(INVEN_ARRAY_TYPE::TYPE_AMULET_COMMON)))
	{
		vecFunc.push_back(ITEM_FUNC::FUNC_EQUIP);
		if (m_vecArray_Inven[iArray]->vecItemInfo[iIndex]->eSlot != EQUIP_SLOT::EQUIP_END)
			vecFunc.push_back(ITEM_FUNC::FUNC_UNEQUIP);
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

	for (_int i = vecFunc.size(); i < 4; ++i)
		vecFunc.push_back(ITEM_FUNC::FUNC_END);

	GET_GAMEINTERFACE->Show_ItemAction(vPos, vSize, vecFunc[0], vecFunc[1], vecFunc[2], vecFunc[3]);
}

void CItem_Manager::Reset_ItemAction()
{
	m_bItemAction_Active = false;
	m_eNow_ActionSlot = EQUIP_SLOT::EQUIP_END;
	m_eNow_ActionArray = INVEN_ARRAY_TYPE::TYPE_END;
	m_iArray_Index = -1;
	m_iActionPopup_Page = 0;
}

ITEM_RESULT CItem_Manager::Operate_ItemAction(ITEM_FUNC eFunc, _Vec2 vPos, _Vec2 vSize)
{





	return ITEM_RESULT();
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

		if (!bAssembleWeapon)
		{
			Assemble_Blade_Handle(0, 0);
			bAssembleWeapon = true;
		}

		if (stoi(iter[2]) != -1)
			EquipItem_Inven(INVEN_ARRAY_TYPE(stoi(iter[2])), EQUIP_SLOT(stoi(iter[0])), stoi(iter[3]));
	}

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

	m_vecItem_BasicSpec.clear();
	m_vecItem_InvenSlotIndex.clear();
	m_vecArray_Inven.clear();
	m_vecEquip_ItemInfo.clear();

	Safe_Release(m_pGameInstance);
}
