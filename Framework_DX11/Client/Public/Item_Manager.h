#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Stat_Enum.h"

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





*/

class CItem_Manager : public CBase
{
public:
	typedef struct ITEM_INFO
	{

	}ITEM;

private:
	CItem_Manager(CGameInstance* pGameInstance);
	virtual ~CItem_Manager() = default;

private:
	HRESULT Initialize_Item();

private:
	CGameInstance* m_pGameInstance = { nullptr };



public:
	static CItem_Manager* Create(CGameInstance* pGameInstance);
	virtual void Free() override;

};

END

