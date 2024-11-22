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

	__super::Array_Control(_int(PART_GROUP::GROUP_ITEMINFO_SLIDE), _int(PART_GROUP::GROUP_ITEMINFO_SLIDE_BAR), CTRL_COMMAND::COM_RENDER, false);
	__super::Array_Control(_int(PART_GROUP::GROUP_ARRAY_FRAME), _int(PART_GROUP::GROUP_CELL_4), CTRL_COMMAND::COM_RENDER, false);
	
	m_bRender = false;

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
				eNow_Tap = INVEN_UI_TAP(iTap);
				break;
			}
				
		++iTap;
	}

	if (KEY_TAP(KEY::Q))
	{
		if (_int(eNow_Tap) > 0)
			eNow_Tap = INVEN_UI_TAP(_int(eNow_Tap) - 1);
	}
	else if (KEY_TAP(KEY::E))
	{
		if (_int(eNow_Tap) < _int(INVEN_UI_TAP::TAP_END) - 1)
			eNow_Tap = INVEN_UI_TAP(_int(eNow_Tap) + 1);
	}
}

void CUIPage_Inven::Action_Focus(_float fTimeDelta)
{
}

void CUIPage_Inven::Update_Array_Position(_float fTimeDelta)
{







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
