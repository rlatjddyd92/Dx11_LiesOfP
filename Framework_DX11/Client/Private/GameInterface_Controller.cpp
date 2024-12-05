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
	}

	// 매니저 업데이트 
	m_pItem_Manager->Update_Item(fTimeDelta); 
	m_pUIManager->Update_UIManager(fTimeDelta);

	// 아이템 조작 팝업 
	const CUIPage::ITEMACTION_INFO* pNowAction = m_pUIManager->Get_ItemAction_Info(); // 현재 UI에서 요청된 아이템 조작 팝업 정보 획득

	if (m_pUIManager->Get_ActiveFunc() != ITEM_FUNC::FUNC_END) // UI 매니저에서 팝업을 통한 조작요청이 확인됨
	{
		m_pItem_Manager->Operate_ItemAction(m_pUIManager->Get_ActiveFunc(), pNowAction->vAction_Pos, pNowAction->vAction_Size); // 아이템 매니저가 요청 사항을 수행

		if (m_pItem_Manager->IsReset_ItemAction()) // 아이템 매니저의 활동 결과, 팝업이 종료되었는 지 확인 
			m_pUIManager->Reset_ItemAction(); // 팝업이 종료된 경우 UI 매니저에서도 요청 사항을 모두 리셋 
	}
			
	if (pNowAction == nullptr) // UI 매니저의 요청 사항이 없었던 경우 
		m_pItem_Manager->Reset_ItemAction(); // 아이템 매니저의 팝업 정보 리셋 
	else if (pNowAction->eAction_Array_Type != INVEN_ARRAY_TYPE::TYPE_END) // UI 매니저의 요청이 장비창에서 발생 
		m_pItem_Manager->Set_ItemAction(pNowAction->eAction_Array_Type, pNowAction->iAction_Array_Index, pNowAction->vAction_Pos, pNowAction->vAction_Size, true);
	else if (pNowAction->eAction_Equip_Slot != EQUIP_SLOT::EQUIP_END) // UI 매니저의 요청이 인벤에서 발생 
		m_pItem_Manager->Set_ItemAction(pNowAction->eAction_Equip_Slot, pNowAction->vAction_Pos, pNowAction->vAction_Size, true);
	else
		m_pItem_Manager->Reset_ItemAction(); // 요청 사항 내용이 nullptr은 아니지만 비정상임(장비창, 인벤 둘 중 하나여야 함)-> 리셋 진행 

	// 장비 착용 내용에 따라 장비 스펙 내용 갱신 
}

void CGameInterface_Controller::Release_GameInterface()
{

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pItem_Manager);
	Safe_Release(m_pPlayer_Stat_Manager);
	//m_pPlayer = nullptr;

	GET_GAMEINTERFACE->Destroy_Instance();
}




void CGameInterface_Controller::Free()
{
	__super::Free();




}