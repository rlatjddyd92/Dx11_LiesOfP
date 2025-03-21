#pragma once

#include "Tools_Defines.h"
#include "UIObject.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CVIBuffer_2DPolygon;
END

BEGIN(Tools)


class CUIRender final : public CUIObject
{
public:
	enum class UI_FONT
	{
		FONT_NORMAL_8,
		FONT_NORMAL_12,
		FONT_NORMAL_16,
		FONT_NORMAL_24,
		FONT_NORMAL_32,
		FONT_NORMAL_40,
		FONT_NORMAL_48,
		FONT_BOLD_8,
		FONT_BOLD_12,
		FONT_BOLD_16,
		FONT_BOLD_24,
		FONT_BOLD_32,
		FONT_BOLD_40,
		FONT_BOLD_48,

		NOTO_NORMAL_10,
		NOTO_MEDIUM_10,
		NOTO_BLACK_10,
		NOTO_EXTRALIGHT_10,
		NOTO_LIGHT_10,
		NOTO_SEMIBLOD_10,


		FONT_END
	};

	enum class UI_SHADER
	{
		SHADER_NORMAL,
		SHADER_CHANGE_COLOR,
		SHADER_MULTIPLE_COLOR,
		SHADER_MASTER,
		SHADER_END
	};

	enum class UI_SHADER_PASS
	{
		PASS_BASIC,
		PASS_COLOR_INPUT,
		PASS_COLOR_MULTI,
		PASS_RANGE,
		PASS_ANGLE,
		PASS_COLOR_INPUT_RANGE,
		PASS_COLOR_MULTI_RANGE,
		PASS_COLOR_INPUT_ANGLE,
		PASS_COLOR_MULTI_ANGLE,
		PASS_COLOR_MULTI_ALPHA_ADJUST,
		PASS_END
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

		_float4 vRange = { -1.f,-1.f,-1.f, -1.f };
		_float2 vAngle = { 0.f,0.f };
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
	void AddRenderUIObject(_int iTextureIndex, _float2 fPosition, _float2 fSize, _float3 fRGB = { -1.f,-1.f,-1.f }, _float fAlpha = -1.f, _float4 vRange = { -1.f,-1.f,-1.f, -1.f }, _float2 vAngle = { 0.f,0.f });
	void AddRenderText(UI_FONT eFont, _bool bIsCenter, _tchar* szText, _float2 fPosition, _float3 fColor = { -1.f,-1.f,-1.f }, _float fAlpha = -1.f);

	const _char* GetTextureTag(_int iIndex) { return m_vecTextureInfo[iIndex]->strTextureTag; }
	const _char* GetTextureTag_Item(_int iIndex) { return m_vecTextureInfo_ItemIcon[iIndex]->strTextureTag; }
	const _char* GetTextFontTag(_int iIndex) { return m_vecFont_char[iIndex]; }

	_int GetTextureCount() { return (_int)m_vecTextureInfo.size(); }
	_int GetTextureCount_Item() { return (_int)m_vecTextureInfo_ItemIcon.size(); }

	HRESULT BackRender(_int iIndex);

	HRESULT Render_Effect_Tool();

public:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	vector<CVIBuffer_2DPolygon*> m_vecVIBuffer_2DPolygon_Com;

private:
	HRESULT Ready_Components();

	void Ready_Font();
	HRESULT Ready_Texture();
	HRESULT Ready_Texture_ItemIcon();

	UI_SHADER_PASS Select_Shader(URCOM& Command);

	vector<UTEXTURE*> m_vecTextureInfo;
	vector<UTEXTURE*> m_vecTextureInfo_ItemIcon;
	list<URCOM*> m_UIRenderlist;
	vector<_char*> m_vecFont_char;
	vector<_tchar*> m_vecFont_tchar;

	vector<class CShader*> m_vecShader_UI;

	_float m_fSize = 1.f;

public:
	static CUIRender* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END