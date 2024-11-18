#include "stdafx.h"
#include "..\Public\UIPage_Loading.h"

#include "GameInstance.h"


CUIPage_Loading::CUIPage_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Loading::CUIPage_Loading(const CUIPage_Loading& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Loading::Initialize_Prototype()
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

HRESULT CUIPage_Loading::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Loading::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Loading::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Loading::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Loading::Render()
{
	return S_OK;
}

void CUIPage_Loading::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Loading::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Loading::Check_Mouse_By_Part_In_Page()
{
	__super::Check_Mouse_By_Part_In_Page();

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Loading::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	return S_OK;
}

CUIPage_Loading* CUIPage_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Loading* pInstance = new CUIPage_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Loading::Clone(void* pArg)
{
	CUIPage_Loading* pInstance = new CUIPage_Loading(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Loading::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
