#include "..\Public\VIBuffer_Instancing.h"

#include "GameInstance.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing & Prototype)
	: CVIBuffer { Prototype }
	, m_InstanceBufferDesc{ Prototype.m_InstanceBufferDesc }
	, m_InstanceInitialData{ Prototype.m_InstanceInitialData }
	, m_iNumInstance{ Prototype.m_iNumInstance }
	, m_iIndexCountPerInstance{ Prototype.m_iIndexCountPerInstance }
	, m_iInstanceStride{ Prototype.m_iInstanceStride }
	, m_vCenterPos{ Prototype.m_vCenterPos }
	, m_vRange{ Prototype.m_vRange }
	, m_vExceptRange{ Prototype.m_vExceptRange }
	, m_pInstanceVertices{ Prototype.m_pInstanceVertices }
	, m_pSpeed{ Prototype.m_pSpeed }
	, m_vLifeTime{ Prototype.m_vLifeTime }
	, m_vMinColor{ Prototype.m_vMinColor }
	, m_vMaxColor{ Prototype.m_vMaxColor }
	, m_iNumRender { Prototype.m_iNumInstance }
	, m_isClient{ Prototype.m_isClient }
{
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype(const INSTANCE_DESC& Desc, _bool isClient)
{	
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void * pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_Buffers()
{
	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance, 
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride,
	};

	_uint		iOffsets[] = {
		0,
		0, 
	};

	

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumRender, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize_Desc(const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	m_iNumInstance = Desc.iNumInstance;
	m_vCenterPos = Desc.vCenter;
	m_vRange = Desc.vRange;
	m_vExceptRange = Desc.vExceptRange;
	m_vSize = Desc.vSize;
	m_vLifeTime = Desc.vLifeTime;
	m_vMinColor = Desc.vMinColor;
	m_vMaxColor = Desc.vMaxColor;
	m_vSpeed = Desc.vSpeed;

	if (m_vRange.x == m_vExceptRange.x && m_vRange.y == m_vExceptRange.y && m_vRange.z == m_vExceptRange.z)
		m_vExceptRange = _float3(0.f, 0.f, 0.f);


	return S_OK;
}

void CVIBuffer_Instancing::Reset()
{
}

_bool CVIBuffer_Instancing::Contain_State(_uint iCurrentState, _uint iCheckState)
{
	if (0 != (iCurrentState & iCheckState))
		return true;
	return false;
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

#pragma region CLIENT
	if (true == m_isClient && false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeed);
		Safe_Delete_Array(m_pInstanceVertices);
	}
#pragma endregion 

#pragma region TOOL
	if (false == m_isClient && true == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeed);
		Safe_Delete_Array(m_pInstanceVertices);
	}
#pragma endregion 

	Safe_Release(m_pVBInstance);
}

