#include "stdafx.h"
#include "..\Public\UIPlay_Component.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPlay_Component::CUIPlay_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage_Play{ pDevice, pContext }
{
}

CUIPlay_Component::CUIPlay_Component(const CUIPlay_Component& Prototype)
	: CUIPage_Play{ Prototype }
{
}

HRESULT CUIPlay_Component::Initialize_Prototype()
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

HRESULT CUIPlay_Component::Initialize(void* pArg)
{
	return S_OK;
}

void CUIPlay_Component::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPlay_Component::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPlay_Component::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPlay_Component::Render()
{
	return S_OK;
}

void CUIPlay_Component::OpenAction()
{
}

void CUIPlay_Component::CloseAction()
{
}

CHECK_MOUSE CUIPlay_Component::Check_Page_Action(_float fTimeDelta)
{
	return CHECK_MOUSE();
}

CUIPlay_Component* CUIPlay_Component::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPlay_Component* pInstance = new CUIPlay_Component(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPagCUIPlay_Componente_Play"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPlay_Component::Clone(void* pArg)
{
	CUIPlay_Component* pInstance = new CUIPlay_Component(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPlay_Component"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPlay_Component::Free()
{
	__super::Free();
}
