#pragma once

#include "Tools_Defines.h"
#include "Base.h"

#include "VIBuffer_Terrain.h"
#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)

class CController_UITool : public CBase
{
	DECLARE_SINGLETON(CController_UITool)

	enum class UI_FONT
	{
		FONT_INFO_12,
		FONT_INFO_18,
		FONT_INFO_24,
		FONT_INFO_36,
		FONT_INFO_48,
		FONT_INFO_60,
		FONT_INFO_72,
		FONT_TITLE_12,
		FONT_TITLE_18,
		FONT_TITLE_24,
		FONT_TITLE_36,
		FONT_TITLE_48,
		FONT_TITLE_60,
		FONT_TITLE_72,
		FONT_END
	};


	typedef struct UIPART_INFO
	{
		_char* strUIPart_Name = {};
		_int iTexture_Index = -1;
		_int iSocket_Index = -1;
		_float2 fSize = { 0.f,0.f };
	}UPART;

	typedef struct UITEXTURE_INFO
	{
		_char* strTexturePath = {};
		_char* strTextureTag = {};
		CTexture* Texture = { nullptr };
	}UTEXTURE;

	typedef struct UISOCKET_INFO
	{
		_char* strUISocket_Name = {};
		_int iSocket_Index = -1;
		_float2 fPosition = { 0.f,0.f }; // <- 중심점(Page의 포지션)에서 어디로 얼마나 떨어져 있는지

	}USOCKET;

	typedef struct UIPAGE_INFO
	{
		_char* strUIPage_Name = {};
		_int iPage_Index = -1;

		
		_int iSocket_Num = 0; // 소켓 갯수
		_int iPart_Num = 0; // 파트 갯수


		_float2 fPosition = { 0.f,0.f }; // 페이지의 포지션
		vector<USOCKET*> vecSocket;
		vector<UPART*> vecPart;
	}UPAGE;

public:
	vector<_wstring>& Get_PrototypeTags() { return m_PrototypeTags; }
	_wstring& Get_PrtotypeTagIndex(_uint iIndex) { return m_PrototypeTags[iIndex]; }

	void Add_ProtytypeTag(_wstring strTag) { m_PrototypeTags.emplace_back(strTag); }



public: // imgui 사용
	void UITool_Edit(); 
	HRESULT UITool_Render();

private:
	void UIPage_Edit();
	void UISocket_Edit();
	void UIPart_Edit();

	void AddNewPage();

	void UISocket_Render();
	void UIPart_Render();

	void FontTest();


private:
	CController_UITool();
	virtual ~CController_UITool() = default;

public:
	HRESULT Initialize();

private:
	HRESULT InitializeResource();
	HRESULT SavePart();
	HRESULT LoadPart();
	HRESULT InitializeTexture();
	HRESULT InitializeFont();
	HRESULT InitializeComponent();


private:
	class CGameInstance* m_pGameInstance = { nullptr };

	vector<_wstring>	m_PrototypeTags;

	_int m_iNowSelectNum = 0;

	_int m_iTextureNum = 0;
	_int m_iPageNum = 100;

	_char* m_ArrPageName[100];

	_char m_InputPageName[100] = "";
	_char m_InputSocketName[100] = "";
	_char m_InputPartName[100] = "";

	vector<UTEXTURE*> m_vecTextureInfo;
	vector<UPAGE*> m_vecPageInfo;

public:
	virtual void Free() override;
};

END

