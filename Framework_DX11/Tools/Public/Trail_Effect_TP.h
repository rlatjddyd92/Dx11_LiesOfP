#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CTrail_TwoPoint_Instance;
class CTexture;
END

BEGIN(Tools)
class CTrail_Effect_TP final :public CEffect_Base
{
public:
	typedef struct
	{
		_uint		iNumInstance = { 100 };
		_float2		vLifeTime = { 2.f, 4.f };
	}BUFFER_DESC;

	typedef struct
	{
		_uint		iRenderState = { 0 };
		_float4		vColor = {};
		_float		fScale = { 0.f };
		_Vec2		vDivide = {};
		_float		fSpriteSpeed = { 0.f };
		_uint		iShaderIndex = { 0 };
	} ACTION_DESC;

	typedef struct
	{
		_tchar strDiffuseTextureTag[MAX_PATH] = TEXT("");
		_tchar strMaskTextureTag_1[MAX_PATH] = TEXT("");
		_tchar strMaskTextureTag_2[MAX_PATH] = TEXT("");
	}TEXTURE_DESC;

	typedef struct
	{
		ACTION_DESC ActionDesc = {};
		TEXTURE_DESC TextureDesc = {};
	}INIT_DESC;

	typedef struct : public CEffect_Base::EFFECT_BASE_DESC
	{
		INIT_DESC InitDesc = {};
		BUFFER_DESC BufferDesc = {};
	}TRAIL_TP_DESC;


private:
	CTrail_Effect_TP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Effect_TP(const CTrail_Effect_TP& Prototype);
	virtual ~CTrail_Effect_TP() = default;

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
public:
	void Set_Desc(const ACTION_DESC& desc);
	TRAIL_TP_DESC Get_Desc();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTrail_TwoPoint_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pDiffuseTextureCom = { nullptr };
	class CTexture* m_pMaskTextureCom_1 = { nullptr };
	class CTexture* m_pMaskTextureCom_2 = { nullptr };

	ACTION_DESC m_ActionDesc = {};

	TRAIL_TP_DESC m_SaveDesc = {};

	_float3 m_vTestTop = {};
	_float3 m_vTestBottom = {};

private:
	HRESULT Ready_Components(const TRAIL_TP_DESC& Desc);

public:
	static CTrail_Effect_TP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
