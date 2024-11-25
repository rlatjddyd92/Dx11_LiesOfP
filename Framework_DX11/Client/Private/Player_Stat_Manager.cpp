#include "stdafx.h"
#include "..\Public\Player_Stat_Manager.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CPlayer_Stat_Manager::CPlayer_Stat_Manager(CGameInstance* pGameInstance)
{
	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGameInstance);
}

void CPlayer_Stat_Manager::Update_Stat(_float fTimeDelta)
{
	for (auto& iter : m_vecBuff)
	{
		iter->fLifeTime_Now -= fTimeDelta;
		if (iter->fLifeTime_Now < 0)
			iter->fLifeTime_Now = 0.f;
	}
}



void CPlayer_Stat_Manager::Add_Stat_Normal(STAT_NORMAL eIndex, _float fValue)
{
	m_vecStat_Normal[_int(eIndex)]->fStat_Now += fValue;

	m_vecStat_Normal[_int(eIndex)]->fStat_Now = max(m_vecStat_Normal[_int(eIndex)]->fStat_Now, 0.f);

	if (m_vecStat_Normal[_int(eIndex)]->eType == STAT_TYPE::STAT_TYPE_CELL)
	{
		_int iValidCell = (_int)(m_vecStat_Normal[_int(eIndex)]->fStat_Max / m_vecStat_Normal[_int(eIndex)]->fStat_Interval);
		m_vecStat_Normal[_int(eIndex)]->fStat_Now = min(m_vecStat_Normal[_int(eIndex)]->fStat_Now, m_vecStat_Normal[_int(eIndex)]->fStat_Interval * iValidCell);
	}
	else
		m_vecStat_Normal[_int(eIndex)]->fStat_Now = min(m_vecStat_Normal[_int(eIndex)]->fStat_Now, m_vecStat_Normal[_int(eIndex)]->fStat_Max);
}

void CPlayer_Stat_Manager::Add_StatMax_Normal(STAT_NORMAL eIndex, _float fValue)
{
	m_vecStat_Normal[_int(eIndex)]->fStat_Max += fValue;

	m_vecStat_Normal[_int(eIndex)]->fStat_Max = max(m_vecStat_Normal[_int(eIndex)]->fStat_Max, 0.f);
	m_vecStat_Normal[_int(eIndex)]->fStat_Max = min(m_vecStat_Normal[_int(eIndex)]->fStat_Max, m_vecStat_Normal[_int(eIndex)]->fStat_Max_Limit);

	if (m_vecStat_Normal[_int(eIndex)]->eType == STAT_TYPE::STAT_TYPE_CELL)
	{
		_int iValidCell = (_int)(m_vecStat_Normal[_int(eIndex)]->fStat_Max / m_vecStat_Normal[_int(eIndex)]->fStat_Interval);
		m_vecStat_Normal[_int(eIndex)]->fStat_Now = min(m_vecStat_Normal[_int(eIndex)]->fStat_Now, m_vecStat_Normal[_int(eIndex)]->fStat_Interval * iValidCell);
	}
	else
		m_vecStat_Normal[_int(eIndex)]->fStat_Now = min(m_vecStat_Normal[_int(eIndex)]->fStat_Now, m_vecStat_Normal[_int(eIndex)]->fStat_Max);
}

HRESULT CPlayer_Stat_Manager::Initialize_Stat()
{
	vector<vector<_wstring>> vecBuffer;

	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Player_Initial_Stat.csv", &vecBuffer)))
		return E_FAIL;

	m_vecStat_Normal.resize(_int(STAT_NORMAL::STAT_END));
	m_vecStat_DEF.resize(_int(STAT_DEF::STAT_END));

	for (_int i = 1; i < _int(STAT_NORMAL::STAT_END) + _int(STAT_DEF::STAT_END); ++i)
	{
		if (i <= _int(STAT_NORMAL::STAT_END))
		{
			m_vecStat_Normal[i - 1] = new STAT;
			m_vecStat_Normal[i - 1]->strStat_Name = vecBuffer[i][2];
			m_vecStat_Normal[i - 1]->eType = STAT_TYPE(stoi(vecBuffer[i][3]));
			m_vecStat_Normal[i - 1]->fStat_Now = stof(vecBuffer[i][4]);
			m_vecStat_Normal[i - 1]->fStat_Max = stof(vecBuffer[i][5]);
			m_vecStat_Normal[i - 1]->fStat_Max_Limit = stof(vecBuffer[i][6]);
			m_vecStat_Normal[i - 1]->fStat_Interval = stof(vecBuffer[i][7]);
		}
		else
		{
			m_vecStat_DEF[i - _int(STAT_NORMAL::STAT_END) - 1] = new STAT;
			m_vecStat_DEF[i - _int(STAT_NORMAL::STAT_END) - 1]->strStat_Name = vecBuffer[i][2];
			m_vecStat_DEF[i - _int(STAT_NORMAL::STAT_END) - 1]->eType = STAT_TYPE(stoi(vecBuffer[i][3]));
			m_vecStat_DEF[i - _int(STAT_NORMAL::STAT_END) - 1]->fStat_Now = stof(vecBuffer[i][4]);
			m_vecStat_DEF[i - _int(STAT_NORMAL::STAT_END) - 1]->fStat_Max = stof(vecBuffer[i][5]);
			m_vecStat_DEF[i - _int(STAT_NORMAL::STAT_END) - 1]->fStat_Max_Limit = stof(vecBuffer[i][6]);
			m_vecStat_DEF[i - _int(STAT_NORMAL::STAT_END) - 1]->fStat_Interval = stof(vecBuffer[i][7]);
		}
	}

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

	Safe_Release(m_pGameInstance);
	for (auto& iter : m_vecStat_Normal)
		Safe_Delete(iter);
	m_vecStat_Normal.clear();

	for (auto& iter : m_vecStat_DEF)
		Safe_Delete(iter);
	m_vecStat_DEF.clear();

	for (auto& iter : m_vecBuff)
		Safe_Delete(iter);
	m_vecBuff.clear();
}
