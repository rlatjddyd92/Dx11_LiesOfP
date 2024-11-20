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

		_uint Get_Array_Size() { return vecItemInfo.size(); }
		ITEM* Get_Item_Info(_uint iIndex) 
		{
			if ((iIndex < 0) || (iIndex >= Get_Array_Size()))
				return nullptr;
			return vecItemInfo[iIndex];
		}
		ITEM_RESULT Input_Item(ITEM* pNew, _uint iCount)
		{
			if (pNew->bStack)
				for (auto& iter : vecItemInfo)
					if (iter->iItem_Index == pNew->iItem_Index)
					{
						iter->iCount += iCount;
						return ITEM_RESULT::RESULT_SUCCESS;
					}

			if (iNextIndex >=Get_Array_Size())
				Add_Array();

			*vecItemInfo[iNextIndex] = *pNew;

			//memcpy(&vecItemInfo[iNextIndex], pNew, sizeof(ITEM));

			++iNextIndex;

			return ITEM_RESULT::RESULT_SUCCESS;
		}
		ITEM_RESULT Use_Item(_uint iIndex, _uint iCount)
		{
			if ((iIndex < 0) || (iIndex >= Get_Array_Size()))
				return ITEM_RESULT::RESULT_INVALID;
			if (vecItemInfo[iIndex]->eType_Index == ITEM_TYPE::ITEMTYPE_END)
				return ITEM_RESULT::RESULT_INVALID;

			if(!vecItemInfo[iIndex]->bStack)
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


private:
	CItem_Manager(CGameInstance* pGameInstance);
	virtual ~CItem_Manager() = default;


public:
	// 접근, 수정
	ITEM_RESULT AddNewItem_Inven(_uint iItemIndex, _uint iCount = 1); // <- 새롭게 아이템을 만들어 인벤에 넣는다 
	ITEM_RESULT EquipItem_Inven(INVEN_ARRAY_TYPE eIndex, EQUIP_SLOT eSlot, _uint iIndex); // <- 인벤에 있는 아이템을 장비한다 
	ITEM_RESULT UseItem_Equip(EQUIP_SLOT eSlot, _uint iCount = 1); // <- 장비된 아이템을 사용한다
	ITEM_RESULT UseItem_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex, _uint iCount = 1); // <- 인벤에 있는 아이템을 직접 사용 
	ITEM_RESULT Remove_Item_Inven(INVEN_ARRAY_TYPE eIndex, _uint iIndex); // <- 인벤의 아이템 제거

	CPlayer::WEAPON_TYPE Get_Weapon_Model_Index(); // 현재 사용 중인 무기의 모델 번호 리턴

	const ITEM& Get_Equip_Item_Info(EQUIP_SLOT eSlot) // 현재 장비창에 장착된 아이템의 정보 레퍼런스 획득
	{ 
		return *m_vecArray_Inven[_uint(m_vecEquip_ItemInfo[_uint(eSlot)]->eType)]->vecItemInfo[_uint(m_vecEquip_ItemInfo[_uint(eSlot)]->iIndex)];
	}

	
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

	_uint m_iInven_Array_Col_Count = 5; // <- 인벤 한 줄에 몇 개의 셀이 들어가는 지




public:
	static CItem_Manager* Create(CGameInstance* pGameInstance);
	virtual void Free() override;

};

END

