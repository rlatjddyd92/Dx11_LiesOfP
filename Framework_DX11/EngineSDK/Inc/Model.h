#pragma once

#include "Component.h"
#include "Bone.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:	
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	vector<class CMesh*>&	Get_Meshes() { return m_Meshes; }
	_uint					Get_NumMeshes() const { return m_iNumMeshes; }

	TYPE					Get_ModelType() const { return m_eType; }

	vector<class CBone*>&	Get_Bones() { return m_Bones; }
	_int					Get_BoneIndex(const _char* pBoneName) const;
	_matrix					Get_BoneCombindTransformationMatrix(_uint iBoneIndex) const { return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix(); }
	const _float4x4*		Get_BoneCombindTransformationMatrix_Ptr(const _char* pBoneName) const { return m_Bones[Get_BoneIndex(pBoneName)]->Get_CombinedTransformationMatrix_Ptr(); }


	vector<class CAnimation*>& Get_Animations() { return m_Animations; }
	_uint					Get_CurrentAnimationIndex() { return m_iCurrentAnimIndex; }
	_char*					Get_CurrentAnimationName();
	_bool					Get_IsEnd_Animation(_uint iAnimationIndex);

	_uint					Get_CurrentFrame() { return m_iCurrentFrame; }
	_double					Get_CurrentTrackPosition() { return m_CurrentTrackPosition; }
	void					Set_CurrentTrackPosition(_double TrackPos) { m_CurrentTrackPosition = TrackPos; }

	void					Set_UFBIndices(_uint eCount, _uint iIndex) { m_UFBIndices[eCount] = iIndex; }
	_uint					Get_UFBIndices(_uint eCount) { return m_UFBIndices[eCount]; }

	class CTexture*			Find_Texture(_uint iMeshNum, TEXTURE_TYPE eMaterialType);

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;	
	virtual HRESULT Render(_uint iMeshIndex);

public:
	_vector Play_Animation(_float fTimeDelta, _bool* pOut = nullptr);

	void		SetUp_Animation(_uint iAnimationIndex, _bool isLoop = false);
	HRESULT     SetUp_NextAnimation(_uint iNextAnimationIndex, _bool isLoop = false, _float fChangeDuration = 0.2f, _uint iStartFrame = 0);

	_uint		Setting_Animation(const _char* szAnimationmName, _double SpeedRatio = 1.0) const;


public:
	HRESULT Bind_Material(class CShader * pShader, const _char * pConstantName, TEXTURE_TYPE eMaterialType, _uint iMeshIndex);
	HRESULT Bind_MeshBoneMatrices(class CShader * pShader, const _char * pConstantName, _uint iMeshIndex);

private:
	TYPE							m_eType = { TYPE_END };

private: /* 메시의 정보를 저장한다. */
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;
	_float4x4						m_PreTransformMatrix = {};

private: 
	_uint							m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>			m_Materials;

private:
	vector<class CBone*>			m_Bones;

private:
	_bool							m_isLoop = { false };
	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;


	/* 현재 애니메이션의 재생 위치. */
	_double							m_CurrentTrackPosition = {};
	_double							m_ChangeTrackPosition = {};

	/* 각 애니메이션당 각 채널들의 현재 키프레임인덱스 */
	vector<vector<_uint>>			m_KeyFrameIndices;

	_float							m_fCurrentAnimationSpeedRadio = { 1.f };

	_int							m_iCurrentFrame = { 0 };
	_bool*							m_isEnd_Animations;
	_bool							m_isChangeAni = { false };	// 바꾸는중?

	CHANGEANIMATION_DESC			m_tChaneAnimDesc = {};

	//이봉준 애니메이션
private:
	vector<_uint>				m_UFBIndices;

public:
	HRESULT	Ready_Meshes(HANDLE* pFile);
	HRESULT Ready_Materials(HANDLE* pFile, const _char* pModelFilePath);
	HRESULT Ready_Bones(HANDLE* pFile, _int iParentBoneIndex);
	HRESULT Ready_Animations(HANDLE* pFile);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END