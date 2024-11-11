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
[Player_Stat_Manager]
1. 역할 
 1) 게임 내의 스탯 최대치 설정 및 관리 
 2) 유저의 스탯 실제 수치를 조정 
 3) 외부에서 필요에 따라 접근하여 스탯 데이터 이용 및 수정

2. 추가 고려 사항 
 1) 몬스터, 보스, NPC의 정보도 관리 필요할 지 논의 
 2) 만일 필요할 경우 별개의 Stat_Manager로 분리하여 관리 

3. 제작 고려 사항 
 1) 각종 스탯 종류의 추가, 삭제에 유연하게 대응할 수 있도록 자료 구조 짜기 
 2) 접근, 수정을 용이하게 하되 정해진 규칙,논리를 벗어나지 않도록 일관된 통제 필요 
 3) 유저의 초기 스탯은 엑셀 파일 파싱하여 관리 

*/

class CPlayer_Stat_Manager : public CBase
{
public:
	typedef struct STAT_INFO
	{
		

		_wstring strStat_Name = TEXT("none");
		STAT_TYPE eType = STAT_TYPE::STAT_TYPE_END; // 스탯의 형식 
		_float fStat_Min_Limit = 0.f; // <- 스탯 하한선의 최소값
		_float fStat_Min = 0.f; // 현재 스탯 하한선 
		_float fStat_Now = 0.f; // 현재 스탯값
		_float fStat_Max = 0.f; // 현재 스탯 상한선 
		_float fStat_Max_Limit = 0.f; // <- 스탯 상한선의 최댓값

		_float fStat_Interval = 0.f; // <- 타입이 CELL인 경우 사용, 스탯 1칸의 수치 



	}STAT;

private:
	CPlayer_Stat_Manager();
	virtual ~CPlayer_Stat_Manager() = default;

public:
	// 정보 접근 
	const _float& Get_Stat(STAT_INDEX eIndex) { return m_vecStat[_int(eIndex)]->fStat_Now; }
	
	//const _float& Get_Stat(STAT_INDEX eIndex) { return m_vecStat[_int(eIndex)]->fStat_Now; }

	// 정보 수정 
	//void Add_Stat(STAT_INDEX eIndex, _float fValue);

private:
	HRESULT Initialize_Stat();



private:
	vector<STAT*> m_vecStat; 


public:
	static CPlayer_Stat_Manager* Create();
	virtual void Free() override;

};

END