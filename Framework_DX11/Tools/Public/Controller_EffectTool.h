#pragma once

#include "Tools_Defines.h"
#include "Base.h"

#include "GameInstance.h"

#include "Particle_Effect.h"
#include "Texture_Effect.h"
#include "Mesh_Effect.h"
#include "Trail_Effect_MP.h"
#include "Trail_Effect_OP.h"
#include "Trail_Effect_TP.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tools)

class CController_EffectTool : public CBase
{
	DECLARE_SINGLETON(CController_EffectTool)

public:
#pragma region ENUM
	enum PARTICLE_BOOL
	{
		// Compute State
		BOOL_ORBIT, BOOL_RANDOM, BOOL_LOOP, BOOL_ACCEL, BOOL_DECEL,
		// Geom State
		BOOL_GROW, BOOL_SHRINK, BOOL_ROTATION,
		BOOL_END
	};

	enum EFFECT_POSTPROCESSING
	{
		PP_NONE,
		PP_DISTORTION,
		PP_BLUR,
		PP_END
	};
#pragma endregion
#pragma region TEXTURE
	vector<_wstring>& Get_Texture_PrototypeTags() { 
		return m_Texture_PrototypeTags;
	}
	
	_wstring& Get_Texture_PrototypeTags_Index(_uint iIndex) {
		return m_Texture_PrototypeTags[iIndex];
	}
	
	void Add_Texture_ProtytypeTag(_wstring strTag) {
		m_Texture_PrototypeTags.emplace_back(strTag);
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
	void Render();

public:
	void RenderGroup_Selection();

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

	HRESULT Add_Mesh();
	void Mesh_Check();
	void Update_Mesh();
	void Select_Mesh();
	void Get_Mesh();
	void Delete_Mesh();

	HRESULT Add_EffectContainer();
	void Reset_EffectContainer();
	void Delete_EffectContainer();
	HRESULT Save_EffectContainer();
	HRESULT Load_Effect();

	void Set_EffectName();

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	vector<_wstring>	m_Texture_PrototypeTags;
	vector<_wstring>	m_Model_PrototypeTags;

	_char m_szEffectName[MAX_PATH] = "";

	_bool m_bJunHoCamera = { false };
	

	_int		m_iSelected_DiffuseTextureIndex = { 0 };
	_int		m_iSelected_NormalTextureIndex = { 0 };
	_int		m_iSelected_MaskTextureIndex_1 = { 0 };
	_int		m_iSelected_MaskTextureIndex_2 = { 0 };
	_int		m_iSelected_ModelIndex = { 0 };

private:
	CParticle_Effect::PARTICLE_EFFECT_DESC	m_ParticleDesc = {};
	CTexture_Effect::TEXTURE_EFFECT_DESC	m_TextureDesc = {};
	CMesh_Effect::MESH_EFFECT_DESC			m_MeshDesc = {};

	CEffect_Base::RENDER_DESC				m_RenderDesc = {};

	_int m_iParticleIndex = { 0 };
	_int m_iTextureIndex = { CEffect_Base::TEXTURE_END };
	_int m_iMeshIndex = { 0 };

	_int m_iTextureSelect = { 0 };

#pragma region CHECKBOX
	_bool m_ParticleStates[BOOL_END] = {};
	_bool m_EffectPP[PP_END] = {};		// 디스토션? 블러? 그냥?
#pragma endregion

private:
	void Set_ParticleState();

	void Texture_Selection();

	void Set_PpState();
public:
	virtual void Free() override;
};

END

