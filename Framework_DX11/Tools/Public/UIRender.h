#pragma once

#include "Tools_Defines.h"
#include "UIObject.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Tools)


class CUIRender final : public CUIObject
{
public:
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

	typedef struct UITEXTURE_INFO
	{
		_char* strTexturePath = {};
		_char* strTextureTag = {};
		CTexture* Texture = { nullptr };
	}UTEXTURE;

	typedef struct UIRENDERCOMMAND
	{
		UIRENDERCOMMAND()
		{
			szText = new _tchar[100];
		}

		~UIRENDERCOMMAND()
		{
			Safe_Delete_Array(szText);
		}

		_int iTextureIndex = -1;
		_float2 fPosition = { 0.f,0.f };
		_float2 fSize = { 0.f,0.f };

		UI_FONT eType = UI_FONT::FONT_END;
		_tchar* szText = TEXT("none");
		_bool bIsCenter = false;

		// -1 이면 작동 안함 
		_float3 fRGB = { -1.f,-1.f,-1.f };
		_float fAlpah = -1.f;
	}URCOM;


private:
	CUIRender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIRender(const CUIRender& Prototype);
	virtual ~CUIRender() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void AddRenderUIObject(_int iTextureIndex, _float2 fPosition, _float2 fSize, _float3 fRGB = { -1.f,-1.f,-1.f }, _float fAlpha = -1.f);
	void AddRenderText(UI_FONT eFont, _bool bIsCenter, _tchar* szText, _float2 fPosition, _float3 fColor = { -1.f,-1.f,-1.f }, _float fAlpha = -1.f);

	const _char* GetTextureTag(_int iIndex) { return m_vecTextureInfo[iIndex]->strTextureTag; }
	const _char* GetTextFontTag(_int iIndex) { return m_vecFont_char[iIndex]; }

	_int GetTextureCount() { return m_vecTextureInfo.size(); }

public:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();

	void Ready_Font();
	HRESULT Ready_Texture();

	vector<UTEXTURE*> m_vecTextureInfo;
	list<URCOM*> m_UIRenderlist;
	vector<_char*> m_vecFont_char;
	vector<_tchar*> m_vecFont_tchar;

	

public:
	static CUIRender* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END