#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* 모델 = 메시 + 메시 + 메시 .. */
/* 메시로 구분하는 이유 : 파츠의 교체를 용이하게 만들어주기 위해서.*/
/* 메시 = 정점버퍼 + 인덱스 */

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;	

public:
	_char*			Get_Name() { return m_szName; }

	_uint			Get_MaterialIndex() const { return m_iMaterialIndex; }


	VTXMESH*		Get_Vertices() { return m_pVertices; }
	VTXANIMMESH*	Get_AnimVertices() { return m_pAnimVertices; }
	_uint*			Get_Indices() { return m_pIndices; }

public:
	virtual HRESULT Initialize_Prototype(HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(const CModel* pModel, class CShader * pShader, const _char * pConstantName);
	_matrix		CalcMatrix_forVtxAnim(vector<class CBone*>&	Bones	,VTXANIMMESH VtxStruct);
	HRESULT		Create_BinaryFile(HANDLE* pFile);

private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumBones = { 0 };
	_uint				m_iNumFaces = {};

	/* uint : 모델에 로드해놓은 전체 뼈 중의 인덱스를 이야기한다. */
	vector<_int>		m_BoneIndices;
	vector<string>		m_BoneNaems;

	_float4x4			m_BoneMatrices[g_iMaxMeshBones] = {};
	vector<_float4x4>	m_OffsetMatrices;

	VTXMESH*			m_pVertices = { nullptr };
	VTXANIMMESH*		m_pAnimVertices = { nullptr };
	_uint*				m_pIndices = { nullptr };

	_int*				m_pWeightsCnts = { nullptr };

private:
	HRESULT	Ready_VertexBuffer_NonAnim(HANDLE* pFile, _fmatrix PreTransformMatrix);
	HRESULT	Ready_VertexBuffer_Anim(HANDLE* pFile, const CModel* pModel);


public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END