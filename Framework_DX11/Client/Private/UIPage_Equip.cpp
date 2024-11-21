#include "stdafx.h"
#include "..\Public\UIPage_Equip.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Equip::CUIPage_Equip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Equip::CUIPage_Equip(const CUIPage_Equip& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Equip::Initialize_Prototype()
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

	m_fTopPartMove_Excel = 100.f;

	return S_OK;
}

HRESULT CUIPage_Equip::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Equip::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Equip::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Equip::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Equip::Render()
{
	return S_OK;
}

void CUIPage_Equip::OpenAction()
{
	__super::OpenAction();
	GET_GAMEINTERFACE->SetIngame(false);
}

void CUIPage_Equip::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Equip::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Equip::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	return S_OK;
}

CUIPage_Equip* CUIPage_Equip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Equip* pInstance = new CUIPage_Equip(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Equip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Equip::Clone(void* pArg)
{
	CUIPage_Equip* pInstance = new CUIPage_Equip(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Equip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Equip::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
