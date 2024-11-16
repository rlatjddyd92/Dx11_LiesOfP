#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instance;
class CTexture;
END

BEGIN(Client)
class CParticle_Effect final : public CEffect_Base
{
public:
	enum TYPE { TYPE_SPREAD, TYPE_MOVE, TYPE_CONVERGE, TYPE_SPREAD_INDEPENDENT, TYPE_MOVE_INDEPENDENT, TYPE_CONVERGE_INDEPENDENT, TYPE_END };

	enum PARTICLE_STATE 
	{ 
		PS_GROW = 0x0001, 
		PS_SHRINK = 0x0002,
		PS_ROTATION = 0x0004,
		PS_END
	};

	enum RENDER_STATE 
	{ 
		RS_NONBLEND = 0x0001,
		RS_NONLIGHT = 0x0002,
		RS_BLUR = 0x0004,
		RS_END = 0x9999
	};

	typedef struct
	{
		TYPE		eType = { TYPE_END };
		_uint		iState = { 0 };
		_Vec4		vPivot = {};
		_float		fGravity = { 0.f };
		_Vec4		vMoveDir = {};

		_uint		iShaderIndex = { 0 };
		_uint		iRenderState = { 0 };
		_uint		iParticleState = { 0 };

		_Vec2		vTexDevide = {};
		_float		fSpriteSpeed = { 0.f };

		_Vec2		vScaling = {};

		_float		fStartRotation = { 0.f };
		_float		fAngle = { 0.f };
	}DEFAULT_DESC;

	typedef struct
	{
		_Vec3		vOrbitAxis = {};
		_float		fAngle = { 0.f };
	}REVOLVE_DESC;

	typedef struct
	{
		_float		fTimeInterval = { 0.f };
		_float		fRandomRatio = { 0.f };
	}RANDOM_DESC;

	typedef struct
	{
		_float		fAccelSpeed = { 0.f };
		_float		fAccelLimit = { 0.f };
	}ACCEL_DESC;

	typedef struct
	{
		_Vec3		vPos = {};
		_Vec3		vRotation = {};
		_Vec3		vScale = {};
	}TRANSFORM_DESC;

	typedef struct
	{
		_tchar		strDiffuseTexturTag[MAX_PATH] = L"";
		_tchar		strNomralTextureTag[MAX_PATH] = L"";
		_tchar		strMaskTextureTag[MAX_PATH] = L"";
	}TEXTURE_DESC;

	typedef struct
	{
		DEFAULT_DESC	DefaultDesc = {};
		REVOLVE_DESC	OrbitDesc = {};
		RANDOM_DESC		RandomDesc = {};
		ACCEL_DESC		AccelDesc = {};
		TRANSFORM_DESC	TransformDesc = {};
		TEXTURE_DESC	TextureDesc = {};
	}INIT_DESC;

	typedef struct : CEffect_Base::EFFECT_BASE_DESC
	{
		INIT_DESC		InitDesc = {};
		_wstring		strVIBufferTag = TEXT("");
	}PARTICLE_EFFECT_DESC;

private:
	CParticle_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Effect(const CParticle_Effect& Prototype);
	virtual ~CParticle_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Reset() override;

public:
	void Set_Transform(TRANSFORM_DESC& Desc);

private:
	class CShader_NonVTX* m_pShaderCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pDiffuseTextureCom = { nullptr };
	class CTexture* m_pNormalTextureCom = { nullptr };
	class CTexture* m_pMaskTextureCom = { nullptr };

	class CShader_Compute* m_pActionCS = { nullptr };
	class CShader_Compute* m_pResetCS = { nullptr };

private:
	DEFAULT_DESC m_DefaultDesc = {};
	REVOLVE_DESC m_OrbitDesc = {};
	RANDOM_DESC m_RandomDesc = {};
	ACCEL_DESC m_AccelDesc = {};

	PARTICLE_EFFECT_DESC m_SaveDesc = {};

private:
	HRESULT Ready_Components(PARTICLE_EFFECT_DESC* pDesc);

public:
	static CParticle_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
