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

	// 체력바 조정
	m_HP_Gauge_Frame = new UG_CTRL;
	m_HP_Gauge_Fill = new UG_CTRL;

	// 스태미나 조정
	m_Stamina_Gauge_Frame = new UG_CTRL;
	m_Stamina_Gauge_Fill = new UG_CTRL;

	m_Ctrllist.push_back(m_HP_Gauge_Frame);
	m_Ctrllist.push_back(m_HP_Gauge_Fill);
	m_Ctrllist.push_back(m_Stamina_Gauge_Frame);
	m_Ctrllist.push_back(m_Stamina_Gauge_Fill);


	// 특수 스킬 바 조정
	for (_int i = 0; i < 5; ++i)
	{
		UG_CTRL* pFrame = new UG_CTRL;
		UG_CTRL* pFill = new UG_CTRL;
		m_vecSpecial_Gauge_Frame.push_back(pFrame);
		m_vecSpecial_Gauge_Fill.push_back(pFill);

		m_Ctrllist.push_back(m_vecSpecial_Gauge_Frame.back());
		m_Ctrllist.push_back(m_vecSpecial_Gauge_Fill.back());
	}

	// 좌하단 포션/도구 조정
	m_Potion_Tool_Tex =  new UG_CTRL;
	m_Potion_Tool_RKey =  new UG_CTRL;
	m_Potion_Queue =  new UG_CTRL;
	m_Potion_Gauge =  new UG_CTRL;
	m_Tool_Queue =  new UG_CTRL;
	m_Portion_Tool_Select =  new UG_CTRL;

	m_Ctrllist.push_back(m_Potion_Tool_Tex);
	m_Ctrllist.push_back(m_Potion_Tool_RKey);
	m_Ctrllist.push_back(m_Potion_Queue);
	m_Ctrllist.push_back(m_Potion_Gauge);
	m_Ctrllist.push_back(m_Tool_Queue);
	m_Ctrllist.push_back(m_Portion_Tool_Select);

	// 좌하단 암 
	m_Arm_Tex =  new UG_CTRL;
	m_Arm_Name =  new UG_CTRL;
	m_Arm_Frame =  new UG_CTRL;
	m_Arm_Fill =  new UG_CTRL;

	m_Ctrllist.push_back(m_Arm_Tex);
	m_Ctrllist.push_back(m_Arm_Name);
	m_Ctrllist.push_back(m_Arm_Frame);
	m_Ctrllist.push_back(m_Arm_Fill);

	// 우상단 코인 
	m_Coin_Count =  new UG_CTRL;

	// 우하단 무기 
	m_Weapon_Durable_Frame =  new UG_CTRL;
	m_Weapon_Durable_Fill =  new UG_CTRL;

	m_Weapon_Normal_Back =  new UG_CTRL;
	m_Weapon_Normal_Tex =  new UG_CTRL;
	m_Weapon_Special_Back =  new UG_CTRL;
	m_Weapon_Special_Tex =  new UG_CTRL;

	m_Weapon_Equip_Num =  new UG_CTRL;

	m_Ctrllist.push_back(m_Coin_Count);
	m_Ctrllist.push_back(m_Weapon_Durable_Frame);
	m_Ctrllist.push_back(m_Weapon_Durable_Fill);
	m_Ctrllist.push_back(m_Weapon_Normal_Back);
	m_Ctrllist.push_back(m_Weapon_Normal_Tex);
	m_Ctrllist.push_back(m_Weapon_Special_Back);
	m_Ctrllist.push_back(m_Weapon_Special_Tex);
	m_Ctrllist.push_back(m_Weapon_Equip_Num);

	// 특수 스킬 바 조정
	for (_int i = 0; i < 6; ++i)
	{
		UG_CTRL* pFrame = new UG_CTRL;
		UG_CTRL* pFill = new UG_CTRL;
		m_vecWeapon_Special_Frame.push_back(pFrame);
		m_vecWeapon_Special_Fill.push_back(pFill);

		m_Ctrllist.push_back(m_vecWeapon_Special_Frame.back());
		m_Ctrllist.push_back(m_vecWeapon_Special_Fill.back());
	}

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
		case _int(PART_GROUP::GROUP_POTION_ITEM):
			m_Potion_Tool_Tex->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_POTION_RKEY):
			m_Potion_Tool_RKey->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_POTION_FILL):
			m_Potion_Gauge->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_POTION_QUEUE):
			m_Potion_Queue->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_TOOL_ITEM):
			m_Potion_Tool_Tex->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_TOOL_RKEY):
			m_Potion_Tool_RKey->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_TOOL_QUEUE):
			m_Tool_Queue->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_SELECT_CELL):
			m_Portion_Tool_Select->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_ARM_BACK):
			//m_Select_Ctrl->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_ARM_TEX):
			m_Arm_Tex->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_ARM_CTRL):
			//m_Select_Ctrl->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_ARM_NAME):
			m_Arm_Name->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_ARM_GAUGE_FRAME):
			m_Arm_Frame->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_ARM_GAUGE_FILL):
			m_Arm_Fill->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_COIN_BACK):
			//m_Select_Ctrl->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_COIN_COUNT):
			m_Coin_Count->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_DURABLE_FRAME):
			m_Weapon_Durable_Frame->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_DURABLE_FILL):
			m_Weapon_Durable_Fill->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_BACK):
			//m_Select_Ctrl->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK):
			m_Weapon_Normal_Back->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_NORMAL_BLADE):
			m_Weapon_Normal_Tex->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_NORMAL_HANDLE):
			m_Weapon_Normal_Tex->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_BACK):
			m_Weapon_Special_Back->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_TEX):
			m_Weapon_Special_Tex->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_EQUIP_NUM):
			m_Weapon_Equip_Num->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FRAME_0):
			m_vecWeapon_Special_Frame[0]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FILL_0):
			m_vecWeapon_Special_Fill[0]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FRAME_1):
			m_vecWeapon_Special_Frame[1]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FILL_1):
			m_vecWeapon_Special_Fill[1]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FRAME_2):
			m_vecWeapon_Special_Frame[2]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FILL_2):
			m_vecWeapon_Special_Fill[2]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FRAME_3):
			m_vecWeapon_Special_Frame[3]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FILL_3):
			m_vecWeapon_Special_Fill[3]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FRAME_4):
			m_vecWeapon_Special_Frame[4]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FILL_4):
			m_vecWeapon_Special_Fill[4]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FRAME_5):
			m_vecWeapon_Special_Frame[5]->PartIndexlist.push_back(i);
			break;
		case _int(PART_GROUP::GROUP_WEAPON_SPECIAL_FILL_5):
			m_vecWeapon_Special_Fill[5]->PartIndexlist.push_back(i);
			break;
		default:
			break;
		}
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

	m_HP_Gauge_Frame->fRatio = (_float)tHP.fStat_Max / (_float)tHP.fStat_Max_Limit;
	m_HP_Gauge_Fill->fRatio = (_float)tHP.fStat_Now / (_float)tHP.fStat_Max_Limit;

	m_Stamina_Gauge_Frame->fRatio = (_float)tStamina.fStat_Max / (_float)tStamina.fStat_Max_Limit;
	m_Stamina_Gauge_Fill->fRatio = (_float)tStamina.fStat_Now / (_float)tStamina.fStat_Max_Limit;

	_int iSP_Cell_Num = (_int)((_float)tRegion.fStat_Max / (_float)tRegion.fStat_Interval);
	_int iSP_Cell_Filled = (_int)((_float)tRegion.fStat_Now / (_float)tRegion.fStat_Interval);
	_float fSP_Ratio = (tRegion.fStat_Now - ((_float)iSP_Cell_Filled * tRegion.fStat_Interval)) / (_float)tRegion.fStat_Interval;

	for (_int i = 0; i < 5; ++i)
	{
		if (i < iSP_Cell_Num)
		{
			m_vecSpecial_Gauge_Frame[i]->fRatio = 1.f;
			m_vecSpecial_Gauge_Frame[i]->bRender = true;
		}
		else
		{
			m_vecSpecial_Gauge_Frame[i]->fRatio = 0.f;
			m_vecSpecial_Gauge_Frame[i]->bRender = false;
		}
	}

	for (_int i = 0; i < 5; ++i)
	{
		if (i < iSP_Cell_Filled)
			m_vecSpecial_Gauge_Fill[i]->fRatio = 1.f;
		else if (i == iSP_Cell_Filled)
			m_vecSpecial_Gauge_Fill[i]->fRatio = fSP_Ratio;
		else
			m_vecSpecial_Gauge_Fill[i]->fRatio = 0.f;
	}

}

void CUIPage_Play::LD_Potion_Tool_Update(_float fTimeDelta)
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



	m_vecSpecial_Gauge_Frame.clear();
	m_vecSpecial_Gauge_Fill.clear();

	m_vecWeapon_Special_Frame.clear();
	m_vecWeapon_Special_Fill.clear();
}
