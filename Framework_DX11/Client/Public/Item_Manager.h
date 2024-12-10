#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Interface_Enums.h"

#include "Player.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

/*
[Item_Manager]
1. 역할
 0) 아이템의 기본 스펙 데이터 관리
 1) 인벤토리 관리
 2) 장착 아이템 관리
 3) 아이템 획득, 사용 제어

 ※ 추가 논의 필요한 사항
 아래는 확정이 아니며 논의 필요
 4) NPC 상점 데이터 관리
 5) 몬스터 별 드랍 아이템 제어
 6) 퀘스트, 이벤트로 획득하는 아이템의 정보 관리

2. 인스턴스
 1) 아이템 스펙 접근
 2) 인벤 접근
 3) 장차 아이템 접근


3. 고려 사항
 1) 테스트 아이템을 수시로 바꿀 수 있는 편의 시스템
 2) 다수의 아이템 정보를 엑셀로 관리하도록 구현

4. 규칙
 1) 신규 아이템 생성 & 복제 -> 깊은 복사
 2) 아이템 이동 -> 얕은 복사 후 기존 포인터 nullptr 처리 (Delete 절대 하지 않음)
 3) 아이템 삭제 -> Safe_Delete 처리
 4) 아이템 장착 -> 얕은 복사만 진행
 5) 아이템 해제 -> Equip쪽에서 nullptr 처리만 진행 (절대 Delete 하지 않음)
 6) 외부 참조 -> 반드시 레퍼런스로



*/



/*
아뮬렛, 방어파츠 스펙 간소화 

아뮬렛 : 당장 구현할 수 있는 간단한 것만 넣기

방어파츠 :
프레임 : 방어력 증가 
컨버터 : 화염 저항 증가 
카트리지 : 전기 저항 증가 
라이너 : 산성 저항 증가 



*/

class CItem_Manager : public CBase
{
public:
	typedef struct ITEM_INFO // 게임에 존재하는 아이템 1종의 모든 스펙 정리 
	{
		ITEM_INFO()
		{
			vecDamege.resize(_uint(DAMEGE_TYPE::DAMEGE_END));
			vecDefence.resize(_uint(DEFENCE_TYPE::DEFENCE_END));
			vecAblity_Grade.resize(_uint(ABLITY_TYPE::ABLITY_END));
		}

		~ITEM_INFO()
		{
			vecDamege.clear();
			vecDefence.clear();
			vecAblity_Grade.clear();
		}

		_uint iItem_Index = -1;
		_wstring strName = {};
		_uint iTexture_Index = -1;
		ITEM_TYPE eType_Index = ITEM_TYPE::ITEMTYPE_END; // <- 어떤 아이템인지 인덱스로 기록
		_bool bStack = false; // 스택 가능 여부
		_uint iCount = 1; // 수량 (비 스택 아이템은 무조건 1)

		// 날/자루 관련
		_bool bModule_Weapon = false; // <- 날/자루 분리하는 무기인지 
		_bool bIsHandele = false; // <- 자루 종류인 경우, false면 날임 
		_uint iOtherPart_Index = -1; // <- 날/자루의 경우 다른 쪽 파츠의 스펙 인덱스
		const ITEM_INFO* pOtherPart = { nullptr }; // <- 날 자루 연결된 경우 다른편 파츠의 포인터 

		// 내구도
		_float fDurable_Now = 0.f;
		_float fDurable_Max = 0.f;

		// 무게 
		_float fWeight = 0.f;

		// 데미지
		vector<_float> vecDamege;

		// 방어
		vector<_float> vecDefence;

		// 특수공격
		_wstring strAttack_Type = {};
		_float fType_Damege = 0.f;
		_float fType_Damege_Fatal_Ratio = 0.f;

		// 충전
		_float fPable_Charge = 0.f;
		_float fPulse_Charge = 0.f;

		// 가드 시 피해감소 
		_float fGuard_Damege_Reduce = 0.f;

		//페이블 아츠 
		_wstring strFable_Art_Name = {};
		_uint iFable_Art_Cost = {};

		// 능력(?) <- 아마 안 쓸 것 같은데 일단 데이터만 가지고 있기 
		vector<_wstring> vecAblity_Grade;

		_wstring strSlash_Grade = {};
		_wstring strPirce_Grade = {};

		_wstring strFabel_Desc = {};
		_wstring strItem_Desc = {};

		// 암 관련
		_float Arm_Gauge_Now = 1000.f;
		_float Arm_Gauge_Max = 1000.f;

		// 장착 정보 
		EQUIP_SLOT eSlot = EQUIP_SLOT::EQUIP_END;

		// 획득 정보 
		_bool bIsNew = false;
		_bool bIsNewMark_Show = false;

		// 상점, 보관함
		_bool bIsAvailable_Shop = false;
		_bool bIsAvailable_Chest = false;

	}ITEM;

	typedef struct ARRAY_INFO // 인벤에 존재하는 배열의 정보 
	{
		ARRAY_INFO()
		{
			vecItemInfo.resize(5);
			for (auto& iter : vecItemInfo)
				iter = new ITEM;
		}

		~ARRAY_INFO()
		{
			for (auto& iter : vecItemInfo)
				Safe_Delete(iter);

			vecItemInfo.clear();
			ValidEquipIndexlist.clear();
		}

		void Add_Array()
		{
			for (_uint i = 0; i < 5; ++i)
			{
				ITEM* pNew = new ITEM;
				vecItemInfo.push_back(pNew);
			}
		}

		_uint Get_Array_Size() { return _uint(vecItemInfo.size()); }
		ITEM* Get_Item_Info(_uint iIndex)
		{
			if ((iIndex < 0) || (iIndex >= Get_Array_Size()))
				return nullptr;
			return vecItemInfo[iIndex];
		}
		ITEM_RESULT Input_Item(ITEM* pNew, _uint iCount)
		{
			if (pNew->bStack)
			{
				for (auto& iter : vecItemInfo)
					if (iter->iItem_Index == pNew->iItem_Index)
					{
						iter->iCount += iCount;
						return ITEM_RESULT::RESULT_SUCCESS;
					}

				pNew->iCount = iCount;
			}
				

			if (iNextIndex >= Get_Array_Size())
				Add_Array();

			*vecItemInfo[iNextIndex] = *pNew;

			//memcpy(&vecItemInfo[iNextIndex], pNew, sizeof(ITEM));

			vecItemInfo[iNextIndex]->bIsNew = true;
			vecItemInfo[iNextIndex]->bIsNewMark_Show = false;

			++iNextIndex;

			return ITEM_RESULT::RESULT_SUCCESS;
		}
		ITEM_RESULT Use_Item(_uint iIndex, _uint iCount)
		{
			if ((iIndex < 0) || (iIndex >= Get_Array_Size()))
				return ITEM_RESULT::RESULT_INVALID;
			if (vecItemInfo[iIndex]->eType_Index == ITEM_TYPE::ITEMTYPE_END)
				return ITEM_RESULT::RESULT_INVALID;

			if (!vecItemInfo[iIndex]->bStack)
				return ITEM_RESULT::RESULT_SUCCESS;
			else if (vecItemInfo[iIndex]->iCount >= iCount)
			{
				vecItemInfo[iIndex]->iCount -= iCount;

				return ITEM_RESULT::RESULT_SUCCESS;
			}
			else
				return ITEM_RESULT::RESULT_INVALID;
		}
		ITEM_RESULT Remove_Item(_uint iIndex)
		{
			if ((iIndex < 0) || (iIndex >= Get_Array_Size()))
				return ITEM_RESULT::RESULT_INVALID;
			if (vecItemInfo[iIndex]->eType_Index == ITEM_TYPE::ITEMTYPE_END)
				return ITEM_RESULT::RESULT_INVALID;

			Safe_Delete(vecItemInfo[iIndex]);
			vecItemInfo[iIndex] = new ITEM;

			return ITEM_RESULT::RESULT_SUCCESS;
		}

		_wstring strInven_Array_Name = {};
		_uint iNextIndex = 0; // 
		vector<ITEM*> vecItemInfo;  // 배열에 들어간 아이템 정보
		list<_uint> ValidEquipIndexlist;
	}ARRAY;

	typedef struct EQUIP_INFO
	{
		EQUIP_INFO()
		{
			vecValid_InvenArray.resize(_uint(INVEN_ARRAY_TYPE::TYPE_END));
		}
		~EQUIP_INFO()
		{
			vecValid_InvenArray.clear();
		}

		INVEN_ARRAY_TYPE eType = INVEN_ARRAY_TYPE::TYPE_END; // <- 장착 중인 아이템이 어떤 Array에 있는 지를 의미한다 
		_uint iIndex = 0; // <- 아이템이 위치한 인덱스 
		vector<_bool> vecValid_InvenArray; // 해당 장착슬롯에 연결될 수 있는 인벤 Array의 목록
	}EQUIP;

	typedef struct SHOP_INFO
	{
		_int iIndex = -1;
		_int iCount = 0;
		_int iPrice_Buy = 0;
	}SHOP;


private:
	CItem_Manager(CGameInstance* pGameInstance);
	virtual ~CItem_Manager() = default;


public:
	// 업데이트
	void Update_Item(_float fDeltatime);

	// 플레이어 기능 구현
	CPlayer::WEAPON_TYPE Get_Weapon_Model_Index(); // 현재 사용 중인 무기의 모델 번호 리턴
	list<SPECIAL_ITEM>& Get_LastFrame_UsingItem_Info(); // 이번 프레임에 기능 구현이 필요한 아이템을 사용했는지 확인
	SPECIAL_ITEM Check_Item_Special_Enum(_int iIndex);
	void Set_Item_Funtion(_int iItem_Index);
	_bool Get_CanSwitch_Weapon(); // 무기 슬롯 2개 중 1개만 아이템이 있는 경우 false 반환

	SPECIAL_ITEM Get_Now_Select_Item()
	{
		_int iIndex = -1;
		if (m_bIsPotion)
		{
			const ITEM* pItem = Get_Equip_Item_Info(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_TOP_0) + m_iPotion_Select));
			if (pItem != nullptr)
				iIndex = pItem->iItem_Index;
		}
		else 
		{
			const ITEM* pItem = Get_Equip_Item_Info(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0) + m_iTool_Select));
			if (pItem != nullptr)
				iIndex = pItem->iItem_Index;
		}

		return Check_Item_Special_Enum(iIndex);
	}

	// 접근, 수정
	_bool Is_ItemData_Change() { return m_bIsChange; }
	ITEM_RESULT AddNewItem_Inven(_uint iItemIndex, _uint iCount = 1); // <- 새롭게 아이템을 만들어 인벤에 넣는다 
	ITEM_RESULT EquipItem_Inven(INVEN_ARRAY_TYPE eIndex, EQUIP_SLOT eSlot, _uint iIndex);
	ITEM_RESULT EquipWeapon_Inven(INVEN_ARRAY_TYPE eIndex, EQUIP_SLOT eSlot, _uint iIndex); // <- 인벤에 있는 아이템을 장비한다 
	ITEM_RESULT UnEquipItem_Inven(EQUIP_SLOT eSlot); // <- 인벤에 있는 아이템을 장비한다 
	ITEM_RESULT UnEquipWeapon_Inven(EQUIP_SLOT eSlot); // <- 인벤에 있는 아이템을 장비한다 

	ITEM_RESULT Move_EquipItem(EQUIP_SLOT eBefore, EQUIP_SLOT eDest); // <-장비된 아이템의 장비 위치 변경

	ITEM_RESULT UseItem_Equip(EQUIP_SLOT eSlot, _uint iCount = 1); // <- 장비된 아이템을 사용한다
	ITEM_RESULT UseItem_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex, _uint iCount = 1); // <- 인벤에 있는 아이템을 직접 사용

	ITEM_RESULT Use_Potion_Slot(_uint iCount = 1) { return UseItem_Equip(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_TOP_0) + m_iPotion_Select), iCount); } // <- 선택된 위 벨트 아이템 사용
	ITEM_RESULT Use_Tool_Slot(_uint iCount = 1) { return UseItem_Equip(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_BOTTOM_0) + m_iTool_Select), iCount); }  // <- 선택된 아래 벨트 아이템 사용
	ITEM_RESULT Use_Bag_Slot(_uint iSelect, _uint iCount = 1) { return UseItem_Equip(EQUIP_SLOT(_int(EQUIP_SLOT::EQUIP_USING_BAG_0) + iSelect), iCount); }  // <- 선택된 보조 가방 아이템 사용

	ITEM_RESULT Remove_Item_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex); // <- 인벤의 아이템 제거

	

	ITEM_RESULT Get_Array_Info(INVEN_ARRAY_TYPE eIndex, _wstring* strName, _int* iRow)
	{
		if ((_int(eIndex) < 0) || (_int(eIndex) >= _int(INVEN_ARRAY_TYPE::TYPE_END)))
			return ITEM_RESULT::RESULT_INVALID;

		*strName = m_vecArray_Inven[_int(eIndex)]->strInven_Array_Name;
		*iRow = m_vecArray_Inven[_int(eIndex)]->Get_Array_Size() / 5;

		return ITEM_RESULT::RESULT_SUCCESS;
	}
	const ITEM* Get_Item_Origin_Spec(_uint iIndex)
	{
		if ((iIndex < 0) || (iIndex >= m_vecItem_BasicSpec.size()))
			return nullptr;

		return m_vecItem_BasicSpec[iIndex];
	}
	const ITEM* Get_Array_Item_Info(INVEN_ARRAY_TYPE eIndex, _uint iIndex) // <- 인벤의 아이템 정보 획득
	{
		if ((_int(eIndex) < 0) || (_int(eIndex) >= _int(INVEN_ARRAY_TYPE::TYPE_END)))
			return nullptr;

		if ((iIndex < 0) || (iIndex >= m_vecArray_Inven[_int(eIndex)]->Get_Array_Size()))
			return nullptr;

		if (m_vecArray_Inven[_int(eIndex)]->vecItemInfo[iIndex]->eType_Index == ITEM_TYPE::ITEMTYPE_END)
			return nullptr;

		return m_vecArray_Inven[_int(eIndex)]->vecItemInfo[iIndex];
	}

	const ITEM* Get_Equip_Item_Info(EQUIP_SLOT eSlot) // 현재 장비창에 장착된 아이템의 정보 획득
	{
		if ((_int(eSlot) < 0) || (_int(eSlot) >= _int(EQUIP_SLOT::EQUIP_END)))
			return nullptr;

		if (m_vecEquip_ItemInfo[_uint(eSlot)]->eType == INVEN_ARRAY_TYPE::TYPE_END)
			return nullptr;

		return m_vecArray_Inven[_uint(m_vecEquip_ItemInfo[_uint(eSlot)]->eType)]->vecItemInfo[_uint(m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex)];
	}

	const EQUIP* Get_Equip_Slot_Info(EQUIP_SLOT eSlot)
	{
		if ((_int(eSlot) < 0) || (_int(eSlot) >= _int(EQUIP_SLOT::EQUIP_END)))
			return nullptr;

		return m_vecEquip_ItemInfo[_int(eSlot)];
	}






	
	// 선택 아이템 조정
	_int Change_Potion_Select(_bool bNext)
	{
		m_bIsChange = true;
		if (bNext)
			++m_iPotion_Select;
		else
			--m_iPotion_Select;

		if (m_iPotion_Select > 2)
			m_iPotion_Select = 0;
		else if (m_iPotion_Select < 0)
			m_iPotion_Select = 2;

		return m_iPotion_Select;
	}
	_int Change_Tool_Select(_bool bNext)
	{
		m_bIsChange = true;
		if (bNext)
			++m_iTool_Select;
		else
			--m_iTool_Select;

		if (m_iTool_Select > 2)
			m_iTool_Select = 0;
		else if (m_iTool_Select < 0)
			m_iTool_Select = 2;

		return m_iTool_Select;
	}
	_int Get_Potion_Select() { return m_iPotion_Select; }
	_int Get_Tool_Select() { return m_iTool_Select; }
	void Set_Select(_bool bIsPotion) { m_bIsPotion = bIsPotion; }
	

	// 무기 관련
	_int Change_Weapon()
	{
		m_bIsChange = true;
		++m_iWeapon_Select;
		if (m_iWeapon_Select >= 2)
			m_iWeapon_Select = 0;

		return m_iWeapon_Select;
	}
	_int Get_Weapon() { return m_iWeapon_Select; }
	const ITEM* Get_Now_Equip_Weapon_Blade()
	{
		if (m_iWeapon_Select == 0)
			return Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0);
		else
			return Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_WEAPON_BLADE_1);
	}

	const ITEM* Get_Now_Equip_Weapon_Handle()
	{
		if (m_iWeapon_Select == 0)
			return Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_WEAPON_HANDLE_0);
		else
			return Get_Equip_Item_Info(EQUIP_SLOT::EQUIP_WEAPON_HANDLE_1);
	}
	
	void Add_Durable_Weapon(_float fAdd)
	{
		ITEM* pNowWeapon = m_vecArray_Inven[_int(m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0) + m_iWeapon_Select * 2]->eType)]->vecItemInfo[m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_WEAPON_BLADE_0) + m_iWeapon_Select * 2]->iIndex];

		if (pNowWeapon != nullptr)
		{
			pNowWeapon->fDurable_Now += fAdd;
			pNowWeapon->fDurable_Now = min(pNowWeapon->fDurable_Now, pNowWeapon->fDurable_Max);
			pNowWeapon->fDurable_Now = max(pNowWeapon->fDurable_Now, 0.f);
		}
	}



	// 포션 관련 
	void Add_Potion_Gauge(_float fAdd)
	{
		m_bIsChange = true;
		m_fNow_Potion_Gauge += fAdd;
		if (m_fNow_Potion_Gauge >= m_fMax_Potion_Gauge)
		{
			m_fNow_Potion_Gauge = 0.f;
			m_iNow_Potion_Count = min(m_iNow_Potion_Count + 1, m_iMax_Potion_Count);

			for (_int i = 0; i < 5; ++i)
				if (m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_USING_BASIC)]->Get_Item_Info(i)->iItem_Index == _int(SPECIAL_ITEM::SP_PULSE_BATTERY))
					m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_USING_BASIC)]->Get_Item_Info(i)->iCount = m_iNow_Potion_Count;
		}
	}
	_bool Use_Potion();

	// 암 관련
	void Add_Arm_Gauge(_float fAdd)
	{
		m_bIsChange = true;
		_int iIndex = m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_RESION_ARM)]->iIndex;
		ITEM* pItem = m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_REASON_ARM)]->Get_Item_Info(iIndex);
		if (pItem == nullptr)
			return;

		if (fAdd > 0)
			pItem->Arm_Gauge_Now = min(pItem->Arm_Gauge_Now + fAdd, pItem->Arm_Gauge_Max);
		else if ((fAdd < 0) && (abs(fAdd) <= pItem->Arm_Gauge_Now))
			pItem->Arm_Gauge_Now = max(pItem->Arm_Gauge_Now + fAdd, 0.f);
	}
	_float Get_Arm_Gauge_Ratio()
	{
		_int iIndex = m_vecEquip_ItemInfo[_int(EQUIP_SLOT::EQUIP_RESION_ARM)]->iIndex;
		ITEM* pItem = m_vecArray_Inven[_int(INVEN_ARRAY_TYPE::TYPE_REASON_ARM)]->Get_Item_Info(iIndex);
		if (pItem == nullptr)
			return 0.f;

		return pItem->Arm_Gauge_Now / pItem->Arm_Gauge_Max;
	}

	// New 표시 조정 관련 
	_int Get_IsNew(INVEN_ARRAY_TYPE eType, _int iIndex) 
	{
		if(!IsValid_Inven(eType, iIndex))
			return -1;

		if (m_vecArray_Inven[_int(eType)]->Get_Item_Info(iIndex))
			return _int(m_vecArray_Inven[_int(eType)]->Get_Item_Info(iIndex)->bIsNew);
		else
			return -1;
	}
	void Set_IsNew(INVEN_ARRAY_TYPE eType, _int iIndex, _bool IsNew)
	{
		if (!IsValid_Inven(eType, iIndex))
			return;

		if (m_vecArray_Inven[_int(eType)]->Get_Item_Info(iIndex))
		{
			m_vecArray_Inven[_int(eType)]->Get_Item_Info(iIndex)->bIsNew = IsNew;
			m_vecArray_Inven[_int(eType)]->Get_Item_Info(iIndex)->bIsNewMark_Show = false;
		}
		else
			return;
	}
	void Set_IsNew_Show(INVEN_ARRAY_TYPE eType, _int iIndex)
	{
		if (!IsValid_Inven(eType, iIndex))
			return;

		if (m_vecArray_Inven[_int(eType)]->Get_Item_Info(iIndex))
			m_vecArray_Inven[_int(eType)]->Get_Item_Info(iIndex)->bIsNewMark_Show = true;
		else
			return;
	}
	void Set_Show_NewMark_Off()
	{
		for (auto& iter : m_vecArray_Inven)
			for (auto& iterItem : iter->vecItemInfo)
				if (iterItem->bIsNewMark_Show)
				{
					iterItem->bIsNew = false;
					iterItem->bIsNewMark_Show = false;
				}
	}

	// ItemAction 관련 
	void Set_ItemAction(EQUIP_SLOT eSlot, _Vec2 vPos, _Vec2 vSize, _bool bMainTainPage = false);
	void Set_ItemAction(INVEN_ARRAY_TYPE eType, _int iIndex, _Vec2 vPos, _Vec2 vSize, _bool bMainTainPage = false);

	void Reset_ItemAction();

	_bool IsReset_ItemAction();


	ITEM_RESULT Operate_ItemAction(ITEM_FUNC eFunc, _Vec2 vPos, _Vec2 vSize);

	ITEM_RESULT Operate_EquipAction(_Vec2 vPos, _Vec2 vSize);



	// Defence_Info 
	void Adjust_Spec();


	// 상점, 보관함
	vector<SHOP*>& Get_ShopData() { return m_vecShop_Item; }
	vector<ITEM*>& Get_ChestData() { return m_vecChest_Item; }
	void Buy_ShopItem(_int iIndex);
	void Sell_ShopItem(INVEN_ARRAY_TYPE eType, _int iIndex);
	void ChestItem_To_Inven(_int iIndex);
	void InvenItem_To_Chest(INVEN_ARRAY_TYPE eType, _int iIndex);



	// 외부에서 아이템 매니저에 필요한 것이 뭐가 있나? 
	/*
	0. 새로운 아이템을 만들어 유저 인벤에 넣는다
	  -> 아이템 스펙 Array에서 복사해서 인벤에 넣는 기능 필요 + 수량도 설정할 수 있어야 한다 (스택 아이템)
	  -> 만일 스택 아이템이고 이미 인벤에 있다면 숫자만 늘려야 한다
	1. 인벤에 있는 아이템의 저장 위치를 변경한다
	  -> 게임 특성상 쓸 일이 많아보이지 않는다 -> 후순위로 미룬다
	2. 장착 정보를 변경한다
	  -> 어느 Array의 몇 번째 아이템을 사용하는 지만 등록하여 간편하게 구성하자
	  -> 장착 가능한 칸만 구분하여야 한다
	  ->



	*/



private:
	HRESULT Initialize_Item();


	// 접근, 수정
	ITEM_RESULT InputItem_Inven(ITEM* pItem, _uint iCount = 1); // <- 이미 게임에 존재하는 아이템을 인벤에 넣는다 


	// 날 + 자루 합체
	// 아래 내용은 추후 필요하면 퍼블릭으로 이동
	ITEM_RESULT Assemble_Blade_Handle(_int iBladeIndex, _int iHandleIndex);

	// 유효 정보 검증 
	_bool IsValid_Inven(INVEN_ARRAY_TYPE eType, _int iIndex)
	{
		if ((_int(eType) < 0) || (_int(eType) >= _int(INVEN_ARRAY_TYPE::TYPE_END)))
			return false;

		if (m_vecArray_Inven[_int(eType)]->Get_Array_Size() <= _uint(iIndex))
			return false;

		return true;
	}

	_bool IsValid_Equip(EQUIP_SLOT eSlot)
	{
		if ((_int(eSlot) < 0) || (_int(eSlot) >= _int(EQUIP_SLOT::EQUIP_END)))
			return false;

		return true;
	}



private:
	CGameInstance* m_pGameInstance = { nullptr };

	vector<ITEM*> m_vecItem_BasicSpec; // 게임 내 존재하는 모든 아이템의 기본 스펙

	vector<_uint> m_vecItem_InvenSlotIndex; // 아이템 타입 별로 들어갈 수 있는 인벤 슬롯 정보 

	_uint m_iAdd_Handle = 1;
	_uint m_iAdd_Heroic = 2;
	_uint m_iAdd_Separate = 4;
	/*
	날/자루의 경우
	0. 일반 합체 날 +0;
	1. 일반 합체 자루 +1;
	2. 특수 합체 날 +2;
	3. 특수 합체 자루 +3;
	4. 일반 분리 날 +4;
	5. 일반 분리 자루 +5;
	*/

	vector<ARRAY*> m_vecArray_Inven; // 인벤토리 정보 모음 (행 단위)
	vector<EQUIP*> m_vecEquip_ItemInfo; // 현재 어떤 장비 장착 중인 지 확인 

	vector<SHOP*> m_vecShop_Item; // 상점 아이템 목록
	vector<ITEM*> m_vecChest_Item; // 보관함 아이템 목록

	_uint m_iInven_Array_Col_Count = 5; // <- 인벤 한 줄에 몇 개의 셀이 들어가는 지

	// 좌하단 조작용
	_uint m_iPotion_Select = 0;
	_uint m_iTool_Select = 0;
	_bool m_bIsPotion = false;

	// 우하단 조작용
	_uint m_iWeapon_Select = -1;

	// 포션 관련 
	_int m_iNow_Potion_Count = 3;
	_int m_iMax_Potion_Count = 5;
	_float m_fNow_Potion_Gauge = 0.f;
	_float m_fMax_Potion_Gauge = 1000.f;

	// 아이템 갱신 
	_bool m_bIsChange = false;
	_int m_iNow_Using = -1;

	list<SPECIAL_ITEM> m_LastFrame_UsingItem;

	// 아이템 액션 조작 
	_bool m_bItemAction_Active = false;
	EQUIP_SLOT m_eNow_ActionSlot = EQUIP_SLOT::EQUIP_END;
	INVEN_ARRAY_TYPE m_eNow_ActionArray = INVEN_ARRAY_TYPE::TYPE_END;
	_int m_iArray_Index = -1;
	_int m_iActionPopup_Page = 0;
	_bool m_bInstant_Weapon_Change = false;

	_bool m_bSpec_Setting = false;

public:
	static CItem_Manager* Create(CGameInstance* pGameInstance);
	virtual void Free() override;

};

END

