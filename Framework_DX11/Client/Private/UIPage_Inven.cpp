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
	__super::Late_Update(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
	
}

HRESULT CUIPage_Inven::Render()
{
	return S_OK;
}

void CUIPage_Inven::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Inven::CloseAction()
{
	__super::CloseAction();
	
}

CHECK_MOUSE CUIPage_Inven::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

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

	return S_OK;


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
				m_eNow_Tap = INVEN_UI_TAP(iTap);
				break;
			}
				
		++iTap;
	}

	if (KEY_TAP(KEY::Q))
	{
		if (_int(m_eNow_Tap) > 0)
			m_eNow_Tap = INVEN_UI_TAP(_int(m_eNow_Tap) - 1);
	}
	else if (KEY_TAP(KEY::E))
	{
		if (_int(m_eNow_Tap) < _int(INVEN_UI_TAP::TAP_END) - 1)
			m_eNow_Tap = INVEN_UI_TAP(_int(m_eNow_Tap) + 1);
	}

	if ((m_eNow_Tap == INVEN_UI_TAP::TAP_WEAPON) || (m_eNow_Tap == INVEN_UI_TAP::TAP_ARM))
		__super::Array_Control(_int(PART_GROUP::GROUP_CELL_0), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RATIO, 1.f);
	else if (m_eNow_Tap == INVEN_UI_TAP::TAP_COSTUME) 
		__super::Array_Control(_int(PART_GROUP::GROUP_CELL_0), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RATIO, 1.f);
	else 
		__super::Array_Control(_int(PART_GROUP::GROUP_CELL_0), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RATIO, 0.5f);
}

void CUIPage_Inven::Action_Focus(_float fTimeDelta)
{
}

void CUIPage_Inven::Update_Top_Part(_float fTimeDelta)
{
	// 하이라이트 라인 조정 
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TOP_HIGHLIGHT)]->fRatio = (_int)m_eNow_Tap / (_int)INVEN_UI_TAP::TAP_END;

	




}

void CUIPage_Inven::Update_Array_Position(_float fTimeDelta)
{
	// 비율에 따른 조정 이후임 

	// 계산식 
	// Title 한 줄 -> GROUP_ARRAY_FRAME y 길이만큼 내려가기 
	// Cell 한 줄 -> GROUP_CELL_0 y 길이 만큼 내려가기 
	// 각 요소 추가 시 마다 m_fGap 만큼 더 내려가기 

	// 슬라이드 조정 반영해야 함

	INVEN_ARRAY_TYPE eStart = INVEN_ARRAY_TYPE::TYPE_USING_BASIC;
	if (_int(m_eNow_Tap) > 0)
		INVEN_ARRAY_TYPE eStart = m_vecSelected_Array[_int(m_eNow_Tap) - 1];

	INVEN_ARRAY_TYPE eEnd = m_vecSelected_Array[_int(m_eNow_Tap)];

	_float fAdjust_Y = 0.f; // <- Y를 얼마나 조정해야 하는지
	_float fAdjust_Title = m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_ARRAY_FRAME))]->fSize.y;
	_float fAdjust_Cell = m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CELL_0))]->fSize.y;

	for (_int i = _int(eStart); i <= _int(eEnd); ++i)
	{
		
	}











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

	m_vecPart.clear();
}
