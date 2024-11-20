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
	typedef struct
	{
		_uint		iShaderIndex = { 0 };
		_float		fDuration = { 10.f };
		_Vec4		vColor = { 0.f, 0.f, 0.f, 1.f };
		_Vec2		vDivide = { 1.f, 1.f };
		_float		fSpriteSpeed = { 0.f };

		_Vec3		vPos = { 0.f, 0.f, 0.f };

		_Vec3		vStartScale = { 1.f, 1.f, 1.f };
		_Vec3		vScalingSpeed = {};

		_float		fStarRotation = { 0.f };
		_float		fRotationPerSecond = { 0.f };

		_float		fAlpha = { 1.f };
		_float		fAlphaSpeed = { 0.f };

		_bool		bPreserveRotation = { false };
		_bool		bLoop = { false };
	}DEFAULT_DESC;

	typedef struct
	{
		_tchar		szDiffuseTexturTag[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_1[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_2[MAX_PATH] = L"";
		_tchar		szNormalTextureTag[MAX_PATH] = L"";
	}TEXT_DESC;

	typedef struct : public CEffect_Base::EFFECT_BASE_DESC
	{
		DEFAULT_DESC DefaultDesc = {};
		TEXT_DESC TextDesc = {};
	} TEXTURE_EFFECT_DESC;

private:
	CTexture_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture_Effect(const CTexture_Effect& Prototype);
	virtual ~CTexture_Effect() = default;

public:
	virtual void Set_Loop(_bool bLoop) override {
		m_DefaultDesc.bLoop = bLoop;
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
	void Set_Desc(const TEXTURE_EFFECT_DESC& desc);
	TEXTURE_EFFECT_DESC Get_Desc() {
		return m_InitDesc;
	}
	TEXTURE_EFFECT_DESC* Get_InitDesc_Ptr() {
		return &m_InitDesc;
	}

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr, nullptr, nullptr, nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	DEFAULT_DESC m_DefaultDesc = {};

	_float	m_fAccumulateTime = { 0.f };
	_float	m_fCurrenrtIndex = { 0.f };

	TEXTURE_EFFECT_DESC m_InitDesc = {};

private:
	HRESULT Ready_Components(const TEXT_DESC& Desc);
	void Preserve_Rotation_Billboard(_Vec3 vCurrentScale, _Vec3 vLook);
	void Billboard(_Vec3 vCurrentScale, _Vec3 vLook);

public:
	static CTexture_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
