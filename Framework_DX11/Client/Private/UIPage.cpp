#include "stdafx.h"
#include "..\Public\UIPage.h"

#include "GameInstance.h"

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
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

    return S_OK;
}

void CUIPage::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& iter : m_vecPart)
	{
		iter->MakeDirec();

		if (iter->iParentPart_Index == -1)
			iter->MovePart({ m_fX,m_fY });
		else
			iter->MovePart(m_vecPart[iter->iParentPart_Index]->fPosition);
	}
}

HRESULT CUIPage::Render()
{
    return S_OK;
}

void CUIPage::OpenAction()
{
}

void CUIPage::CloseAction()
{
}

HRESULT CUIPage::Ready_UIPart()
{
	return S_OK;
}

CUIPage* CUIPage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage* pInstance = new CUIPage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage"));
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
