
#include "stdafx.h"
#include "..\Public\UIManager.h"

#include "GameInstance.h"

CUIManager::CUIManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIManager::CUIManager(const CUIManager& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIManager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIManager::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	return S_OK;
}

void CUIManager::Priority_Update(_float fTimeDelta)
{
}

void CUIManager::Update(_float fTimeDelta)
{
}

void CUIManager::Late_Update(_float fTimeDelta)
{


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIManager::Render()
{
	if (FAILED(m_pUIRender_Client->Render_UI(m_vecPage)))
		return E_FAIL;

	return S_OK;
}

CUIManager* CUIManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIManager* pInstance = new CUIManager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIManager::Clone(void* pArg)
{
	CUIManager* pInstance = new CUIManager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIManager::Free()
{
	__super::Free();


	for (auto& iter : m_vecPage)
		Safe_Release(iter);

	m_vecPage.clear();

	Safe_Release(m_pUIRender_Client);
}
