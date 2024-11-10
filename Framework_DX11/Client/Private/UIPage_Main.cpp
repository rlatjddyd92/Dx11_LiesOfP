#include "stdafx.h"
#include "..\Public\UIPage_Main.h"

#include "GameInstance.h"


CUIPage_Main::CUIPage_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{
	
}

CUIPage_Main::CUIPage_Main(const CUIPage_Main& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Main::Initialize_Prototype()
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

	if (FAILED(Ready_UIPart()))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CUIPage_Main::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Main::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Main::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Main::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Main::Render()
{
	return S_OK;
}

void CUIPage_Main::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Main::CloseAction()
{
	__super::CloseAction();
}

HRESULT CUIPage_Main::Ready_UIPart()
{
	__super::Ready_UIPart();

	return S_OK;
}

CUIPage_Main* CUIPage_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Main* pInstance = new CUIPage_Main(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Main"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Main::Clone(void* pArg)
{
	CUIPage_Main* pInstance = new CUIPage_Main(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Main"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Main::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
