#include "..\Public\Mesh.h"

#include "Model.h"
#include "Shader.h"
#include "Octree.h"
#include "GameInstance.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh & Prototype)
	: CVIBuffer{ Prototype }
	, m_pOctree{ Prototype.m_pOctree }
{
	Safe_AddRef(m_pOctree);
}

HRESULT CMesh::Initialize_Prototype(HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	ReadFile(*pFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	m_iNumVertexBuffers = 1;
	ReadFile(*pFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(*pFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(*pFile, &m_iNumFaces, sizeof(_uint), &dwByte, nullptr);
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT hr = pModel->Get_ModelType() == CModel::TYPE_NONANIM ? Ready_VertexBuffer_NonAnim(pFile, PreTransformMatrix) : Ready_VertexBuffer_Anim(pFile, pModel);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 동적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumFaces; i++)
	{
		ReadFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);
		ReadFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);
		ReadFile(*pFile, &m_pIndices[iNumIndices++], sizeof(_uint), &dwByte, nullptr);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	//Safe_Delete_Array(m_pIndices);

#pragma endregion

	_int iDepth = -1;
	vector<int> tmp = {};

	if (m_pVertices != nullptr)
		m_pOctree = COctree::Create(m_vMinPos, m_vMaxPos, m_pIndices, m_pVertices, m_iNumFaces, tmp, &iDepth);
	
	return S_OK;
}

HRESULT CMesh::Initialize_Prototype_To_Binary(HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix)
{
	_ulong dwByte = 0;

	ReadFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	ReadFile(*pFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	m_iNumVertexBuffers = 1;
	ReadFile(*pFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(*pFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(*pFile, &m_iNumFaces, sizeof(_uint), &dwByte, nullptr);
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	//HRESULT hr = pModel->Get_ModelType() == CModel::TYPE_NONANIM ? Ready_VertexBuffer_NonAnim(pFile, PreTransformMatrix) : Ready_VertexBuffer_Anim(pFile, pModel);
	//if (FAILED(hr))
	//	return E_FAIL;

	Ready_VertexBuffer_To_Binary(pFile);

#pragma endregion



#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		ReadFile(*pFile, &m_pIndices[i], sizeof(_uint), &dwByte, nullptr);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(const CModel* pModel, CShader * pShader, const _char * pConstantName)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * g_iMaxMeshBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{	
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * pModel->Get_BoneCombindTransformationMatrix(m_BoneIndices[i]));
	}	

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);	
}

_matrix CMesh::CalcMatrix_forVtxAnim(vector<class CBone*>& Bones, VTXANIMMESH VtxStruct)
{
	_float fWeightW = 1 - (VtxStruct.vBlendWeights.x + VtxStruct.vBlendWeights.y + VtxStruct.vBlendWeights.z);

	_matrix BoneMat =
		VtxStruct.vBlendWeights.x * (m_BoneMatrices[VtxStruct.vBlendIndices.x]) +
		VtxStruct.vBlendWeights.y * (m_BoneMatrices[VtxStruct.vBlendIndices.y]) +
		VtxStruct.vBlendWeights.z * (m_BoneMatrices[VtxStruct.vBlendIndices.z]) +
		fWeightW * (m_BoneMatrices[VtxStruct.vBlendIndices.w]);

	return BoneMat;
}

HRESULT CMesh::Create_BinaryFile(HANDLE* pFile)
{
	_ulong dwByte = 0;
	//이름 저장
	WriteFile(*pFile, &m_szName, MAX_PATH, &dwByte, nullptr);

	//머티리얼 인덱스 저장
	WriteFile(*pFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

	//버텍스 갯수 저장
	WriteFile(*pFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);

	//인덱스 갯수 저장
	WriteFile(*pFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);

	//면 수 저장
	WriteFile(*pFile, &m_iNumFaces, sizeof(_uint), &dwByte, nullptr);

	//정점 버퍼 저장
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		WriteFile(*pFile, &m_pAnimVertices[i], sizeof(VTXANIMMESH), &dwByte, nullptr);
	}

	//영향받는 뼈 갯수 저장
	WriteFile(*pFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

	for (size_t j = 0; j < m_iNumBones; ++j)
	{//오프셋 매트릭스 저장
		WriteFile(*pFile, &m_OffsetMatrices[j], sizeof(_float4x4), &dwByte, nullptr);
		
		string strBoneName;
		_char	BoneName[MAX_PATH] = {};
		
		strcpy_s(BoneName, m_BoneNaems[j].c_str());

		WriteFile(*pFile, &BoneName, MAX_PATH, &dwByte, nullptr);

		WriteFile(*pFile, &m_BoneIndices[j], sizeof(_int), &dwByte, nullptr);

	}

	for (_uint k = 0; k < m_iNumIndices; ++k)
	{//인덱스 버퍼 저장
		WriteFile(*pFile, &m_pIndices[k], sizeof(_uint), &dwByte, nullptr);
	}
	

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(HANDLE* pFile, _fmatrix PreTransformMatrix)
{
	_ulong dwByte = 0;
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	m_pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		ReadFile(*pFile, &m_pVertices[i], sizeof(VTXMESH), &dwByte, nullptr);

		XMStoreFloat3(&m_pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[i].vPosition), PreTransformMatrix));
		//XMStoreFloat3(&m_pVertices[i].vNormal, XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[i].vNormal), PreTransformMatrix));
		//XMStoreFloat3(&m_pVertices[i].vTangent, XMVector3TransformCoord(XMLoadFloat3(&m_pVertices[i].vTangent), PreTransformMatrix));

		//m_ConvexHullPoints.push_back(m_pVertices[i].vPosition);

		//모델 사이즈 계산
		CalculateBoundingBox_Mesh( m_pVertices[i].vPosition);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	//Select_InitPoint();

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(HANDLE* pFile, const CModel* pModel)
{
	_ulong dwByte = 0;
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		ReadFile(*pFile, &m_pAnimVertices[i], sizeof(VTXANIMMESH), &dwByte, nullptr);
	}

	ReadFile(*pFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

	m_OffsetMatrices.resize(m_iNumBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		ReadFile(*pFile, &m_OffsetMatrices[i], sizeof(_float4x4), &dwByte, nullptr);

		_uint iNumWeights = { 0 };
		ReadFile(*pFile, &iNumWeights, sizeof(_uint), &dwByte, nullptr);

		/* 내 모델이 들고 있는 전체 뼈들중에 이 메시가 사용하는 뼈와이름이 같은 뼈의 인덱스를 저장해둔다. */
		_char szBoneName[MAX_PATH];
		ReadFile(*pFile, &szBoneName, MAX_PATH, &dwByte, nullptr);

		string strBoneName = szBoneName;
		m_BoneNaems.emplace_back(strBoneName);
		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(szBoneName));

		/* 이 뼈는 몇개(mNumWeights) 정점에게 영향을 주는가? */
		for (size_t j = 0; j < iNumWeights; j++)
		{
			_uint iWeightIndex = { 0 };
			ReadFile(*pFile, &iWeightIndex, sizeof(_uint), &dwByte, nullptr);

			ReadFile(*pFile, &m_pAnimVertices[iWeightIndex].vBlendIndices, sizeof(XMUINT4), &dwByte, nullptr);
			ReadFile(*pFile, &m_pAnimVertices[iWeightIndex].vBlendWeights, sizeof(XMFLOAT4), &dwByte, nullptr);

		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		// 뼈가 없는 상황에서는 메쉬의 이름과 같은 뼈의 이름을 찾아 강제로 1로 늘려 넣어줌
		// Weight가 0이어도 fWeightW로 살려두었기에 행렬 곱에서는 상관없음
		// OffsetMatrix가 문제임 -> 항등으로 저장해버리기
		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(m_szName));

		_float4x4	OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pAnimVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_To_Binary(HANDLE* pFile)
{
	_ulong dwByte = 0;
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		ReadFile(*pFile, &m_pAnimVertices[i], sizeof(VTXANIMMESH), &dwByte, nullptr);
	}

	ReadFile(*pFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	
	m_OffsetMatrices.reserve(m_iNumBones);
	m_BoneNaems.reserve(m_iNumBones);
	m_BoneIndices.reserve(m_iNumBones);

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		_float4x4 OffsetMat{};
		ReadFile(*pFile, &OffsetMat, sizeof(_float4x4), &dwByte, nullptr);
		m_OffsetMatrices.emplace_back(OffsetMat);

		_char szBoneName[MAX_PATH];
		ReadFile(*pFile, &szBoneName, MAX_PATH, &dwByte, nullptr);

		string strBoneName = szBoneName;
		m_BoneNaems.emplace_back(strBoneName);

		_int iIndexBone = {};
		ReadFile(*pFile, &iIndexBone, sizeof(_int), &dwByte, nullptr);
		m_BoneIndices.emplace_back(iIndexBone);

	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pAnimVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	return S_OK;
}

void CMesh::Culling(CGameInstance* pGameInstance, _Matrix worldMatrix)
{
	if (m_pOctree != nullptr)
		if (m_pOctree->Get_haveChild() == false)
			return;

	pGameInstance->Transform_ToLocalSpace(worldMatrix);
	_uint			iNumIndices = { 0 };
	unordered_set<_int> pDrawTriIndexes = {};

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	if(m_pOctree != nullptr)
		m_pOctree->Culling(pGameInstance, m_pIndices, (_uint*)SubResource.pData, pDrawTriIndexes,  &iNumIndices);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;
}


void CMesh::CalculateBoundingBox_Mesh(const _Vec3& vVertexPos)
{
	m_vMinPos.x = min(m_vMinPos.x, vVertexPos.x);
	m_vMinPos.y = min(m_vMinPos.y, vVertexPos.y);
	m_vMinPos.z = min(m_vMinPos.z, vVertexPos.z);

	m_vMaxPos.x = max(m_vMaxPos.x, vVertexPos.x);
	m_vMaxPos.y = max(m_vMaxPos.y, vVertexPos.y);
	m_vMaxPos.z = max(m_vMaxPos.z, vVertexPos.z);
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFile, pModel, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Create_To_Binary(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE* pFile, const CModel* pModel, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype_To_Binary(pFile, pModel, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMesh::Clone(void * pArg)
{
	CMesh*		pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	Safe_Release(m_pOctree);
	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pAnimVertices);
	Safe_Delete_Array(m_pIndices);
	Safe_Delete_Array(m_pWeightsCnts);
}
