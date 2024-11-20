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

	
	for(auto& iter : m_vec_Group_Ctrl)
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
	GET_GAMEINTERFACE->SetIngame(true);
	
}

void CUIPage_Play::CloseAction()
{
	__super::CloseAction();

	
}

CHECK_MOUSE CUIPage_Play::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	if (KEY_TAP(KEY::T))
	{
		if (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio != 0.f)
		{
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio = 0.f;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_TOOL_RKEY)]->fRatio = 0.f;
		}
		else
		{
			_int iSelect = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_ITEM)]->PartIndexlist.front();
			_int iQueueA = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_QUEUE_ITEM)]->PartIndexlist.front();
			_int iQueueB = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_QUEUE_ITEM)]->PartIndexlist.back();

			_int iItem = m_vecPart[iSelect]->iTexture_Index;

			m_vecPart[iSelect]->iTexture_Index = m_vecPart[iQueueA]->iTexture_Index;
			m_vecPart[iQueueA]->iTexture_Index = m_vecPart[iQueueB]->iTexture_Index;
			m_vecPart[iQueueB]->iTexture_Index = iItem;
		}
	}
	else if (KEY_TAP(KEY::G))
	{
		if (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio != 1.f)
		{
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio = 1.f;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_TOOL_RKEY)]->fRatio = 1.f;
		}
		else
		{
			_int iSelect = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOOL_ITEM)]->PartIndexlist.front();
			_int iQueueA = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOOL_QUEUE_ITEM)]->PartIndexlist.front();
			_int iQueueB = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOOL_QUEUE_ITEM)]->PartIndexlist.back();

			_int iItem = m_vecPart[iSelect]->iTexture_Index;

			m_vecPart[iSelect]->iTexture_Index = m_vecPart[iQueueA]->iTexture_Index;
			m_vecPart[iQueueA]->iTexture_Index = m_vecPart[iQueueB]->iTexture_Index;
			m_vecPart[iQueueB]->iTexture_Index = iItem;
		}
	}

	if (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio == 1.f)
		if (KEY_HOLD(KEY::R))
		{
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_DURABLE_FILL)]->fRatio += fTimeDelta;
			if (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_DURABLE_FILL)]->fRatio > 1.f)
				m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_DURABLE_FILL)]->fRatio = 0.f;
		}

	if (KEY_HOLD(KEY::E))
	{
		m_fBag_Open_Waiting_Now += fTimeDelta;
		if (m_fBag_Open_Waiting_Now >= m_fBag_Open_Waiting_Limit)
		{
			m_fBag_Open_Waiting_Now = 0.f;
			if (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_FRAME)]->bRender)
			{
				__super::Array_Control(_int(PART_GROUP::GROUP_POTION_FRAME), _int(PART_GROUP::GROUP_SELECT_CELL), CTRL_COMMAND::COM_RENDER, false);
				__super::Array_Control(_int(PART_GROUP::GROUP_BAG_FRAMELINE), _int(PART_GROUP::GROUP_BAG_NUM), CTRL_COMMAND::COM_RENDER, true);
			}
			else
			{
				__super::Array_Control(_int(PART_GROUP::GROUP_POTION_FRAME), _int(PART_GROUP::GROUP_SELECT_CELL), CTRL_COMMAND::COM_RENDER, true);
				__super::Array_Control(_int(PART_GROUP::GROUP_BAG_FRAMELINE), _int(PART_GROUP::GROUP_BAG_NUM), CTRL_COMMAND::COM_RENDER, false);
			}
		}
	}

	return CHECK_MOUSE::MOUSE_NONE;
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

	__super::Array_Control(_int(PART_GROUP::GROUP_POTION_FRAME), _int(PART_GROUP::GROUP_SELECT_CELL), CTRL_COMMAND::COM_RENDER, true);
	__super::Array_Control(_int(PART_GROUP::GROUP_BAG_FRAMELINE), _int(PART_GROUP::GROUP_BAG_NUM), CTRL_COMMAND::COM_RENDER, false);
		
	
	m_bWeapon_Top = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK_FX)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BLADE)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_HANDLE)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_SPECIAL_BACK)]->bRender = false;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_SPECIAL_TEX)]->bRender = false;

	--__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_WEAPON_EQUIP_NUM))->iTexture_Index;

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_FRAME)]->bRender = false;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_WHITE)]->bRender = false;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_FILL)]->bRender = false;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_FRAME)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_WHITE)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_FILL)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_FRAME)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_WHITE)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_FILL)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_FRAME)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_WHITE)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_FILL)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_KEYSET_A)]->bRender = false;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_KEYSET_B)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_KEYSET_A)]->bRender = true;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_KEYSET_B)]->bRender = false;

	return S_OK;
}

void CUIPage_Play::LU_Gauge_Update(_float fTimeDelta)
{
	const CPlayer_Stat_Manager::STAT& tRegion = GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_REGION);

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_HP_FRAME)]->fRatio = GET_GAMEINTERFACE->Get_Max_Limit_Ratio(STAT_NORMAL::STAT_GAUGE_HP);
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_HP_FILL)]->fRatio = GET_GAMEINTERFACE->Get_Now_Limit_Ratio(STAT_NORMAL::STAT_GAUGE_HP);

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ST_FRAME)]->fRatio = GET_GAMEINTERFACE->Get_Max_Limit_Ratio(STAT_NORMAL::STAT_GAUGE_STAMINA);
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ST_FILL)]->fRatio = GET_GAMEINTERFACE->Get_Now_Limit_Ratio(STAT_NORMAL::STAT_GAUGE_STAMINA);

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
	// 테스트 코드 
	if (KEY_TAP(KEY::TAPKEY))
	{
		if (!m_bWeapon_Top)
		{
			m_bWeapon_Top = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK_FX)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BLADE)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_HANDLE)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_SPECIAL_BACK)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_SPECIAL_TEX)]->bRender = false;
			--m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_EQUIP_NUM)]->PartIndexlist.front()]->iTexture_Index;
			
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_FRAME)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_WHITE)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_FILL)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_FRAME)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_WHITE)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_FILL)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_FRAME)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_WHITE)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_FILL)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_FRAME)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_WHITE)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_FILL)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_KEYSET_A)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_KEYSET_B)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_KEYSET_A)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_KEYSET_B)]->bRender = false;
		}
		else
		{
			m_bWeapon_Top = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK_FX)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BLADE)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_HANDLE)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_SPECIAL_BACK)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_SPECIAL_TEX)]->bRender = true;
			++m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_EQUIP_NUM)]->PartIndexlist.front()]->iTexture_Index;

			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_FRAME)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_WHITE)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_FILL)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_FRAME)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_WHITE)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_CENTER_FILL)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_FRAME)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_WHITE)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_FILL)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_FRAME)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_WHITE)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_CENTER_FILL)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_KEYSET_A)]->bRender = true;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_KEYSET_B)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_KEYSET_A)]->bRender = false;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_KEYSET_B)]->bRender = true;
		}
	}




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
