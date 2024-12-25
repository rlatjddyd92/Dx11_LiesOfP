#include "stdafx.h"
#include "UIPage.h"
#include "..\Public\Statistic_Data.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CStatistic_Data::CStatistic_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage_Achievment{ pDevice , pContext }
{
}

CStatistic_Data::CStatistic_Data(const CStatistic_Data& Prototype)
	: CUIPage_Achievment{ Prototype }
{
}

HRESULT CStatistic_Data::Initialize_Prototype()
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

HRESULT CStatistic_Data::Initialize(void* pArg)
{
	return S_OK;
}

void CStatistic_Data::Priority_Update(_float fTimeDelta)
{
}

void CStatistic_Data::Update(_float fTimeDelta)
{
}

void CStatistic_Data::Late_Update(_float fTimeDelta)
{
}

HRESULT CStatistic_Data::Render()
{
	return S_OK;
}

void CStatistic_Data::OpenAction()
{
}

void CStatistic_Data::CloseAction()
{
}

CHECK_MOUSE CStatistic_Data::Check_Page_Action(_float fTimeDelta)
{
	return CHECK_MOUSE();
}

void CStatistic_Data::Set_Statistic_Data(vector<struct CUIPage::UIPART_INFO*>& vecOrigin, _int iIndex)
{
	for (auto& iter : vecOrigin)
	{
		_int iGroup = iter->iGroupIndex;

		if ((m_vParent == _Vec2{ 0.f,0.f }) && (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_STATISTIC_FRAME))
			m_vParent = iter->fPosition;
		if (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_STATISTIC_LINE)
			m_vecStatic.push_back(iter);
	}

	m_pTitle = m_vecStatic[m_vecStatic.size() - 2];
	m_pStat = m_vecStatic[m_vecStatic.size() - 1];

	m_iStaticIndex = iIndex;
	m_fRatio_Data = _float(iIndex) / _float(3.f);

	if (m_iStaticIndex == 0)
		m_strTitle = TEXT("플레이 시간");
	else if (m_iStaticIndex == 1)
		m_strTitle = TEXT("적에게 가한 데미지 합계");
	else if (m_iStaticIndex == 2)
		m_strTitle = TEXT("처치한 적의 수");
	else if (m_iStaticIndex == 3)
		m_strTitle = TEXT("획득한 에르고 합계");
}

void CStatistic_Data::Input_Data_Playtime(_float fTimeDelta)
{
	m_fPlayTime += fTimeDelta;
}

void CStatistic_Data::Input_Data_AttackDamege(_float fDamege)
{
	m_fAttackDamege += fDamege;
}

void CStatistic_Data::Add_Data_KillCount()
{
	++m_iKillCount;
}

void CStatistic_Data::Input_Data_ErgoGain(_int iCount)
{
	m_iEgroGainCount += iCount;
}

void CStatistic_Data::Update_Statistic_Line(_float fAlpha)
{
	m_fTopPartMove = fAlpha;

	if (m_iStaticIndex == 0)
	{
		_int iSecond = _int(m_fPlayTime);
		_int iMin = iSecond / 60;
		iSecond = iSecond % 60;
		_int iHour = iMin / 60;
		iMin = iMin % 60;

		_wstring strTime = to_wstring(iHour);
		if (iHour != 0)
			strTime += TEXT(":");
		strTime += to_wstring(iMin);
		if (iMin != 0)
			strTime += TEXT(":");
		strTime += to_wstring(iSecond);

		m_strStat = strTime;
	}
	else if (m_iStaticIndex == 1)
	{
		m_strStat = to_wstring(_int(m_fAttackDamege));
	}
	else if (m_iStaticIndex == 2)
	{
		m_strStat = to_wstring(m_iKillCount);
	}
	else if (m_iStaticIndex == 3)
	{
		m_strStat = to_wstring(m_iEgroGainCount);
	}

	m_vecStatic[0]->fRatio = m_fRatio_Data;
	m_vecStatic[0]->MovePart(m_vParent, 1.f);
	m_pTitle->strText = m_strTitle;
	m_pStat->strText = m_strStat;

	for (_int i = 0; i < m_vecStatic.size(); ++i)
	{
		if (i > 1)
			m_vecStatic[i]->MovePart(m_vecStatic[1]->fPosition, 1.f);
		else if (i == 1)
			m_vecStatic[i]->MovePart(m_vecStatic[0]->fPosition, 1.f);

		__super::Input_Render_Info(*m_vecStatic[i]);
	}
}

CStatistic_Data* CStatistic_Data::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStatistic_Data* pInstance = new CStatistic_Data(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CStatistic_Data"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStatistic_Data::Clone(void* pArg)
{
	CStatistic_Data* pInstance = new CStatistic_Data(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CStatistic_Data"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatistic_Data::Free()
{
	__super::Free();
	m_vecStatic.clear();
}
