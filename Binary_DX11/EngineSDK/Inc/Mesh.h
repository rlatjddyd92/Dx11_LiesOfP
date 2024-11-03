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
	_uint			Get_MaterialIndex() { return m_iMaterialIndex; }
	VTXMESH*		Get_Vertices() { return m_pVertices; }
	_uint*			Get_Indices() { return m_pIndices; }

public:
	virtual HRESULT Initialize_Prototype(HANDLE* pFile, const CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool isPicking(const _fmatrix& WorldMatrix, _float3* pOut) override;

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = { 0 };
	_int				m_iNumBones = { 0 };

private:
	_uint				m_iNumFaces = {};
	VTXMESH*			m_pVertices = { nullptr };
	VTXANIMMESH*		m_pAnimVertices = { nullptr };
	_uint*				m_pIndices = { nullptr };

private:
	/* uint : 모델에 로드해놓은 전체 뼈 중의 인덱스를 이야기한다. */
	/* 얕은 복사로 뼈를 불러오면 모든 모델의 행동이 일치해버림 */
	vector<_int>		m_BoneIndices;

	_float4x4			m_BoneMatrices[g_iMaxMeshBones] = {};
	vector<_float4x4>	m_OffsetMatrices;

private:
	HRESULT	Ready_VertexBuffer_NonAnim(HANDLE* pFile, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT	Ready_VertexBuffer_Anim(HANDLE* pFile, const CModel* pModel, const aiMesh* pAIMesh);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE* pFile, const CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END