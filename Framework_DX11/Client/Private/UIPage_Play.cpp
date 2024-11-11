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
	
	__super::UpdatePart_ByControl(HP_Gauge_Frame);
	__super::UpdatePart_ByControl(HP_Gauge_Fill);

	__super::UpdatePart_ByControl(Stamina_Gauge_Frame);
	__super::UpdatePart_ByControl(Stamina_Gauge_Fill);

	for (_int i = 0; i < 5; ++i)
	{
		__super::UpdatePart_ByControl(vecSpecial_Gauge_Frame[i]);
		__super::UpdatePart_ByControl(vecSpecial_Gauge_Fill[i]);
	}
	

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

	// 체력바 조정
	HP_Gauge_Frame = new UG_CTRL;
	HP_Gauge_Fill = new UG_CTRL;

	// 스태미나 조정
	Stamina_Gauge_Frame = new UG_CTRL;
	Stamina_Gauge_Fill = new UG_CTRL;

	// 특수 스킬 바 조정
	for (_int i = 0; i < 5; ++i)
	{
		UG_CTRL* pFrame = new UG_CTRL;
		UG_CTRL* pFill = new UG_CTRL;
		vecSpecial_Gauge_Frame.push_back(pFrame);
		vecSpecial_Gauge_Fill.push_back(pFill);
	}
	
	for (_int i = 0; i < m_vecPart.size(); ++i)
	{
		switch (m_vecPart[i]->iGroupIndex)
		{
		case _int(PART_GROUP::GROUP_HP_FRAME):
			HP_Gauge_Frame->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_HP_FILL):
			HP_Gauge_Fill->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_ST_FRAME):
			Stamina_Gauge_Frame->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_ST_FILL):
			Stamina_Gauge_Fill->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP0_FRAME):
			vecSpecial_Gauge_Frame[0]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP0_FILL):
			vecSpecial_Gauge_Fill[0]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP1_FRAME):
			vecSpecial_Gauge_Frame[1]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP1_FILL):
			vecSpecial_Gauge_Fill[1]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP2_FRAME):
			vecSpecial_Gauge_Frame[2]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP2_FILL):
			vecSpecial_Gauge_Fill[2]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP3_FRAME):
			vecSpecial_Gauge_Frame[3]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP3_FILL):
			vecSpecial_Gauge_Fill[3]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP4_FRAME):
			vecSpecial_Gauge_Frame[4]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP4_FILL):
			vecSpecial_Gauge_Fill[4]->PartIndexlist.push_back(i);
			break;
		default:
			break;
		}
	}

	return S_OK;
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

	__super::Release_Control(HP_Gauge_Frame);
	__super::Release_Control(HP_Gauge_Fill);

	__super::Release_Control(Stamina_Gauge_Frame);
	__super::Release_Control(Stamina_Gauge_Fill);

	for (_int i = 0; i < 5; ++i)
	{
		__super::Release_Control(vecSpecial_Gauge_Frame[i]);
		__super::Release_Control(vecSpecial_Gauge_Fill[i]);
	}

	vecSpecial_Gauge_Frame.clear();
	vecSpecial_Gauge_Fill.clear();
}
