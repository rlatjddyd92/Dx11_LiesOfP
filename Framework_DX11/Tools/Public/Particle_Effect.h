#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instance;
class CTexture;
END

BEGIN(Tools)
class CParticle_Effect final : public CEffect_Base
{
public:
	enum TYPE { TYPE_SPREAD, TYPE_MOVE, TYPE_CONVERGE, TYPE_SPREAD_INDEPENDENT, TYPE_MOVE_INDEPENDENT, TYPE_CONVERGE_INDEPENDENT, TYPE_END };

	enum RENDER_STATE 
	{ 
		RS_BLUR = 0x0001,
		RS_END = 0x9999
	};

	typedef struct
	{
		TYPE		eType = { TYPE_END };
		_uint		iState = { 0 };
		_float		fRenderRatio = { 0.f };
		_Vec4		vPivot = {};
		_float		fGravity = { 0.f };
		_Vec4		vMoveDir = {};
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

	typedef struct : CEffect_Base::EFFECT_BASE_DESC
	{
		_uint		iNumInstance = { 100 };
		_float3		vCenter = {};
		_float3		vRange = { 1.f, 1.f, 1.f };
		_float3		vExceptRange = {};
		_float2		vSize = { 0.5f, 1.f };
		_float2		vSpeed = { 1.f, 2.f };
		_float2		vLifeTime = { 2.f, 4.f };
		_float4		vMinColor = { 0.f, 0.f, 0.f, 1.f };
		_float4		vMaxColor = { 0.f, 0.f, 0.f, 1.f };

		DEFAULT_DESC DefaultDesc = {};
		REVOLVE_DESC OrbitDesc = {};
		RANDOM_DESC RandomDesc = {};
		ACCEL_DESC AccelDesc = {};
		TRANSFORM_DESC TransformDesc = {};

		_wstring strPrototypeTextureTag = L"";
		_uint iShaderIndex = { 0 };
		_uint iRenderState = { 0 };

	}PARTICLE_TEST_DESC;


private:
	CParticle_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Effect(const CParticle_Effect& Prototype);
	virtual ~CParticle_Effect() = default;

public:
	void Set_Default(DEFAULT_DESC desc) {
		m_DefaultDesc = desc;
		m_SaveDesc.DefaultDesc = desc;
	}
	void Set_Revolev(REVOLVE_DESC desc) {
		m_OrbitDesc = desc;
		m_SaveDesc.OrbitDesc = desc;
	}
	void Set_Random(RANDOM_DESC desc) {
		m_RandomDesc = desc;
		m_SaveDesc.RandomDesc = desc;
	}
	void Set_Accel(ACCEL_DESC desc) {
		m_AccelDesc = desc;
		m_SaveDesc.AccelDesc = desc;
	}
	void Set_ShaderIndex(_uint iShaderIndex) {
		m_iShaderIndex = iShaderIndex;
		m_SaveDesc.iShaderIndex = iShaderIndex;
	}

	void Set_RenderState(_uint iRenderState) {
		m_iRenderState = iRenderState;
		m_SaveDesc.iRenderState = iRenderState;
	}

	DEFAULT_DESC Get_Default() {
		return m_DefaultDesc;
	}

	REVOLVE_DESC Get_Orbit() {
		return m_OrbitDesc;
	}

	RANDOM_DESC Get_Random() {
		return m_RandomDesc;
	}

	ACCEL_DESC Get_Accel() {
		return m_AccelDesc;
	}

	_uint Get_RenderState() {
		return m_iRenderState;
	}

	PARTICLE_TEST_DESC* Get_SaveDesc_Ptr() {
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

public:
	void Set_Transform(TRANSFORM_DESC& Desc);

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };

private:
	DEFAULT_DESC m_DefaultDesc = {};
	REVOLVE_DESC m_OrbitDesc = {};
	RANDOM_DESC m_RandomDesc = {};
	ACCEL_DESC m_AccelDesc = {};

	_uint m_iShaderIndex = { 0 };
	_uint m_iRenderState = { 0 };

	PARTICLE_TEST_DESC m_SaveDesc = {};

private:
	HRESULT Ready_Components(PARTICLE_TEST_DESC* pDesc);

public:
	static CParticle_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
