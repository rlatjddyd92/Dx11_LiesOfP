#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

#include "UIPage_Defines.h"
#include "Interface_Enums.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)


class CUIRender_Client final : public CUIObject
{
public:
	typedef struct UITEXTURE_INFO
	{
		_tchar* strTexturePath = {};
		_tchar* strTextureTag = {};
		CTexture* Texture = { nullptr };
	}UTEXTURE;




private:
	CUIRender_Client(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIRender_Client(const CUIRender_Client& Prototype);
	virtual ~CUIRender_Client() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Render_UI(vector<CUIPage*>& rPage);
	HRESULT Make_Texture(_int iTextureIndex);

	HRESULT Render_TestFont(_bool bIsKorean);

	void Input_TestPageInfo(
		_float2 fPosition,
		_float2 fSize,
		vector<_wstring>& vecName,
		vector<_wstring>& vecValue
	);


public:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();

	void Ready_Font();
	HRESULT Ready_Texture_UIPart();
	HRESULT Ready_Texture_ItemIcon();

	HRESULT Render_TestPage_Info();
	void RemoveTestPageInfo();

	vector<UTEXTURE*> m_vecTextureInfo_UIPart;
	vector<UTEXTURE*> m_vecTextureInfo_ItemIcon;
	vector<_tchar*> m_vecFont_tchar;

	vector<vector<_tchar*>> m_vecTestPageInfo;
	_float2 m_fTestPage_Pos = { 500.f,500.f };
	_float2 m_fTestPage_Size = { 100.f,100.f };

public:
	static CUIRender_Client* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END