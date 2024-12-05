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
		PB_ORBIT, PB_RANDOM, PB_LOOP, PB_ACCEL, PB_DECEL,
		// Geom State
		PB_GROW, PB_SHRINK, PB_ROTATION,
		PB_END
	};

	enum TRAIL_MT_BOOL
	{
		// Compute State
		TMP_ORBIT, TMP_RANDOM, TMP_LOOP, TMP_ACCEL, TMP_DECEL, TMP_TAILSPREAD,
		// Geom State
		TMP_GROW, TMP_SHRINK, TMP_ROTATION,
		TMP_END
	};

	enum TRAIL_OP_BOOL
	{
		TOP_GROW, TOP_SHRINK, TOP_END
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
	HRESULT Initialize(ID3D11Device* pDevice,  ID3D11DeviceContext* pContext, const _wstring& strEffectPath);
	void Render();

public:
	void RenderGroup_Selection();

	HRESULT Add_Particle();
	void Particle_Check();
	void Update_Particle();
	void Select_Particle();
	void Get_Particle();
	void Delete_Particle();
	void Clear_Particle();

	HRESULT Add_TE();
	void TE_Check();
	void Update_TE();
	void Select_TE();
	void Get_TE();
	void Delete_TE();
	void Clear_TE();

	HRESULT Add_Mesh();
	void Mesh_Check();
	void Update_Mesh();
	void Select_Mesh();
	void Get_Mesh();
	void Delete_Mesh();
	void Clear_ME();

	HRESULT Add_Trail_OP();
	void Trail_OP_Check();
	void Update_Trail_OP();
	void Select_Trail_OP();
	void Get_Trail_OP();
	void Delete_Trail_OP();
	void Clear_Trail_OP();

	HRESULT Add_Trail_TP();
	void Trail_TP_Check();
	void Update_Trail_TP();
	void Select_Trail_TP();
	void Get_Trail_TP();
	void Delete_Trail_TP();
	void Clear_Trail_TP();

	HRESULT Add_Trail_MP();
	void Trail_MP_Check();
	void Update_Trail_MP();
	void Select_Trail_MP();
	void Get_Trail_MP();
	void Delete_Trail_MP();
	void Clear_Trail_MP();

	HRESULT Check_EffectContainer();
	HRESULT Add_EffectContainer();
	HRESULT Back_EffectContainer();
	void	Reset_EffectContainer();
	void	Delete_EffectContainer();

	HRESULT Load_Effect();
	HRESULT Save_EffectContainer();
	HRESULT Load_EffectContainer();

	HRESULT Load_Texture(const _wstring& strEffectPath);
	HRESULT Load_Model(const _wstring& strEffectPath);
	HRESULT Load_Shader();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	vector<_wstring>	m_Texture_PrototypeTags;
	vector<_wstring>	m_Model_PrototypeTags;

	_char m_szParticleName[MAX_PATH] = "";
	_char m_szTextureName[MAX_PATH] = "";
	_char m_szMeshName[MAX_PATH] = "";

	_char m_szTrail_OPName[MAX_PATH] = "";
	_char m_szTrail_TPName[MAX_PATH] = "";
	_char m_szTrail_MPName[MAX_PATH] = "";

	_char m_szEffectContainerName[MAX_PATH] = "";

	_bool m_bJunHoCamera = { false };

	_int		m_iSelected_DiffuseTextureIndex = { 0 };
	_int		m_iSelected_NormalTextureIndex = { 0 };
	_int		m_iSelected_MaskTextureIndex_1 = { 0 };
	_int		m_iSelected_MaskTextureIndex_2 = { 0 };
	_int		m_iSelected_ModelIndex = { 0 };

	_int		m_iTextureSelect = { 0 }; // 리스트 박스에 띄울 텍스처

private:
	CParticle_Effect::PARTICLE_EFFECT_DESC	m_ParticleDesc = {};
	CTexture_Effect::TEXTURE_EFFECT_DESC	m_TextureDesc = {};
	CMesh_Effect::MESH_EFFECT_DESC			m_MeshDesc = {};

	CTrail_Effect_OP::TRAIL_OP_DESC			m_Trail_OPDesc = {};
	CTrail_Effect_TP::TRAIL_TP_DESC			m_Trail_TPDesc = {};
	CTrail_Effect_MP::TRAIL_MP_DESC			m_Trail_MPDesc = {};

	CEffect_Base::RENDER_DESC				m_RenderDesc = {};

	_int m_iParticleIndex = { 0 };
	_int m_iTextureIndex = { 0 };
	_int m_iMeshIndex = { 0 };
	_int m_iTrail_OPIndex = { 0 };
	_int m_iTrail_TPIndex = { 0 };
	_int m_iTrail_MPIndex = { 0 };

#pragma region CHECKBOX
	_bool m_ParticleStates[PB_END] = {};

	_bool m_Trail_OPState[TOP_END] = {};
	_bool m_Trail_MPState[TMP_END] = {};

	_bool m_EffectPP[PP_END] = {};
#pragma endregion


public:
	list<string> logs;

private:
	void Set_ParticleState();
	void Set_TrailOP_State();
	void Set_TrailMP_State();
	void Texture_Selection();
	void Set_PpState();

	_wstring Get_FileExtentin(const _wstring& strFileTag);
	_wstring Remove_FileExtentin(const _wstring& strFileTag);

public:
	virtual void Free() override;
};

END
