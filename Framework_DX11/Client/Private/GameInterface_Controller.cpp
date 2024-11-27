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
	// 테스트용 
	if (KEY_HOLD(KEY::ALT))
	{
		if (KEY_TAP(KEY::F3))
		{
			_int iItem = _int(m_pGameInstance->Get_Random(0.f, 3.1f));
			if (iItem == 0)
				m_pItem_Manager->AddNewItem_Inven(255);
			if (iItem == 1)
				m_pItem_Manager->AddNewItem_Inven(247);
			if (iItem == 2)
				m_pItem_Manager->AddNewItem_Inven(277);
			if (iItem == 3)
				m_pItem_Manager->AddNewItem_Inven(279);
		}
		else if (KEY_TAP(KEY::F5))
		{
			/*for (_int i = 0; i < _int(BUFF_INDEX::BUFF_END); ++i)
			{
				if (m_pPlayer_Stat_Manager->Get_Buff_Ratio(BUFF_INDEX(i)) == 0.f)
				{
					m_pPlayer_Stat_Manager->Start_Buff(BUFF_INDEX(i));
					break;
				}
			}*/
		}
		else if (KEY_HOLD(KEY::F6))
			m_pUIManager->Action_InterAction(TEXT("테스트 상호작용"));
	}





	m_pItem_Manager->Update_Item(fTimeDelta);
	//m_pPlayer_Stat_Manager->Update_Stat(fTimeDelta);
	m_pUIManager->Update_UIManager(fTimeDelta);


}

void CGameInterface_Controller::Release_GameInterface()
{

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pItem_Manager);
	Safe_Release(m_pPlayer_Stat_Manager);
	m_pPlayer = nullptr;

	GET_GAMEINTERFACE->Destroy_Instance();
}




void CGameInterface_Controller::Free()
{
	__super::Free();




}