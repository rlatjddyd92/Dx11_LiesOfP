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
구역 목록
플레이어 위치 

1. 목적지에서 플레이어가 있는 구역 입구까지 지점 리스트 만들기 
2. 플레이어가 있는 구역의 지점 중 플레이어와 가장 가까운 것 찾기 
3. 플레이어에 가장 가까운 지점과 입구까지 연결 
4. 연결된 리스트에 맞춰 UI 객체 그리기 (인스턴싱 필요)

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
	


private: // <- 디바이스 
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END