#include "stdafx.h"
#include "..\Public\Item_Manager.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CItem_Manager::CItem_Manager()
{
}

HRESULT CItem_Manager::Initialize_Item()
{
	return S_OK;
}

CItem_Manager* CItem_Manager::Create()
{
	CItem_Manager* pInstance = new CItem_Manager();

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
}
