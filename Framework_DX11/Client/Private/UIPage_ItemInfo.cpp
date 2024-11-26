#include "stdafx.h"
#include "..\Public\UIPage_ItemInfo.h"

#include "GameInstance.h"


CUIPage_ItemInfo::CUIPage_ItemInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_ItemInfo::CUIPage_ItemInfo(const CUIPage_ItemInfo& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_ItemInfo::Initialize_Prototype()
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

HRESULT CUIPage_ItemInfo::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_ItemInfo::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_ItemInfo::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_ItemInfo::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CUIPage_ItemInfo::Render()
{
	return S_OK;
}

void CUIPage_ItemInfo::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_ItemInfo::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_ItemInfo::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_ItemInfo::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();


	m_bRender = false;

	return S_OK;
}

CUIPage_ItemInfo* CUIPage_ItemInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_ItemInfo* pInstance = new CUIPage_ItemInfo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_ToolTip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_ItemInfo::Clone(void* pArg)
{
	CUIPage_ItemInfo* pInstance = new CUIPage_ItemInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_ToolTip"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_ItemInfo::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
