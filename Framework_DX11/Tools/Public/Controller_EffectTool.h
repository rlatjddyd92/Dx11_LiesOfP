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
#pragma region PARTICLE
	vector<_wstring>& Get_Particle_PrototypeTags() { 
		return m_Particle_PrototypeTags; 
	}
	
	_wstring& Get_Particle_PrototypeTags_Index(_uint iIndex) {
		return m_Particle_PrototypeTags[iIndex]; 
	}
	
	void Add_Particle_ProtytypeTag(_wstring strTag) { 
		m_Particle_PrototypeTags.emplace_back(strTag); 
	}
#pragma endregion
#pragma region TEXTURE
	vector<_wstring>& Get_TE_PrototypeTags() {
		return m_TE_PrototypeTags;
	}

	_wstring& Get_TE_PrototypeTags_Index(_uint iIndex) {
		return m_TE_PrototypeTags[iIndex];
	}

	void Add_TE_ProtytypeTag(_wstring strTag) {
		m_TE_PrototypeTags.emplace_back(strTag);
	}
#pragma endregion

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
	void Particle_Check();
	void Update_Particle();
	void Select_Particle();
	void Get_Particle();
	void Delete_Particle();

	HRESULT Add_TE();
	void TE_Check();
	void Update_TE();
	void Select_TE();
	void Get_TE();
	void Delete_TE();

	HRESULT Add_EffectContainer();
	void Reset_EffectContainer();
	void Delete_EffectContainer();

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	vector<_wstring>	m_Particle_PrototypeTags;
	vector<_wstring>	m_TE_PrototypeTags;

	_bool m_bJunHoCamera = { false };

private:
#pragma region PARTICLE
	// Particle Create
	_uint		m_iNumInstance = { 100 };
	_float3		m_vCenter = {};
	_float3		m_vRange = { 1.f, 1.f, 1.f };
	_float3		m_vExceptRange = {};
	_float2		m_vSize = { 0.5f, 1.f };
	_float2		m_vSpeed = { 1.f, 2.f };
	_float2		m_vLifeTime = { 2.f, 4.f };
	_float4		m_vMinColor = { 0.f, 0.f, 0.f, 1.f };
	_float4		m_vMaxColor = { 1.f, 1.f, 1.f, 1.f };
	
	// Particle Action
	_uint		m_iParticleType = { 0 };	// 퍼지거나 모이거나 단방향이거나
	
	_uint		m_iParticleState = { 0 };	// 공전? 랜덤? 반복?
	_uint		m_iParticleRenderState = { 0 };

	_float		m_fRenderRatio = { 1.f };
	_Vec4		m_vPivot = { 0.f, 0.f, 0.f, 1.f };
	_float		m_fGravity = { 0.f };

	_Vec4		m_vMoveDir = {};	// 단방향 이동 방향.

	_Vec3		m_vOrbitAxis = {0.f, 1.f, 0.f};
	_float		m_fAngle = { 90.f };

	_float		m_fTimeInterval = { 0.5f };
	_float		m_fRandomRatio = { 0.5f };

	_float		m_fAccelLimit = { 0.f };
	_float		m_fAccelSpeed = { 1.f };

	_bool		m_bOrbit = { false };
	_bool		m_bRandom = { false };
	_bool		m_bLoop = { false };
	_bool		m_bAccel = { false };
	_bool		m_bDecel = { false };
	_bool		m_bBlur = { false };

	_Vec3		m_vParticlePos = {};
	_Vec3		m_vParticleRotation = {};
	_Vec3		m_vParticleScale = {1.f, 1.f, 1.f};

	_int		m_iSelectedParticleIndex = { 0 };
	_int		m_iSelectedParticleTextureIndex = { 0 };

	_uint		m_iParticleShaderIndex = { 0 };
	_uint		m_iMax_ParticleShaderIndex = { 2 };
#pragma endregion

#pragma region TEXTURE
	_uint	m_iTEState = { 0 };
	_float	m_fDuration = { 10.f };
	_Vec4	m_vColor = {1.f, 1.f, 1.f, 1.f};
	_Vec2	m_vDivide = {};
	_float	m_fSpriteSpeed = { 0.f };

	_Vec3	m_vTEPos = {};

	_float	m_fRotationAngle = { 0.f };
	_float	m_fRotationSpeed = { 0.f };

	_Vec3	m_vTEScale = {1.f, 1.f, 1.f};
	_float	m_fScalingSpeed = { 0.f };

	_float	m_fAlpha = { 1.f };
	_float	m_fAlphaSpeed = { 0.f };

	_int	m_iSelectedTETextureIndex = { 0 };
	_int	m_iSelectedTEIndex = { 0 };

	_uint	m_iTEShaderIndex = { 0 };
	_uint	m_iMax_TEShaderIndex = { 0 };

	_bool m_bScaling		= { false };
	_bool m_bTurn			= { false };
	_bool m_bFadeIn			= { false };
	_bool m_bFadeOut		= { false };
	_bool m_isLoop			= { false };
	_bool m_isDistortion	= { false };
	_bool m_isBlur			= { false };

#pragma endregion

private:
	void Set_ParticleState();
	void Set_TEState();

public:
	virtual void Free() override;
};

END

