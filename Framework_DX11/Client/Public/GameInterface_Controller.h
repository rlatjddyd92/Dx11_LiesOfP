#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "GameInstance.h"
#include "UIManager.h"
#include "Item_Manager.h"
#include "Player_Stat_Manager.h"
#include "Player.h"

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

	void Input_Player_Pointer(CPlayer* pPointer) { m_pPlayer = pPointer; }

	// 플레이어 정보 
	CPlayer* Get_Player() { return m_pPlayer; }
	//_bool Get_InvenLock() { return m_pPlayer->Get_Player_Stat().m_bInvenLock; }


#pragma endregion


#pragma region UIManager
	_bool IsGamePause() { return m_pUIManager->IsGamePause(); }

	_bool Is_UIOff() { return m_pUIManager->Is_UIOff(); }

	void OpenMainPage() { m_pUIManager->OpenMainPage(); }
	void OpenLoadingPage() { m_pUIManager->OpenLoadingPage(); }

	void Input_Render_Info(CUIRender_Batching::UIRENDER_INFO* pInfo) { m_pUIManager->Input_Render_Info(pInfo); }

	_Vec2 CheckMouse(_Vec2 fPos, _Vec2 fSize) { return m_pUIManager->CheckMouse(fPos, fSize); }

	_bool Action_InterAction(_wstring strInterName) { return m_pUIManager->Action_InterAction(strInterName); }

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
	{
		m_pUIManager->ShowTestPage(eName, DataNameA, eTypeA, ValueA, DataNameB, eTypeB, ValueB, DataNameC, eTypeC, ValueC, DataNameD, eTypeD, ValueD, DataNameE, eTypeE, ValueE,
			DataNameF, eTypeF, ValueF, DataNameG, eTypeG, ValueG, DataNameH, eTypeH, ValueH, DataNameI, eTypeI, ValueI, DataNameJ, eTypeJ, ValueJ);
	}// 함수 사용하면 입력 내용 대로 해당 프레임에 창을 띄워 준다, DataNameA: 데이터의 이름, TEST_PAGE_VALUE_TYPE: 자료형(float,int,tchar), ValueA: 값을 받아올 포인터 <- 이렇게 한줄
// 매 프레임 마다 함수를 사용해줘야 작동함 (Gameinterface가 외부 정보를 저장/관리하지 않도록 하기 위함)

	void Set_Scroll_Area(SCROLL_AREA eArea, _Vec2 vPos, _Vec2 vSize) { m_pUIManager->Set_Scroll_Area(eArea, vPos, vSize); }
	void Select_Scroll_Area(SCROLL_AREA eArea) { m_pUIManager->Select_Scroll_Area(eArea); }

	void Set_Scroll_Y_Offset(SCROLL_AREA eArea, _float fOffset) { m_pUIManager->Set_Scroll_Y_Offset(eArea, fOffset); }

	// 직교 UI 요청 
	void Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE eType, void* pObj) { m_pUIManager->Register_Pointer_Into_OrthoUIPage(eType, pObj); }
	void Set_OnOff_OrthoUI(_bool bIsOn, void* pObj) { m_pUIManager->Set_OnOff_OrthoUI(bIsOn, pObj); }

	// 보스 체력바 
	void Activate_Boss_Hp_Bar(_bool bActive) { m_pUIManager->Activate_Boss_Hp_Bar(bActive); }
	void Set_Boss_Hp_Bar_Info(_wstring strName, _float HpNow, _float HpMax) { m_pUIManager->Set_Boss_Hp_Bar_Info(strName, HpNow, HpMax); }


	// 아이템 관련
	void Input_Drop_Item_Info(_int iIndex, _int iCount) { m_pUIManager->Input_Drop_Item_Info(iIndex, iCount); }
	void SetWeaponLock(_bool bIsWeaponLock) { m_pUIManager->SetWeaponLock(bIsWeaponLock); }

	// 메인 조정 
	HRESULT Open_Loading_Page() { return m_pUIManager->Open_Loading_Page(); }
	_bool Get_Start_Loading() { return m_pUIManager->Get_Start_Loading(); }

	// 로딩바 조정 
	void Set_Loading_Status(_wstring strMessage, _float fProgress) { m_pUIManager->Set_Loading_Status(strMessage, fProgress); }

	// 아이템 관련 UI 조작 
	void Show_Focus(_Vec2 vItemCellPos, _Vec2 vItemCellSize) { m_pUIManager->Show_Focus(vItemCellPos, vItemCellSize); }
	void Off_Focus() { m_pUIManager->Off_Focus(); }

	void Show_NewMark(_Vec2 vItemCellPos, _Vec2 vItemCellSize) { m_pUIManager->Show_NewMark(vItemCellPos, vItemCellSize); }

	void Show_ItemAction(_Vec2 vItemCellPos, _Vec2 vItemCellSize, ITEM_FUNC eFunc0, ITEM_FUNC eFunc1 = ITEM_FUNC::FUNC_END, ITEM_FUNC eFunc2 = ITEM_FUNC::FUNC_END, ITEM_FUNC eFunc3 = ITEM_FUNC::FUNC_END)
	{
		m_pUIManager->Show_ItemAction(vItemCellPos, vItemCellSize, eFunc0, eFunc1, eFunc2, eFunc3);
	}
	void Off_ItemAction() { m_pUIManager->Off_ItemAction(); }

	void Off_ItemInfo_UI() { m_pUIManager->Off_ItemInfo_UI(); }

	void Show_Tooltip_Shop(_int iShopIndex) { m_pUIManager->Show_Tooltip_Shop(iShopIndex); }
	void Show_Tooltip_Chest(_int iChestIndex) { m_pUIManager->Show_Tooltip_Chest(iChestIndex); }
	void Show_Tooltip(INVEN_ARRAY_TYPE eType, _int iIndex) { m_pUIManager->Show_Tooltip(eType, iIndex); }
	void Show_Tooltip(EQUIP_SLOT eSlot) { m_pUIManager->Show_Tooltip(eSlot); }

	// UI 연출 
	void Fade_Out(_wstring strTitle, _wstring strDesc, _Vec3 vColor = _Vec3{ 0.f,0.f,0.f }, _float fTime = 1.f) {  m_pUIManager->Fade_Out(strTitle, strDesc, vColor, fTime); }
	void Fade_In(_float fTime = 1.f) {  m_pUIManager->Fade_In(fTime); }
	void Show_Script(_wstring strScript0, _wstring strScript1 = TEXT("none"), _float fTime = 1.f, _Vec3 vColor = _Vec3{ 0.f,0.f,0.f }) { m_pUIManager->Show_Script(strScript0, strScript1, fTime, vColor); }
	_float Check_Fade() { return m_pUIManager->Check_Fade(); }
	void UIPart_On() { m_pUIManager->UIPart_On(); }
	void UIPart_Off() { m_pUIManager->UIPart_Off(); }

	// NPC 스크립트 
	void Show_Script_Npc_Talking(NPC_SCRIPT eNPC, _int iScriptNum = -1) { m_pUIManager->Show_Script_Npc_Talking(eNPC, iScriptNum); }
	_bool IsTalking_WithNPC() { return m_pUIManager->IsTalking_WithNPC(); }
	_bool IsTalking_SelectPage() { return m_pUIManager->IsTalking_SelectPage(); }
	void Next_Script() { m_pUIManager->Next_Script(); }
	void OFF_Script() { m_pUIManager->OFF_Script(); }
	void Show_Select_Script(_wstring strLeft, _wstring strRight, _float fTime) { m_pUIManager->Show_Select_Script(strLeft, strRight, fTime); }
	_bool IsLeft_LastSelect_Result() { return m_pUIManager->IsLeft_LastSelect_Result(); }

	// 안내 표시 
	void Show_Region_Info(_wstring strName, _wstring strDesc, _float fTime_Emerge = 1.f, _float fTime_Show = 2.f) { m_pUIManager->Show_Region_Info(strName, strDesc, fTime_Emerge, fTime_Show); }
	void Reservate_Region_Info(_int iCellnum) { m_pUIManager->Reservate_Region_Info(iCellnum); }
	void Show_Reservate_Region_Info() { m_pUIManager->Show_Reservate_Region_Info(); }

	void Show_Inform(INFORM_MESSAGE eInform, _float fTime_Emerge = 1.f, _float fTime_Show = 2.f) { m_pUIManager->Show_Inform(eInform, fTime_Emerge, fTime_Show); }
	void Show_Heart(_wstring strScript, _float fTime_Emerge = 1.f, _float fTime_Show = 2.f) { m_pUIManager->Show_Heart(strScript, fTime_Emerge, fTime_Show); }

	void Show_Popup(_wstring strTitle, _wstring strDescA, _wstring strDescB = TEXT("none")) { m_pUIManager->Show_Popup(strTitle, strDescA, strDescB); }
	void Off_Popup() { m_pUIManager->Off_Popup(); }
	void Show_ItemPopup(_wstring strTitle, _wstring strInputTitle = TEXT("none"), _int iMin = 0, _int* pNow_Input = nullptr, _int iMax = 0, _wstring strCountTitle = TEXT("none"), _int iInterval = 0.f, _int* pNow_Count = nullptr)
	{
		m_pUIManager->Show_ItemPopup(strTitle, strInputTitle, iMin, pNow_Input, iMax, strCountTitle, iInterval, pNow_Count);
	}
	_bool IsPopupOn() { return m_pUIManager->IsPopupOn(); }

	void Show_ItemUsePopup(_wstring strTitle, _wstring strDescA, _bool bIsTop) { m_pUIManager->Show_ItemUsePopup(strTitle, strDescA, bIsTop); }
	void Show_TrueFalsePopup(_wstring strTitle, _wstring strDescA, _int* iResult) { m_pUIManager->Show_TrueFalsePopup(strTitle, strDescA, iResult); } // true,false,none 3개 상태 표현 위해 _int 사용

	void Show_PlayerDead_UI() { m_pUIManager->Show_PlayerDead_UI(); }
	_bool IsPlayerDead_UI_NowEnd() { return m_pUIManager->IsPlayerDead_UI_NowEnd(); }
	void SetPlayerDead_UI_NowEnd_False() { m_pUIManager->SetPlayerDead_UI_NowEnd_False(); }

	// 텔레포트
	void Set_Now_Interact_Stargezer(_int iNaviIndex) { return m_pUIManager->Set_Now_Interact_Stargezer(iNaviIndex); }

	// 업적
	void Input_Achievment_Data(_int iIndex, _int Data) { m_pUIManager->Input_Achievment_Data(iIndex, Data); }

	// 튜토리얼
	void Set_PlayerStateChange() { m_pUIManager->Set_PlayerStateChange(); }

	void Start_Tutorial() { return m_pUIManager->Start_Tutorial(); }
	void End_Tutorial() { return m_pUIManager->End_Tutorial(); }

	_bool IsTutorial_Open() { return m_pUIManager->IsTutorial_Open(); } // <- 튜토리얼 진행 중인 지 확인 
	_bool Get_TutorialPopup_Open() { return m_pUIManager->Get_TutorialPopup_Open(); } // <- 튜토리얼 중 안내 팝업이 떴는 지 확인
	_int Get_NowChapter() { return m_pUIManager->Get_NowChapter(); } // <- 현재 진행 중인 챕터 확인

	void Input_TrainingMonsterPointer_Attack(class CGameObject* pPoiter) { m_pUIManager->Input_TrainingMonsterPointer_Attack(pPoiter); }
	void Input_TrainingMonsterPointer_Normal(class CGameObject* pPoiter) { m_pUIManager->Input_TrainingMonsterPointer_Normal(pPoiter); }

	// 게임 종료 
	void Exit_Program() { m_pUIManager->Exit_Program(); }

#pragma endregion


#pragma region Item_Manager
	// 플레이어 참조용 
	void Adjust_Spec() { m_pItem_Manager->Adjust_Spec(); }
	CPlayer::WEAPON_TYPE Get_Weapon_Model_Index() { return m_pItem_Manager->Get_Weapon_Model_Index(); } // 현재 사용 중인 무기의 모델 번호 리턴
	list<SPECIAL_ITEM>& Get_LastFrame_UsingItem_Info() { return m_pItem_Manager->Get_LastFrame_UsingItem_Info(); }
	_bool Get_CanSwitch_Weapon() { return m_pItem_Manager->Get_CanSwitch_Weapon(); }
	void Add_Durable_Weapon(_float fAdd) { return m_pItem_Manager->Add_Durable_Weapon(fAdd); }
	SPECIAL_ITEM Get_Now_Select_Item() { return m_pItem_Manager->Get_Now_Select_Item(); }

	// 접근, 수정
	_bool Is_ItemData_Change() { return m_pItem_Manager->Is_ItemData_Change(); }

	ITEM_RESULT AddNewItem_Inven(_uint iItemIndex, _uint iCount = 1) { return m_pItem_Manager->AddNewItem_Inven(iItemIndex, iCount); } // <- 새롭게 아이템을 만들어 인벤에 넣는다 
	ITEM_RESULT EquipItem_Inven(INVEN_ARRAY_TYPE eIndex, EQUIP_SLOT eSlot, _uint iIndex) { return m_pItem_Manager->EquipItem_Inven(eIndex, eSlot, iIndex); } // <- 인벤에 있는 아이템을 장비한다 
	ITEM_RESULT UseItem_Equip(EQUIP_SLOT eSlot, _uint iCount = 1) { return m_pItem_Manager->UseItem_Equip(eSlot, iCount); } // <- 장비된 아이템을 사용한다
	ITEM_RESULT UseItem_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex, _uint iCount = 1) { return m_pItem_Manager->UseItem_Inven(eIndex, iIndex, iCount); } // <- 인벤에 있는 아이템을 직접 사용 
	ITEM_RESULT Remove_Item_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex) { return m_pItem_Manager->Remove_Item_Inven(eIndex, iIndex); } // <- 인벤의 아이템 제거

	ITEM_RESULT Use_Potion_Slot(_uint iCount = 1) { return m_pItem_Manager->Use_Potion_Slot(iCount); } // <- 선택된 위 벨트 아이템 사용
	ITEM_RESULT Use_Tool_Slot(_uint iCount = 1) { return m_pItem_Manager->Use_Tool_Slot(iCount); }  // <- 선택된 아래 벨트 아이템 사용
	ITEM_RESULT Use_Bag_Slot(_uint iSelect, _uint iCount = 1) { return m_pItem_Manager->Use_Bag_Slot(iSelect, iCount); }  // <- 선택된 보조 가방 아이템 사용

	ITEM_RESULT Get_Array_Info(INVEN_ARRAY_TYPE eIndex, _wstring* strName, _int* iRow) { return m_pItem_Manager->Get_Array_Info(eIndex, strName, iRow); } // <- 인벤 배열 정보 획득

	const CItem_Manager::ITEM* Get_Item_Origin_Spec(_uint iIndex) { return m_pItem_Manager->Get_Item_Origin_Spec(iIndex); }

	const CItem_Manager::ITEM* Get_Array_Item_Info(INVEN_ARRAY_TYPE eIndex, _uint iIndex) { return m_pItem_Manager->Get_Array_Item_Info(eIndex, iIndex); } // <- 인벤의 아이템 정보 획득

	

	const CItem_Manager::ITEM* Get_Equip_Item_Info(EQUIP_SLOT eSlot) { return m_pItem_Manager->Get_Equip_Item_Info(eSlot); } // 현재 장비창에 장착된 아이템의 정보 레퍼런스 획득

	const CItem_Manager::EQUIP* Get_Equip_Slot_Info(EQUIP_SLOT eSlot) { return m_pItem_Manager->Get_Equip_Slot_Info(eSlot); }

	// 포션, 투척물 
	void Set_PotionThrow_Lock(_bool bIsLock) { m_pItem_Manager->Set_PotionThrow_Lock(bIsLock); }


	// 선택 아이템 조정
	_int Change_Potion_Select(_bool bNext) { return m_pItem_Manager->Change_Potion_Select(bNext); }
	_int Change_Tool_Select(_bool bNext) { return m_pItem_Manager->Change_Tool_Select(bNext); }
	_int Get_Potion_Select() { return m_pItem_Manager->Get_Potion_Select(); }
	_int Get_Tool_Select() { return m_pItem_Manager->Get_Tool_Select(); }
	void Set_Select(_bool bIsPotion) { m_pItem_Manager->Set_Select(bIsPotion); }

	// 무기 관련
	_int Change_Weapon() { return m_pItem_Manager->Change_Weapon(); }
	_int Get_Weapon() { return m_pItem_Manager->Get_Weapon(); }

	const CItem_Manager::ITEM* Get_Now_Equip_Weapon_Blade() { return m_pItem_Manager->Get_Now_Equip_Weapon_Blade(); }
	const CItem_Manager::ITEM* Get_Now_Equip_Weapon_Handle() { return m_pItem_Manager->Get_Now_Equip_Weapon_Handle(); }

	

	// 포션 관련
	void Add_Potion_Gauge(_float fAdd) { m_pItem_Manager->Add_Potion_Gauge(fAdd); }
	_bool Use_Potion() { return m_pItem_Manager->Use_Potion(); }

	// 리전 암 관련
	void Add_Arm_Gauge(_float fAdd) { m_pItem_Manager->Add_Arm_Gauge(fAdd); }
	_float Get_Arm_Gauge_Ratio() { return m_pItem_Manager->Get_Arm_Gauge_Ratio(); }


	// New 관련 
	_int Get_IsNew(INVEN_ARRAY_TYPE eType, _int iIndex) { m_pItem_Manager->Get_IsNew(eType, iIndex); }
	void Set_IsNew(INVEN_ARRAY_TYPE eType, _int iIndex, _bool IsNew) { m_pItem_Manager->Set_IsNew(eType, iIndex, IsNew); }
	void Set_IsNew_Show(INVEN_ARRAY_TYPE eType, _int iIndex) { m_pItem_Manager->Set_IsNew_Show(eType, iIndex); }
	void Set_Show_NewMark_Off() { m_pItem_Manager->Set_Show_NewMark_Off(); }

	// 상점, 보관함
	vector<CItem_Manager::SHOP*>& Get_ShopData() { return m_pItem_Manager->Get_ShopData(); }
	map<_int, CItem_Manager::ITEM*>& Get_ChestData() { return m_pItem_Manager->Get_ChestData(); }
	void Buy_ShopItem(_int iIndex, _int iCount = 1) { m_pItem_Manager->Buy_ShopItem(iIndex, iCount); }
	void Sell_ShopItem(INVEN_ARRAY_TYPE eType, _int iIndex, _int iCount = 1) { m_pItem_Manager->Sell_ShopItem(eType, iIndex, iCount); }
	void ChestItem_To_Inven(_int iIndex, _int iCount = 0) { m_pItem_Manager->ChestItem_To_Inven(iIndex, iCount); }
	_int InvenItem_To_Chest(INVEN_ARRAY_TYPE eType, _int iIndex, _int iCount = 0) { return m_pItem_Manager->InvenItem_To_Chest(eType, iIndex, iCount); }

#pragma endregion


#pragma region Player_Stat_Manager
	// 정보 접근 
	//const _float& Get_NowStat_Normal(STAT_NORMAL eIndex) { return m_pPlayer_Stat_Manager->Get_NowStat_Normal(eIndex); }
	//const CPlayer_Stat_Manager::STAT& Get_StatInfo_Normal(STAT_NORMAL eIndex) { return m_pPlayer_Stat_Manager->Get_StatInfo_Normal(eIndex); }

	//const _float Get_Now_Max_Ratio(STAT_NORMAL eIndex) { return  m_pPlayer_Stat_Manager->Get_Now_Max_Ratio(eIndex); }
	//const _float Get_Max_Limit_Ratio(STAT_NORMAL eIndex) { return  m_pPlayer_Stat_Manager->Get_Max_Limit_Ratio(eIndex); }
	//const _float Get_Now_Limit_Ratio(STAT_NORMAL eIndex) { return m_pPlayer_Stat_Manager->Get_Now_Limit_Ratio(eIndex); }

	//const CPlayer_Stat_Manager::STAT& Get_StatInfo_DEF(STAT_DEF eIndex) { return m_pPlayer_Stat_Manager->Get_StatInfo_DEF(eIndex); }

	//_bool Get_Buff(BUFF_INDEX eIndex) { return m_pPlayer_Stat_Manager->Get_Buff(eIndex); }
	//_float Get_Buff_Ratio(BUFF_INDEX eIndex) { return m_pPlayer_Stat_Manager->Get_Buff_Ratio(eIndex); }
	const CPlayer_Stat_Manager::BUFF* Get_Buff_Info(BUFF_INDEX eIndex) { return m_pPlayer_Stat_Manager->Get_Buff_Info(eIndex); }
	//void Start_Buff(BUFF_INDEX eIndex) { m_pPlayer_Stat_Manager->Start_Buff(eIndex); }

	// 정보 수정 
	//void Add_Stat_Normal(STAT_NORMAL eIndex, _float fValue) { return m_pPlayer_Stat_Manager->Add_Stat_Normal(eIndex, fValue); }
	//void Add_StatMax_Normal(STAT_NORMAL eIndex, _float fValue) { return m_pPlayer_Stat_Manager->Add_StatMax_Normal(eIndex, fValue); }

#pragma endregion


private:
	CGameInstance* m_pGameInstance = { nullptr };
	CUIManager* m_pUIManager = { nullptr };
	CItem_Manager* m_pItem_Manager = { nullptr };
	CPlayer_Stat_Manager* m_pPlayer_Stat_Manager = { nullptr };

	// 플레이어 포인터
	// 얕은 복사로 사용한다 
	// 레퍼런스 조정하지 않는다 
	CPlayer* m_pPlayer = { nullptr };


private: // <- 디바이스 
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END