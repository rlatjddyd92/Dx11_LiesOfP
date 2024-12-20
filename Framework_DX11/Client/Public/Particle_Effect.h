#pragma once

#include "Effect_Base.h"

BEGIN(Engine)
class CShader_NonVtx;
class CVIBuffer_Point_Instance;
class CTexture;
class CShader_Compute;
END

BEGIN(Client)
class CParticle_Effect final : public CEffect_Base
{
public:
	enum PARTICLE_TYPE { PT_SPREAD, PT_MOVE, PT_CONVERGE, PT_LOCALSPREAD, PT_LOCALMOVE, PT_LOCALCONVERGE, PT_END };

	typedef struct
	{
		// 컴퓨트 셰이더에 전달
		PARTICLE_TYPE		eType = { PT_END };
		_uint				iComputeState = { 0 };
		_Vec4				vPivot = { 0.f, 0.f, 0.f, 1.f };
		_float				fGravity = { 0.f };
		_Vec4				vMoveDir = { 0.f, -1.f, 0.f, 0.f };
		_Vec3				vOrbitAxis = { 0.f, 1.f, 0.f };
		_float				fOrbitAngle = { 90.f };
		_float				fRandomTimeInterval = { 0.f };
		_float				fRandomMoveRatio = { 0.f };
		_float				fAccelSpeed = { 0.f };
		_float				fAccelLimit = { 0.f };

		// 위치 초기화
		_Vec3				vPos = {};
		_Vec3				vRotation = {};
		_Vec3				vScale = { 1.f, 1.f, 1.f };
		_Vec3				vOverallScaling = { 0.f, 0.f, 0.f };

		// 지속 시간
		_float				fDuration = { 0.f };

		// 렌더 셰이더에 전달
		_uint				iShaderIndex = { 0 };
		_uint				iGeomState = { 0 };
		_Vec2				vTexDevide = { 1.f ,1.f };
		_float				fSpriteSpeed = { 0.f };
		_Vec2				vStartScaling = { 1.f, 1.f };
		_Vec2				vScalingRatio = { 0.f, 0.f };
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

	typedef struct : CEffect_Base::EFFECT_BASE_DESC
	{
		DEFAULT_DESC	DefaultDesc = {};
		TEXT_DESC		TextDesc = {};
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
	virtual void Set_Loop(_bool bLoop) override;

private:
	class CShader_NonVTX* m_pShaderCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom[TEXTURE_END] = { nullptr, nullptr, nullptr, nullptr };

	class CShader_Compute* m_pActionCS = { nullptr };
	class CShader_Compute* m_pResetCS = { nullptr };

private:
	DEFAULT_DESC m_DefaultDesc = {};
	DEFAULT_DESC m_InitDesc = {};

	_float m_fAccumulateTime = { 0.f };

private:
	HRESULT Ready_Components(const PARTICLE_EFFECT_DESC& Desc);

public:
	static CParticle_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
