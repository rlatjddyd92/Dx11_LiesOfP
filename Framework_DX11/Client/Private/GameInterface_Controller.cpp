#include "stdafx.h"
#include "..\Public\GameInterface_Controller.h"



IMPLEMENT_SINGLETON(CGameInterface_Controller)

CGameInterface_Controller::CGameInterface_Controller()
{
}

HRESULT CGameInterface_Controller::Initialize_GameInterface(ID3D11Device** pDevice, ID3D11DeviceContext** pContext, CGameInstance* pInstance)
{
	m_pGameInstance = pInstance;
	Safe_AddRef(m_pGameInstance);

	m_pDevice = *pDevice;
	m_pContext = *pContext;



	// UIManager
	m_pUIManager = CUIManager::Create(m_pDevice, m_pContext);

	// Item_Manager
	m_pItem_Manager = CItem_Manager::Create(m_pGameInstance);

	// Stat_Manager
	m_pPlayer_Stat_Manager = CPlayer_Stat_Manager::Create(m_pGameInstance);




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
	Safe_Release(m_pItem_Manager);
	Safe_Release(m_pPlayer_Stat_Manager);




	GET_GAMEINTERFACE->Destroy_Instance();
}




void CGameInterface_Controller::Free()
{
	__super::Free();




}
