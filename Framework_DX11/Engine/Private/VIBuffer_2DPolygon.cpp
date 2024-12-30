#include "..\Public\VIBuffer_2DPolygon.h"

CVIBuffer_2DPolygon::CVIBuffer_2DPolygon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_2DPolygon::CVIBuffer_2DPolygon(const CVIBuffer_2DPolygon& Prototype)
	: CVIBuffer{ Prototype }
{
	for (auto& iter : Prototype.m_vecOrigin_Vertex)
		m_vecOrigin_Vertex.push_back(iter);
}

HRESULT CVIBuffer_2DPolygon::Initialize_Prototype(const UIPOLIGON_DESC& Desc)
{
	m_iPoint = Desc.iPoint;
	m_fTurn = Desc.fAngle;

	if (m_iPoint < 3)
		return E_FAIL;

	

	return S_OK;
}

HRESULT CVIBuffer_2DPolygon::Initialize(void* pArg)
{
	CVIBuffer_2DPolygon::UIPOLIGON_DESC* pDesc = static_cast<UIPOLIGON_DESC*>(pArg);

	m_iPoint = pDesc->iPoint;
	m_fTurn = pDesc->fAngle;



 	m_iNumVertexBuffers = 1;
	m_iNumVertices = m_iPoint + 2;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumIndices = m_iPoint * 3;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 동적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	_float fInterval_Angle = _float(360.f) / _float(m_iPoint);

	_Vec4 vVertices = Make_Vertices(m_fTurn);

	pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);
	pVertices[0].vTexcoord = _float2(0.5f, 0.5f);

	for (_uint i = 1; i < m_iNumVertices; ++i)
	{
		vVertices = Make_Vertices(m_fTurn + (fInterval_Angle * (i - 1)));

		pVertices[i].vPosition = _float3(vVertices.x, vVertices.y, 0.f);
		pVertices[i].vTexcoord = _float2(vVertices.z, vVertices.x);

		m_vecOrigin_Vertex.push_back(vVertices);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	for (_int i = 0; i < m_iPoint; ++i)
	{
		pIndices[(i * 3) + 0] = 0;
		pIndices[(i * 3) + 1] = i + 1;
		pIndices[(i * 3) + 2] = i + 2;
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

void CVIBuffer_2DPolygon::Set_Point_Ratio(_float fRatio, _int iIndex)
{
	if ((iIndex < 0) || (iIndex >= m_vecOrigin_Vertex.size()))
		return;

	if (fRatio < 0.f)
		fRatio = 0.f;

	if (fRatio > 1.f)
		fRatio = 1.f;

	D3D11_MAPPED_SUBRESOURCE	SubResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXPOSTEX* pVertices = static_cast<VTXPOSTEX*>(SubResource.pData);

	_Vec4 vNewVertex = m_vecOrigin_Vertex[iIndex] - _Vec4{ 0.f,0.f,0.5f,0.5f };
	vNewVertex *= fRatio;
	vNewVertex += _Vec4{ 0.f,0.f,0.5f,0.5f };

	pVertices[iIndex + 1].vPosition.x = vNewVertex.x;
	pVertices[iIndex + 1].vPosition.y = vNewVertex.y;
	pVertices[iIndex + 1].vTexcoord.x = vNewVertex.z;
	pVertices[iIndex + 1].vTexcoord.y = vNewVertex.w;

	if (iIndex == 0)
	{
		pVertices[m_vecOrigin_Vertex.size()].vPosition.x = vNewVertex.x;
		pVertices[m_vecOrigin_Vertex.size()].vPosition.y = vNewVertex.y;
		pVertices[m_vecOrigin_Vertex.size()].vTexcoord.x = vNewVertex.z;
		pVertices[m_vecOrigin_Vertex.size()].vTexcoord.y = vNewVertex.w;
	}

	m_pContext->Unmap(m_pVB, 0);
}

_Vec4 CVIBuffer_2DPolygon::Make_Vertices(_float fAngle)
{
	_Vec4 vResult = { 0.f,0.f,0.f,0.f };
	// _Vec4.
	/*
	x. Pos.x
	y. Pos.y
	z. Tex.x
	w. Tex.y

	순서 
	1. 일단 무조건 빗변의 길이를 1, 중심점은 (0,0), 기준 선분의 끝점은 (1,0)인 것으로 한다 
	2. Direct에 맞추기 위해 y는 반전시킨다 
	3. 먼저 각도에 맞춰 sin 함수로 빗변 끝점을 구한다 
	4. 빗변 끝점에 0.5f를 곱해 pos를 구한다 
	5. 구한 pos값에 0.5f를 더해 tex를 구한다 
	*/

	_float fRadian = XMConvertToRadians(fAngle);

	vResult.x = cos(fRadian) * 0.5f;
	vResult.y = -sin(fRadian) * 0.5f;
	vResult.z = vResult.x + 0.5f;
	vResult.w = vResult.y + 0.5f;

	return vResult;
}

CVIBuffer_2DPolygon* CVIBuffer_2DPolygon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UIPOLIGON_DESC& Desc)
{
	CVIBuffer_2DPolygon* pInstance = new CVIBuffer_2DPolygon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_2DPolygon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CVIBuffer_2DPolygon::Clone(void* pArg)
{
	CVIBuffer_2DPolygon* pInstance = new CVIBuffer_2DPolygon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_2DPolygon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_2DPolygon::Free()
{
	__super::Free();
	m_vecOrigin_Vertex.clear();
}
