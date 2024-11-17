#include "stdafx.h"
#include "..\Public\UIPage_Inven.h"

#include "GameInstance.h"


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

HRESULT CUIPage_Inven::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	return S_OK;
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
