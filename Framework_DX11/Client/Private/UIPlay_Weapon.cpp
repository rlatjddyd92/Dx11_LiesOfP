#include "stdafx.h"
#include "..\Public\UIPlay_Weapon.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPlay_Weapon::CUIPlay_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPlay_Component{ pDevice, pContext }
{
}

CUIPlay_Weapon::CUIPlay_Weapon(const CUIPlay_Weapon& Prototype)
	: CUIPlay_Component{ Prototype }
{
}

HRESULT CUIPlay_Weapon::Initialize_Prototype()
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

HRESULT CUIPlay_Weapon::Initialize(void* pArg)
{
	return S_OK;
}

void CUIPlay_Weapon::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPlay_Weapon::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPlay_Weapon::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPlay_Weapon::Render()
{
	return S_OK;
}

void CUIPlay_Weapon::OpenAction()
{
}

void CUIPlay_Weapon::CloseAction()
{
}

CHECK_MOUSE CUIPlay_Weapon::Check_Page_Action(_float fTimeDelta)
{
	return CHECK_MOUSE();
}

CUIPlay_Weapon* CUIPlay_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPlay_Weapon* pInstance = new CUIPlay_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPlay_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPlay_Weapon::Clone(void* pArg)
{
	CUIPlay_Weapon* pInstance = new CUIPlay_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPlay_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPlay_Weapon::Free()
{
	__super::Free();
}
