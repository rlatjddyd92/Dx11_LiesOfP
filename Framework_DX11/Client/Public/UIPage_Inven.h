#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Inven : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_BACK,
		GROUP_TOP_STATIC,
		GROUP_TOP_MOUSE,
		GROUP_TOP_TEXT,

		GROUP_TOP_HIGHLIGHT,
		
		GROUP_WEIGHT_TEXT,
		GROUP_WEIGHT_STAT,
		GROUP_WEIGHT_RATIO,

		GROUP_COIN_COUNT,

		GROUP_ITEMINFO_FRAME,
		GROUP_ITEMINFO_SLIDE,
		GROUP_ITEMINFO_SLIDE_BAR,

		// 여기부터 반복작업 필요
		GROUP_ARRAY_FRAME,
		GROUP_ARRAY_TEXT,

		GROUP_CELL_0,
		GROUP_CELL_1,
		GROUP_CELL_2,
		GROUP_CELL_3,
		GROUP_CELL_4,
		// 여기까지

		GROUP_BOX_STATIC,
		GROUP_BOX_COUNT_0,
		GROUP_BOX_COUNT_1,

		GROUP_END
	};

	enum class INVEN_UI_TAP
	{
		TAP_NORMAL_ITEM,
		TAP_INGRADIANT,
		TAP_FUNTION,
		TAP_WEAPON,
		TAP_WEAPON_PART,
		TAP_ARM,
		TAP_DEFENCE,
		TAP_COSTUME,
		TAP_GESTURE,
		TAP_COLLECTION,
		TAP_END
	};

	typedef struct ARRAY_RENDER_INFO // 어떤 인벤 배열을 어디에 그릴 지만 결정 
	{
		_float fAdjust_Y = 0.f; // <- 여기 표시된 만큼 내려가야 함

		INVEN_ARRAY_TYPE eType = INVEN_ARRAY_TYPE::TYPE_END; // <- 여기 있는 내용을 그림
		_int iRow = 0; // <- 인벤 배열이 차지한 줄이 여러 줄일 경우 몇 번째 줄을 그릴 지 기록

	}RENDER;


protected:
	CUIPage_Inven(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Inven(const CUIPage_Inven& Prototype);
	virtual ~CUIPage_Inven() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OpenAction() override;
	virtual void CloseAction() override;

	virtual CHECK_MOUSE Check_Page_Action(_float fTimeDelta) override;

	HRESULT Render_Inven_Array(class CUIRender_Client* pRender_Client); // <- Array 단위로 Part를 업데이트 후 렌더 요청한다 

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

private:
	// action
	void Action_Inven_Page(_float fTimeDelta); // 열어야 하는 페이지 결정 + 스크롤 확인
	void Action_Focus(_float fTimeDelta); // 셀 위에 마우스가 있을 때 조작, + 툴팁 페이지 작동 요청도 진행

	// update
	void Update_Top_Part(_float fTimeDelta); // 상단 탭버튼 하이라이트 라인 및 무게, 코인 표시 조정 
	void Update_Array_Position(_float fTimeDelta); // 페이지 조작 내용에 따라 Array들의 위치 및 형태를 잡음
	void Update_BoxInfo(_float fTimeDelta); // 하단의 보유 정보 갱신

	// Part 조정 후
	void Update_Tap_Button(_float fTimeDelta); // 상단 탭버튼 이미지, 텍스트 조정





private:
	INVEN_UI_TAP eNow_Tap = INVEN_UI_TAP::TAP_NORMAL_ITEM;



public:
	static CUIPage_Inven* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END