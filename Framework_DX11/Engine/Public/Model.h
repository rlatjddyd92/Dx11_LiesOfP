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
	const _float4x4*		Get_BoneCombindTransformationMatrix_Ptr(_uint iBoneIndex) const { return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix_Ptr(); }

	vector<class CAnimation*>& Get_Animations() { return m_Animations; }
	_uint					Get_CurrentAnimationIndex() { return m_iCurrentAnimIndex; }
	_uint					Get_CurrentAnimationIndex_Boundary() { return m_iCurrentAnimIndex_Boundary; }
	_char*					Get_CurrentAnimationName();

	_uint					Get_CurrentFrame() { return m_iCurrentFrame; }
	_double					Get_CurrentTrackPosition() { return m_CurrentTrackPosition; }
	void					Set_CurrentTrackPosition(_double TrackPos) { m_CurrentTrackPosition = TrackPos; }
	void					Set_CurrentTrackPosition_Boundary(_double TrackPos) { m_CurrentTrackPosition_Boundary = TrackPos; }

	void					Set_UFBIndices(_uint eCount, _uint iIndex) { m_UFBIndices[eCount] = iIndex; }
	_uint					Get_UFBIndices(_uint eCount) { return m_UFBIndices[eCount]; }

	class CTexture*			Find_Texture(_uint iMeshNum, TEXTURE_TYPE eMaterialType);

	void					Add_UFVtxIndices(UFVTX UFVtx) { m_UseFullVtxIndices.push_back(UFVtx); }
	vector<UFVTX>*			Get_UFVtxIndices() { return &m_UseFullVtxIndices; }

	void					Set_AnimPlay(_bool bCtrAnim) { m_bPlayAnimCtr = bCtrAnim; }

	_bool					Get_IsUseBoundary() { return m_isUseBoundary; }

	HRESULT					Update_Boundary();

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix, _bool isBinaryAnimModel, FilePathStructStack* pStructStack);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);
	HRESULT Render_Instance(_uint iMeshIndex);

	void Add_InstanceData(_Matrix WorldMatrix) { m_InstanceDatas.push_back(WorldMatrix); }
	void Clear_InstanceData() { m_InstanceDatas.clear(); }

public:		//_bool pOut은 메인 애니메이션의 종료를 반환,				
	_vector		Play_Animation(_float fTimeDelta, _bool* pOut = nullptr, list<OUTPUT_EVKEY>* pEvKeyList = nullptr);

	_uint		Find_AnimationIndex(const _char* pAnimationmName, _float fSpeedRatio = 1.f);
	void		SetUp_Animation(_uint iAnimationIndex, _bool isLoop = false);
	HRESULT     SetUp_NextAnimation(_uint iNextAnimationIndex, _bool isLoop = false, _float fChangeDuration = 0.2f, _uint iStartFrame = 0);
	HRESULT     SetUp_NextAnimation_Boundary(_uint iNextAnimationIndex, _bool isLoop = false, _float fChangeDuration = 0.2f, _uint iStartFrame = 0);

	_uint		Setting_Animation(const _char* szAnimationmName, _double SpeedRatio = 1.0) const;
	_matrix		CalcMatrix_forVtxAnim(_uint iMeshNum, VTXANIMMESH VtxStruct);


public:
	HRESULT		Bind_Material(class CShader* pShader, const _char* pConstantName, TEXTURE_TYPE eMaterialType, _uint iMeshIndex);
	HRESULT		Bind_MeshBoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

	HRESULT		Create_BinaryFile(const _char* ModelTag);
	HRESULT		Create_Bin_Bones(HANDLE* pFile);
	HRESULT		Create_Bin_Meshes(HANDLE* pFile);
	HRESULT		Create_Bin_Materials(HANDLE* pFile);
	HRESULT		Create_Bin_Animations(HANDLE* pFile);

	HRESULT		ReadyModel_To_Binary(HANDLE* pFile);

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
	_bool							m_isLoop_Boundary = { false };		//상하체 분리
	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iCurrentAnimIndex_Boundary = { 0 };		//상하체 분리
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;


	/* 현재 애니메이션의 재생 위치. */
	_double							m_CurrentTrackPosition = {};
	_double							m_ChangeTrackPosition = {};
	_double							m_CurrentTrackPosition_Boundary = {};	//상하체 분리
	_double							m_ChangeTrackPosition_Boundary = {};	//상하체 분리

	/* 각 애니메이션당 각 채널들의 현재 키프레임인덱스 */
	vector<vector<_uint>>			m_KeyFrameIndices;

	_float							m_fCurrentAnimationSpeedRadio = { 1.f };

	_int							m_iCurrentFrame = { 0 };
	_int							m_iCurrentFrame_Boundary = { 0 };		//상하체 분리
	_bool* m_isEnd_Animations;
	_bool* m_isEnd_Animations_Boundary;			//상하체 분리
	_bool							m_isChangeAni = { false };	// 바꾸는중?
	_bool							m_isChangeAni_Boundary = { false };		//상하체 분리

	_bool							m_bPlayAnimCtr = { true };	// 애니메이션 정지, 재생


	CHANGEANIMATION_DESC			m_tChaneAnimDesc = {};
	CHANGEANIMATION_DESC			m_tChaneAnimDesc_Boundary = {};			//상하체 분리

	//이봉준 애니메이션
	_bool							m_isUseBoundary = { false };	//상하체 분리 여부
	_vector							m_vCurRootMove = {};			//이전에 사용한 루트본에의한 움직임


	// 정승현 모델 인스턴스
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
	D3D11_SUBRESOURCE_DATA		m_InstanceInitialData = {};

	_uint						m_iInstanceStride = { 0 };
	_uint						m_iNumInstance = { 0 };

	_bool						m_isInstance = {};

	vector<_Matrix>				m_InstanceDatas;
	void*						m_pInstanceVertices = { nullptr };

private:
	vector<_uint>					m_UFBIndices;
	vector<UFVTX>					m_UseFullVtxIndices;
	//바이너리화 용도
	FilePathStructStack* m_FilePaths = { nullptr };

public:
	HRESULT	Ready_Meshes(HANDLE* pFile);
	HRESULT Ready_Materials(HANDLE* pFile, const _char* pModelFilePath);
	HRESULT Ready_Bones(HANDLE* pFile, _int iParentBoneIndex);
	HRESULT Ready_Animations(HANDLE* pFile);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity(), _bool isBinaryAnimModel = false, FilePathStructStack* pStructStack = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END