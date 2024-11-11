#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Tools)
class CMesh_Effect final : public CEffect_Base
{
public:
	typedef struct : CEffect_Base::EFFECT_BASE_DESC
	{
		_tchar		szModelTag[MAX_PATH];

		_tchar		strDiffuseTexturTag[MAX_PATH] = L"";
		_tchar		strMaskTextureTag_1[MAX_PATH] = L"";
		_tchar		strMaskTextureTag_2[MAX_PATH] = L"";

		_float3		vPosition;
		_float3		vScale;
		_float3		vRotation;
	}MESH_EFFECT_DESC;

private:
	CMesh_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_Effect(const CMesh_Effect& Prototype);
	virtual ~CMesh_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Reset();
	virtual HRESULT Save(_wstring strFilePath);

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CTexture* m_pDiffueseTextureCom = { nullptr };
	class CTexture* m_pMaskTextureCom_1 = { nullptr };
	class CTexture* m_pMaskTextureCom_2 = { nullptr };

private:
	HRESULT Ready_Components(const MESH_EFFECT_DESC& desc);

public:
	static CMesh_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
