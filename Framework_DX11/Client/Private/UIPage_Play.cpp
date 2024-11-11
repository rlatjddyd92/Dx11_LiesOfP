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
	
	__super::UpdatePart_ByControl(m_HP_Gauge_Frame);
	__super::UpdatePart_ByControl(m_HP_Gauge_Fill);

	__super::UpdatePart_ByControl(m_Stamina_Gauge_Frame);
	__super::UpdatePart_ByControl(m_Stamina_Gauge_Fill);

	for (_int i = 0; i < 5; ++i)
	{
		__super::UpdatePart_ByControl(m_vecSpecial_Gauge_Frame[i]);
		__super::UpdatePart_ByControl(m_vecSpecial_Gauge_Fill[i]);
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
	m_HP_Gauge_Frame = new UG_CTRL;
	m_HP_Gauge_Fill = new UG_CTRL;

	// 스태미나 조정
	m_Stamina_Gauge_Frame = new UG_CTRL;
	m_Stamina_Gauge_Fill = new UG_CTRL;

	// 특수 스킬 바 조정
	for (_int i = 0; i < 5; ++i)
	{
		UG_CTRL* pFrame = new UG_CTRL;
		UG_CTRL* pFill = new UG_CTRL;
		m_vecSpecial_Gauge_Frame.push_back(pFrame);
		m_vecSpecial_Gauge_Fill.push_back(pFill);
	}

	// 좌하단 포션/도구 조정
	m_PO_Data = new UG_CTRL;
	m_Tool_Data = new UG_CTRL;
	m_Select_Ctrl = new UG_CTRL;
	
	for (_int i = 0; i < m_vecPart.size(); ++i)
	{
		switch (m_vecPart[i]->iGroupIndex)
		{
		case _int(PART_GROUP::GROUP_HP_FRAME):
			m_HP_Gauge_Frame->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_HP_FILL):
			m_HP_Gauge_Fill->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_ST_FRAME):
			m_Stamina_Gauge_Frame->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_ST_FILL):
			m_Stamina_Gauge_Fill->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP0_FRAME):
			m_vecSpecial_Gauge_Frame[0]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP0_FILL):
			m_vecSpecial_Gauge_Fill[0]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP1_FRAME):
			m_vecSpecial_Gauge_Frame[1]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP1_FILL):
			m_vecSpecial_Gauge_Fill[1]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP2_FRAME):
			m_vecSpecial_Gauge_Frame[2]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP2_FILL):
			m_vecSpecial_Gauge_Fill[2]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP3_FRAME):
			m_vecSpecial_Gauge_Frame[3]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP3_FILL):
			m_vecSpecial_Gauge_Fill[3]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP4_FRAME):
			m_vecSpecial_Gauge_Frame[4]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SP4_FILL):
			m_vecSpecial_Gauge_Fill[4]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_POTION_DATA):
			m_PO_Data->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_TOOL_DATA):
			m_Tool_Data->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SELECT):
			m_Select_Ctrl->PartIndexlist.push_back(i);
			break;
		default:
			break;
		}
	}

	return S_OK;
}

void CUIPage_Play::Move_SelectCtrl(_bool bIsUp)
{
	if (bIsUp)
	{
		m_vecPart[m_Select_Ctrl->PartIndexlist.front()]->fRatio = 0.f;
		// test
		m_vecPart[48]->bRender = true;
		m_vecPart[52]->bRender = false;
	}
	else 
	{
		m_vecPart[m_Select_Ctrl->PartIndexlist.front()]->fRatio = 1.f;
		// test
		m_vecPart[48]->bRender = false;
		m_vecPart[52]->bRender = true;
	}
}

void CUIPage_Play::Swich_ToolItem()
{
	// 테스트 코드 
	wstring strBuffer = m_vecPart[50]->strText;
	m_vecPart[50]->strText = m_vecPart[55]->strText;
	m_vecPart[55]->strText = m_vecPart[57]->strText;
	m_vecPart[57]->strText = strBuffer;
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

	__super::Release_Control(m_HP_Gauge_Frame);
	__super::Release_Control(m_HP_Gauge_Fill);

	__super::Release_Control(m_Stamina_Gauge_Frame);
	__super::Release_Control(m_Stamina_Gauge_Fill);

	for (_int i = 0; i < 5; ++i)
	{
		__super::Release_Control(m_vecSpecial_Gauge_Frame[i]);
		__super::Release_Control(m_vecSpecial_Gauge_Fill[i]);
	}

	m_vecSpecial_Gauge_Frame.clear();
	m_vecSpecial_Gauge_Fill.clear();

	__super::Release_Control(m_PO_Data);
	__super::Release_Control(m_Tool_Data);
	__super::Release_Control(m_Select_Ctrl);
}
