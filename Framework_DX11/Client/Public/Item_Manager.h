#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Interface_Enums.h"

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
		ITEM_INDEX eIndex = ITEM_INDEX::INDEX_END; // <- 어떤 어이템인지 인덱스로 기록
		ITEM_TYPE eType = ITEM_TYPE::TYPE_END; // <- 아이템의 타입 
		_bool bStack = false; // 스택 가능 여부
		_int iCount = 1; // 수량 (비 스택 아이템은 무조건 1)
	}ITEM;

	typedef struct ARRAY_INFO // 인벤에 존재하는 배열의 정보 
	{
		ARRAY_INFO()
		{

		}

		~ARRAY_INFO()
		{
			for (auto& iter : vecItemInfo)
				Safe_Delete(iter);

			vecItemInfo.clear();
			ValidEquipIndexlist.clear();
		}

		_int iRow = 1; // 
		vector<ITEM*> vecItemInfo;  // 배열에 들어간 아이템 정보
		list<_int> ValidEquipIndexlist;
	}ARRAY;



private:
	CItem_Manager(CGameInstance* pGameInstance);
	virtual ~CItem_Manager() = default;


public:
	// 접근, 수정
	ITEM_RESULT AddNewItem_Inven(ITEM_INDEX eIndex, _uint iCount = 1); // <- 새롭게 아이템을 만들어 인벤에 넣는다 
	ITEM_RESULT EquipItem_Inven(ITEM_INDEX eIndex); // <- 인벤에 있는 아이템을 장비한다 
	ITEM_RESULT UseItem_Equip(EQUIP_SLOT eSlot, _uint iCount = 1); // <- 장비된 아이템을 사용한다
	ITEM_RESULT UseItem_Inven(ITEM_INDEX eIndex, _uint iCount = 1); // <- 인벤에 있는 아이템을 직접 사용 (

	const ITEM_INDEX& Get_Equip_Item_Index(EQUIP_SLOT eSlot) { return m_vecEquip_ItemInfo[_int(eSlot)]->eIndex; } // 현재 장비창에 장착된 아이템 확인
	const ITEM& Get_Equip_Item_Info(EQUIP_SLOT eSlot) { return *m_vecEquip_ItemInfo[_int(eSlot)]; }






private:
	HRESULT Initialize_Item();

	// 접근, 수정
	ITEM_RESULT InputItem_Inven(ITEM* pItem, _uint iCount = 1); // <- 이미 게임에 존재하는 아이템을 인벤에 넣는다 



private:
	CGameInstance* m_pGameInstance = { nullptr };

	vector<ITEM*> m_vecItem_BasicSpec; // 게임 내 존재하는 모든 아이템의 기본 스펙

	vector<ARRAY*> m_vecArray_Inven; // 인벤토리 정보 모음 

	vector<ITEM*> m_vecEquip_ItemInfo; // 현재 어떤 장비 장착 중인 지 확인 






public:
	static CItem_Manager* Create(CGameInstance* pGameInstance);
	virtual void Free() override;

};

END

