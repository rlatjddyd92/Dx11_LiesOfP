#include "stdafx.h"
#include "..\Public\Player_Stat_Manager.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CPlayer_Stat_Manager::CPlayer_Stat_Manager()
{
}

HRESULT CPlayer_Stat_Manager::Initialize_Stat()
{
	return S_OK;
}

CPlayer_Stat_Manager* CPlayer_Stat_Manager::Create()
{
	CPlayer_Stat_Manager* pInstance = new CPlayer_Stat_Manager();

	if (FAILED(pInstance->Initialize_Stat()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Stat_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Stat_Manager::Free()
{
	__super::Free();
}
