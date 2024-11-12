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
 1) 신규 아이템 생성 -> 깊은 복사 
 2) 아이템 이동 -> 얕은 복사 후 기존 포인터 nullptr 처리 (Delete 절대 하지 않음)
 3) 외부 참조 -> 반드시 레퍼런스로 



*/

class CItem_Manager : public CBase
{
public:
	typedef struct ITEM_INFO
	{

	}ITEM;

	typedef struct SLOT_INFO
	{
		SLOT_INFO(_int iSlotNum)
		{
			vecValidType.resize(_int(ITEM_TYPE::TYPE_END));
			vecItemInfo.resize(iSlotNum);
		}

		~SLOT_INFO()
		{
			for (auto& iter : vecItemInfo)
				Safe_Delete(iter);

			vecItemInfo.clear();
			vecValidType.clear();
		}

		vector<ITEM*> vecItemInfo; 
		vector<_bool> vecValidType;
	}SLOT;


public:
	// 접근, 수정
	ITEM_RESULT AddItem_Inven(ITEM_INDEX eIndex, _int iCount = 1); // <- 인벤에 아이템을 추가한다 
	ITEM_RESULT EquipItem_Inven(ITEM_INDEX eIndex); // <- 인벤에 있는 아이템을 사용한다 




private:
	CItem_Manager(CGameInstance* pGameInstance);
	virtual ~CItem_Manager() = default;

private:
	HRESULT Initialize_Item();

private:
	CGameInstance* m_pGameInstance = { nullptr };

	/*
	필요한 거
	[Item Array]
	0. 아이템 기본 스펙
	
	[Slot Array - Slot]
	0. 인벤토리 
	 1) 

	1. 장착
	
	*/

	vector<ITEM*> m_vecItem_BasicSpec; // 게임 내 존재하는 모든 아이템의 기본 스펙
	
	vector<SLOT*> m_vecSlot_Inven; // 인벤토리 정보 모음 

	vector<_int> m_vecEquip_ItemIndex; // 현재 어떤 장비 장착 중인 지 확인 






public:
	static CItem_Manager* Create(CGameInstance* pGameInstance);
	virtual void Free() override;

};

END

