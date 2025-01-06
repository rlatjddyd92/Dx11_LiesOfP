#include "stdafx.h"
#include "..\Public\UIPage_Play.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"
#include "UIPlay_Weapon.h"

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
	if (KEY_TAP(KEY::O))
		GET_GAMEINTERFACE->Set_Potion_Count_Full();

	LU_Gauge_Update(fTimeDelta);
	LD_Potion_Tool_Update(fTimeDelta);
	LD_Bag_Update(fTimeDelta);
	LD_Arm_Update(fTimeDelta);
	RU_Coin_Update(fTimeDelta);
	RD_Weapon_Update(fTimeDelta);
	STAT_Page_Update(fTimeDelta);

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CTRL)]->bRender = m_bIsKeyGuideOn;

	for (_int i = 0; i < m_vec_Group_Ctrl.size(); ++i)
	{
		if (i == _int(PART_GROUP::GROUP_POTION_FILL))
			continue;

		if ((i >= _int(PART_GROUP::GROUP_WEAPON_DURABLE_FRAME)) && (i <= _int(PART_GROUP::GROUP_WEAPON_GAUGE_RIGHT_KEYSET_B)))
			continue;

		__super::UpdatePart_ByControl(m_vec_Group_Ctrl[i]);
	}

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

	if (m_bIsStart == true)
	{
		m_bIsStart = false;
		GET_GAMEINTERFACE->Show_Region_Info(TEXT("아르케 대수도원"), TEXT("흐린 날씨"));
	}
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

	for (auto& iter : m_vec_Group_Ctrl)
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

	_int iIndex = -1;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK_FX)]->PartIndexlist)
	{
		++iIndex;
		m_fNormal_Weapon_Fx_Alpha_Origin[iIndex] = m_vecPart[iter]->fTextureColor.w;
		m_vNormal_Weapon_Fx_Size_Origin[iIndex] = m_vecPart[iter]->fSize;
	}

	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_HP_NUM))->iParentPart_Index = -1;
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_HP_NUM))->fAdjust = __super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_HP_NUM))->fPosition - _Vec2{ 640,360 };


	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ST_NUM))->iParentPart_Index = -1;
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ST_NUM))->fAdjust = __super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ST_NUM))->fPosition - _Vec2{ 640,360 };

	m_pPlayCom_Weapon = CUIPlay_Weapon::Create(m_pDevice, m_pContext);
	m_pPlayCom_Weapon->Initialize_Weapon_Component(m_vecPart);

	return S_OK;
}

HRESULT CUIPage_Play::Render_PlayInfo(CUIRender_Client* pRender_Client)
{
	return S_OK;
}

void CUIPage_Play::SetWeaponLock(_bool bIsWeaponLock)
{
	if (m_bIsWeapon_Lock[1] != bIsWeaponLock)
		m_bIsWeapon_Lock[1] = bIsWeaponLock;

	if (m_bIsWeapon_Lock[1])
	{
		m_pSoundCom->Play2D(TEXT("SE_UI_WeaponChange_04.wav"), &g_fUIVolume);
		GET_GAMEINTERFACE->Change_Weapon();
	}
}

void CUIPage_Play::Action_Potion_Tool(_float fTimeDelta)
{
	if (m_bIsItemLock == true)
		return;

	_bool bUseGrinder = false;

	if ((!m_bIsBagOpen) && (KEY_TAP(KEY::T)))
	{
		m_pSoundCom->Play2D(TEXT("SE_UI_Tap_01.wav"), &g_fUIVolume);

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
		m_pSoundCom->Play2D(TEXT("SE_UI_Tap_01.wav"), &g_fUIVolume);

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
		
			m_pSoundCom->Play2D(TEXT("SE_UI_Move_04.wav"), &g_fUIVolume);
			
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



	if (m_bIsBagOpen)
	{
		if (KEY_TAP(KEY::NUM1))
			GET_GAMEINTERFACE->Use_Bag_Slot(0);
		if (KEY_TAP(KEY::NUM2))
			GET_GAMEINTERFACE->Use_Bag_Slot(1);
		if (KEY_TAP(KEY::NUM3))
			GET_GAMEINTERFACE->Use_Bag_Slot(2);
		if (KEY_TAP(KEY::NUM4))
			GET_GAMEINTERFACE->Use_Bag_Slot(3);

		if (KEY_HOLD(KEY::NUM1))
		{
			if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_USING_BAG_0) != nullptr)
				if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_USING_BAG_0)->iItem_Index == _int(SPECIAL_ITEM::SP_GRINDER))
				{
					m_vGrinder_Wait.x += fTimeDelta;
					bUseGrinder = true;
					if (m_vGrinder_Wait.x > m_vGrinder_Wait.y)
						GET_GAMEINTERFACE->Use_Bag_Slot(0);
				}
		}
		if (KEY_HOLD(KEY::NUM2))
		{
			if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_USING_BAG_1) != nullptr)
				if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_USING_BAG_1)->iItem_Index == _int(SPECIAL_ITEM::SP_GRINDER))
				{
					m_vGrinder_Wait.x += fTimeDelta;
					bUseGrinder = true;
					if (m_vGrinder_Wait.x > m_vGrinder_Wait.y)
						GET_GAMEINTERFACE->Use_Bag_Slot(1);
				}
		}
		if (KEY_HOLD(KEY::NUM3))
		{
			if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_USING_BAG_2) != nullptr)
				if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_USING_BAG_2)->iItem_Index == _int(SPECIAL_ITEM::SP_GRINDER))
				{
					m_vGrinder_Wait.x += fTimeDelta;
					bUseGrinder = true;
					if (m_vGrinder_Wait.x > m_vGrinder_Wait.y)
						GET_GAMEINTERFACE->Use_Bag_Slot(2);
				}
		}
		if (KEY_HOLD(KEY::NUM4))
		{
			if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_USING_BAG_3) != nullptr)
				if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_USING_BAG_3)->iItem_Index == _int(SPECIAL_ITEM::SP_GRINDER))
				{
					m_vGrinder_Wait.x += fTimeDelta;
					bUseGrinder = true;
					if (m_vGrinder_Wait.x > m_vGrinder_Wait.y)
						GET_GAMEINTERFACE->Use_Bag_Slot(3);
				}
		}
	}
	else if (KEY_TAP(KEY::R))
	{
		if ((!m_bIsBagOpen) && (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio == 0.f))
		{
			_int iPotion = GET_GAMEINTERFACE->Get_Potion_Select();
			if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_TOP_0) + iPotion)) != nullptr)
			{
				if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_TOP_0) + iPotion))->iItem_Index == _int(SPECIAL_ITEM::SP_TELEPOT))
				{
					
				}
				else 
					GET_GAMEINTERFACE->Use_Potion_Slot();
			}
			else
				GET_GAMEINTERFACE->Use_Potion_Slot();
		}
		else if ((!m_bIsBagOpen) && (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio == 1.f))
		{
			_int iTool = GET_GAMEINTERFACE->Get_Tool_Select();
			if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0) + iTool)) != nullptr)
			{
				if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0) + iTool))->iItem_Index == _int(SPECIAL_ITEM::SP_TELEPOT))
				{
					
				}
				else
					GET_GAMEINTERFACE->Use_Tool_Slot();
			}
			else
				GET_GAMEINTERFACE->Use_Tool_Slot();
		}
			
	}
	else if (KEY_HOLD(KEY::R))
	{
		_int iPotion = GET_GAMEINTERFACE->Get_Potion_Select();
		_int iTool = GET_GAMEINTERFACE->Get_Tool_Select();

		if ((!m_bIsBagOpen) && (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio == 0.f))
		{
			if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_TOP_0) + iPotion)) != nullptr)
				if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_TOP_0) + iPotion))->iItem_Index == _int(SPECIAL_ITEM::SP_GRINDER))
				{
					m_vGrinder_Wait.x += fTimeDelta;
					bUseGrinder = true;
					if (m_vGrinder_Wait.x > m_vGrinder_Wait.y)
						GET_GAMEINTERFACE->Use_Potion_Slot();
				}
		}
		else if ((!m_bIsBagOpen) && (m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio == 1.f))
		{
			if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0) + iTool)) != nullptr)
				if (GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0) + iTool))->iItem_Index == _int(SPECIAL_ITEM::SP_GRINDER))
				{
					m_vGrinder_Wait.x += fTimeDelta;
					bUseGrinder = true;
					if (m_vGrinder_Wait.x > m_vGrinder_Wait.y)
						GET_GAMEINTERFACE->Use_Tool_Slot();
				}
		}
					
	}

	if (!GET_GAMEINTERFACE->Get_LastFrame_UsingItem_Info().empty())
	if (GET_GAMEINTERFACE->Get_LastFrame_UsingItem_Info().back() == SPECIAL_ITEM::SP_GRINDER)
		GET_GAMEINTERFACE->Add_Durable_Weapon(fTimeDelta * 10.f);

	if (bUseGrinder == false)
		m_vGrinder_Wait.x = 0.f;

	GET_GAMEINTERFACE->Set_Select(!_bool(m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_SELECT_CELL)]->fRatio));
}

void CUIPage_Play::Action_Arm(_float fTimeDelta)
{
	// 무기 조작은 플레이어로 
}

void CUIPage_Play::Action_Weapon(_float fTimeDelta)
{
	if (m_bIsWeapon_Lock[0] + m_bIsWeapon_Lock[1] == 2)
		return;

	if (KEY_TAP(KEY::TAPKEY))
	{
		_bool bIsNormal = false;

		const CItem_Manager::ITEM* pNow = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0);

		if (pNow != nullptr)
			_bool bIsNormal = pNow->bModule_Weapon;

		if (!bIsNormal)
		{
			pNow = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_WEAPON_BLADE_1);
			bIsNormal = pNow->bModule_Weapon;
		}
		
		m_vSwitch_Time.x = 0.01f;
	}

	m_bIsWeapon_Lock[0] = m_bIsWeapon_Lock[1];
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
	const CPlayer::STAT_INFO& Info = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat();
	CPlayer::STAT_INFO* pAdjust = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust();

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_HP_FRAME)]->fRatio = (Info.vGauge_Hp.z + pAdjust->vGauge_Hp.z) / Info.vGauge_Hp.w;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_PLAYER_HP_SECOND)]->fRatio = Info.vGauge_Hp.y / Info.vGauge_Hp.w;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_HP_FILL)]->fRatio = Info.vGauge_Hp.x / Info.vGauge_Hp.w;

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ST_FRAME)]->fRatio = (Info.vGauge_Stamina.z + pAdjust->vGauge_Stamina.z)/ Info.vGauge_Stamina.w;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_PLAYER_STAMINA_SECOND)]->fRatio = Info.vGauge_Stamina.y / Info.vGauge_Stamina.w;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ST_FILL)]->fRatio = Info.vGauge_Stamina.x  / Info.vGauge_Stamina.w;

	_int iSP_Cell_Num = (_int)((_float)Info.vGauge_Region.z / (_float)Info.fRegion_Interval);
	_int iSP_Cell_Filled = (_int)((_float)Info.vGauge_Region.x / (_float)Info.fRegion_Interval);
	_float fSP_Ratio = (Info.vGauge_Region.x - ((_float)iSP_Cell_Filled * Info.fRegion_Interval)) / (_float)Info.fRegion_Interval;

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

	m_iFable_Art_Cell_Now = iSP_Cell_Filled;

	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_HP_NUM))->bRender = false;
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_HP_NUM))->strText = {};
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ST_NUM))->bRender = false;
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ST_NUM))->strText = {};
}

void CUIPage_Play::LD_Potion_Tool_Update(_float fTimeDelta)
{
	if (m_bIsBagOpen)
		return;

	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_FILL)]->PartIndexlist.front()]->fRatio = 0.f;
	m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_FILL)]->PartIndexlist.back()]->fRatio = 0.f;
	

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
			__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POTION_ITEM))->m_bEmpty_Stack_Item = false;
			__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POTION_ITEM))->iTexture_Index = pNow->iTexture_Index;

			if (pNow->bStack)
			{
				m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_COUNT)]->bRender = true;
				__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POTION_COUNT))->strText = to_wstring(pNow->iCount);
				if (pNow->iCount <= 0)
				{
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_POTION_ITEM))->m_bEmpty_Stack_Item = true;
					if (pNow->iItem_Index == _int(SPECIAL_ITEM::SP_PULSE_BATTERY))
						Set_Potion_Gauge(true);
				}
					
			}
			else
				m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_COUNT)]->bRender = false;
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
			__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_TOOL_ITEM))->m_bEmpty_Stack_Item = false;
			__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_TOOL_ITEM))->iTexture_Index = pNow->iTexture_Index;
			if (pNow->bStack)
			{
				m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOOL_COUNT)]->bRender = true;
				__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_TOOL_COUNT))->strText = to_wstring(pNow->iCount);
				if (pNow->iCount <= 0)
				{
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_TOOL_ITEM))->m_bEmpty_Stack_Item = true;
					if (pNow->iItem_Index == _int(SPECIAL_ITEM::SP_PULSE_BATTERY))
						Set_Potion_Gauge(false);
				}
					
			}
			else
				m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOOL_COUNT)]->bRender = false;
				
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
	if (m_bIsBagOpen)
	{
		if (m_vBag_OpenAction_Time.x < m_vBag_OpenAction_Time.y)
			m_vBag_OpenAction_Time.x = min(m_vBag_OpenAction_Time.x + fTimeDelta, m_vBag_OpenAction_Time.y);
	}
	else
	{
		if (m_vBag_OpenAction_Time.x > 0.f)
			m_vBag_OpenAction_Time.x = max(m_vBag_OpenAction_Time.x - fTimeDelta, 0.f);
	}
	
	Switch_Bag_UI_Action(fTimeDelta, m_bIsBagOpen);

	if ((!m_bIsBagOpen) && (m_vBag_OpenAction_Time.x == 0.f))
		return;

	vector<const CItem_Manager::ITEM*> vecItem;

	for (_int i = 0; i < 4; ++i)
	{
		const CItem_Manager::ITEM* pNow = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(i + _int(EQUIP_SLOT::EQUIP_USING_BAG_0)));
		vecItem.push_back(pNow);
	}

	_int iIndex = 0;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_BAG_ITEM)]->PartIndexlist)
	{
		if (vecItem[iIndex] != nullptr)
		{
			m_vecPart[iter]->iTexture_Index = vecItem[iIndex]->iTexture_Index;

			if (vecItem[iIndex]->bStack)
				if (vecItem[iIndex]->iCount <= 0)
					m_vecPart[iter]->m_bEmpty_Stack_Item = true;
			
			if (!vecItem[iIndex]->bStack)
				m_vecPart[iter]->bRender = false;
		}
		else
			m_vecPart[iter]->bRender = false;
		++iIndex;
	}

	iIndex = 0;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_BAG_COUNT)]->PartIndexlist)
	{
		if (vecItem[iIndex] != nullptr)
		{
			if (vecItem[iIndex]->bStack)
			{
				m_vecPart[iter]->bRender = true;
				m_vecPart[iter]->strText = to_wstring(vecItem[iIndex]->iCount);	
			}
			m_vecPart[iter]->bRender = false;
		}
		else
			m_vecPart[iter]->bRender = false;

		++iIndex;
	}

	vecItem.clear();
}

void CUIPage_Play::LD_Arm_Update(_float fTimeDelta)
{
	const CItem_Manager::ITEM* pNow = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_RESION_ARM);

	if (pNow == nullptr)
		return;

	_Vec2 vGauge = *GET_GAMEINTERFACE->Get_Player()->Get_ArmGuage_Ptr();

	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ARM_NAME))->strText = pNow->strName;
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_ARM_TEX))->iTexture_Index = pNow->iTexture_Index;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ARM_GAUGE_FILL)]->fRatio = vGauge.x / vGauge.y;
}

void CUIPage_Play::RU_Coin_Update(_float fTimeDelta)
{
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_COIN_COUNT))->strText = to_wstring(GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat().iErgo);
}

void CUIPage_Play::RD_Weapon_Update(_float fTimeDelta)
{
	m_pPlayCom_Weapon->Update_WeaponInfo(m_iFable_Art_Cell_Now, fTimeDelta);
}

void CUIPage_Play::STAT_Page_Update(_float fTimeDelta)
{
	if (KEY_TAP(KEY::F3))
	{
		m_bStat_Open = !m_bStat_Open;
	}

	if ((m_bStat_Open) && (m_vStat_Open_Time.x < m_vStat_Open_Time.y))
	{
		m_vStat_Open_Time.x += fTimeDelta;
		m_vStat_Open_Time.x = min(m_vStat_Open_Time.x, m_vStat_Open_Time.y);
	}

	if ((!m_bStat_Open) && (m_vStat_Open_Time.x > 0.f))
	{
		m_vStat_Open_Time.x -= fTimeDelta;
		m_vStat_Open_Time.x = max(m_vStat_Open_Time.x, 0.f);
	}

	if (m_vStat_Open_Time.x == 0.f)
		return;

	vector<const CItem_Manager::ITEM*> vecItem;

	for (_int i = _int(EQUIP_SLOT::EQUIP_AMULET_0); i <= _int(EQUIP_SLOT::EQUIP_DEFENCE_RAINER); ++i)
	{
		const CItem_Manager::ITEM* pNow = GET_GAMEINTERFACE->Get_Equip_Item_Info(EQUIP_SLOT(i));
		vecItem.push_back(pNow);
	}

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_AMULET_DEFENCE_STATIC)]->fRatio = m_vStat_Open_Time.x / m_vStat_Open_Time.y;

	for (auto & iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_AMULET_DEFENCE_STATIC)]->PartIndexlist)
	{
		m_vecPart[iter]->bRender = true;
	}

	_int iIndex = 0;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_AMULET_DEFENCE_ITEM)]->PartIndexlist)
	{
		if (vecItem[iIndex])
		{
			m_vecPart[iter]->bRender = true;
			m_vecPart[iter]->iTexture_Index = vecItem[iIndex]->iTexture_Index;
		}
		else 
			m_vecPart[iter]->bRender = false;

		++iIndex;
	}

	iIndex = 0;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_AMULET_DEFENCE_NAME)]->PartIndexlist)
	{
		if (vecItem[iIndex])
		{
			m_vecPart[iter]->bRender = true;
			m_vecPart[iter]->strText = vecItem[iIndex]->strName;
		}
		else
			m_vecPart[iter]->bRender = false;

		++iIndex;
	}

	iIndex = 0;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_AMULET_STAT_ICON)]->PartIndexlist)
	{
		if (vecItem[iIndex])
		{
			m_vecPart[iter]->bRender = true;
		}
		else
			m_vecPart[iter]->bRender = false;

		++iIndex;
	}

	iIndex = 0;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_AMULET_DEFENCE_DESC)]->PartIndexlist)
	{
		if (vecItem[iIndex])
		{
			m_vecPart[iter]->bRender = true;
			m_vecPart[iter]->strText = vecItem[iIndex]->strItem_Desc;
		}
		else
			m_vecPart[iter]->bRender = false;

		++iIndex;
	}

	iIndex = 0;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_AMULET_DEFENCE_STAT)]->PartIndexlist)
	{
		m_vecPart[iter]->bRender = false;

		if (vecItem[iIndex] == nullptr)
		{
			m_vecPart[iter]->bRender = false;
		}
		else if ((iIndex == 0) || (iIndex == 1))
		{
			if (vecItem[iIndex]->iItem_Index == 88)
			{
				m_vecPart[iter]->bRender = true;
				m_vecPart[iter]->strText = TEXT("+ 200");
			}
			else if (vecItem[iIndex]->iItem_Index == 101)
			{
				m_vecPart[iter]->bRender = true;
				m_vecPart[iter]->strText = TEXT("+ 200");
			}
			else if (vecItem[iIndex]->iItem_Index == 111)
			{
				m_vecPart[iter]->bRender = true;
				m_vecPart[iter]->strText = TEXT("+ 10");
			}
			else if (vecItem[iIndex]->iItem_Index == 113)
			{
				m_vecPart[iter]->bRender = true;
				m_vecPart[iter]->strText = TEXT("+ 30");
			}
		}
		else if (iIndex == 2)
		{
		m_vecPart[iter]->bRender = true;
				m_vecPart[iter]->strText = to_wstring(_int(vecItem[iIndex]->vecDamege[0]));
		}
		else if (iIndex == 3)
		{
			m_vecPart[iter]->bRender = true;
			m_vecPart[iter]->strText = to_wstring(_int(vecItem[iIndex]->vecDefence[0]));
		}
		else if (iIndex == 4)
		{
			m_vecPart[iter]->bRender = true;
			m_vecPart[iter]->strText = to_wstring(_int(vecItem[iIndex]->vecDefence[1]));
		}
		else if (iIndex == 5)
		{
			m_vecPart[iter]->bRender = true;
			m_vecPart[iter]->strText = to_wstring(_int(vecItem[iIndex]->vecDefence[2]));
		}
		else
			m_vecPart[iter]->strText = vecItem[iIndex]->strItem_Desc;
		
		++iIndex;
	}

	_int iStart = __super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_AMULET_DEFENCE_STATIC));
	_int iEnd = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_AMULET_DEFENCE_STAT)]->PartIndexlist.back();
		
	for (_int i = iStart; i <= iEnd; ++i)
	{
		UpdatePart_ByIndex(i, fTimeDelta);

		if (m_vecPart[i]->bRender)
		{
			_float TextOrigin = m_vecPart[i]->fTextColor.w;
			_float TextureOrigin = m_vecPart[i]->fTextureColor.w;

			m_vecPart[i]->fTextColor.w *= m_vStat_Open_Time.x / m_vStat_Open_Time.y;
			m_vecPart[i]->fTextureColor.w *= m_vStat_Open_Time.x / m_vStat_Open_Time.y;

			__super::Input_Render_Info(*m_vecPart[i]);

			m_vecPart[i]->fTextColor.w = TextOrigin;
			m_vecPart[i]->fTextureColor.w = TextureOrigin;
		}
	}

	__super::Array_Control(_int(PART_GROUP::GROUP_AMULET_DEFENCE_STATIC), _int(PART_GROUP::GROUP_AMULET_DEFENCE_STAT), CTRL_COMMAND::COM_RENDER, false);
}

void CUIPage_Play::Boss_Hp_Update(_float fTimeDelta)
{
	if (!m_bIs_BossHp_Activate)
	{
		__super::Array_Control(_int(PART_GROUP::GROUP_BOSS_NAME), _int(PART_GROUP::GROUP_BOSS_HP_FILL), CTRL_COMMAND::COM_RENDER, false);
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_BOSS_HP_SECOND))->iTexture_Index = -1;
	}
	else
	{
		__super::Array_Control(_int(PART_GROUP::GROUP_BOSS_NAME), _int(PART_GROUP::GROUP_BOSS_HP_FILL), CTRL_COMMAND::COM_RENDER, true);

		__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_BOSS_NAME))->strText = m_strBossName;
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_BOSS_HP_FILL))->fRatio = _float(m_fBoss_Hp_Now) / _float(m_fBoss_Hp_Max);
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_BOSS_HP_SECOND))->fRatio = 0.f; // <- 임시 수치 
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_BOSS_HP_FILL))->MovePart(__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_BOSS_NAME))->fPosition, fTimeDelta);
		__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_BOSS_HP_SECOND))->MovePart(__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_BOSS_NAME))->fPosition, fTimeDelta);

		m_bIs_BossHp_Activate = false;
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
	const CPlayer::STAT_INFO& Info = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat();

	_float fAlpha_Ratio_Origin = m_fTopPartMove;
	_float fHeight = __super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DEBUFF_STATIC))->fSize.y * 1.1f;

	for (_int i = 0; i < _int(BUFF_INDEX::BUFF_END); ++i)
	{
		_Vec2 vBuff = { 0.f,0.f };

		if (i == _int(BUFF_INDEX::BUFF_FIRE))
			vBuff = Info.fDebuff_Fire;
		if (i == _int(BUFF_INDEX::BUFF_ELECTRIC))
			vBuff = Info.fDebuff_Electric;
		if (i == _int(BUFF_INDEX::BUFF_AICD))
			vBuff = Info.fDebuff_Acid;


		if (vBuff.x > 0.f)
		{
			for (_int j = _int(PART_GROUP::GROUP_DEBUFF_STATIC); j <= _int(PART_GROUP::GROUP_DEBUFF_FILL); ++j)
			{
				if (j == _int(PART_GROUP::GROUP_DEBUFF_ICON))
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DEBUFF_ICON))->iTexture_Index = GET_GAMEINTERFACE->Get_Buff_Info(BUFF_INDEX(i))->iTexture_Index;

				if (j == _int(PART_GROUP::GROUP_DEBUFF_NAME))
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DEBUFF_NAME))->strText = GET_GAMEINTERFACE->Get_Buff_Info(BUFF_INDEX(i))->strBuff_Name;

				if (j == _int(PART_GROUP::GROUP_DEBUFF_FILL))
				{
					__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_DEBUFF_FILL))->fRatio = vBuff.x / vBuff.y;
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

void CUIPage_Play::Switch_Weapon_UI_Action(_float fTimeDelta)
{
	if (m_vSwitch_Time.x > 0.f)
	{
		_float fRatio = (m_vSwitch_Time.x / m_vSwitch_Time.y);
		m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK_FX)]->bRender = true;
		_int iCount = -1;
		for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK_FX)]->PartIndexlist)
		{
			++iCount;
			m_vecPart[iter]->fTextureColor.w = m_fNormal_Weapon_Fx_Alpha_Origin[iCount] * (0.5f + fRatio);
			if (iCount % 2 == 0) m_vecPart[iter]->fSize = m_vNormal_Weapon_Fx_Size_Origin[iCount] * (0.5f + fRatio);
		}
	}
	else
	{
		m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_WEAPON_NORMAL_BACK_FX)]->bRender = false;
	}
}

void CUIPage_Play::Switch_Bag_UI_Action(_float fTimeDelta, _bool bIsOpen)
{
	for (_int i = _int(PART_GROUP::GROUP_BAG_FRAMELINE); i <= _int(PART_GROUP::GROUP_BAG_NUM); ++i)
	{
		m_vec_Group_Ctrl[i]->fRatio = m_vBag_OpenAction_Time.x / m_vBag_OpenAction_Time.y;
		for (auto& iter : m_vec_Group_Ctrl[i]->PartIndexlist)
		{
			if (i == _int(PART_GROUP::GROUP_BAG_FRAMELINE))
				m_vecPart[iter]->fTextureColor.w = 0.5f * (m_vBag_OpenAction_Time.x / m_vBag_OpenAction_Time.y);
			else 
				m_vecPart[iter]->fTextureColor.w = (m_vBag_OpenAction_Time.x / m_vBag_OpenAction_Time.y);
		}
	}
}

void CUIPage_Play::Using_Fable_UI_Action(_float fTimeDelta)
{
	
}

void CUIPage_Play::Set_Potion_Gauge(_bool bIsTop)
{
	_float fRatio = GET_GAMEINTERFACE->Get_Potion_Gauge_Ratio();

	if (bIsTop == true)
	{
		m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_FILL)]->PartIndexlist.front()]->fRatio = fRatio;
		m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_FILL)]->PartIndexlist.back()]->fRatio = 0.f;
	}
	else
	{
		m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_FILL)]->PartIndexlist.front()]->fRatio = 0.f;
		m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POTION_FILL)]->PartIndexlist.back()]->fRatio = fRatio;
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
		Safe_Delete(iter);

	for (auto& iter : m_DropItem_Index_list)
		Safe_Delete(iter);

	m_DropItem_Index_list.clear();

	m_vecPart.clear();

	Safe_Release(m_pPlayCom_Weapon);
}