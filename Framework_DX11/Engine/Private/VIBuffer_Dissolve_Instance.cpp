#include "..\Public\VIBuffer_Dissolve_Instance.h"

#include "GameInstance.h"

CVIBuffer_Dissolve_Instance::CVIBuffer_Dissolve_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Dissolve_Instance::CVIBuffer_Dissolve_Instance(const CVIBuffer_Dissolve_Instance& Prototype)
	: CVIBuffer_Instancing{ Prototype }
	, m_ParticleBuffer_Desc{ Prototype.m_ParticleBuffer_Desc }
	, m_MoveBuffer_Desc{ Prototype.m_MoveBuffer_Desc }
	, m_DissolveBuffer_Desc{ Prototype.m_DissolveBuffer_Desc }
	, m_UAV_Desc{ Prototype.m_UAV_Desc }
	, m_SRV_Desc{ Prototype.m_SRV_Desc }
	, m_InitParticleBuffer_Desc{ Prototype.m_InitParticleBuffer_Desc }
{
}

HRESULT CVIBuffer_Dissolve_Instance::Initialize_Prototype(const DISSOLVE_INSTANCE_DESC& Desc)
{
	m_isClient = true;

	CModel* pModel = static_cast<CModel*>(m_pGameInstance->Find_ComponentPrototype(Desc.iModelLevelIndex, Desc.strModelTag));

	vector<DISSOLVE_PARTICLE> Instances;
	if (FAILED(Ready_Buffers(pModel->Create_Particles(Desc, Instances))))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Dissolve_Instance::Initialize(void* pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_ParticleBuffer_Desc, &m_InstanceInitialData, &m_pParticleBuffer)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_MoveBuffer_Desc, nullptr, &m_pMovementBuffer)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_DissolveBuffer_Desc, nullptr, &m_pDissolveBuffer)))
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

HRESULT CVIBuffer_Dissolve_Instance::Bind_Buffer(CShader_NonVTX* pShader, const _char* pConstantName)
{
	//For_Debug();

	//// SRV 바인딩하고
	if (FAILED(pShader->Bind_SRV(pConstantName, m_pParticleSRV)))
		return E_FAIL;

	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	m_pContext->IASetInputLayout(nullptr);

	return S_OK;
}

HRESULT CVIBuffer_Dissolve_Instance::Render()
{
	m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);

	// UAV 해제하고
	ID3D11ShaderResourceView* nullSRV = { nullptr };
	m_pContext->VSSetShaderResources(0, 1, &nullSRV);

	return S_OK;
}

void CVIBuffer_Dissolve_Instance::Reset()
{
	m_bFirst = false;
	m_fTime = 0.f;
}

_bool CVIBuffer_Dissolve_Instance::DispatchCS(class CShader_Compute* pComputeShader, class CTexture* pTexture, class CModel* pModel, const PARTICLE_MOVEMENT& MovementData, const DISSOLVE_DATA& DissolveData)
{
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

	if (FAILED(m_pContext->Map(m_pDissolveBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return true;

	DISSOLVE_INITIAL* pDissolve = static_cast<DISSOLVE_INITIAL*>(mappedResource.pData);

	pDissolve->fThreshold = DissolveData.fThreshold;
	pDissolve->iModelType = pModel->Get_ModelType();
	pDissolve->vTextureSize = DissolveData.vTextureSize;
	memcpy_s(pDissolve->m_BoneMatrices, sizeof(_float4x4) * g_iMaxMeshBones, pModel->Get_ModelBoneMatrices(), sizeof(_float4x4) * g_iMaxMeshBones);
	//memcpy_s(&Test, sizeof(_float4x4) * g_iMaxMeshBones, pModel->Get_BoneMatrices(0), sizeof(_float4x4) * g_iMaxMeshBones);
	
	m_pContext->Unmap(m_pDissolveBuffer, 0);

	m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pParticleUAV, nullptr);
	m_pContext->CSSetShaderResources(0, 1, &m_pInitParticleSRV);

	if(nullptr != pTexture)
	{
		ID3D11ShaderResourceView* pSRV = pTexture->Get_SRV(0);
		m_pContext->CSSetShaderResources(1, 1, &pSRV);
	}

	m_pContext->CSSetConstantBuffers(0, 1, &m_pMovementBuffer);
	m_pContext->CSSetConstantBuffers(1, 1, &m_pDissolveBuffer);

	pComputeShader->Execute_ComputeShader((m_iNumInstance + 255) / 256, 1, 1);

	// UAV 해제하고
	ID3D11UnorderedAccessView* nullUAV[] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	// SRV 해제하고
	ID3D11ShaderResourceView* nullSRV[] = { nullptr, nullptr };
	m_pContext->CSSetShaderResources(0, 2, nullSRV);

	// 셰이더까지 해제하면 끝
	m_pContext->CSSetShader(nullptr, nullptr, 0);

	if (false == (STATE_LOOP & MovementData.iState) && DissolveData.fThreshold <= 0.f)
	{
		m_fTime += MovementData.fTimeDelta;
		if (m_vLifeTime.y < m_fTime)
			return true;
	}

	return false;
}

void CVIBuffer_Dissolve_Instance::ResetCS(CShader_Compute* pResetShader)
{
	m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pParticleUAV, nullptr);
	m_pContext->CSSetShaderResources(0, 1, &m_pInitParticleSRV);
	m_pContext->CSSetConstantBuffers(0, 1, &m_pMovementBuffer);
	m_pContext->CSSetConstantBuffers(1, 1, &m_pDissolveBuffer);

	pResetShader->Execute_ComputeShader((m_iNumInstance + 255) / 256, 1, 1);

	// UAV 해제하고
	ID3D11UnorderedAccessView* nullUAV[] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	// SRV 해제하고
	ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	m_pContext->CSSetShaderResources(0, 1, nullSRV);

	// 셰이더까지 해제하면 끝
	m_pContext->CSSetShader(nullptr, nullptr, 0);

	Reset();
}

HRESULT CVIBuffer_Dissolve_Instance::Ready_Buffers(const DISSOLVE_PARTICLE_DESC& Desc)
{
	m_iNumInstance = Desc.iNumInstance;
	m_vLifeTime.y = Desc.fMaxLifeTime;

	m_pInstanceVertices = new DISSOLVE_PARTICLE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(DISSOLVE_PARTICLE) * m_iNumInstance);

	DISSOLVE_PARTICLE* pParticle = static_cast<DISSOLVE_PARTICLE*>(m_pInstanceVertices);

	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		pParticle[i] = Desc.pParticles[i];
	}
	
	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;

	// 파티클 버퍼 구성 -> 파티클 하나마다 갖고있어야되는거
	m_ParticleBuffer_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_ParticleBuffer_Desc.ByteWidth = sizeof(DISSOLVE_PARTICLE) * m_iNumInstance;
	m_ParticleBuffer_Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	m_ParticleBuffer_Desc.StructureByteStride = sizeof(DISSOLVE_PARTICLE);
	m_ParticleBuffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	// Constant Buffer 구성
	m_MoveBuffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_MoveBuffer_Desc.ByteWidth = sizeof(PARTICLE_MOVEMENT);
	m_MoveBuffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_MoveBuffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	m_DissolveBuffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_DissolveBuffer_Desc.ByteWidth = sizeof(DISSOLVE_INITIAL);
	m_DissolveBuffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_DissolveBuffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

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
	m_InitParticleBuffer_Desc.ByteWidth = sizeof(DISSOLVE_PARTICLE) * m_iNumInstance;
	m_InitParticleBuffer_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	m_InitParticleBuffer_Desc.StructureByteStride = sizeof(DISSOLVE_PARTICLE);
	m_InitParticleBuffer_Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	return S_OK;
}

void CVIBuffer_Dissolve_Instance::For_Debug()
{
	// 스테이징 버퍼 (CPU에서 읽을 수 있는 버퍼)
	ID3D11Buffer* pStagingBuffer = nullptr;
	m_ParticleBuffer_Desc.Usage = D3D11_USAGE_STAGING;  // CPU 접근 가능
	m_ParticleBuffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;  // 읽기 가능
	m_ParticleBuffer_Desc.BindFlags = 0;  // GPU 바인딩 플래그는 사용하지 않음
	m_ParticleBuffer_Desc.MiscFlags = 0;  // MiscFlags도 사용하지 않음

	HRESULT hr = m_pDevice->CreateBuffer(&m_ParticleBuffer_Desc, nullptr, &pStagingBuffer);
	if (FAILED(hr)) {
		OutputDebugString(L"Failed to create staging buffer.\n");
	}

	if (nullptr != pStagingBuffer)
	{
		m_pContext->CopyResource(pStagingBuffer, m_pParticleBuffer);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hr = m_pContext->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);
		if (SUCCEEDED(hr)) {
			// 데이터 확인
			DISSOLVE_PARTICLE* pData = static_cast<DISSOLVE_PARTICLE*>(mappedResource.pData);
			for (_uint i = 0; i < m_iNumInstance; ++i)
			{
				_Vec4 vCheck = pData[i].Particle.vTranslation;
				//_float fWeightW = 1.f - (pData[i].vBlendWeights.x + pData[i].vBlendWeights.y + pData[i].vBlendWeights.z);

				//_Matrix BoneMatrix = Test[pData[i].vBlendIndices.x] * pData[i].vBlendWeights.x +
				//	Test[pData[i].vBlendIndices.y] * pData[i].vBlendWeights.y +
				//	Test[pData[i].vBlendIndices.z] * pData[i].vBlendWeights.z +
				//	Test[pData[i].vBlendIndices.w] * fWeightW;

				//vCheck = XMVector3TransformCoord(vCheck, BoneMatrix);

				if (1 == pData[i].isActive)
				{
					_uint b = 0;
				}

				_uint a = 0;
			}
			m_pContext->Unmap(pStagingBuffer, 0);
		}
	}
	Safe_Release(pStagingBuffer);
}

CVIBuffer_Dissolve_Instance* CVIBuffer_Dissolve_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const DISSOLVE_INSTANCE_DESC& Desc)
{
	CVIBuffer_Dissolve_Instance* pInstance = new CVIBuffer_Dissolve_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Dissolve_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Dissolve_Instance::Clone(void* pArg)
{
	CVIBuffer_Dissolve_Instance* pInstance = new CVIBuffer_Dissolve_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Dissolve_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Dissolve_Instance::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pParticleBuffer);
		Safe_Release(m_pMovementBuffer);
		Safe_Release(m_pDissolveBuffer);
		Safe_Release(m_pParticleUAV);
		Safe_Release(m_pParticleSRV);
		Safe_Release(m_pInitParticleBuffer);
		Safe_Release(m_pInitParticleSRV);
	}

}

