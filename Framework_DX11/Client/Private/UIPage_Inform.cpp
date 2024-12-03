#include "stdafx.h"
#include "..\Public\UIPage_Inform.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Inform::CUIPage_Inform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Inform::CUIPage_Inform(const CUIPage_Inform& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Inform::Initialize_Prototype()
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

HRESULT CUIPage_Inform::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Inform::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Inform::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Inform::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
}

HRESULT CUIPage_Inform::Render()
{
	return S_OK;
}

void CUIPage_Inform::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Inform::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Inform::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Inform::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	

	m_bRender = false;
	m_bUpdate = false;

	return S_OK;
}

CUIPage_Inform* CUIPage_Inform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Inform* pInstance = new CUIPage_Inform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Inform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Inform::Clone(void* pArg)
{
	CUIPage_Inform* pInstance = new CUIPage_Inform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Inform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Inform::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
