#pragma once

#include "Tools_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instance;
class CTexture;
END

BEGIN(Tools)
class CParticle_Test final : public CGameObject
{
public:
	enum TYPE { TYPE_SPREAD, TYPE_MOVE, TYPE_CONVERGE, TYPE_SPREAD_INDEPENDENT, TYPE_MOVE_INDEPENDENT, TYPE_CONVERGE_INDEPENDENT, TYPE_END };

	typedef struct : CGameObject::GAMEOBJECT_DESC
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
	}PARTICLE_TEST_DESC;

private:
	CParticle_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Test(const CParticle_Test& Prototype);
	virtual ~CParticle_Test() = default;

public:
	void Set_Type(TYPE eType) {
		m_eType = eType;
	}

	void Set_State(_uint iState) {
		m_iState = iState;
	}

	void Set_RenderRatio(_float fRenderRatio) {
		m_fRenderRatio = fRenderRatio;
	}

	void Set_Pivot(_Vec4 vPivot) {
		m_vPivot = vPivot;
	}

	void Set_Speed(_float fSpeed) {
		m_fSpeed = fSpeed;
	}

	void Set_Gravity(_float fGravity) {
		m_fGravity = fGravity;
	}

	void Set_MoveDir(_Vec4 vMoveDir) {
		m_vMoveDir = vMoveDir;
	}

	void Set_RevolveAxis(_Vec3 vRevolveAxis) {
		m_vRevolveAxis = vRevolveAxis;
	}

	void Set_Angle(_float fAngle) {
		m_fAngle = fAngle;
	}

	void Set_TimeInterval(_float fTimeInterval) {
		m_fTimeInterval = fTimeInterval;
	}

	void Set_RandomRatio(_float fRandomRatio) {
		m_fRandomRatio = fRandomRatio;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };

private:
	TYPE		m_eType = { TYPE_END };

	_uint		m_iState = { 0 };	// 공전? 랜덤? 반복?
	_float		m_fRenderRatio = { 0.f };
	_Vec4		m_vPivot = {};
	_float		m_fSpeed = { 0.f };
	_float		m_fGravity = { 0.f };

	_Vec4		m_vMoveDir = {};	// 단방향 이동 방향.

	// 공전시
	_Vec3		m_vRevolveAxis = {};
	_float		m_fAngle = { 0.f };

	// 랜덤시
	_float		m_fTimeInterval = { 0.f };
	_float		m_fRandomRatio = { 0.f };

private:
	HRESULT Ready_Components(PARTICLE_TEST_DESC* pDesc);

public:
	static CParticle_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
