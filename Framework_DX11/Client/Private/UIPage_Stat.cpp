#include "stdafx.h"
#include "..\Public\UIPage_Stat.h"

#include "GameInstance.h"


CUIPage_Stat::CUIPage_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Stat::CUIPage_Stat(const CUIPage_Stat& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Stat::Initialize_Prototype()
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

HRESULT CUIPage_Stat::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Stat::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Stat::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Stat::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Stat::Render()
{
	return S_OK;
}

void CUIPage_Stat::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Stat::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Stat::Check_Mouse_By_Part_In_Page()
{
	__super::Check_Mouse_By_Part_In_Page();

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Stat::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	return S_OK;
}

CUIPage_Stat* CUIPage_Stat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Stat* pInstance = new CUIPage_Stat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Stat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Stat::Clone(void* pArg)
{
	CUIPage_Stat* pInstance = new CUIPage_Stat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Stat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Stat::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
