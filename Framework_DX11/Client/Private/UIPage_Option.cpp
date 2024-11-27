#include "stdafx.h"
#include "..\Public\UIPage_Option.h"

#include "GameInstance.h"


CUIPage_Option::CUIPage_Option(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Option::CUIPage_Option(const CUIPage_Option& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Option::Initialize_Prototype()
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

HRESULT CUIPage_Option::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Option::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Option::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Option::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Option::Render()
{
	return S_OK;
}

void CUIPage_Option::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Option::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Option::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Option::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();



	return S_OK;
}

CUIPage_Option* CUIPage_Option::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Option* pInstance = new CUIPage_Option(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Option"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Option::Clone(void* pArg)
{
	CUIPage_Option* pInstance = new CUIPage_Option(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Option"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Option::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
