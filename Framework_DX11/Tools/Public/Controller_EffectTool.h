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
#pragma region DIFFUSE
	vector<_wstring>& Get_Diffuse_PrototypeTags() { 
		return m_Diffuse_PrototypeTags;
	}
	
	_wstring& Get_Diffuse_PrototypeTags_Index(_uint iIndex) {
		return m_Diffuse_PrototypeTags[iIndex];
	}
	
	void Add_Diffuse_ProtytypeTag(_wstring strTag) {
		m_Diffuse_PrototypeTags.emplace_back(strTag);
	}
#pragma endregion
#pragma region NORMAL
	vector<_wstring>& Get_Normal_PrototypeTags() {
		return m_Normal_PrototypeTags;
	}

	_wstring& Get_Normal_PrototypeTags_Index(_uint iIndex) {
		return m_Normal_PrototypeTags[iIndex];
	}

	void Add_Normal_ProtytypeTag(_wstring strTag) {
		m_Normal_PrototypeTags.emplace_back(strTag);
	}
#pragma endregion
#pragma region MODEL
	vector<_wstring>& Get_Model_PrototypeTags() {
		return m_Model_PrototypeTags;
	}

	_wstring& Get_Model_PrototypeTags_Index(_uint iIndex) {
		return m_Model_PrototypeTags[iIndex];
	}

	void Add_Model_ProtytypeTag(_wstring strTag) {
		m_Model_PrototypeTags.emplace_back(strTag);
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
	void TextureCheck();

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

	HRESULT Add_Model();
	void Model_Check();
	void Update_Model();
	void Select_Model();
	void Get_Model();
	void Delete_Model();

	HRESULT Add_EffectContainer();
	void Reset_EffectContainer();
	void Delete_EffectContainer();

	HRESULT Save_EffectContainer();
	HRESULT Load_Effect();

	void Set_EffectName();

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	vector<_wstring>	m_Diffuse_PrototypeTags;
	vector<_wstring>	m_Normal_PrototypeTags;
	vector<_wstring>	m_Model_PrototypeTags;

	_char m_szEffectName[MAX_PATH] = "";

	_bool m_bJunHoCamera = { false };
	
	_int		m_iTextureSelect = { 0 };

	_int		m_iSelected_DiffuseTextureIndex = { 0 };
	_int		m_iSelected_NormalTextureIndex = { 0 };
	_int		m_iSelected_MaskTextureIndex_1 = { 0 };
	_int		m_iSelected_MaskTextureIndex_2 = { 0 };
	_int		m_iSelected_ModelIndex = { 0 };

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
	
	_uint		m_iInstanceState = { 0 };	// 공전? 랜덤? 반복?
	_uint		m_iParticleRenderState = { 0 };
	_uint		m_iParticleState = { 0 };

	_float		m_fRenderRatio = { 1.f };
	_Vec4		m_vPivot = { 0.f, 0.f, 0.f, 1.f };
	_float		m_fGravity = { 0.f };

	_Vec4		m_vMoveDir = {};	// 단방향 이동 방향.
	_Vec2		m_vTexDevide = {1.f, 1.f};
	_float		m_fParticle_SpriteSpeed = { 0.f };

	_Vec2		m_vScaling = {1.f, 1.f};
	_float		m_fStartRotation = { 0.f };
	_float		m_fParticleAngle = { 0.f };

	_Vec3		m_vOrbitAxis = {0.f, 1.f, 0.f};

	
	_float		m_fAngle = { 90.f };

	_float		m_fTimeInterval = { 0.5f };
	_float		m_fRandomRatio = { 0.5f };

	_float		m_fAccelLimit = { 0.f };
	_float		m_fAccelSpeed = { 1.f };

	_bool		m_bGrow = { false };
	_bool		m_bShrink = { false };
	_bool		m_bRotation = { false };

	_bool		m_bOrbit = { false };
	_bool		m_bRandom = { false };
	_bool		m_bLoop = { false };
	_bool		m_bAccel = { false };
	_bool		m_bDecel = { false };

	_bool		m_bBlur = { false };
	_bool		m_bNonBlend = { false };
	_bool		m_bNonLight = { false };
	

	_Vec3		m_vParticlePos = {};
	_Vec3		m_vParticleRotation = {};
	_Vec3		m_vParticleScale = {1.f, 1.f, 1.f};

	_int		m_iSelectedParticleIndex = { 0 };

	_uint		m_iParticleShaderIndex = { 0 };
	_uint		m_iMax_ParticleShaderIndex = { 10 };
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
	_Vec3	m_vScalingSpeed = { };

	_float	m_fAlpha = { 1.f };
	_float	m_fAlphaSpeed = { 0.f };

	_int	m_iSelectedTEIndex = { 0 };

	_uint	m_iTEShaderIndex = { 0 };
	_uint	m_iMax_TEShaderIndex = { 10 };

	_bool	m_isDistortion	= { false };
	_bool	m_isBlur			= { false };
	_bool	m_isBlend			= { false };
	_bool	m_isNonBlend		= { false };

#pragma endregion

#pragma region
	_uint		m_iMesh_State = { 0 };
	_uint		m_iMesh_ShaderIndex = { 0 };
	_float		m_fMesh_Duration = { 10.f };
	_Vec4		m_vMesh_Color = {0.f, 0.f, 0.f, 1.f};
	_Vec2		m_vMesh_TileRepeat = {1.f, 1.f};
	_Vec2		m_vMesh_TileMoveDir = {};
	_float		m_fMesh_TileMoveSpeed = { 0.f };
	_Vec3		m_vMesh_Pos = {};
	_Vec3		m_vMesh_StartRotation = {0.f, 0.f, 0.f};
	_Vec4		m_vMesh_RotationAxis = {0.f, 1.f, 0.f, 0.f};
	_float		m_fMesh_RotationSpeed = { 0.f };
	_Vec3		m_vMesh_Scale = {1.f, 1.f, 1.f};
	_Vec3		m_vMesh_ScalingSpeed = {0.f, 0.f, 0.f};
	_float		m_fMesh_Alpha = { 1.f };
	_float		m_fMesh_AlphaSpeed = { 0.f };

	_bool		m_isMesh_Distortion		= { false };
	_bool		m_isMesh_Blur			= { false };
	_bool		m_isMesh_Blend			= { false };
	_bool		m_isMesh_NonLight		= { false };

	_int		m_iSelectedMEIndex = { 0 };

#pragma endregion

private:
	void Set_ParticleState();
	void Set_TEState();
	void Set_MeshState();
	

public:
	virtual void Free() override;
};

END

