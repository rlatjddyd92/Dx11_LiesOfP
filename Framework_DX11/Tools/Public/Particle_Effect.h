#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instance;
class CTexture;
class CShader_Compute;
END

BEGIN(Tools)
class CParticle_Effect final : public CEffect_Base
{
public:
	enum PARTICLE_TYPE { PT_SPREAD, PT_MOVE, PT_CONVERGE, PT_END };

	typedef struct
	{
		// 컴퓨트 셰이더에 전달
		PARTICLE_TYPE		eType = { PT_END };
		_uint				iComputeState = { 0 };
		_Vec4				vPivot = { 0.f, 0.f, 0.f, 1.f };
		_float				fGravity = { 0.f };
		_Vec4				vMoveDir = { 0.f, -1.f, 0.f, 0.f };
		_Vec3				vOrbitAxis = { 0.f, 1.f, 0.f};
		_float				fOrbitAngle = { 90.f };
		_float				fRandomTimeInterval = { 0.f };
		_float				fRandomMoveRatio = { 0.f };
		_float				fAccelSpeed = { 0.f };
		_float				fAccelLimit = { 0.f };

		// 위치 초기화
		_Vec3				vPos = {};
		_Vec3				vRotation = {};
		_Vec3				vScale = {1.f, 1.f, 1.f};

		// 렌더 셰이더에 전달
		_uint				iShaderIndex = { 0 };
		_uint				iGeomState = { 0 };
		_Vec2				vTexDevide = {1.f ,1.f};
		_float				fSpriteSpeed = { 0.f };
		_Vec2				vScaling = {1.f, 1.f};
		_float				fStartRotation = { 0.f };
		_float				fRotationPerSecond = { 0.f };
	}DEFAULT_DESC;

	typedef struct
	{
		_tchar		szDiffuseTexturTag[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_1[MAX_PATH] = L"";
		_tchar		szMaskTextureTag_2[MAX_PATH] = L"";
		_tchar		szNormalTextureTag[MAX_PATH] = L"";
	}TEXT_DESC;

	typedef struct
	{
		_uint		iNumInstance = { 100 };
		_float3		vCenter = {};
		_float3		vRange = { 1.f, 1.f, 1.f };
		_float3		vExceptRange = {};
		_float2		vSize = { 0.5f, 1.f };
		_float2		vSpeed = { 1.f, 2.f };
		_float2		vLifeTime = { 2.f, 4.f };
		_float4		vMinColor = { 0.f, 0.f, 0.f, 1.f };
		_float4		vMaxColor = { 1.f, 1.f, 1.f, 1.f };
	}BUFFER_DESC;

	typedef struct : CEffect_Base::EFFECT_BASE_DESC
	{
		DEFAULT_DESC	DefaultDesc = {};
		TEXT_DESC		TextDesc = {};
		BUFFER_DESC		BufferDesc = {};
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
	virtual HRESULT Save(_wstring strFilePath) override;

public:
	PARTICLE_EFFECT_DESC Get_Desc() {
		return m_InitDesc;
	}
	PARTICLE_EFFECT_DESC* Get_InitDesc_Ptr() {
		return &m_InitDesc;
	}
	void Set_Desc(const PARTICLE_EFFECT_DESC& ParticleDesc);

	virtual void Set_Loop(_bool bLoop) override;

private:
	class CShader_NonVTX* m_pShaderCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr, nullptr, nullptr, nullptr };

	// 이거 3개는 월드의 역행렬
	class CShader_Compute* m_pSpreadCS = { nullptr };
	class CShader_Compute* m_pMoveCS = { nullptr };
	class CShader_Compute* m_pConvergeCS = { nullptr };

	// 초기화.
	class CShader_Compute* m_pResetCS = { nullptr };

private:
	DEFAULT_DESC m_DefaultDesc = {};
	PARTICLE_EFFECT_DESC m_InitDesc = {};

private:
	HRESULT Ready_Components(const PARTICLE_EFFECT_DESC& Desc);

public:
	static CParticle_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
