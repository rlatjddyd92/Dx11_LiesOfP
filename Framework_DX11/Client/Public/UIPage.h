#pragma once

#include "Client_Defines.h"
#include "UIObject.h"
#include "Interface_Enums.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage : public CUIObject
{
public:
	enum class CTRL_COMMAND
	{
		COM_RENDER,
		COM_UPDATE,
		COM_RATIO,
	};




	typedef struct UIPART_INFO
	{


		void MakeDirec()
		{
			fDirec = { fAdjust_End.x - fAdjust_Start.x,  fAdjust_End.y - fAdjust_Start.y };
		}

		_Vec2 GetBarSize()
		{
			if (bBarDirecX == 1)
				return { abs(fAdjust_Start.x - fAdjust_End.x) * fRatio, fSize.y };
			else
				return { fSize.x, abs(fAdjust_Start.y - fAdjust_End.y) * fRatio };
		}

		_Vec2 MovePart(_Vec2 fParentPosition, _float fTimeDelta)
		{
			if (iMoveType == _int(MOVETYPE::TYPE_BAR))
			{
				fAdjust.x = fAdjust_Start.x + (fDirec.x * fRatio * 0.5f);
				fAdjust.y = fAdjust_Start.y + (fDirec.y * fRatio * 0.5f);
			}
			else if (iMoveType == _int(MOVETYPE::TYPE_MOVE))
			{
				fAdjust.x = fAdjust_Start.x + (fDirec.x * fRatio);
				fAdjust.y = fAdjust_Start.y + (fDirec.y * fRatio);
			}

			fPosition.x = fParentPosition.x + fAdjust.x;
			fPosition.y = fParentPosition.y + fAdjust.y;



			return fPosition;
		}

		_Vec2 Get_Shaking() { return fShaking_Adjust; }

		void Set_RedText() { fTextColor = { 1.f,0.f,0.f,1.f }; }
		void Set_WhiteText() { fTextColor = { 0.f,0.f,0.f,1.f }; }
		void Set_BlackText() { fTextColor = { 1.f,1.f,1.f,1.f }; }

		_wstring strUIPart_Name = {};

		_int iParentPart_Index = -1; // <- 

		_Vec2 fSize = { 0.f,0.f };
		_Vec2 fPosition = { 0.f,0.f };

		_int iGroupIndex = 0;

		// 현재 위치
		_Vec2 fAdjust = { 0.f,0.f }; // <- 중심점(Page의 포지션)에서 어디로 얼마나 떨어져 있는지

		// 이동을 위한 변수
		_int iMoveType = _int(MOVETYPE::TYPE_STATIC);
		_Vec2 fAdjust_Start = { 0.f,0.f }; // adjust가 움직이는 선의 시작
		_Vec2 fAdjust_End = { 0.f,0.f }; // adjust가 움직이는 선의 종료
		_Vec2 fDirec = { 0.f,0.f }; // Start에서 End로 가는 벡터 
		_float fRatio = 0.5f;  // 지금 Adjust가 Start-End 사이 어느 지점에 있는 지 비율로 표시 (0.f ~ 1.f);
		_int bBarDirecX = true; // Combo에 맞추느라 int로 사용함, 클라에서는 bool로 사용

		// 텍스쳐 관련
		_int iTexture_Index = -1;
		_bool bIsItem = false; // <- 아이템 아이콘 표시 여부 
		_float4 fTextureColor = { -1.f,-1.f ,-1.f ,-1.f };

		// 텍스트 관련
		_int iFontIndex = _int(UI_FONT::FONT_END);
		_wstring strText = {};
		_bool bCenter = false;
		_float4 fTextColor = { 1.f,1.f,1.f,1.f };

		// 작동 제어
		_bool bUpdate = true;
		_bool bRender = true;
		_bool bTurn = false;
		_float fTurn_Degree = 0.f;

		// 쉐이킹
		_Vec2 fShaking_Adjust = { 0.f,0.f };
		_Vec2 fShaking_Direc = { 0.f,0.f };
		_float fShaking_Power = 0.f;
		_float fShaking_Interval_Now = 0.f;
		_float fShaking_Interval = 0.f;
		_float fShaking_Time = 0.f;

		// 렌더 관련 제어 변수 
		_bool bTexture_Color_Multiple = false; // <- true인 경우 텍스쳐 컬러 보정값을 원래 값에 곱하는 방식으로 진행
		_float fStrash_Alpha = 0.3f; // 파트 별로 버리는 알파 기준을 다르게 설정해야 하는 경우 사용
		_bool bText_Right = false; // 텍스트를 오른쪽 정렬로 그리는 경우 

	}UPART;

	typedef struct UIPART_GROUP_CONTROL
	{
		list<_int> PartIndexlist;

		_float fRatio = 1.f;

		_bool bUpdate = true;
		_bool bRender = true;
	}UG_CTRL;


protected:
	CUIPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage(const CUIPage& Prototype);
	virtual ~CUIPage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void InputPart(UPART* pPart) { m_vecPart.push_back(pPart); }
	void SetUIPageName(_wstring strName) { m_UIPageName = strName; }
	void SetUIPagePosition(_Vec2 fPosition) { m_fX = fPosition.x; m_fY = fPosition.y; }
	_bool GetUpdate() { return m_bUpdate; }
	_bool GetRender() { return m_bRender; }
	void SetUpdate(_bool bUpdate) { m_bUpdate = bUpdate; }
	void SetRender(_bool bRender) { m_bRender = bRender; }
	const vector<UPART*>& GetPartInfo() { return m_vecPart; }
	_float GetTopPartMove() { return m_fTopPartMove; }
	_bool GetPageAction(PAGEACTION eAction) { return m_vecPageAction[_int(eAction)]; }

	virtual void OpenAction();
	virtual void CloseAction();

	virtual HRESULT Ready_UIPart_Group_Control();

	virtual CHECK_MOUSE Check_Page_Action(_float fTimeDelta);

	

protected:
	void UpdatePart_ByControl(UG_CTRL* pCtrl);
	void Release_Control(UG_CTRL* pCtrl);

	UPART* Get_Front_Part_In_Control(_int Ctrl_Index)
	{
		if (!m_vec_Group_Ctrl[Ctrl_Index]->PartIndexlist.empty())
			return m_vecPart[m_vec_Group_Ctrl[Ctrl_Index]->PartIndexlist.front()];
		else
			return nullptr;
	}
	_int Get_Front_PartIndex_In_Control(_int Ctrl_Index) { return m_vec_Group_Ctrl[Ctrl_Index]->PartIndexlist.front(); }

	void Array_Control(_int iStart, _int iEnd, CTRL_COMMAND eCom, _float fInput)
	{
		switch (eCom)
		{
		case Client::CUIPage::CTRL_COMMAND::COM_RENDER:
			for (_int i = iStart; i <= iEnd; ++i)
				m_vec_Group_Ctrl[i]->bRender = (_bool)fInput;
			break;
		case Client::CUIPage::CTRL_COMMAND::COM_UPDATE:
			for (_int i = iStart; i <= iEnd; ++i)
				m_vec_Group_Ctrl[i]->bUpdate = (_bool)fInput;
			break;
		case Client::CUIPage::CTRL_COMMAND::COM_RATIO:
			for (_int i = iStart; i <= iEnd; ++i)
				m_vec_Group_Ctrl[i]->fRatio = fInput;
			break;
		default:
			break;
		}
	}

	_Vec2 Check_Mouse_By_Part(UPART& Part);


	void UpdatePart_ByIndex(_int Index, _float fTimeDelta);

	void Input_Render_Info(UPART& Part);
	


protected:
	vector<UPART*> m_vecPart;
	_wstring m_UIPageName = {};

protected: // 제어 변수
	_bool m_bUpdate = true; // 업데이트 진행 여부 
	_bool m_bRender = false; // 렌더 진행 여부 


protected: // 열기/닫기 액션
	vector<_bool> m_vecPageAction;
	_float m_fTopPartMove = 0.f; // 0.f->닫힘, 1.f->열림
	_float m_fTopPartMove_Excel = 2.f; // <- 열고 닫는 속도 가속

protected: // 그룹 컨트롤 모음 -> 여기 있는 건 전체 업데이트 + 전체 릴리즈 용도로만 사용, 구체적인 사용은 각 Paage에 얕은 복사로 따로 포인터를 마련해 진행 
	vector<UG_CTRL*> m_vec_Group_Ctrl;

	// 테스트 
	_bool m_bIsBatching = false;

public:
	static CUIPage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END