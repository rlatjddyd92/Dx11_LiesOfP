#include "stdafx.h"
#include "..\Public\UIPage_Popup.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Popup::CUIPage_Popup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Popup::CUIPage_Popup(const CUIPage_Popup& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Popup::Initialize_Prototype()
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

HRESULT CUIPage_Popup::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Popup::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Popup::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Popup::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
}

HRESULT CUIPage_Popup::Render()
{
	return S_OK;
}

void CUIPage_Popup::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Popup::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Popup::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Popup::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	

	m_bRender = false;
	m_bUpdate = false;

	return S_OK;
}

CUIPage_Popup* CUIPage_Popup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Popup* pInstance = new CUIPage_Popup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Popup"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Popup::Clone(void* pArg)
{
	CUIPage_Popup* pInstance = new CUIPage_Popup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Popup"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Popup::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
