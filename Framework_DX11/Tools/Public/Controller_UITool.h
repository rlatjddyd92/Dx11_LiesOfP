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

	enum class PART_TYPE
	{
		TYPE_STATIC,
		TYPE_STATIC_TEXT,
		TYPE_VARIABLE_TEXT,
		TYPE_BAR,
		TYPE_SHADING,
		TYPE_SPRITE,
		TYPE_END
	};

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
		PART_TYPE eType = PART_TYPE::TYPE_END;
		_char* strUIPart_Name = {};
		_int iUI_Index = -1;
		_int iTextureIndex = -1;
		_char* szTexture_Path = {};
		_float2 fSize = { 0.f,0.f };
		_int iSocket_Num = 0;
		vector<_float2> vecSocketPosition;

	}UPART;

public:
	vector<_wstring>& Get_PrototypeTags() { return m_PrototypeTags; }
	_wstring& Get_PrtotypeTagIndex(_uint iIndex) { return m_PrototypeTags[iIndex]; }

	void					Add_ProtytypeTag(_wstring strTag) { m_PrototypeTags.emplace_back(strTag); }

	_int& GetUIPartNumMax() { return m_iPartIndex_NumMax; }
	_char* GetUIPartName(_int iIndex) { return m_vecUPart[iIndex]->strUIPart_Name; }

	_char* GetUIPartTypeName(_int iIndex) { return m_UIPartTypeName[iIndex]; }

	UPART* GetUIPartInfo(_int iIndex) { return m_vecUPart[iIndex]; }

	 _int* GetSelectIndex() { return &m_iSelectIndex; }
	void SetSelectIndex(_int iIndex)
	{
		if ((iIndex >= 0) && (iIndex < m_iPartIndex_NumMax))
			m_iSelectIndex = iIndex;
	}

	_char** GetUIPartName_Array() { return m_UIPartName; };

	HRESULT UITool_Render();






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

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	vector<_wstring>	m_PrototypeTags;
	_char* m_UIPartName[100] = {nullptr};
	_char* m_UIPartTypeName[_int(PART_TYPE::TYPE_END)] = { nullptr };



	_int m_iPartIndex_NumMax = 100;
	_int m_iSelectIndex = 0;



	vector<UPART*> m_vecUPart;
	vector<CTexture*> m_vecTexture;




public:
	virtual void Free() override;
};

END

