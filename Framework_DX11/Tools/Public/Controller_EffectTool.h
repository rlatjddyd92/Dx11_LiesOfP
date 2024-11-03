#pragma once

#include "Tools_Defines.h"
#include "Base.h"

#include "VIBuffer_Terrain.h"
#include "GameInstance.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)

class CController_EffectTool : public CBase
{
	DECLARE_SINGLETON(CController_EffectTool)

public:
	vector<_wstring>& Get_PrototypeTags() { return m_PrototypeTags; }
	_wstring& Get_PrtotypeTagIndex(_uint iIndex) { return m_PrototypeTags[iIndex]; }

	void Add_ProtytypeTag(_wstring strTag) { m_PrototypeTags.emplace_back(strTag); }

	void Set_JumhoCamera(_bool bCamera) {
		m_bJunHoCamera = bCamera;
	}

	_bool Get_JunhoCamera() {
		return m_bJunHoCamera;
	}

private:
	CController_EffectTool();
	virtual ~CController_EffectTool() = default;

public:
	HRESULT Initialize();

public:
	HRESULT Add_Particle();
	void Check();
	void Update_Particle();
	void Select_Effect();

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	vector<_wstring>	m_PrototypeTags;

	_bool m_bJunHoCamera = { false };

private:
	// Particle Create
	_uint		m_iNumInstance = { 100 };
	_float3		m_vCenter = {};
	_float3		m_vRange = { 1.f, 1.f, 1.f };
	_float3		m_vExceptRange = {};
	_float2		m_vSize = { 0.5f, 1.f };
	_float2		m_vSpeed = { 1.f, 2.f };
	_float2		m_vLifeTime = { 2.f, 4.f };
	_float4		m_vMinColor = { 0.f, 0.f, 0.f, 1.f };
	_float4		m_vMaxColor = { 0.f, 0.f, 0.f, 1.f };
	
	// Particle Action
	_uint		m_iParticleType = { 0 };	// 퍼지거나 모이거나 단방향이거나
	
	_uint		m_iParticleState = { 0 };	// 공전? 랜덤? 반복?
	_float		m_fRenderRatio = { 0.f };
	_Vec4		m_vPivot = {};
	_float		m_fSpeed = { 0.f };
	_float		m_fGravity = { 0.f };

	_Vec4		m_vMoveDir = {};	// 단방향 이동 방향.

	_Vec3		m_vRevolveAxis = {};
	_float		m_fAngle = { 0.f };

	_float		m_fTimeInterval = { 0.f };
	_float		m_fRandomRatio = { 0.f };

	_bool		m_bRevolve = { false };
	_bool		m_bRandom = { false };
	_bool		m_bLoop = { false };

	_Vec3		m_vPos = {};
	_Vec3		m_vRotation = {};
	_Vec3		m_vScale = {};

	_uint		m_iParticleCount = {0};

public:
	virtual void Free() override;
};

END

