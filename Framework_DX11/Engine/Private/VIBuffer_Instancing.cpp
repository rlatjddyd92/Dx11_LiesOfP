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
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

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

void CVIBuffer_Instancing::Init_Particle(PARTICLE* pParticles)
{
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pParticles[i].vPosition = _float3(0.f, 0.f, 0.f);

		_float fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);
		pParticles[i].vPSize = _float2(fScale, fScale);

		pParticles[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pParticles[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pParticles[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

		pParticles[i].vTranslation = Get_ExceptedTranslation();
		pParticles[i].vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
		pParticles[i].vColor = _float4(m_pGameInstance->Get_Random(m_vMinColor.x, m_vMaxColor.x),
			m_pGameInstance->Get_Random(m_vMinColor.y, m_vMaxColor.y),
			m_pGameInstance->Get_Random(m_vMinColor.z, m_vMaxColor.z),
			m_pGameInstance->Get_Random(m_vMinColor.w, m_vMaxColor.w));

		pParticles[i].fSpeed = m_pGameInstance->Get_Random(m_vSpeed.x, m_vSpeed.y);

		pParticles[i].vCurrenrRandomDir = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);
		pParticles[i].vNextRandomDir = _float4(m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), m_pGameInstance->Get_Random(-1.f, 1.f), 0.f);

	}
}

_float4 CVIBuffer_Instancing::Get_ExceptedTranslation()
{
	_float fX_Small{}, fX_Big{}, fY_Small{}, fY_Big{}, fZ_Small{}, fZ_Big{};

	_float fX{}, fY{}, fZ{};

	while (true)
	{
		_float fRandomNum = m_pGameInstance->Get_Random_Normal();
		if (fRandomNum < 0.333f)
		{
			if (m_vRange.x == m_vExceptRange.x)
				continue;

			fX_Small = m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x - m_vExceptRange.x * 0.5f);
			fX_Big = m_pGameInstance->Get_Random(m_vCenterPos.x + m_vExceptRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f);

			if (0.5f < m_pGameInstance->Get_Random_Normal())
				fX = fX_Small;
			else
				fX = fX_Big;
			fY = m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f);
			fZ = m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f);
		}
		else if (0.333f <= fRandomNum && fRandomNum < 0.666f)
		{
			if (m_vRange.y == m_vExceptRange.y)
				continue;

			fY_Small = m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y - m_vExceptRange.y * 0.5f);
			fY_Big = m_pGameInstance->Get_Random(m_vCenterPos.y + m_vExceptRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f);

			if (0.5f < m_pGameInstance->Get_Random_Normal())
				fY = fY_Small;
			else
				fY = fY_Big;

			fX = m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f);
			fZ = m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f);
		}
		else
		{
			if (m_vRange.z == m_vExceptRange.z)
				continue;

			fZ_Small = m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z - m_vExceptRange.z * 0.5f);
			fZ_Big = m_pGameInstance->Get_Random(m_vCenterPos.z + m_vExceptRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f);

			if (0.5f < m_pGameInstance->Get_Random_Normal())
				fZ = fZ_Small;
			else
				fZ = fZ_Big;

			fX = m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f);
			fY = m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f);
		}

		break;
	}

	return _float4(fX, fY, fZ, 1.f);
}

HRESULT CVIBuffer_Instancing::Ready_Buffers(const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	return S_OK;
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

