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

	m_bIsBatching = false;

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

	Boss_Hp_Update(fTimeDelta);
	__super::Late_Update(fTimeDelta);

	

	PlayInfo_Update(fTimeDelta);
	m_bCan_InterAction = true;
	__super::Array_Control(_int(PART_GROUP::GROUP_INTER_STATIC), _int(PART_GROUP::GROUP_INTER_TEXT), CTRL_COMMAND::COM_RENDER, false);
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

CHECK_MOUSE CUIPage_Play::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	Action_Potion_Tool(fTimeDelta);
	Action_Arm(fTimeDelta);
	Action_Weapon(fTimeDelta);

	

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
	__super::Array_Control(_int(PART_GROUP::GROUP_DROP_STATIC), _int(PART_GROUP::GROUP_DEBUFF_FILL), CTRL_COMMAND::COM_RENDER, false);

	m_iWeapon_Equip_0_Symbol = m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_EQUIP_NUM)]->PartIndexlist.front()]->iTexture_Index;
	
	return S_OK;
}

HRESULT CUIPage_Play::Render_PlayInfo(CUIRender_Client* pRender_Client)
{
	return S_OK;
}

void CUIPage_Play::Action_Potion_Tool(_float fTimeDelta)
{
	if ((!m_bIsBagOpen)&&(KEY_TAP(KEY::T)))
	{
		if (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio != 0.f)
		{
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio = 0.f;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_TOOL_RKEY)]->fRatio = 0.f;
		}
		else // 이거 나중에 휠로 조정하는 것으로 수정
			GET_GAMEINTERFACE->Change_Potion_Select(true);
	}
	else if ((!m_bIsBagOpen) && (KEY_TAP(KEY::G)))
	{
		if (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio != 1.f)
		{
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio = 1.f;
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_TOOL_RKEY)]->fRatio = 1.f;
		}
		else // 이거 나중에 휠로 조정하는 것으로 수정
			GET_GAMEINTERFACE->Change_Tool_Select(true);
	}
	else if (KEY_HOLD(KEY::E))
	{
		m_fBag_Open_Waiting_Now += fTimeDelta;
		if (m_fBag_Open_Waiting_Now >= m_fBag_Open_Waiting_Limit)
		{
			m_fBag_Open_Waiting_Now = 0.f;
			if (m_bIsBagOpen)
			{
				m_bIsBagOpen = false;
				__super::Array_Control(_int(PART_GROUP::GROUP_POTION_FRAME), _int(PART_GROUP::GROUP_SELECT_CELL), CTRL_COMMAND::COM_RENDER, true);
				__super::Array_Control(_int(PART_GROUP::GROUP_BAG_FRAMELINE), _int(PART_GROUP::GROUP_BAG_NUM), CTRL_COMMAND::COM_RENDER, false);
			}
			else
			{
				m_bIsBagOpen = true;
				__super::Array_Control(_int(PART_GROUP::GROUP_POTION_FRAME), _int(PART_GROUP::GROUP_SELECT_CELL), CTRL_COMMAND::COM_RENDER, false);
				__super::Array_Control(_int(PART_GROUP::GROUP_BAG_FRAMELINE), _int(PART_GROUP::GROUP_BAG_NUM), CTRL_COMMAND::COM_RENDER, true);
			}
		}
	}
	else if (KEY_TAP(KEY::R))
	{
		if ((m_bIsBagOpen) && (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio == 0.f))
			GET_GAMEINTERFACE->Use_Potion_Slot();
		else if ((m_bIsBagOpen) && (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio == 0.f))
			GET_GAMEINTERFACE->Use_Tool_Slot();
		else
		{
			if (KEY_TAP(KEY::NUM1))
				GET_GAMEINTERFACE->Use_Bag_Slot(0);
			if (KEY_TAP(KEY::NUM2))
				GET_GAMEINTERFACE->Use_Bag_Slot(1);
			if (KEY_TAP(KEY::NUM3))
				GET_GAMEINTERFACE->Use_Bag_Slot(2);
			if (KEY_TAP(KEY::NUM4))
				GET_GAMEINTERFACE->Use_Bag_Slot(3);
		}
	}
}

void CUIPage_Play::Action_Arm(_float fTimeDelta)
{
	if (KEY_TAP(KEY::CTRL))
		GET_GAMEINTERFACE->UseItem_Equip(EQUIP_SLOT::EQUIP_RESION_ARM);
}

void CUIPage_Play::Action_Weapon(_float fTimeDelta)
{
	if (KEY_TAP(KEY::TAPKEY))
	{
		GET_GAMEINTERFACE->Change_Weapon();
	}
	else if ((KEY_HOLD(KEY::LSHIFT)) && (KEY_TAP(KEY::F)))
	{
		// 무기 별 페이블 아츠 구현할 것 
	}
	else if (KEY_TAP(KEY::F))
	{
		// 무기 별 페이블 아츠 구현할 것 
	}
}

_bool CUIPage_Play::Action_InterAction(_wstring strInterName)
{
	if (m_bCan_InterAction == false)
		return false;
	else
		m_bCan_InterAction = false;

	__super::Array_Control(_int(PART_GROUP::GROUP_INTER_STATIC), _int(PART_GROUP::GROUP_INTER_TEXT), CTRL_COMMAND::COM_RENDER, true);

	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_INTER_TEXT))->strText = strInterName;

	return KEY_TAP(KEY::E);
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
	if (m_bIsBagOpen)
		return;

	_int iPotion_Select = GET_GAMEINTERFACE->Get_Potion_Select();
	deque<const CItem_Manager::ITEM*> dequeTexture_Index_Potion;
	
	for (_int i = 0; i < 3; ++i)
	{
		if (i == iPotion_Select)
			dequeTexture_Index_Potion.push_front(GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(i + _int(EQUIP_SLOT::EQUIP_USING_TOP_0))));
		else 
			dequeTexture_Index_Potion.push_back(GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(i + _int(EQUIP_SLOT::EQUIP_USING_TOP_0))));
	}

	for (_int i = 0; i < 3; ++i)
	{
		const CItem_Manager::ITEM* pNow = dequeTexture_Index_Potion.front();
		dequeTexture_Index_Potion.pop_front();

		if (pNow == nullptr)
		{
			if (i == 0)
			{
				__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POTION_ITEM))->iTexture_Index = -1;
				m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_COUNT)]->bRender = false;
				//__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POTION_FILL))
			}
			else if (i != 2)
			{
				m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_QUEUE_ITEM)]->PartIndexlist.front()]->iTexture_Index = -1;
			}
			else
				m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_QUEUE_ITEM)]->PartIndexlist.back()]->iTexture_Index = -1;
			continue;
		}

		if (i == 0)
		{
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_COUNT)]->bRender = true;
			__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POTION_ITEM))->iTexture_Index = pNow->iTexture_Index;
			__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POTION_COUNT))->strText = to_wstring(pNow->iCount);
			//__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POTION_FILL))
		}
		else if (i != 2)
		{
			m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_QUEUE_ITEM)]->PartIndexlist.front()]->iTexture_Index = pNow->iTexture_Index;
		}
		else
		{
			m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_QUEUE_ITEM)]->PartIndexlist.back()]->iTexture_Index = pNow->iTexture_Index;
		}
			
	}

	_int iTool_Select = GET_GAMEINTERFACE->Get_Tool_Select();
	deque<const CItem_Manager::ITEM*> dequeTexture_Index_Tool;

	for (_int i = 0; i < 3; ++i)
	{
		if (i == iTool_Select)
			dequeTexture_Index_Tool.push_front(GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(i + _int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0))));

		else
			dequeTexture_Index_Tool.push_back(GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(i + _int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0))));
	}

	for (_int i = 0; i < 3; ++i)
	{
		const CItem_Manager::ITEM* pNow = dequeTexture_Index_Tool.front();
		dequeTexture_Index_Tool.pop_front();

		if (pNow == nullptr)
		{
			if (i == 0)
			{
				__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_TOOL_ITEM))->iTexture_Index = -1;
				m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOOL_COUNT)]->bRender = false;
				__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_TOOL_COUNT))->bRender = false;
				//__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POTION_FILL))
			}
			else if (i != 2)
			{
				m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOOL_QUEUE_ITEM)]->PartIndexlist.front()]->iTexture_Index = -1;
			}
			else
				m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOOL_QUEUE_ITEM)]->PartIndexlist.back()]->iTexture_Index = -1;
			continue;
		}

		if (i == 0)
		{
			m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOOL_COUNT)]->bRender = true;
			__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_TOOL_ITEM))->iTexture_Index = pNow->iTexture_Index;
			__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_TOOL_COUNT))->strText = to_wstring(pNow->iCount);
			//__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POTION_FILL))
		}
		else if (i != 2)
		{
			m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOOL_QUEUE_ITEM)]->PartIndexlist.front()]->iTexture_Index = pNow->iTexture_Index;
		}
		else
		{
			m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOOL_QUEUE_ITEM)]->PartIndexlist.back()]->iTexture_Index = pNow->iTexture_Index;
		}
			
	}		
}

void CUIPage_Play::LD_Bag_Update(_float fTimeDelta)
{
	if (!m_bIsBagOpen)
		return;

	vector<const CItem_Manager::ITEM*> vecItem;

	for (_int i = 0; i < 4; ++ i)
	{
		const CItem_Manager::ITEM* pNow = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(i + _int(EQUIP_SLOT::EQUIP_USING_BAG_0)));
		vecItem.push_back(pNow);
	}

	_int iIndex = 0;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_BAG_ITEM)]->PartIndexlist)
	{
		m_vecPart[iter]->iTexture_Index = vecItem[iIndex]->iTexture_Index;
		++iIndex;
	}

	iIndex = 0;
		
	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_BAG_COUNT)]->PartIndexlist)
	{
		m_vecPart[iter]->strText = to_wstring(vecItem[iIndex]->iCount);
		++iIndex;
	}

	vecItem.clear();
}

void CUIPage_Play::LD_Arm_Update(_float fTimeDelta)
{
	const CItem_Manager::ITEM* pNow = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_RESION_ARM);

	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ARM_NAME))->strText = pNow->strName;
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ARM_TEX))->iTexture_Index = pNow->iItem_Index;
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ARM_GAUGE_FILL))->fRatio = GET_GAMEINTERFACE->Get_Arm_Gauge_Ratio();
}

void CUIPage_Play::RU_Coin_Update(_float fTimeDelta)
{
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_COIN_COUNT))->strText = to_wstring(GET_GAMEINTERFACE->Get_Coin());
}

void CUIPage_Play::RD_Weapon_Update(_float fTimeDelta)
{
	const CItem_Manager::ITEM* pNowBlade = GET_GAMEINTERFACE->Get_Now_Equip_Weapon_Blade();
	const CItem_Manager::ITEM* pNowHandle = GET_GAMEINTERFACE->Get_Now_Equip_Weapon_Handle();
	_int iSelect = GET_GAMEINTERFACE->Get_Weapon();
	_bool bNormal = pNowBlade->bModule_Weapon;

	__super::Array_Control(_int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK), _int(PART_GROUP::GROUP_WEAPON_NORMAL_HANDLE), CTRL_COMMAND::COM_RENDER, bNormal);
	__super::Array_Control(_int(PART_GROUP::GROUP_WEAPON_SPECIAL_BACK), _int(PART_GROUP::GROUP_WEAPON_SPECIAL_TEX), CTRL_COMMAND::COM_RENDER, !bNormal);
	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_EQUIP_NUM)]->PartIndexlist.front()]->iTexture_Index = m_iWeapon_Equip_0_Symbol + iSelect;

	_bool bSingle_Cell_Blade = pNowBlade->iFable_Art_Cost ? 1 : true;

	__super::Array_Control(_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_FRAME), _int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_SIDE_FILL), CTRL_COMMAND::COM_RENDER, !bSingle_Cell_Blade);
	__super::Array_Control(_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_FRAME), _int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_SIDE_FILL), CTRL_COMMAND::COM_RENDER, bSingle_Cell_Blade);
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_KEYSET_A)]->bRender = bSingle_Cell_Blade;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_LEFT_KEYSET_B)]->bRender = !bSingle_Cell_Blade;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_KEYSET_A)]->bRender = !bSingle_Cell_Blade;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_KEYSET_B)]->bRender = bSingle_Cell_Blade;
}

void CUIPage_Play::Boss_Hp_Update(_float fTimeDelta)
{
	if (!m_bIs_BossHp_Activate)
	{
		__super::Array_Control(_int(PART_GROUP::GROUP_BOSS_NAME), _int(PART_GROUP::GROUP_BOSS_HP_FILL), CTRL_COMMAND::COM_RENDER, false);
	}
	else
	{
		__super::Array_Control(_int(PART_GROUP::GROUP_BOSS_NAME), _int(PART_GROUP::GROUP_BOSS_HP_FILL), CTRL_COMMAND::COM_RENDER, true);

		__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ARM_NAME))->strText = m_strBossName;
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_BOSS_HP_FILL))->fRatio = _float(m_fBoss_Hp_Now) / _float(m_fBoss_Hp_Max);
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_BOSS_HP_FILL))->MovePart(__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ARM_NAME))->fPosition, fTimeDelta);
	}
}

void CUIPage_Play::PlayInfo_Update(_float fTimeDelta)
{
	Add_Render_Info_DropInfo(fTimeDelta);
	Add_Render_Info_BuffInfo(fTimeDelta);
}

void CUIPage_Play::Add_Render_Info_DropInfo(_float fTimeDelta)
{
	_float fAlpha_Ratio_Origin = m_fTopPartMove;
	_float fHeight = __super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DROP_STATIC))->fSize.y * 1.1f;

	for (list<DROP_ITEM_INFO*>::iterator iter = m_DropItem_Index_list.begin(); iter != m_DropItem_Index_list.end(); )
	{
		(*iter)->fLifeTime -= fTimeDelta;

		if ((*iter)->fLifeTime <= 0.f)
		{
			Safe_Delete(*iter);
			iter = m_DropItem_Index_list.erase(iter);
		}
		else
		{
			for (_int i = _int(PART_GROUP::GROUP_DROP_STATIC); i <= _int(PART_GROUP::GROUP_DROP_NAME); ++i)
			{
				if (i == _int(PART_GROUP::GROUP_DROP_ITEM))
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DROP_ITEM))->iTexture_Index = GET_GAMEINTERFACE->Get_Item_Origin_Spec((*iter)->iIndex)->iTexture_Index;

				if (i == _int(PART_GROUP::GROUP_DROP_NAME))
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DROP_NAME))->strText = GET_GAMEINTERFACE->Get_Item_Origin_Spec((*iter)->iIndex)->strName;

				if (m_fDrop_Item_ShowTime - (*iter)->fLifeTime < m_fEmerge_Effect_Time)
					m_fTopPartMove = (m_fDrop_Item_ShowTime - (*iter)->fLifeTime) / m_fEmerge_Effect_Time;
				else if ((*iter)->fLifeTime < m_fEmerge_Effect_Time)
					m_fTopPartMove = (*iter)->fLifeTime / m_fEmerge_Effect_Time;
				

				for (auto& iterIndex : m_vec_Group_Ctrl[i]->PartIndexlist)
				{
					__super::Input_Render_Info(*m_vecPart[iterIndex]);
					m_vecPart[iterIndex]->fPosition.y -= fHeight;
				}
			}

			++iter;
		}
	}

	m_fTopPartMove = fAlpha_Ratio_Origin;
}

void CUIPage_Play::Add_Render_Info_BuffInfo(_float fTimeDelta)
{
	_float fAlpha_Ratio_Origin = m_fTopPartMove;
	_float fHeight = __super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DEBUFF_STATIC))->fSize.y * 1.1f;

	for (_int i = 0; i < _int(BUFF_INDEX::BUFF_END); ++i)
	{
		if (GET_GAMEINTERFACE->Get_Buff_Ratio(BUFF_INDEX(i)) > 0.f)
		{
			for (_int j = _int(PART_GROUP::GROUP_DEBUFF_STATIC); j <= _int(PART_GROUP::GROUP_DEBUFF_FILL); ++j)
			{
				if (j == _int(PART_GROUP::GROUP_DEBUFF_ICON))
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DEBUFF_ICON))->iTexture_Index = GET_GAMEINTERFACE->Get_Buff_Info(BUFF_INDEX(i))->iTexture_Index;

				if (j == _int(PART_GROUP::GROUP_DEBUFF_NAME))
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DEBUFF_NAME))->strText = GET_GAMEINTERFACE->Get_Buff_Info(BUFF_INDEX(i))->strBuff_Name;

				if (j == _int(PART_GROUP::GROUP_DEBUFF_FILL))
				{
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DEBUFF_FILL))->fRatio = GET_GAMEINTERFACE->Get_Buff_Ratio(BUFF_INDEX(i));
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DEBUFF_FILL))->fTextureColor = GET_GAMEINTERFACE->Get_Buff_Info(BUFF_INDEX(i))->vTexture_Color;
					_Vec2 vPos = m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_DEBUFF_STATIC)]->PartIndexlist.back()]->fPosition;
					vPos.y += fHeight;

					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DEBUFF_FILL))->MovePart(vPos, fTimeDelta);
				}
					

				for (auto& iterIndex : m_vec_Group_Ctrl[j]->PartIndexlist)
				{
					__super::Input_Render_Info(*m_vecPart[iterIndex]);
					m_vecPart[iterIndex]->fPosition.y -= fHeight;
				}
			}
		}
	}

	m_fTopPartMove = fAlpha_Ratio_Origin;
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
		Safe_Delete(iter);

	for (auto& iter : m_DropItem_Index_list)
		Safe_Delete(iter);

	m_DropItem_Index_list.clear();

	m_vecPart.clear();
}
