#include "stdafx.h"
#include "..\Public\Item_Manager.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CItem_Manager::CItem_Manager(CGameInstance* pGameInstance)
{
	m_pGameInstance = pGameInstance;
}

HRESULT CItem_Manager::Initialize_Item()
{
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
