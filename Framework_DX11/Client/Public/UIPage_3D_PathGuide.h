#pragma once

#include "Client_Defines.h"
#include "UIPage_3D.h"
#include "GameInstance.h"


/*
[CUIPage_3D_PathGuide]

1. 가이드 관련 UI 객체 관리
2. 가이드 UI ON/OFF
3. 기준점이 되는 Navi 인덱스 관리
4. UI 객체에게 위치 전달 
5. UI 객체 업데이트 및 렌더 진행



목적지 목록 
노드 목록
플레이어 위치 

1. 목적지에서 플레이어까지 지점 리스트 만들기 
   1) 노드 목록으로 BFS
   2) 각 구역 별로 다음 구역으로 넘어가려면 어떤 지점을 밟아야 하는 지 확인하여 리스트에 추가 
   3) 위 내용을 모두 연결 
2. 플레이어가 있는 구역의 지점 중 플레이어와 가장 가까운 것 찾기 
  라인이 부자연스럽게 이어지는 것을 방지하기 위해 다음 논리 사용 
   1) 가장 가까운 것 1,2위 찾기 
   2) 1,2위 중 최종 목적지에 더 가까운 것과 유저 위치 연결 
    -> 이를 통해 유저가 목적지를 향해 갈 때 유저 뒤에 있는 지점과 연결되는 현상 방지 
3. 플레이어에 가장 가까운 지점과 입구까지 연결 
4. 연결된 리스트에 맞춰 UI 객체 그리기 (인스턴싱 필요)


맵의 형태가 단순하고 일직선 형태임 -> DFS가 더 유리할 듯 



*/

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_3D_PathGuide : public CUIPage_3D
{
private:
	CUIPage_3D_PathGuide();
	virtual ~CUIPage_3D_PathGuide() = default;







private:
	vector<list<_int>> m_vecNode_Route; // <- 노드 목록
	vector<_int> m_vecDestination; // <- 목적지 좌표 목록





private: // <- 디바이스 
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END