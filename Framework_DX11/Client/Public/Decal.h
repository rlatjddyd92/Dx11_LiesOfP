#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
END

BEGIN(Client)
class CDecal :
    public CGameObject
{
private:
	CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecal(const CDecal& Prototype);
	virtual ~CDecal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };

	CTexture* m_pTextureCom_Diffuse = { nullptr };
	CTexture* m_pTextureCom_Normal = { nullptr };
	CTexture* m_pTextureCom_ARM = { nullptr };

private:
	_bool	m_isARM = { false };
	_bool	m_isNormal = { false };
	_bool	m_bUseWorldColor = { false };

	_tchar m_szTextureTag_Diffuse[MAX_PATH];
	_tchar m_szTextureTag_Normal[MAX_PATH];
	_tchar m_szTextureTag_ARM[MAX_PATH];

private:
	HRESULT Ready_Components(OBJECT_DEFAULT_DESC* pObjDesc);

public:
	static CDecal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
