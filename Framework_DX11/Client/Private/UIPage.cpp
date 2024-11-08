#include "stdafx.h"
#include "GameInterface_Controller.h"

#include "GameInstance.h"

#include "UIPage.h"

CUIPage::CUIPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CUIPage::CUIPage(const CUIPage& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CUIPage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIPage::Initialize(void* pArg)
{
    return S_OK;
}

void CUIPage::Priority_Update(_float fTimeDelta)
{
}

void CUIPage::Update(_float fTimeDelta)
{
}

void CUIPage::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIPage::Render()
{
    return S_OK;
}

CUIPage* CUIPage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage* pInstance = new CUIPage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBacCUIPagekGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage::Clone(void* pArg)
{
	CUIPage* pInstance = new CUIPage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage::Free()
{
	__super::Free();
}
