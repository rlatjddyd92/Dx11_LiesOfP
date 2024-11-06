#pragma once

#include "Tools_Defines.h"
#include "Base.h"

#include "VIBuffer_Terrain.h"
#include "GameInstance.h"
#include "UIRender.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)

class CController_UITool : public CBase
{
	DECLARE_SINGLETON(CController_UITool)

public:
	enum class MOVETYPE
	{
		TYPE_STATIC, // 움직이지 않음
		TYPE_MOVE, // 움직임
		TYPE_BAR, // 바 형태로 표시 (HP바 등)
		TYPE_END
	};

	enum class UIPAGE
	{
		PAGE_MAIN,
		PAGE_LOADING,
		PAGE_PLAY,
		PAGE_MENU,
		PAGE_INVEN,
		PAGE_EQUIP,
		PAGE_STAT,
		PAGE_LEVELUP,
		PAGE_SKILL,
		PAGE_END
	};


	typedef struct UIPART_INFO
	{
		
		~UIPART_INFO() { Safe_Delete_Array(szText); }

		void MakeDirec() 
		{
			fDirec = { fAdjust_End.x - fAdjust_Start.x,  fAdjust_End.y - fAdjust_Start.y };
		}

		_float2 GetBarSize()
		{
			if (bBarDirecX == 1)
				return { fSize.x * fRatio, fSize.y };
			else
				return { fSize.x, fSize.y * fRatio };
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

		_char* strUIPart_Name = {};

		_int iParentPart_Index = -1; // <- 

		_float2 fSize = { 0.f,0.f };
		_float2 fPosition = { 0.f,0.f };

		// 현재 위치
		_float2 fAdjust = { 0.f,0.f }; // <- 중심점(Page의 포지션)에서 어디로 얼마나 떨어져 있는지

		// 이동을 위한 변수
		_int iMoveType = _int(MOVETYPE::TYPE_STATIC);
		_float2 fAdjust_Start = { 0.f,0.f }; // adjust가 움직이는 선의 시작
		_float2 fAdjust_End = { 0.f,0.f }; // adjust가 움직이는 선의 종료
		_float2 fDirec = { 0.f,0.f }; // Start에서 End로 가는 벡터 
		_float fRatio = 0.5f;  // 지금 Adjust가 Start-End 사이 어느 지점에 있는 지 비율로 표시 (0.f ~ 1.f);
		_int bBarDirecX = true; // Combo에 맞추느라 int로 사용함, 클라에서는 bool로 사용

		_int iTexture_Index = -1;

		_int iFontIndex = _int(CUIRender::UI_FONT::FONT_END);
		
		_tchar* szText = {};
		_bool bCenter = false;
		_float4 fTextColor = { 1.f,1.f,1.f,1.f };

		
	}UPART;

	

	typedef struct UIPAGE_INFO
	{
		_char* strUIPage_Name = {};
		_int iPage_Index = -1;
		_int iNowSelectPart = 0;
		_float2 fPosition = { g_iWinSizeX * 0.5f,g_iWinSizeY*0.5f }; // 페이지의 포지션
		vector<UPART*> vecPart;
	}UPAGE;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	vector<_wstring>& Get_PrototypeTags() { return m_PrototypeTags; }
	_wstring& Get_PrtotypeTagIndex(_uint iIndex) { return m_PrototypeTags[iIndex]; }

	void Add_ProtytypeTag(_wstring strTag) { m_PrototypeTags.emplace_back(strTag); }

	UPART& Get_PartRenderInfo(_int iIndex) 
	{ 
		if ((iIndex >= 0) && (iIndex < m_vecPageInfo[m_iNowSelectNum]->vecPart.size()))
			return *m_vecPageInfo[m_iNowSelectNum]->vecPart[iIndex];
		
		return UPART{};
	}

	_int GetPartCount() { return m_vecPageInfo[m_iNowSelectNum]->vecPart.size(); }


public: // imgui 사용
	void UITool_Edit(); 
	HRESULT UITool_Render();

private:
	void UIPage_Edit();
	void UIPart_Edit();

	void AddNewPage();

	void UIPart_Render();

	void FontTest();



private:
	CController_UITool();
	virtual ~CController_UITool() = default;

private:
	HRESULT InitializeResource();
	HRESULT SavePart();
	HRESULT LoadPart();
	HRESULT InitializeComponent();


private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	vector<_wstring>	m_PrototypeTags;

	_int m_iNowSelectNum = 0;

	

	_int m_iTextureNum = 0;
	_int m_iPageNum = 100;

	_char* m_ArrPageName[100];

	_char m_InputPageName[100] = "";

	_float2 m_InputPageCenter = { 0.f,0.f };




	_char m_InputText[100] = "";
	_char m_InputPartName[100] = "";

	vector<UPAGE*> m_vecPageInfo;

	CUIRender* m_pUIRender = { nullptr };

	_char* m_szPositionType[3] = { "Static", "Move", "Bar" };
	_char* m_szBarDirec[2] = { "Y", "X" };
	


public:
	virtual void Free() override;
};

END

