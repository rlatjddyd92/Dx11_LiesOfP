#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "GameInstance.h"
#include "UIManager.h"
#include "Item_Manager.h"
#include "Player_Stat_Manager.h"

BEGIN(Engine)
class CGameObject;
END




BEGIN(Client)

class CGameInterface_Controller : public CBase
{
	DECLARE_SINGLETON(CGameInterface_Controller)




private:
	CGameInterface_Controller();
	virtual ~CGameInterface_Controller() = default;






public: // 외부에서 매니저 접근 




#pragma region Common
	HRESULT Initialize_GameInterface(ID3D11Device** pDevice, ID3D11DeviceContext** pContext, CGameInstance* pInstance);
	void Update_GameInterface(_float fTimeDelta);
	void Release_GameInterface();

#pragma endregion


#pragma region UIManager
	void OpenMainPage() { m_pUIManager->OpenMainPage(); }
	void OpenLoadingPage() { m_pUIManager->OpenLoadingPage(); }

	_Vec2 CheckMouse(_Vec2 fPos, _Vec2 fSize) { return m_pUIManager->CheckMouse(fPos, fSize); }

	void SetIngame(_bool bTrue) { m_pUIManager->SetIngame(bTrue); }

	void SwicthPage(UIPAGE ePageA, UIPAGE ePageB) { m_pUIManager->SwicthPage(ePageA, ePageB); }
	CUIManager::TDATA* GetTestData() { return m_pUIManager->GetTestData(); }
	// TestPage 
	void ShowTestPage(TEST_PAGE_NAME eName,
		_wstring DataNameA, TEST_PAGE_VALUE_TYPE eTypeA, const void* ValueA,
		_wstring DataNameB = {}, TEST_PAGE_VALUE_TYPE eTypeB = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueB = nullptr,
		_wstring DataNameC = {}, TEST_PAGE_VALUE_TYPE eTypeC = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueC = nullptr,
		_wstring DataNameD = {}, TEST_PAGE_VALUE_TYPE eTypeD = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueD = nullptr,
		_wstring DataNameE = {}, TEST_PAGE_VALUE_TYPE eTypeE = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueE = nullptr,
		_wstring DataNameF = {}, TEST_PAGE_VALUE_TYPE eTypeF = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueF = nullptr,
		_wstring DataNameG = {}, TEST_PAGE_VALUE_TYPE eTypeG = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueG = nullptr,
		_wstring DataNameH = {}, TEST_PAGE_VALUE_TYPE eTypeH = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueH = nullptr,
		_wstring DataNameI = {}, TEST_PAGE_VALUE_TYPE eTypeI = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueI = nullptr,
		_wstring DataNameJ = {}, TEST_PAGE_VALUE_TYPE eTypeJ = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueJ = nullptr
	)
	{ m_pUIManager->ShowTestPage(eName, DataNameA, eTypeA, ValueA, DataNameB, eTypeB, ValueB, DataNameC, eTypeC, ValueC, DataNameD, eTypeD, ValueD, DataNameE, eTypeE, ValueE,
		DataNameF, eTypeF, ValueF, DataNameG, eTypeG, ValueG, DataNameH, eTypeH, ValueH, DataNameI, eTypeI, ValueI, DataNameJ, eTypeJ, ValueJ); }// 함수 사용하면 입력 내용 대로 해당 프레임에 창을 띄워 준다, DataNameA: 데이터의 이름, TEST_PAGE_VALUE_TYPE: 자료형(float,int,tchar), ValueA: 값을 받아올 포인터 <- 이렇게 한줄
	// 매 프레임 마다 함수를 사용해줘야 작동함 (Gameinterface가 외부 정보를 저장/관리하지 않도록 하기 위함)

	// 직교 UI 요청 
	void Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE eType, void* pObj) { m_pUIManager->Register_Pointer_Into_OrthoUIPage(eType, pObj); }

#pragma endregion


#pragma region Item_Manager
	// 접근, 수정
	ITEM_RESULT AddNewItem_Inven(_uint iItemIndex, _uint iCount = 1) { return m_pItem_Manager->AddNewItem_Inven(iItemIndex, iCount); } // <- 새롭게 아이템을 만들어 인벤에 넣는다 
	ITEM_RESULT EquipItem_Inven(INVEN_ARRAY_TYPE eIndex, EQUIP_SLOT eSlot, _uint iIndex) { return m_pItem_Manager->EquipItem_Inven(eIndex, eSlot, iIndex); } // <- 인벤에 있는 아이템을 장비한다 
	ITEM_RESULT UseItem_Equip(EQUIP_SLOT eSlot, _uint iCount = 1) { return m_pItem_Manager->UseItem_Equip(eSlot, iCount); } // <- 장비된 아이템을 사용한다
	ITEM_RESULT UseItem_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex, _uint iCount = 1) { return m_pItem_Manager->UseItem_Inven(eIndex, iIndex, iCount); } // <- 인벤에 있는 아이템을 직접 사용 
	ITEM_RESULT Remove_Item_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex) { return m_pItem_Manager->Remove_Item_Inven(eIndex, iIndex); } // <- 인벤의 아이템 제거

	ITEM_RESULT Get_Array_Info(INVEN_ARRAY_TYPE eIndex, _wstring* strName, _int* iRow) { return m_pItem_Manager->Get_Array_Info(eIndex, strName, iRow); } // <- 인벤 배열 정보 획득
	const CItem_Manager::ITEM* Get_Array_Item_Info(INVEN_ARRAY_TYPE eIndex, _uint iIndex) { return m_pItem_Manager->Get_Array_Item_Info(eIndex, iIndex); } // <- 인벤의 아이템 정보 획득

	CPlayer::WEAPON_TYPE Get_Weapon_Model_Index() { return m_pItem_Manager->Get_Weapon_Model_Index(); } // 현재 사용 중인 무기의 모델 번호 리턴

	const CItem_Manager::ITEM* Get_Equip_Item_Info(EQUIP_SLOT eSlot) { return m_pItem_Manager->Get_Equip_Item_Info(eSlot); } // 현재 장비창에 장착된 아이템의 정보 레퍼런스 획득
	
	// 코인
	ITEM_RESULT Add_Coin(_int iAdd, _bool bForce) { return m_pItem_Manager->Add_Coin(iAdd, bForce); }
	_int Get_Coin() { return m_pItem_Manager->Get_Coin(); }

	// 선택 아이템 조정
	_int Change_Potion_Select(_bool bNext) { return m_pItem_Manager->Change_Potion_Select(bNext); }
	_int Change_Tool_Select(_bool bNext) { return m_pItem_Manager->Change_Tool_Select(bNext); }
	_int Change_Weapon() { return m_pItem_Manager->Change_Weapon(); }
	_int Get_Potion_Select() { return m_pItem_Manager->Get_Potion_Select(); }
	_int Get_Tool_Select() { return m_pItem_Manager->Get_Tool_Select(); }
	_int Get_Weapon() { return m_pItem_Manager->Get_Weapon(); }



#pragma endregion


#pragma region Player_Stat_Manager
	// 정보 접근 
	const _float& Get_NowStat_Normal(STAT_NORMAL eIndex) { return m_pPlayer_Stat_Manager->Get_NowStat_Normal(eIndex); }
	const CPlayer_Stat_Manager::STAT& Get_StatInfo_Normal(STAT_NORMAL eIndex) { return m_pPlayer_Stat_Manager->Get_StatInfo_Normal(eIndex); }

	const _float Get_Now_Max_Ratio(STAT_NORMAL eIndex) { return  m_pPlayer_Stat_Manager->Get_Now_Max_Ratio(eIndex);}
	const _float Get_Max_Limit_Ratio(STAT_NORMAL eIndex) { return  m_pPlayer_Stat_Manager->Get_Max_Limit_Ratio(eIndex);}
	const _float Get_Now_Limit_Ratio(STAT_NORMAL eIndex) { return m_pPlayer_Stat_Manager->Get_Now_Limit_Ratio(eIndex); }

	const CPlayer_Stat_Manager::STAT& Get_StatInfo_DEF(STAT_DEF eIndex) { return m_pPlayer_Stat_Manager->Get_StatInfo_DEF(eIndex); }

	// 정보 수정 
	void Add_Stat_Normal(STAT_NORMAL eIndex, _float fValue) { return m_pPlayer_Stat_Manager->Add_Stat_Normal(eIndex, fValue); }
	void Add_StatMax_Normal(STAT_NORMAL eIndex, _float fValue) { return m_pPlayer_Stat_Manager->Add_StatMax_Normal(eIndex, fValue); }

#pragma endregion


private:
	CGameInstance* m_pGameInstance = { nullptr };
	CUIManager* m_pUIManager = { nullptr };
	CItem_Manager* m_pItem_Manager = { nullptr };
	CPlayer_Stat_Manager* m_pPlayer_Stat_Manager = { nullptr };




private: // <- 디바이스 
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END
