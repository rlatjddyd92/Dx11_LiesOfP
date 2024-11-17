#include "stdafx.h"
#include "..\Public\UIPage_Play.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Play::CUIPage_Play(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIPage{ pDevice, pContext }
{
	
}

CUIPage_Play::CUIPage_Play(const CUIPage_Play& Prototype)
    : CUIPage{ Prototype }
{
}

HRESULT CUIPage_Play::Initialize_Prototype()
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


    return S_OK;
}

HRESULT CUIPage_Play::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Play::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Play::Update(_float fTimeDelta)
{

	__super::Update(fTimeDelta);

	
}

void CUIPage_Play::Late_Update(_float fTimeDelta)
{
	LU_Gauge_Update(fTimeDelta);
	LD_Potion_Tool_Update(fTimeDelta);
	LD_Arm_Update(fTimeDelta);
	RU_Coin_Update(fTimeDelta);
	RD_Weapon_Update(fTimeDelta);

	
	for(auto& iter : m_Ctrllist)
		__super::UpdatePart_ByControl(iter);

	__super::Late_Update(fTimeDelta);

	
}

HRESULT CUIPage_Play::Render()
{
    return S_OK;
}

void CUIPage_Play::OpenAction()
{
	__super::OpenAction();

	
}

void CUIPage_Play::CloseAction()
{
	__super::CloseAction();

	
}

HRESULT CUIPage_Play::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	m_vec_Group_Ctrl.resize(_int(PART_GROUP::GROUP_END));

	for(auto& iter : m_vec_Group_Ctrl)
		iter = new UG_CTRL;

	for (_int i = 0; i < m_vecPart.size(); ++i)
	{
		if (m_vecPart[i]->iGroupIndex != -1)
			m_vec_Group_Ctrl[m_vecPart[i]->iGroupIndex]->PartIndexlist.push_back(i);
	}
		

	return S_OK;
}

void CUIPage_Play::Move_SelectCtrl(_bool bIsUp)
{
	
}

void CUIPage_Play::Swich_ToolItem()
{
	
}

void CUIPage_Play::LU_Gauge_Update(_float fTimeDelta)
{
	const CPlayer_Stat_Manager::STAT& tHP = GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_HP);
	const CPlayer_Stat_Manager::STAT& tStamina = GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_STAMINA);
	const CPlayer_Stat_Manager::STAT& tRegion = GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_REGION);

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_HP_FRAME)]->fRatio = (_float)tHP.fStat_Max / (_float)tHP.fStat_Max_Limit;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_HP_FILL)]->fRatio = (_float)tHP.fStat_Now / (_float)tHP.fStat_Max_Limit;

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ST_FRAME)]->fRatio = (_float)tStamina.fStat_Max / (_float)tStamina.fStat_Max_Limit;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ST_FILL)]->fRatio = (_float)tStamina.fStat_Now / (_float)tStamina.fStat_Max_Limit;

	_int iSP_Cell_Num = (_int)((_float)tRegion.fStat_Max / (_float)tRegion.fStat_Interval);
	_int iSP_Cell_Filled = (_int)((_float)tRegion.fStat_Now / (_float)tRegion.fStat_Interval);
	_float fSP_Ratio = (tRegion.fStat_Now - ((_float)iSP_Cell_Filled * tRegion.fStat_Interval)) / (_float)tRegion.fStat_Interval;

	for (_int i = 0; i < 5; ++i)
	{
		if (i < iSP_Cell_Num)
		{
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SP0_FRAME) + i]->fRatio = 1.f;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SP0_FRAME) + i]->bRender = true;
		}
		else
		{
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SP0_FRAME) + i]->fRatio = 0.f;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SP0_FRAME) + i]->bRender = false;
		}
	}

	for (_int i = 0; i < 5; ++i)
	{
		if (i < iSP_Cell_Filled)
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SP0_FILL) + i]->fRatio = 1.f;
		else if (i == iSP_Cell_Filled)
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SP0_FILL) + i]->fRatio = fSP_Ratio;
		else
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SP0_FILL) + i]->fRatio = 0.f;
	}
}

void CUIPage_Play::LD_Potion_Tool_Update(_float fTimeDelta)
{

}

void CUIPage_Play::LD_Bag_Update(_float fTimeDelta)
{

}

void CUIPage_Play::LD_Arm_Update(_float fTimeDelta)
{
}

void CUIPage_Play::RU_Coin_Update(_float fTimeDelta)
{
}

void CUIPage_Play::RD_Weapon_Update(_float fTimeDelta)
{
}

CUIPage_Play* CUIPage_Play::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Play* pInstance = new CUIPage_Play(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Play"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Play::Clone(void* pArg)
{
	CUIPage_Play* pInstance = new CUIPage_Play(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Play"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Play::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
