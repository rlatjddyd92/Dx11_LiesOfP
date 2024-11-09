#include "stdafx.h"
#include "..\Public\GameInterface_Controller.h"



IMPLEMENT_SINGLETON(CGameInterface_Controller)

CGameInterface_Controller::CGameInterface_Controller()
{
}

HRESULT CGameInterface_Controller::Initialize_GameInterface(ID3D11Device** pDevice, ID3D11DeviceContext** pContext, CGameInstance* pInstance)
{
	m_pGameInstance = pInstance;
	m_pDevice = *pDevice;
	m_pContext = *pContext;

	// UI 매니저 세팅 
	m_pUIManager = CUIManager::Create(m_pDevice, m_pContext);
	//Safe_AddRef(m_pUIManager);




	return S_OK;
}

void CGameInterface_Controller::Update_GameInterface(_float fTimeDelta)
{
	m_pUIManager->Update_UIManager(fTimeDelta);
}

void CGameInterface_Controller::Release_GameInterface()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pUIManager);





	GET_GAMEINTERFACE->Destroy_Instance();
}



void CGameInterface_Controller::Free()
{
	__super::Free();




}
