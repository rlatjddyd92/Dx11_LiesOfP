#include "Trail_OnePoint_Instance.h"

#include "GameInstance.h"

CTrail_OnePoint_Instance::CTrail_OnePoint_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing( pDevice, pContext )
{
}

CTrail_OnePoint_Instance::CTrail_OnePoint_Instance(const CTrail_OnePoint_Instance& Prototype)
	: CVIBuffer_Instancing( Prototype )
	, m_TrailPoses( Prototype.m_TrailPoses )
{
}

HRESULT CTrail_OnePoint_Instance::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient)
{
	m_isClient = isClient;

	if(true == m_isClient)
	{
		if (FAILED(Ready_Buffers(Desc)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CTrail_OnePoint_Instance::Initialize(void* pArg)
{
	if (false == m_isClient)
	{
		INSTANCE_DESC* pDesc = static_cast<INSTANCE_DESC*>(pArg);

		if (FAILED(Ready_Buffers(*pDesc)))
			return E_FAIL;

	}

#pragma region INSTANCE_BUFFER
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

_bool CTrail_OnePoint_Instance::Update_Buffer(_fvector vWorldPos, _float fTimeDelta)
{
	if (false == m_bFirst)
	{
		for (auto& vPos : m_TrailPoses)
			XMStoreFloat3(&vPos, vWorldPos);

		m_bFirst = true;
	}

	_float3 vPos = {};
	XMStoreFloat3(&vPos, vWorldPos);
	m_TrailPoses.emplace_back(vPos);

	if (m_TrailPoses.size() > m_iNumInstance)
		m_TrailPoses.pop_front();

	D3D11_MAPPED_SUBRESOURCE	SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXTRAIL_ONEPOINT_INSTANCE* pVertices = static_cast<VTXTRAIL_ONEPOINT_INSTANCE*>(SubResource.pData);

	auto& iter = m_TrailPoses.begin();
	_bool m_bOver = { true };
	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		pVertices[i].vCurPos = *iter;
		if (iter == m_TrailPoses.begin())
		{
			pVertices[i].vPrePos = pVertices[i].vCurPos;
		}
		else
		{
			pVertices[i].vPrePos = pVertices[i - 1].vCurPos;
		}
		++iter;

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			m_bOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	return m_bOver;
}

void CTrail_OnePoint_Instance::Set_LifeTime(_float fTime)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXTRAIL_ONEPOINT_INSTANCE* pVertices = static_cast<VTXTRAIL_ONEPOINT_INSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		pVertices[i].vLifeTime.x = fTime;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CTrail_OnePoint_Instance::Reset()
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXTRAIL_ONEPOINT_INSTANCE* pVertices = static_cast<VTXTRAIL_ONEPOINT_INSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		pVertices[i] = static_cast<VTXTRAIL_ONEPOINT_INSTANCE*>(m_pInstanceVertices)[i];
	}
	m_pContext->Unmap(m_pVBInstance, 0);

	m_bFirst = false;
}

HRESULT CTrail_OnePoint_Instance::Ready_Buffers(const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	if (FAILED(__super::Initialize_Desc(Desc)))
		return E_FAIL;

	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);
	m_iNumIndices = m_iNumInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iInstanceStride = sizeof(VTXTRAIL_ONEPOINT_INSTANCE);
	m_iIndexCountPerInstance = 1;

	m_TrailPoses.resize(m_iNumInstance);

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	pVertices->vPosition = _float3(0.f, 0.f, 0.f);


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

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 정적버퍼로 생성한다. */
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXTRAIL_ONEPOINT_INSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXTRAIL_ONEPOINT_INSTANCE) * m_iNumInstance);

	VTXTRAIL_ONEPOINT_INSTANCE* pInstanceVertices = static_cast<VTXTRAIL_ONEPOINT_INSTANCE*>(m_pInstanceVertices);

	auto& iter = m_TrailPoses.begin();
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pInstanceVertices[i].vCurPos = {};
		pInstanceVertices[i].vPrePos = {};
		pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(Desc.vLifeTime.x, Desc.vLifeTime.y), 0.f);
		*iter = {};
		++iter;
	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;

#pragma endregion


	return S_OK;
}

CTrail_OnePoint_Instance* CTrail_OnePoint_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient)
{
	CTrail_OnePoint_Instance* pInstance = new CTrail_OnePoint_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc, isClient)))
	{
		MSG_BOX(TEXT("Failed to Created : CTrail_OnePoint_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CTrail_OnePoint_Instance::Clone(void* pArg)
{
	CTrail_OnePoint_Instance* pInstance = new CTrail_OnePoint_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTrail_OnePoint_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_OnePoint_Instance::Free()
{
	__super::Free();

}
