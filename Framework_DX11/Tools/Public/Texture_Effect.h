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
		STATE_SCALING		= 0x0001,
		STATE_ROTATION		= 0x0002,
		STATE_FADEIN		= 0x0004,
		STATE_FADEOUT		= 0x0008,
		STATE_LOOP			= 0x0010,
		STATE_DISTORTION	= 0x0020,
		STATE_BLUR			= 0x0040,
		STATE_END			= 0x9999
	};

	typedef struct : public CEffect_Base::EFFECT_BASE_DESC
	{
		_wstring	strTexturTag = L"";

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
		_float		fScalingSpeed = { 0.f };

		_float		fAlpha = { 0.f };
		_float		fAlphaSpeed = { 0.f };
	} TEXTURE_EFFECT_DESC;

private:
	CTexture_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture_Effect(const CTexture_Effect& Prototype);
	virtual ~CTexture_Effect() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_Desc(const TEXTURE_EFFECT_DESC& desc);
	TEXTURE_EFFECT_DESC Get_Desc();
	void Reset();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_uint	m_iState = { 0 };
	_uint	m_iShaderIndex = { 0 };
	_float	m_fDuration = { 0.f };
	_Vec4	m_vColor = {};
	_Vec2	m_vDivide = {};
	_float	m_fSpriteSpeed = { 0.f };

	_float	m_fAlpha = { 0.f };
	_float	m_fStartAlpha = { 0.f };
	_float	m_fAlphaSpeed = { 0.f };

	_float	m_fRotationSpeed = { 0.f };
	
	_Vec3	m_vStartScale = {};
	_float	m_fScalingSpeed = { 0.f };

	_float	m_fAccumulateTime = { 0.f };
	_float	m_fCurrenrtIndex = { 0.f };

private:
	HRESULT Ready_Components(const _wstring strTexturTag);

public:
	static CTexture_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
