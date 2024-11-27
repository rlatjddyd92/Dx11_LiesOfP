#include "stdafx.h"
#include "..\Public\Player_Stat_Manager.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CPlayer_Stat_Manager::CPlayer_Stat_Manager(CGameInstance* pGameInstance)
{
	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGameInstance);
}


HRESULT CPlayer_Stat_Manager::Initialize_Stat()
{
	vector<vector<_wstring>> vecBuffer_Buff_Info;

	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Buff_Info.csv", &vecBuffer_Buff_Info)))
		return E_FAIL;

	for (_int i = 1; i <= _int(BUFF_INDEX::BUFF_END); ++i)
	{
		BUFF* pNew = new BUFF;

		pNew->strBuff_Name = vecBuffer_Buff_Info[i][1];
		pNew->iTexture_Index = stoi(vecBuffer_Buff_Info[i][2]);
		pNew->vTexture_Color.x = stof(vecBuffer_Buff_Info[i][3]);
		pNew->vTexture_Color.y = stof(vecBuffer_Buff_Info[i][4]);
		pNew->vTexture_Color.z = stof(vecBuffer_Buff_Info[i][5]);
		pNew->vTexture_Color.w = stof(vecBuffer_Buff_Info[i][6]);
		pNew->fLifeTime_Max = stof(vecBuffer_Buff_Info[i][7]);

		m_vecBuff.push_back(pNew);
	}

	return S_OK;
}

CPlayer_Stat_Manager* CPlayer_Stat_Manager::Create(CGameInstance* pGameInstance)
{
	CPlayer_Stat_Manager* pInstance = new CPlayer_Stat_Manager(pGameInstance);

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

	for (auto& iter : m_vecBuff)
		Safe_Delete(iter);
	m_vecBuff.clear();

	Safe_Release(m_pGameInstance);
}
