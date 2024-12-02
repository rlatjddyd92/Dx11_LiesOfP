#include "..\Public\VIBuffer_Point_Instance.h"

#include "GameInstance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing { pDevice, pContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance & Prototype)
	: CVIBuffer_Instancing { Prototype }
	, m_ParticleBuffer_Desc { Prototype.m_ParticleBuffer_Desc }
	, m_MoveBuffer_Desc { Prototype.m_MoveBuffer_Desc }
	, m_UAV_Desc { Prototype.m_UAV_Desc }
	, m_SRV_Desc { Prototype.m_SRV_Desc }
	, m_InitParticleBuffer_Desc{ Prototype.m_InitParticleBuffer_Desc }
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient)
{
	m_isClient = isClient;

	if(true == m_isClient)
	{
		if(FAILED(Ready_Buffers(Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void * pArg)
{
	if(false == m_isClient)
	{
		INSTANCE_DESC* pDesc = static_cast<INSTANCE_DESC*>(pArg);

		if(FAILED(Ready_Buffers(*pDesc)))
			return E_FAIL;
	}
	
	HRESULT hr = m_pDevice->CreateBuffer(&m_ParticleBuffer_Desc, &m_InstanceInitialData, &m_pParticleBuffer);
	int a = 0;

	if (FAILED(hr))
	{
		HRESULT reason = m_pDevice->GetDeviceRemovedReason();
		switch (reason) {
		case DXGI_ERROR_DEVICE_HUNG:
			// 디바이스가 응답을 중단했습니다.
			 a = 0;
			break;
		case DXGI_ERROR_DEVICE_RESET:
			// GPU가 재설정되었습니다.
			 a = 0;
			break;
		case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
			// 드라이버 내부 오류 발생.
			 a = 0;
			break;
		case DXGI_ERROR_DEVICE_REMOVED:
			// 디바이스가 제거됨.
			 a = 0;
			break;
		default:
			// 기타 오류.
			 a = 0;
			break;
		}
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateBuffer(&m_MoveBuffer_Desc, nullptr, &m_pMovementBuffer)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pParticleBuffer, &m_UAV_Desc, &m_pParticleUAV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pParticleBuffer, &m_SRV_Desc, &m_pParticleSRV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_InitParticleBuffer_Desc, &m_InstanceInitialData, &m_pInitParticleBuffer)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pInitParticleBuffer, &m_SRV_Desc, &m_pInitParticleSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Bind_Buffer(CShader_NonVTX* pShader, const _char* pConstantName)
{
	//// SRV 바인딩하고
	if (FAILED(pShader->Bind_SRV(pConstantName, m_pParticleSRV)))
		return E_FAIL;

	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pContext->IASetInputLayout(nullptr);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Render()
{
	m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);

	// UAV 해제하고
	ID3D11ShaderResourceView* nullSRV = { nullptr };
	m_pContext->VSSetShaderResources(0, 1, &nullSRV);

	return S_OK;
}

void CVIBuffer_Point_Instance::Reset()
{
	m_bFirst = false;
	m_fTime = 0.f;
}

_bool CVIBuffer_Point_Instance::DispatchCS(class CShader_Compute* pComputeShader, const PARTICLE_MOVEMENT& MovementData)
{
	/* 순서 중요 !! */
	// 상수 버퍼 업데이트 하고
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (FAILED(m_pContext->Map(m_pMovementBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return true;

	PARTICLE_MOVEMENT* pMovement = static_cast<PARTICLE_MOVEMENT*>(mappedResource.pData);
	*pMovement = MovementData;
	(*pMovement).iNumInstance = m_iNumInstance;

	if (false == m_bFirst)
	{
		pMovement->vPadding_1 = { 0.f, 0.f };
		m_bFirst = true;
	}
	else
		pMovement->vPadding_1 = { 1.f, 1.f };

	m_pContext->Unmap(m_pMovementBuffer, 0);

	m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pParticleUAV, nullptr);
	m_pContext->CSSetShaderResources(0, 1, &m_pInitParticleSRV);
	m_pContext->CSSetConstantBuffers(0, 1, &m_pMovementBuffer);

	pComputeShader->Execute_ComputeShader((m_iNumInstance + 255) / 256, 1, 1);

	// UAV 해제하고
	ID3D11UnorderedAccessView* nullUAV[] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	// SRV 해제하고
	ID3D11ShaderResourceView* nullSRV = { nullptr };
	m_pContext->CSSetShaderResources(0, 1, &nullSRV);

	// 셰이더까지 해제하면 끝
	m_pContext->CSSetShader(nullptr, nullptr, 0);

	if (false == (STATE_LOOP & MovementData.iState))
	{
		m_fTime += MovementData.fTimeDelta;
		if (m_vLifeTime.y < m_fTime)
			return true;
	}

	return false;
}


HRESULT CVIBuffer_Point_Instance::Ready_Buffers(const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	if (FAILED(Initialize_Desc(Desc)))
		return E_FAIL;

	m_pInstanceVertices = new PARTICLE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(PARTICLE) * m_iNumInstance);

	PARTICLE* pParticle = static_cast<PARTICLE*>(m_pInstanceVertices);

	Init_Particle(pParticle);

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;

	// 파티클 버퍼 구성 -> 파티클 하나마다 갖고있어야되는거
	m_ParticleBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_ParticleBuffer_Desc.ByteWidth = sizeof(PARTICLE) * m_iNumInstance;
	m_ParticleBuffer_Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	m_ParticleBuffer_Desc.StructureByteStride = sizeof(PARTICLE);
	m_ParticleBuffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	// Constant Buffer 구성
	m_MoveBuffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_MoveBuffer_Desc.ByteWidth = sizeof(PARTICLE_MOVEMENT);
	m_MoveBuffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_MoveBuffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// UAV 구성
	m_UAV_Desc.Format = DXGI_FORMAT_UNKNOWN;
	m_UAV_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	m_UAV_Desc.Buffer.NumElements = m_iNumInstance;

	// SRV 구성
	m_SRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
	m_SRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	m_SRV_Desc.Buffer.NumElements = m_iNumInstance;

	// 파티클 버퍼 구성
	m_InitParticleBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_InitParticleBuffer_Desc.ByteWidth = sizeof(PARTICLE) * m_iNumInstance;
	m_InitParticleBuffer_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	m_InitParticleBuffer_Desc.StructureByteStride = sizeof(PARTICLE);
	m_InitParticleBuffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	return S_OK;
}

CVIBuffer_Point_Instance * CVIBuffer_Point_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient)
{
	CVIBuffer_Point_Instance*		pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc, isClient)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Point_Instance::Clone(void * pArg)
{
	CVIBuffer_Point_Instance*		pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pParticleBuffer);
		Safe_Release(m_pMovementBuffer);
		Safe_Release(m_pParticleUAV);
		Safe_Release(m_pParticleSRV);
		Safe_Release(m_pInitParticleBuffer);
		Safe_Release(m_pInitParticleSRV);
	}

}

