#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader_NonVTX;
class CTrail_MultiPoint_Instance;
class CTexture;
class CShader_Compute;
END

BEGIN(Tools)
class CTrail_Effect_MP final : public CEffect_Base
{
private:
	CTrail_Effect_MP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Effect_MP(const CTrail_Effect_MP& Prototype);
	virtual ~CTrail_Effect_MP() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Reset() override;
	virtual HRESULT Save(_wstring strFilePath) override;

private:
	class CShader_NonVTX* m_pShaderCom = { nullptr };					// 테일만.
	class CTrail_MultiPoint_Instance* m_pVIBufferCom = { nullptr };		// 
	class CTexture* m_pDiffuseTextureCom = { nullptr };
	class CTexture* m_pNormalTextureCom = { nullptr };
	class CTexture* m_pMaskTextureCom = { nullptr };

	class CShader_Compute* m_pSpreadCS = { nullptr };

	// 초기화.
	class CShader_Compute* m_pResetCS = { nullptr };


	_Vec2 m_vScaling = _Vec2(1.f, 1.f);

private:
	HRESULT Ready_Components();

public:
	static CTrail_Effect_MP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
