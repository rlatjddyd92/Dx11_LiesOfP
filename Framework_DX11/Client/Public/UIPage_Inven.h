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
		TAP_AMULET,
		TAP_COSTUME,
		TAP_GESTURE,
		TAP_COLLECTION,
		TAP_END
	};

	typedef struct ARRAY_RENDER_INFO // 어떤 인벤 배열을 어디에 그릴 지만 결정 
	{
		_float fAdjust_Y = 0.f; // <- 여기 표시된 만큼 내려가야 함
		_wstring strName = TEXT("none");
		_int iTexture[5] = { -1,-1,-1,-1,-1 };
		_int iTexture_Handle[5] = { -1,-1,-1,-1,-1 };
		_int iTexture_Symbol[5] = { -1,-1,-1,-1,-1 };
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
	void Update_Array_Position_Weapon(_float fTimeDelta); // 페이지 조작 내용에 따라 Array들의 위치 및 형태를 잡음
	void Update_Array_Position_Weapon_Heroic(_float fTimeDelta, _float fAdjust, _int iRowCount, _float fStart); // 페이지 조작 내용에 따라 Array들의 위치 및 형태를 잡음


	void Update_BoxInfo(_float fTimeDelta); // 하단의 보유 정보 갱신

	// Part 조정 후
	void Update_Tap_Button(_float fTimeDelta); // 상단 탭버튼 이미지, 텍스트 조정

	// 슬라이드 조정 
	// 
	// 1. 바 움직일 수 있는 최대 거리 
	// 2. 바 움직인 거리
	// 
	// 3. 아이템 배경판 기본 크기 
	// 4. 아이템이 그려진 총 높이 (Y)
	// 5. 배경판 움직일 수 있는 거리 (4 - 3)
	// 
	// 6. 움직임 감지 불 변수 
	// 7. 마우스 이전 위치 기록 
	// 8. 
	// 
	// 
	// 

	// 슬라이드 조정 







	void Change_Data_Y_Size(_float fSize);
	
	void Action_Slide(_float fSize);
	void Update_Slide(_float fSize);


	_float m_fData_Adjust_Y_Origin = 0.f;
	
	// Focus 조정 

	void Reset_Focus() { m_vFocus_Pos = { -1.f,-1.f }; m_vFocus_Size = { -1.f,-1.f }; }

	_Vec2 m_vFocus_Pos = { -1.f,-1.f };
	_Vec2 m_vFocus_Size = { -1.f,-1.f };

	// ItemActionInfo 
	_bool m_bReset_ItemAction_Info = false;
	
	// Sound 관련 
	_int m_iNowCell = -1;

private:
	INVEN_UI_TAP m_eNow_Tap = INVEN_UI_TAP::TAP_NORMAL_ITEM;

	_float m_fGap = 2.5f; // Array 위치 계산 때 각 요소 사이의 간격 

	vector<INVEN_ARRAY_TYPE> m_vecSelected_Array; // 선택된 Tap에 따라 그려야 하는 Inven_array 목록

	queue<RENDER*> m_queueRender; // <- 이번 프레임에 그려야 하는 내용

	_bool m_IsTab_Change = false;

	SCROLL_INFO* m_pScroll = { nullptr };

public:
	static CUIPage_Inven* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END