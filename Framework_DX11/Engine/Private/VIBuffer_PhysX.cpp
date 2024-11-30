#include "VIBuffer_PhysX.h"

CVIBuffer_PhysX::CVIBuffer_PhysX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

HRESULT CVIBuffer_PhysX::Initialize(void* pArg)
{
	VIBUFFER_PHYSX_DESC* pDesc = static_cast<VIBUFFER_PHYSX_DESC*>(pArg);

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXCUBETEX);
	m_iNumVertices = 8;

	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = 36;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	_float3 vCubeSize = pDesc->vSize;

	VTXCUBETEX* pVertices = new VTXCUBETEX[m_iNumVertices]{};

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f) * vCubeSize;
	pVertices[0].vTexcoord = _float3(-0.5f, 0.5f, -0.5f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f) * vCubeSize;
	pVertices[1].vTexcoord = _float3(0.5f, 0.5f, -0.5f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f) * vCubeSize;
	pVertices[2].vTexcoord = _float3(0.5f, -0.5f, -0.5f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f) * vCubeSize;
	pVertices[3].vTexcoord = _float3(-0.5f, -0.5f, -0.5f);

	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f) * vCubeSize;
	pVertices[4].vTexcoord = _float3(-0.5f, 0.5f, 0.5f);

	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f) * vCubeSize;
	pVertices[5].vTexcoord = _float3(0.5f, 0.5f, 0.5f);

	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f) * vCubeSize;
	pVertices[6].vTexcoord = _float3(0.5f, -0.5f, 0.5f);

	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f) * vCubeSize;
	pVertices[7].vTexcoord = _float3(-0.5f, -0.5f, 0.5f);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), XMMatrixRotationQuaternion(pDesc->vOffSetQuat)) + XMLoadFloat3(&pDesc->vOffSetPosition));
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices]{};

	/* +X */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* -X */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;

	/* +Y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* -Y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* +Z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* -Z */
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

CVIBuffer_PhysX * CVIBuffer_PhysX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, VIBUFFER_PHYSX_DESC* pDesc)
{
	CVIBuffer_PhysX* pInstance = new CVIBuffer_PhysX(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_PhysX"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_PhysX::Clone(void* pArg)
{
	CVIBuffer_PhysX* pInstance = new CVIBuffer_PhysX(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_PhysX"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_PhysX::Free()
{
	__super::Free();
}
