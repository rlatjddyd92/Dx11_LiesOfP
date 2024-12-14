#include "stdafx.h"
#include "..\Public\UIPage_Inven.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Inven::CUIPage_Inven(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Inven::CUIPage_Inven(const CUIPage_Inven& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Inven::Initialize_Prototype()
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

HRESULT CUIPage_Inven::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Inven::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Inven::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Inven::Late_Update(_float fTimeDelta)
{
	if (m_pScroll == nullptr)
	{
		m_pScroll = new SCROLL;
		m_pScroll->Initialize_Scroll(m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ITEMINFO_FRAME))], m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ITEMINFO_SLIDE_BAR))], SCROLL_AREA::SCROLL_INVEN);
	}

	if ((KEY_TAP(KEY::RBUTTON))&&(m_bReset_ItemAction_Info == true))
		Reset_ItemAction();



	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);

	Update_Top_Part(fTimeDelta);

	__super::Array_Control(_int(PART_GROUP::GROUP_ITEMINFO_SLIDE), _int(PART_GROUP::GROUP_ITEMINFO_SLIDE_BAR), CTRL_COMMAND::COM_RENDER, false);
	__super::Array_Control(_int(PART_GROUP::GROUP_ARRAY_FRAME), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RENDER, false);

	__super::Late_Update(fTimeDelta);

	if (m_bRender)
		Update_Array_Position(fTimeDelta);
	if (m_pScroll->bIsActive)
		Update_Slide(fTimeDelta);

	if (m_vFocus_Pos.x != -1)
	{
		GET_GAMEINTERFACE->Show_Focus(m_vFocus_Pos, m_vFocus_Size);
	}
	

	GET_GAMEINTERFACE->Select_Scroll_Area(SCROLL_AREA::SCROLL_NONE);

	
	m_IsTab_Change = false;
	m_bReset_ItemAction_Info = true;
}

HRESULT CUIPage_Inven::Render()
{
	return S_OK;
}

void CUIPage_Inven::OpenAction()
{
	__super::OpenAction();
	m_IsTab_Change = true;
	GET_GAMEINTERFACE->Off_ItemInfo_UI();
	m_pSoundCom->Play2D(TEXT("SE_UI_OpenMenu_04.wav"), &g_fUIVolume);
	Reset_ItemAction();
}

void CUIPage_Inven::CloseAction()
{
	__super::CloseAction();
	GET_GAMEINTERFACE->Set_Show_NewMark_Off();
	GET_GAMEINTERFACE->Off_ItemInfo_UI();
	m_pSoundCom->Play2D(TEXT("SE_UI_CloseWindow_01.wav"), &g_fUIVolume);
	Reset_ItemAction();
}

CHECK_MOUSE CUIPage_Inven::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	Action_Inven_Page(fTimeDelta);
	Action_Focus(fTimeDelta);
	if (m_pScroll->bIsActive)
		Action_Slide(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Inven::Render_Inven_Array(class CUIRender_Client* pRender_Client)
{
	return E_NOTIMPL;
}

HRESULT CUIPage_Inven::Ready_UIPart_Group_Control()
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
	__super::Array_Control(_int(PART_GROUP::GROUP_BACK), _int(PART_GROUP::GROUP_TOP_MOUSE), CTRL_COMMAND::COM_RENDER, true);
	__super::Array_Control(_int(PART_GROUP::GROUP_ITEMINFO_SLIDE), _int(PART_GROUP::GROUP_ITEMINFO_SLIDE_BAR), CTRL_COMMAND::COM_RENDER, false);
	__super::Array_Control(_int(PART_GROUP::GROUP_ARRAY_FRAME), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RENDER, false);

	m_bRender = false;

	m_vecSelected_Array.resize(_int(INVEN_UI_TAP::TAP_END));

	m_vecSelected_Array[_int(INVEN_UI_TAP::TAP_NORMAL_ITEM)] = INVEN_ARRAY_TYPE::TYPE_USING_RARE_ERGO;
	m_vecSelected_Array[_int(INVEN_UI_TAP::TAP_INGRADIANT)] = INVEN_ARRAY_TYPE::TYPE_INGRADIENT_SPECIAL;
	m_vecSelected_Array[_int(INVEN_UI_TAP::TAP_FUNTION)] = INVEN_ARRAY_TYPE::TYPE_FUNTIONAL_COMMON;
	m_vecSelected_Array[_int(INVEN_UI_TAP::TAP_WEAPON)] = INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_HANDLE;
	m_vecSelected_Array[_int(INVEN_UI_TAP::TAP_WEAPON_PART)] = INVEN_ARRAY_TYPE::TYPE_HANDEL_PART;
	m_vecSelected_Array[_int(INVEN_UI_TAP::TAP_ARM)] = INVEN_ARRAY_TYPE::TYPE_REASON_ARM;
	m_vecSelected_Array[_int(INVEN_UI_TAP::TAP_DEFENCE)] = INVEN_ARRAY_TYPE::TYPE_DEFENCE_RAINER;
	m_vecSelected_Array[_int(INVEN_UI_TAP::TAP_AMULET)] = INVEN_ARRAY_TYPE::TYPE_AMULET_COMMON;
	m_vecSelected_Array[_int(INVEN_UI_TAP::TAP_COSTUME)] = INVEN_ARRAY_TYPE::TYPE_COUSTUME_ACC;
	m_vecSelected_Array[_int(INVEN_UI_TAP::TAP_GESTURE)] = INVEN_ARRAY_TYPE::TYPE_GEUSTURE_COMMON;
	m_vecSelected_Array[_int(INVEN_UI_TAP::TAP_COLLECTION)] = INVEN_ARRAY_TYPE::TYPE_COLLECTION_MEMO;
	
	m_fData_Adjust_Y_Origin = m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ITEMINFO_FRAME))]->fAdjust.y;

	__super::Array_Control(_int(PART_GROUP::GROUP_CELL_0), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RATIO, 0.5f);
	m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ITEMINFO_SLIDE_BAR))]->fRatio = 0.f;
	UpdatePart_ByIndex(__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ITEMINFO_SLIDE_BAR)), 1.f);
	
	return S_OK;
}

void CUIPage_Inven::Action_Inven_Page(_float fTimeDelta)
{
	_int iTap = 0;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOP_MOUSE)]->PartIndexlist)
	{
		_Vec2 fPos = Check_Mouse_By_Part(*m_vecPart[iter]);

		if (fPos.x != -1.f)
			if (KEY_TAP(LBUTTON))
			{
				m_IsTab_Change = true;
				m_eNow_Tap = INVEN_UI_TAP(iTap);
				break;
			}

		++iTap;
	}

	if (KEY_TAP(KEY::Q))
	{
		if (_int(m_eNow_Tap) > 0)
		{
			m_IsTab_Change = true;
			m_eNow_Tap = INVEN_UI_TAP(_int(m_eNow_Tap) - 1);
		}
			
	}
	else if (KEY_TAP(KEY::E))
	{
		if (_int(m_eNow_Tap) < _int(INVEN_UI_TAP::TAP_END) - 1)
		{
			m_IsTab_Change = true;
			m_eNow_Tap = INVEN_UI_TAP(_int(m_eNow_Tap) + 1);
		}
			
	}

	if ((m_eNow_Tap == INVEN_UI_TAP::TAP_WEAPON) || (m_eNow_Tap == INVEN_UI_TAP::TAP_ARM))
		__super::Array_Control(_int(PART_GROUP::GROUP_CELL_0), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RATIO, 1.f);
	else if (m_eNow_Tap == INVEN_UI_TAP::TAP_COSTUME)
		__super::Array_Control(_int(PART_GROUP::GROUP_CELL_0), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RATIO, 1.f);
	else
		__super::Array_Control(_int(PART_GROUP::GROUP_CELL_0), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RATIO, 0.5f);

	if (m_IsTab_Change)
	{
		m_vFocus_Pos = { -1.f,-1.f };
		GET_GAMEINTERFACE->Off_Focus();
		Reset_ItemAction();

		m_pSoundCom->Play2D(TEXT("SE_UI_TutorialChangePage_01.wav"), &g_fUIVolume);

		m_iNowCell = 0;
	}
}

void CUIPage_Inven::Action_Focus(_float fTimeDelta)
{





}

void CUIPage_Inven::Update_Top_Part(_float fTimeDelta)
{
	// 하이라이트 라인 조정 
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOP_HIGHLIGHT)]->fRatio = (_float)m_eNow_Tap / ((_float)INVEN_UI_TAP::TAP_END - 1.f);

	_int iTap = 0;
	
	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOP_TEXT)]->PartIndexlist)
	{
		if (iTap < _int(INVEN_UI_TAP::TAP_END))
		{
			m_vecPart[iter]->bRender = true;
		}	
		else if (_int(m_eNow_Tap) == iTap - _int(INVEN_UI_TAP::TAP_END))
		{
			m_vecPart[iter]->bRender = true;
			m_vecPart[iter]->Set_RedText();
		}
		else
			m_vecPart[iter]->bRender = false;

		++iTap;
	}

	_int iErgo = GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat().iErgo + GET_GAMEINTERFACE->Get_Player()->Get_Player_Stat_Adjust()->iErgo;
	__super::Get_Front_Part_In_Control(_int(PART_GROUP::GROUP_COIN_COUNT))->strText = to_wstring(iErgo);



}

void CUIPage_Inven::Update_Array_Position(_float fTimeDelta)
{
	// 비율에 따른 조정 이후임 

	// 계산식 
	// Title 한 줄 -> GROUP_ARRAY_FRAME y 길이만큼 내려가기 
	// Cell 한 줄 -> GROUP_CELL_0 y 길이 만큼 내려가기 
	// 각 요소 추가 시 마다 m_fGap 만큼 더 내려가기 

	// 슬라이드 조정 반영해야 함

	if (m_eNow_Tap == INVEN_UI_TAP::TAP_WEAPON)
	{
		Update_Array_Position_Weapon(fTimeDelta);
		return;
	}


	INVEN_ARRAY_TYPE eStart = INVEN_ARRAY_TYPE::TYPE_USING_BASIC;
	if (_int(m_eNow_Tap) > 0)
		eStart = INVEN_ARRAY_TYPE(_int(m_vecSelected_Array[_int(m_eNow_Tap) - 1]) + 1);

	INVEN_ARRAY_TYPE eEnd = m_vecSelected_Array[_int(m_eNow_Tap)];

	_float fAdjust_Y = 0.f; // <- Y를 얼마나 조정해야 하는지
	_float fAdjust_Title = m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ARRAY_FRAME))]->fSize.y * 1.1f;
	_float fAdjust_Cell = m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CELL_0))]->GetBarSize().y * 1.1f;

	__super::Array_Control(_int(PART_GROUP::GROUP_ARRAY_FRAME), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RENDER, false);

	_int iTitle = 0;

	_float fStartY = -1.f;

	for (_int i = _int(eStart); i <= _int(eEnd); ++i)
	{
		_wstring strArrayTitle = {};
		_int iRow = 0;
		GET_GAMEINTERFACE->Get_Array_Info(INVEN_ARRAY_TYPE(i), &strArrayTitle, &iRow);

		if (strArrayTitle[0] == 'n')
			fAdjust_Y -= fAdjust_Title + (fAdjust_Cell * iTitle);
		else
		{
			for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ARRAY_FRAME)]->PartIndexlist)
			{
				m_vecPart[iter]->fPosition.y += fAdjust_Y + (fAdjust_Cell * iTitle);
				m_vecPart[iter]->bRender = true;
				if (fStartY == -1.f) fStartY = m_vecPart[iter]->fPosition.y - m_vecPart[iter]->fSize.y * 0.5f;
				__super::Input_Render_Info(*m_vecPart[iter], SCROLL_AREA::SCROLL_INVEN);
			}

			_int iText = __super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ARRAY_TEXT));

			m_vecPart[iText]->strText = strArrayTitle;

			for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ARRAY_TEXT)]->PartIndexlist)
			{
				m_vecPart[iter]->fPosition.y += fAdjust_Y + (fAdjust_Cell * iTitle);
				m_vecPart[iter]->bRender = true;
				__super::Input_Render_Info(*m_vecPart[iter], SCROLL_AREA::SCROLL_INVEN);
			}
			
		}

		iTitle = 0;

		for (_int j = 0; j < iRow * 5; ++j)
		{
			const CItem_Manager::ITEM* pNow = GET_GAMEINTERFACE->Get_Array_Item_Info(INVEN_ARRAY_TYPE(i), j);

			list<_int>::iterator iter = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0) + (j % 5)]->PartIndexlist.begin();

			m_vecPart[*iter]->fPosition.y += fAdjust_Y;  m_vecPart[*iter]->bRender = true;
			_Vec2 vCheck = GET_GAMEINTERFACE->CheckMouse(m_vecPart[*iter]->fPosition, m_vecPart[*iter]->fSize);
			m_vecPart[*iter]->fPosition.y -= fAdjust_Y;
			_Vec2 vRootPos = m_vecPart[*iter]->fPosition;
			_Vec2 vRootSize = m_vecPart[*iter]->fSize;

			_bool bIsFocus = false;
			++iter;
			m_vecPart[*iter]->fPosition = vRootPos;
			m_vecPart[*iter]->fSize = vRootSize;

			if (vCheck.x != -1)
			{
				GET_GAMEINTERFACE->Show_Tooltip(INVEN_ARRAY_TYPE(i), j);

				if (m_iNowCell != j)
					m_pSoundCom->Play2D(TEXT("SE_UI_ToggleChange_01.wav"), &g_fUIVolume);

				m_iNowCell = j;

				m_vecPart[*iter]->bRender = true;  bIsFocus = true;
				if (KEY_TAP(KEY::RBUTTON))
				{
					m_bReset_ItemAction_Info = false;
					Set_ItemAction(m_vecPart[*iter]->fPosition + _Vec2(0.f, fAdjust_Y), m_vecPart[*iter]->GetBarSize(), INVEN_ARRAY_TYPE(i), EQUIP_SLOT::EQUIP_END, j);
				}
				
			}
			else
				m_vecPart[*iter]->bRender = false;

			if ((pNow != nullptr) && (pNow->eSlot != EQUIP_SLOT::EQUIP_END))
			{
				m_vecPart[*iter]->fTextureColor.x = 0.3f;
				m_vecPart[*iter]->fTextureColor.z = 0.3f;
				m_vecPart[*iter]->bRender = true;
			}
			else 
			{
				m_vecPart[*iter]->fTextureColor.x = 1.f;
				m_vecPart[*iter]->fTextureColor.z = 1.f;
			}

				++iter;
				if (pNow == nullptr) m_vecPart[*iter]->iTexture_Index = -1;
				else m_vecPart[*iter]->iTexture_Index = pNow->iTexture_Index;
				m_vecPart[*iter]->bRender = true;
				++iter;
				++iter;
				++iter;
				++iter;
				m_vecPart[*iter]->bRender = false;
				if (pNow != nullptr)
				{
					if (pNow->bStack)  m_vecPart[*iter]->bRender = true; m_vecPart[*iter]->strText = to_wstring(pNow->iCount);
				}

				++iter;
				m_vecPart[*iter]->bRender = false;
				if (pNow != nullptr)
				{
					EQUIP_SLOT eSlot = pNow->eSlot;

					if ((_int(eSlot) >= _int(EQUIP_SLOT::EQUIP_USING_TOP_0)) && (_int(eSlot) <= _int(EQUIP_SLOT::EQUIP_USING_TOP_2)))
					{
						m_vecPart[*iter]->bRender = true;
						m_vecPart[*iter]->iTexture_Index = 472;
					}
					else if ((_int(eSlot) >= _int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0)) && (_int(eSlot) <= _int(EQUIP_SLOT::EQUIP_USING_BOTTOM_2)))
					{
						m_vecPart[*iter]->bRender = true;
						m_vecPart[*iter]->iTexture_Index = 0;
					}
					else if ((_int(eSlot) >= _int(EQUIP_SLOT::EQUIP_USING_BAG_0)) && (_int(eSlot) <= _int(EQUIP_SLOT::EQUIP_USING_BAG_3)))
					{
						m_vecPart[*iter]->bRender = true;
						m_vecPart[*iter]->iTexture_Index = 2;
					}
				}
			

			iter = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0) + (j % 5)]->PartIndexlist.begin();

			if (bIsFocus)
			{
				m_vFocus_Pos = m_vecPart[*iter]->fPosition + _Vec2(0.f, fAdjust_Y);
				m_vFocus_Size = m_vecPart[*iter]->GetBarSize();
			}
			else if ((m_IsTab_Change) && (i + j == _int(eStart)))
			{
				m_vFocus_Pos = m_vecPart[*iter]->fPosition + _Vec2(0.f, fAdjust_Y);
				m_vFocus_Size = m_vecPart[*iter]->GetBarSize();
			}
			if ((pNow != nullptr)&&(pNow->bIsNew))
			{
				GET_GAMEINTERFACE->Show_NewMark(m_vecPart[*iter]->fPosition + _Vec2(0.f,fAdjust_Y), m_vecPart[*iter]->GetBarSize());
				GET_GAMEINTERFACE->Set_IsNew_Show(INVEN_ARRAY_TYPE(i), j);
			}

			for (iter; iter != m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0) + (j % 5)]->PartIndexlist.end(); ++iter)
			{

				m_vecPart[*iter]->fPosition.y += fAdjust_Y;

				if ((m_eNow_Tap == INVEN_UI_TAP::TAP_ARM) && (m_vecPart[*iter]->bIsItem))
				{
					m_vecPart[*iter]->fPosition.y += m_vecPart[*iter]->fSize.y * 0.5f;

					if (m_vecPart[*iter]->bRender == true)
						__super::Input_Render_Info(*m_vecPart[*iter], SCROLL_AREA::SCROLL_INVEN);

					m_vecPart[*iter]->fPosition.y -= m_vecPart[*iter]->fSize.y * 0.5f;
				}
				else if (m_vecPart[*iter]->bRender == true)
					__super::Input_Render_Info(*m_vecPart[*iter], SCROLL_AREA::SCROLL_INVEN);
			}

			if (j % 5 == 4)
			{
				++iTitle;
				fAdjust_Y = fAdjust_Cell;
			}

		}

		--iTitle;

		fAdjust_Y += fAdjust_Title;
	}

	_float fEndY = 0.f;
	fEndY = m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0)]->PartIndexlist.front()]->fPosition.y + m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0)]->PartIndexlist.front()]->fSize.y * 0.5f;

	if ((!m_pScroll->bIsActive) || (m_IsTab_Change))
		m_pScroll->Activate_Scroll(fEndY - fStartY);
}

void CUIPage_Inven::Update_Array_Position_Weapon(_float fTimeDelta)
{
	// 비율에 따른 조정 이후임 

	// 계산식 
	// Title 한 줄 -> GROUP_ARRAY_FRAME y 길이만큼 내려가기 
	// Cell 한 줄 -> GROUP_CELL_0 y 길이 만큼 내려가기 
	// 각 요소 추가 시 마다 m_fGap 만큼 더 내려가기 

	// 슬라이드 조정 반영해야 함

	_float fAdjust_Y = 0.f; // <- Y를 얼마나 조정해야 하는지
	_float fAdjust_Title = m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ARRAY_FRAME))]->fSize.y * 1.1f;
	_float fAdjust_Cell = m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CELL_0))]->GetBarSize().y * 1.1f;

	__super::Array_Control(_int(PART_GROUP::GROUP_ARRAY_FRAME), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RENDER, false);

	_wstring strArrayTitle = {};
	_int iRow = 0;
	GET_GAMEINTERFACE->Get_Array_Info(INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE, &strArrayTitle, &iRow);

	_float fStartY = -1.f;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ARRAY_FRAME)]->PartIndexlist)
	{
		m_vecPart[iter]->fPosition.y += fAdjust_Y;
		m_vecPart[iter]->bRender = true;
		if (fStartY == -1.f) fStartY = m_vecPart[iter]->fPosition.y - m_vecPart[iter]->fSize.y * 0.5f;
		__super::Input_Render_Info(*m_vecPart[iter], SCROLL_AREA::SCROLL_INVEN);
	}

	_int iText = __super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ARRAY_TEXT));

	m_vecPart[iText]->strText = strArrayTitle;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ARRAY_TEXT)]->PartIndexlist)
	{
		m_vecPart[iter]->fPosition.y += fAdjust_Y;
		m_vecPart[iter]->bRender = true;
		__super::Input_Render_Info(*m_vecPart[iter], SCROLL_AREA::SCROLL_INVEN);
	}

	_int iTitle = 0;

	// 일반 무기 
	for (_int j = 0; j < iRow * 5; ++j)
	{
		const CItem_Manager::ITEM* pNowBlade = GET_GAMEINTERFACE->Get_Array_Item_Info(INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE, j);
		const CItem_Manager::ITEM* pNowHandle = GET_GAMEINTERFACE->Get_Array_Item_Info(INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_HANDLE, j);

		list<_int>::iterator iter = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0) + (j % 5)]->PartIndexlist.begin();

		m_vecPart[*iter]->fPosition.y += fAdjust_Y;  m_vecPart[*iter]->bRender = true;
		_Vec2 vCheck = GET_GAMEINTERFACE->CheckMouse(m_vecPart[*iter]->fPosition, m_vecPart[*iter]->fSize);
		m_vecPart[*iter]->fPosition.y -= fAdjust_Y;
		
		_Vec2 vRootPos = m_vecPart[*iter]->fPosition;
		_Vec2 vRootSize = m_vecPart[*iter]->fSize;

		_bool bIsFocus = false;
		++iter;
		m_vecPart[*iter]->fPosition = vRootPos;
		m_vecPart[*iter]->fSize = vRootSize;
		
			if (vCheck.x != -1)
			{
				if (m_iNowCell != j)
					m_pSoundCom->Play2D(TEXT("SE_UI_ToggleChange_01.wav"), &g_fUIVolume);

				m_iNowCell = j;

				if (pNowBlade != nullptr)
					GET_GAMEINTERFACE->Show_Tooltip(INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE, j);
				m_vecPart[*iter]->bRender = true;  bIsFocus = true;
				if (KEY_TAP(KEY::RBUTTON))
				{
					m_bReset_ItemAction_Info = false;
					Set_ItemAction(m_vecPart[*iter]->fPosition + _Vec2(0.f, fAdjust_Y), m_vecPart[*iter]->GetBarSize(), INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE, EQUIP_SLOT::EQUIP_END, j);
				}
			
			}
			else
				m_vecPart[*iter]->bRender = false;

			if ((pNowBlade != nullptr) && (pNowBlade->eSlot != EQUIP_SLOT::EQUIP_END))
			{
				m_vecPart[*iter]->fTextureColor.x = 0.3f;
				m_vecPart[*iter]->fTextureColor.z = 0.3f;
				m_vecPart[*iter]->bRender = true;
			}
			else
			{
				m_vecPart[*iter]->fTextureColor.x = 1.f;
				m_vecPart[*iter]->fTextureColor.z = 1.f;
			}
		
			++iter;
			if (pNowBlade == nullptr) m_vecPart[*iter]->iTexture_Index = -1;
			else m_vecPart[*iter]->iTexture_Index = pNowBlade->iTexture_Index;
			m_vecPart[*iter]->bRender = true;
			++iter;
			if (pNowHandle == nullptr) m_vecPart[*iter]->iTexture_Index = -1;
			else m_vecPart[*iter]->iTexture_Index = pNowHandle->iTexture_Index;
			m_vecPart[*iter]->bRender = true;
			++iter; m_vecPart[*iter]->bRender = true;
			++iter;
			++iter;
			++iter;
			m_vecPart[*iter]->bRender = false;
			if (pNowBlade != nullptr)
			{
				EQUIP_SLOT eSlot = pNowBlade->eSlot;
				if (eSlot == EQUIP_SLOT::EQUIP_WEAPON_BLADE_0)
				{
					m_vecPart[*iter]->bRender = true;
					m_vecPart[*iter]->iTexture_Index = 381;
				}
				else if (eSlot == EQUIP_SLOT::EQUIP_WEAPON_BLADE_1)
				{
					m_vecPart[*iter]->bRender = true;
					m_vecPart[*iter]->iTexture_Index = 382;
				}
			}
		
		

		iter = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0) + (j % 5)]->PartIndexlist.begin();

		if (bIsFocus)
		{
			m_vFocus_Pos = m_vecPart[*iter]->fPosition + _Vec2(0.f, fAdjust_Y);
			m_vFocus_Size = m_vecPart[*iter]->GetBarSize();
		}
		else if ((m_IsTab_Change) && (j == 0))
		{
			m_vFocus_Pos = m_vecPart[*iter]->fPosition + _Vec2(0.f, fAdjust_Y);
			m_vFocus_Size = m_vecPart[*iter]->GetBarSize();
		}
		if ((pNowBlade != nullptr) && (pNowBlade->bIsNew))
		{
			GET_GAMEINTERFACE->Show_NewMark(m_vecPart[*iter]->fPosition + _Vec2(0.f, fAdjust_Y), m_vecPart[*iter]->GetBarSize());
			GET_GAMEINTERFACE->Set_IsNew_Show(INVEN_ARRAY_TYPE::TYPE_WEAPON_NORMAL_BLADE, j);
		}

		for (iter; iter != m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0) + (j % 5)]->PartIndexlist.end(); ++iter)
		{
			m_vecPart[*iter]->fPosition.y += fAdjust_Y;
			if (m_vecPart[*iter]->bRender == true)
				__super::Input_Render_Info(*m_vecPart[*iter], SCROLL_AREA::SCROLL_INVEN);
		}

		if (j % 5 == 4)
		{
			++iTitle;
			fAdjust_Y = fAdjust_Cell;
		}
	}

	--iTitle;

	fAdjust_Y += fAdjust_Title;

	Update_Array_Position_Weapon_Heroic(fTimeDelta, fAdjust_Y, iTitle, fStartY);
}

void CUIPage_Inven::Update_Array_Position_Weapon_Heroic(_float fTimeDelta, _float fAdjust, _int iRowCount, _float fStart)
{
	// 비율에 따른 조정 이후임 

	// 계산식 
	// Title 한 줄 -> GROUP_ARRAY_FRAME y 길이만큼 내려가기 
	// Cell 한 줄 -> GROUP_CELL_0 y 길이 만큼 내려가기 
	// 각 요소 추가 시 마다 m_fGap 만큼 더 내려가기 

	// 슬라이드 조정 반영해야 함

	_float fAdjust_Y = fAdjust; // <- Y를 얼마나 조정해야 하는지
	_float fAdjust_Title = m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ARRAY_FRAME))]->fSize.y * 1.1f;
	_float fAdjust_Cell = m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CELL_0))]->GetBarSize().y * 1.1f;

	__super::Array_Control(_int(PART_GROUP::GROUP_ARRAY_FRAME), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RENDER, false);
	for (_int i = _int(PART_GROUP::GROUP_ARRAY_FRAME); i <= _int(PART_GROUP::GROUP_CELL_4); ++i)
		__super::UpdatePart_ByControl(m_vec_Group_Ctrl[i]);

	_wstring strArrayTitle = {};
	_int iRow = 0;
	GET_GAMEINTERFACE->Get_Array_Info(INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_BLADE, &strArrayTitle, &iRow);

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ARRAY_FRAME)]->PartIndexlist)
	{
		m_vecPart[iter]->fPosition.y += fAdjust_Y + (fAdjust_Cell * iRowCount);
		m_vecPart[iter]->bRender = true;

		__super::Input_Render_Info(*m_vecPart[iter], SCROLL_AREA::SCROLL_INVEN);
	}

	_int iText = __super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ARRAY_TEXT));

	m_vecPart[iText]->strText = strArrayTitle;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ARRAY_TEXT)]->PartIndexlist)
	{
		m_vecPart[iter]->fPosition.y += fAdjust_Y + (fAdjust_Cell * iRowCount);
		m_vecPart[iter]->bRender = true;
		__super::Input_Render_Info(*m_vecPart[iter], SCROLL_AREA::SCROLL_INVEN);
	}
	

	// 특수 무기 
	for (_int j = 0; j < iRow * 5; ++j)
	{
		const CItem_Manager::ITEM* pNowBlade = GET_GAMEINTERFACE->Get_Array_Item_Info(INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_BLADE, j);
		const CItem_Manager::ITEM* pNowHandle = GET_GAMEINTERFACE->Get_Array_Item_Info(INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_HANDLE, j);

		list<_int>::iterator iter = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0) + (j % 5)]->PartIndexlist.begin();

		m_vecPart[*iter]->fPosition.y += fAdjust_Y;  m_vecPart[*iter]->bRender = true;
		_Vec2 vCheck = GET_GAMEINTERFACE->CheckMouse(m_vecPart[*iter]->fPosition, m_vecPart[*iter]->fSize);
		m_vecPart[*iter]->fPosition.y -= fAdjust_Y;

		_Vec2 vRootPos = m_vecPart[*iter]->fPosition;
		_Vec2 vRootSize = m_vecPart[*iter]->fSize;

		_bool bIsFocus = false;
		++iter;
		m_vecPart[*iter]->fPosition = vRootPos;
		m_vecPart[*iter]->fSize = vRootSize;
		
		if (vCheck.x != -1)
		{
			if (m_iNowCell != j)
				m_pSoundCom->Play2D(TEXT("SE_UI_ToggleChange_01.wav"), &g_fUIVolume);

			m_iNowCell = j;

			if (pNowBlade != nullptr)
				GET_GAMEINTERFACE->Show_Tooltip(INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_BLADE, j);

			m_vecPart[*iter]->bRender = true;  bIsFocus = true;
			if (KEY_TAP(KEY::RBUTTON))
			{
				m_bReset_ItemAction_Info = false;
				Set_ItemAction(m_vecPart[*iter]->fPosition + _Vec2(0.f, fAdjust_Y), m_vecPart[*iter]->GetBarSize(), INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_BLADE, EQUIP_SLOT::EQUIP_END, j);
			}
			
		}
		else
			m_vecPart[*iter]->bRender = false;

		if ((pNowBlade != nullptr) && (pNowBlade->eSlot != EQUIP_SLOT::EQUIP_END))
		{
			m_vecPart[*iter]->fTextureColor.x = 0.3f;
			m_vecPart[*iter]->fTextureColor.z = 0.3f;
			m_vecPart[*iter]->bRender = true;
		}
		else
		{
			m_vecPart[*iter]->fTextureColor.x = 1.f;
			m_vecPart[*iter]->fTextureColor.z = 1.f;
		}
		
		++iter;
		++iter;
		++iter;
		++iter;
		if (pNowBlade == nullptr) m_vecPart[*iter]->iTexture_Index = -1;
		else m_vecPart[*iter]->iTexture_Index = pNowBlade->iTexture_Index;
		m_vecPart[*iter]->bRender = true;
		++iter;
		++iter;
		m_vecPart[*iter]->bRender = false;
		if (pNowBlade != nullptr)
		{
			EQUIP_SLOT eSlot = pNowBlade->eSlot;
			if (eSlot == EQUIP_SLOT::EQUIP_WEAPON_BLADE_0)
			{
				m_vecPart[*iter]->bRender = true;
				m_vecPart[*iter]->iTexture_Index = 381;
			}
			else if (eSlot == EQUIP_SLOT::EQUIP_WEAPON_BLADE_1)
			{
				m_vecPart[*iter]->bRender = true;
				m_vecPart[*iter]->iTexture_Index = 382;
			}
		}
		

		iter = m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0) + (j % 5)]->PartIndexlist.begin();

		if (bIsFocus)
		{
			m_vFocus_Pos = m_vecPart[*iter]->fPosition + _Vec2(0.f, fAdjust_Y);
			m_vFocus_Size = m_vecPart[*iter]->GetBarSize();
		}
		else if ((m_IsTab_Change) && (j == 0))
		{
			m_vFocus_Pos = m_vecPart[*iter]->fPosition + _Vec2(0.f, fAdjust_Y);
			m_vFocus_Size = m_vecPart[*iter]->GetBarSize();
		}
		if ((pNowBlade != nullptr) && (pNowBlade->bIsNew))
		{
			GET_GAMEINTERFACE->Show_NewMark(m_vecPart[*iter]->fPosition + _Vec2(0.f, fAdjust_Y), m_vecPart[*iter]->GetBarSize());
			GET_GAMEINTERFACE->Set_IsNew_Show(INVEN_ARRAY_TYPE::TYPE_WEAPON_SPECIAL_BLADE, j);
		}
			
		for (iter; iter != m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0) + (j % 5)]->PartIndexlist.end(); ++iter)
		{
			m_vecPart[*iter]->fPosition.y += fAdjust_Y;
			if (m_vecPart[*iter]->bRender == true)
				__super::Input_Render_Info(*m_vecPart[*iter], SCROLL_AREA::SCROLL_INVEN);
		}

		if (j % 5 == 4)
			fAdjust_Y = fAdjust_Cell;
	}

	_float fEndY = 0.f;
	fEndY = m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0)]->PartIndexlist.front()]->fPosition.y + m_vecPart[m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_CELL_0)]->PartIndexlist.front()]->fSize.y * 0.5f;

	if ((!m_pScroll->bIsActive) || (m_IsTab_Change))
		m_pScroll->Activate_Scroll(fEndY - fStart);
}

void CUIPage_Inven::Update_BoxInfo(_float fTimeDelta)
{




}

void CUIPage_Inven::Update_Tap_Button(_float fTimeDelta)
{
	// 현재 탭의 버튼 조정
	_int iNow_Tap = 0;
	_bool bSymbol = true;

	for (auto& iter : m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOP_TEXT)]->PartIndexlist)
	{
		if (iNow_Tap != (_int)m_eNow_Tap)
		{
			if (bSymbol)
				m_vecPart[iter]->fTextureColor = { -1.f,-1.f ,-1.f ,-1.f };
			else
				m_vecPart[iter]->bRender = false;
		}
		else
		{
			if (bSymbol)
				m_vecPart[iter]->fTextureColor = { 1.f,0.f ,0.f ,-1.f };
			else
			{
				m_vecPart[iter]->bRender = true;
				m_vecPart[iter]->Set_RedText();
			}
		}

		++iNow_Tap;
		if (iNow_Tap >= _int(INVEN_UI_TAP::TAP_END))
			iNow_Tap = 0;
	}
}

void CUIPage_Inven::Change_Data_Y_Size(_float fSize)
{

}

void CUIPage_Inven::Action_Slide(_float fSize)
{
	if ((m_pScroll->bIsBarMoving) && (KEY_HOLD(KEY::LBUTTON)))
	{
		POINT			ptMouse{};
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		_float fOffset_Before = m_pScroll->fData_Offset_Y;

		m_pScroll->Bar_Moving(_float(ptMouse.y));

		m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_ITEMINFO_SLIDE_BAR)]->fRatio = m_pScroll->fScroll_Ratio;
		m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ITEMINFO_FRAME))]->fAdjust.y = m_fData_Adjust_Y_Origin - m_pScroll->fData_Offset_Y;

		m_vFocus_Pos.y -= (m_pScroll->fData_Offset_Y - fOffset_Before);
	}
	else if ((!m_pScroll->bIsBarMoving) && (KEY_TAP(KEY::LBUTTON)))
	{
		_int iBar = __super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ITEMINFO_SLIDE_BAR));
		_Vec2 vCheck = GET_GAMEINTERFACE->CheckMouse(m_vecPart[iBar]->fPosition, m_vecPart[iBar]->fSize);

		if (vCheck.x != -1.f)
			if (KEY_TAP(KEY::LBUTTON))
			{
				m_pScroll->Start_Bar_Moving(vCheck.y);
			}
	}
	else
	{
		m_pScroll->End_Bar_Moving();
	}
}

void CUIPage_Inven::Update_Slide(_float fSize)
{
	if (m_pScroll->bIsActive)
	{
		for (_int i = _int(PART_GROUP::GROUP_ITEMINFO_FRAME); i <= _int(PART_GROUP::GROUP_ITEMINFO_SLIDE_BAR); ++i)
		{

			__super::UpdatePart_ByControl(m_vec_Group_Ctrl[i]);

			for (auto& iter : m_vec_Group_Ctrl[i]->PartIndexlist)
				__super::Input_Render_Info(*m_vecPart[iter]);
		}
	}
}



CUIPage_Inven* CUIPage_Inven::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Inven* pInstance = new CUIPage_Inven(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Inven"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Inven::Clone(void* pArg)
{
	CUIPage_Inven* pInstance = new CUIPage_Inven(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Inven"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Inven::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	Safe_Delete(m_pScroll);
	m_vecPart.clear();
}
