#pragma once

#include "Client_Defines.h"
#include "UIObject.h"
#include "UI_Enum.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage : public CUIObject
{
public:
	typedef struct UIPART_INFO
	{


		void MakeDirec()
		{
			fDirec = { fAdjust_End.x - fAdjust_Start.x,  fAdjust_End.y - fAdjust_Start.y };
		}

		_float2 GetBarSize()
		{
			if (bBarDirecX == 1)
				return { abs(fAdjust_Start.x - fAdjust_End.x) * fRatio, fSize.y };
			else
				return { fSize.x, abs(fAdjust_Start.y - fAdjust_End.y) * fRatio };
		}

		_float2 MovePart(_float2 fParentPosition)
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

		_wstring strUIPart_Name = {};

		_int iParentPart_Index = -1; // <- 

		_float2 fSize = { 0.f,0.f };
		_float2 fPosition = { 0.f,0.f };

		_int iGroupIndex = 0;

		// 현재 위치
		_float2 fAdjust = { 0.f,0.f }; // <- 중심점(Page의 포지션)에서 어디로 얼마나 떨어져 있는지

		// 이동을 위한 변수
		_int iMoveType = _int(MOVETYPE::TYPE_STATIC);
		_float2 fAdjust_Start = { 0.f,0.f }; // adjust가 움직이는 선의 시작
		_float2 fAdjust_End = { 0.f,0.f }; // adjust가 움직이는 선의 종료
		_float2 fDirec = { 0.f,0.f }; // Start에서 End로 가는 벡터 
		_float fRatio = 0.5f;  // 지금 Adjust가 Start-End 사이 어느 지점에 있는 지 비율로 표시 (0.f ~ 1.f);
		_int bBarDirecX = true; // Combo에 맞추느라 int로 사용함, 클라에서는 bool로 사용

		// 텍스쳐 관련
		_int iTexture_Index = -1;
		_float4 fTextureColor = { -1.f,-1.f ,-1.f ,-1.f };

		// 텍스트 관련
		_int iFontIndex = _int(UI_FONT::FONT_END);
		_wstring strText = {};
		_bool bCenter = false;
		_float4 fTextColor = { 1.f,1.f,1.f,1.f };


	}UPART;



	typedef struct UIPAGE_INFO
	{
		_char* strUIPage_Name = {};
		_int iNowSelectPart = 0;
		_float2 fPosition = { g_iWinSizeX * 0.5f,g_iWinSizeY * 0.5f }; // 페이지의 포지션
		vector<UPART*> vecPart;
	}UPAGE;


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
	void SetUIPagePosition(_float2 fPosition) { m_fX = fPosition.x; m_fY = fPosition.y; }
	_bool GetUpdate() { return m_bUpdate; }
	_bool GetRender() { return m_bRender; }
	void SetUpdate(_bool bUpdate) { m_bUpdate = bUpdate; }
	void SetRender(_bool bRender) { m_bRender = bRender; }
	const vector<UPART*>& GetPartInfo() { return m_vecPart; }

protected:
	virtual HRESULT Ready_UIPart();

protected:
	vector<UPART*> m_vecPart;
	_wstring m_UIPageName = {};

protected: // 제어 변수
	_bool m_bUpdate = true; // 업데이트 진행 여부 
	_bool m_bRender = true; // 렌더 진행 여부 

public:
	static CUIPage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END