#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Tools)
class CTexture_Effect final : public CEffect_Base
{
public:
	enum STATE
	{
		STATE_DISTORTION	= 0x0001,
		STATE_BLUR			= 0x0002,
		STATE_BLEND			= 0x0004,
		STATE_NONBLEND		= 0x0008,
		STATE_END			= 0x9999
	};

	typedef struct
	{
		_tchar	strDiffuseTexturTag[MAX_PATH] = L"";
		_tchar	strNomralTextureTag[MAX_PATH] = L"";
		_tchar	strMaskTextureTag_1[MAX_PATH] = L"";
		_tchar	strMaskTextureTag_2[MAX_PATH] = L"";
	}TEXTURE_DESC;

	typedef struct
	{
		_uint		iState = { 0 };
		_uint		iShaderIndex = { 0 };
		_float		fDuration = { 0.f };
		_Vec4		vColor = {};
		_Vec2		vDivide = {};
		_float		fSpriteSpeed = { 0.f };

		_Vec3		vPos = {};

		_float		fRotationAngle = {};
		_float		fRotationSpeed = { 0.f };

		_Vec3		vScale = {};
		_Vec3		vScalingSpeed = {};

		_float		fAlpha = { 0.f };
		_float		fAlphaSpeed = { 0.f };
	}ACTION_DESC;

	typedef struct : public CEffect_Base::EFFECT_BASE_DESC
	{
		TEXTURE_DESC TextureDesc = {};
		ACTION_DESC ActionDesc = {};
	} TEXTURE_EFFECT_DESC;

private:
	CTexture_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture_Effect(const CTexture_Effect& Prototype);
	virtual ~CTexture_Effect() = default;

public:
	TEXTURE_EFFECT_DESC* Get_SaveDesc_Ptr() {
		return &m_SaveDesc;
	}

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
	TEXTURE_EFFECT_DESC Get_Desc();

private:
	class CShader* m_pShaderCom = { nullptr };

	class CTexture* m_pDiffuseTextureCom = { nullptr };
	class CTexture* m_pNormalTextureCom = { nullptr };
	class CTexture* m_pMaskTextureCom[2] = { nullptr, nullptr };

	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	ACTION_DESC m_ActionDesc = {};

	_float	m_fAccumulateTime = { 0.f };
	_float	m_fCurrenrtIndex = { 0.f };

	TEXTURE_EFFECT_DESC m_SaveDesc = {};

private:
	HRESULT Ready_Components(TEXTURE_DESC Desc);

public:
	static CTexture_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
