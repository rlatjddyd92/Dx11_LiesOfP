#include "stdafx.h"
#include "..\Public\Item_Manager.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"


CItem_Manager::CItem_Manager(CGameInstance* pGameInstance)
{
	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGameInstance);
}

ITEM_RESULT CItem_Manager::AddNewItem_Inven(INVEN_ARRAY_TYPE eIndex, _uint iCount)
{
	return ITEM_RESULT::RESULT_SUCCESS;
}

ITEM_RESULT CItem_Manager::InputItem_Inven(ITEM* pItem, _uint iCount)
{
	return ITEM_RESULT::RESULT_SUCCESS;
}

ITEM_RESULT CItem_Manager::EquipItem_Inven(INVEN_ARRAY_TYPE eIndex)
{
	return ITEM_RESULT::RESULT_SUCCESS;
}

ITEM_RESULT CItem_Manager::UseItem_Equip(EQUIP_SLOT eSlot, _uint iCount)
{
	return ITEM_RESULT::RESULT_SUCCESS;
}

ITEM_RESULT CItem_Manager::UseItem_Inven(INVEN_ARRAY_TYPE eIndex, _uint iCount)
{
	return ITEM_RESULT::RESULT_SUCCESS;
}


HRESULT CItem_Manager::Initialize_Item()
{
	m_vecArray_Inven.resize(_int(INVEN_ARRAY_TYPE::TYPE_END));
	m_vecEquip_ItemInfo.resize(_int(EQUIP_SLOT::EQUIP_END));


	// 아이템 기본 스펙 저장 
	vector<vector<_wstring>> vecBuffer_Spec;
	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Item_Spec_Data.csv", &vecBuffer_Spec)))
		return E_FAIL;

	_int iStartRow = 2;

	for (auto& iter : vecBuffer_Spec)
	{
		if (iStartRow > 0)
		{
			--iStartRow;
			continue;
		}

		ITEM* pNew = new ITEM;

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

		for (_int i = 0; i < _int(DAMEGE_TYPE::DAMEGE_END); ++i)
			pNew->vecDamege[i] = stof(iter[13 + i]);

		for (_int i = 0; i < _int(DEFENCE_TYPE::DEFENCE_END); ++i)
			pNew->vecDefence[i] = stof(iter[17 + i]);
		
		pNew->strAttack_Type = iter[26];
		pNew->fType_Damege = stof(iter[27]);
		pNew->fType_Damege_Fatal_Ratio = stof(iter[28]);
		

			/*_Fatal_Ratio
			_Fable_Charge
			_Battery_Charge
			_Reduce_Damege
			_Fable_Art_Name
			_Art_Cost
			_Ablity_Power
			_Ablity_Skill
			_Ablity_Evolution
			_Slash_Grade
			_Piece_Grade
			_Fable_Desc
			_Item_Desc*/


		





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

	Safe_Release(m_pGameInstance);
}
