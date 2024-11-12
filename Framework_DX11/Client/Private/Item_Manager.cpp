#include "stdafx.h"
#include "..\Public\Item_Manager.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

ITEM_RESULT CItem_Manager::AddItem_Inven(ITEM_INDEX eIndex, _int iCount)
{
	return ITEM_RESULT();
}

ITEM_RESULT CItem_Manager::EquipItem_Inven(ITEM_INDEX eIndex)
{
	return ITEM_RESULT();
}

CItem_Manager::CItem_Manager(CGameInstance* pGameInstance)
{
	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGameInstance);
}

HRESULT CItem_Manager::Initialize_Item()
{
	m_vecItem_BasicSpec.resize(_int(ITEM_INDEX::INDEX_END));
	m_vecSlot_Inven.resize(_int(ITEM_TYPE::TYPE_END));
	m_vecEquip_ItemIndex.resize(_int(EQUIP_SLOT::EQUIP_END));

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
