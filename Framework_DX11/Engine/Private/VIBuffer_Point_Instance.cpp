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
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient)
{
	m_isClient = isClient;

	if(FAILED(Initialize_Desc(Desc)))
		return E_FAIL;

	if(true == m_isClient)
	{
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
		m_MoveBuffer_Desc.CPUAccessFlags = 0;

		// UAV 구성
		m_UAV_Desc.Format = DXGI_FORMAT_UNKNOWN;
		m_UAV_Desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		m_UAV_Desc.Buffer.NumElements = m_iNumInstance;


		// SRV 구성
		m_SRV_Desc.Format = DXGI_FORMAT_UNKNOWN;
		m_SRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		m_SRV_Desc.Buffer.NumElements = m_iNumInstance;
	}

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void * pArg)
{
	if(false == m_isClient)
	{
		INSTANCE_DESC* pDesc = static_cast<INSTANCE_DESC*>(pArg);
		if (FAILED(Initialize_Desc(*pDesc)))
			return E_FAIL;


		m_pInstanceVertices = new PARTICLE[m_iNumInstance];
		ZeroMemory(m_pInstanceVertices, sizeof(PARTICLE) * m_iNumInstance);

		PARTICLE* pParticle = static_cast<PARTICLE*>(m_pInstanceVertices);

		Init_Particle(pParticle);

		ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
		m_InstanceInitialData.pSysMem = m_pInstanceVertices;

		// 파티클 버퍼 구성
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
		
	}
	
	if (FAILED(m_pDevice->CreateBuffer(&m_ParticleBuffer_Desc, &m_InstanceInitialData, &m_pParticleBuffer)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_MoveBuffer_Desc, nullptr, &m_pMovementBuffer)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pParticleBuffer, &m_UAV_Desc, &m_pParticleUAV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pParticleBuffer, &m_SRV_Desc, &m_pParticleSRV)))
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

	return S_OK;
}

void CVIBuffer_Point_Instance::Spread_Test(class CShader_Compute* pComputeShader, const PARTICLE_MOVEMENT& MovementData)
{
	/* 순서 중요 !! */
	// 상수 버퍼 업데이트 하고
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(m_pContext->Map(m_pMovementBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return;
	PARTICLE_MOVEMENT* pMovement = static_cast<PARTICLE_MOVEMENT*>(mappedResource.pData);
	*pMovement = MovementData;

	m_pContext->Unmap(m_pMovementBuffer, 0);

	// 버퍼랑 UAV 설정하고
	pComputeShader->Bind_UAV(m_pParticleUAV);
	pComputeShader->Bind_CBuffer(m_pMovementBuffer);

	pComputeShader->Execute_ComputeShader((m_iNumInstance + 255) / 256, 1, 1);
}


#pragma region KEEP
_bool CVIBuffer_Point_Instance::Spread(_uint iState, _Matrix WorldMatrix, _float fRenderRatio, _Vec4 vPivot, _float fGravity, _float fTimeDelta, _Vec3 vRevolveAxis, _float fAngle, _float fTimeInterval, _float fRandomRatio, _float fAccelLimit, _float fAccelSpeed)
{
	if (1.f < fRenderRatio)
		fRenderRatio = 1.f;
	else if (fRenderRatio < 0.f)
		fRenderRatio = 0.f;
	m_iNumRender = (_uint)(m_iNumInstance * fRenderRatio);

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_bool isOver = { true };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float test = pVertices[i].vTranslation.x;
		_Vec4		vDir = pVertices[(_uint)i].vTranslation - vPivot;

		vDir = XMVector3Normalize(vDir);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == Contain_State(iState, STATE_RANDOM))
		{
			_float fTime = fmod(pVertices[i].vLifeTime.y, fTimeInterval);
			if (fTime < fTimeDelta)
			{
				m_CurrentRandomDir[i] = m_NextRandomDir[i];

				m_NextRandomDir[i].x = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].y = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].z = m_pGameInstance->Get_Random(-1.f, 1.f);

				XMStoreFloat3(&m_NextRandomDir[i], XMVector3Normalize(XMLoadFloat3(&m_NextRandomDir[i])));
			}

			vDir += Get_RandomDir((_uint)i, fTime / fTimeInterval) * fRandomRatio;
			vDir.Normalize();
		}

		_Vec4 vRotateDir = {};
		if (true == Contain_State(iState, STATE_ORBIT))
		{
			_Vec4 vTargetDir = vPivot - (pVertices[i].vTranslation);
			vRotateDir = Get_RevolveDir(vTargetDir, vRevolveAxis, fAngle, fTimeDelta);
		}

		_float fAddSpeed = 1.f;
		if (true == Contain_State(iState, STATE_ACCEL))
		{
			fAddSpeed *= pVertices[i].vLifeTime.y / pVertices->vLifeTime.x * fAccelSpeed;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}
		else if (true == Contain_State(iState, STATE_DECEL))
		{
			fAddSpeed *= 1.f - (pVertices[i].vLifeTime.y / pVertices->vLifeTime.x) * fAccelSpeed;
			if (fAddSpeed < 0.f)
				fAddSpeed = 0.f;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}

		_Vec4		vMoveDir = vDir * m_pSpeed[i];

		_Vec4		vGravityDir = _Vec4(0.f, -1.f, 0.f, 0.f);
		vGravityDir = XMVector3TransformNormal(vGravityDir, XMMatrixInverse(nullptr, WorldMatrix));
		vMoveDir += XMVector3Normalize(vGravityDir) * fGravity * pVertices[i].vLifeTime.y;

		pVertices[i].vTranslation = pVertices[i].vTranslation + (vMoveDir * fTimeDelta + vRotateDir) * fAddSpeed;
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));

		if (true == Contain_State(iState, STATE_LOOP) && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i] = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i];
			XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}

_bool CVIBuffer_Point_Instance::Move(_uint iState, _Matrix WorldMatrix, _float fRenderRatio, _Vec4 vDir, _float fGravity, _float fTimeDelta, _Vec4 vCenter, _Vec3 vRevolveAxis, _float fAngle, _float fTimeInterval, _float fRandomRatio, _float fAccelLimit, _float fAccelSpeed)
{
	if (1.f < fRenderRatio)
		fRenderRatio = 1.f;
	else if (fRenderRatio < 0.f)
		fRenderRatio = 0.f;
	m_iNumRender = (_uint)(m_iNumInstance * fRenderRatio);

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_bool isOver = { true };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_Vec4 vStartDir = vDir;
		vStartDir = XMVector3Normalize(vStartDir);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == Contain_State(iState, STATE_RANDOM))
		{
			_float fTime = fmod(pVertices[i].vLifeTime.y, fTimeInterval);
			if (fTime < fTimeDelta)
			{
				m_CurrentRandomDir[i] = m_NextRandomDir[i];

				m_NextRandomDir[i].x = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].y = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].z = m_pGameInstance->Get_Random(-1.f, 1.f);

				XMStoreFloat3(&m_NextRandomDir[i], XMVector3Normalize(XMLoadFloat3(&m_NextRandomDir[i])));
			}

			vDir += Get_RandomDir((_uint)i, fTime / fTimeInterval) * fRandomRatio;
			vStartDir.Normalize();
		}

		_Vec4 vRotateDir = {};
		if (true == Contain_State(iState, STATE_ORBIT))
		{
			_Vec4 vTargetDir = vCenter - (pVertices[i].vTranslation);
			vRotateDir = Get_RevolveDir(vTargetDir, vRevolveAxis, fAngle, fTimeDelta);
		}

		_float fAddSpeed = 1.f;
		if (true == Contain_State(iState, STATE_ACCEL))
		{
			fAddSpeed *= pVertices[i].vLifeTime.y / pVertices->vLifeTime.x * fAccelSpeed;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}
		else if (true == Contain_State(iState, STATE_DECEL))
		{
			fAddSpeed *= 1.f - (pVertices[i].vLifeTime.y / pVertices->vLifeTime.x) * fAccelSpeed;
			if (fAddSpeed < 0.f)
				fAddSpeed = 0.f;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}


		_Vec4		vMoveDir = vStartDir * m_pSpeed[i];
		_Vec4		vGravityDir = _Vec4(0.f, -1.f, 0.f, 0.f);
		vGravityDir = XMVector3TransformNormal(vGravityDir, XMMatrixInverse(nullptr, WorldMatrix));
		vMoveDir += XMVector3Normalize(vGravityDir) * fGravity * pVertices[i].vLifeTime.y;

		pVertices[i].vTranslation = pVertices[i].vTranslation + (vMoveDir * fTimeDelta + vRotateDir) * fAddSpeed;
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));

		if (true == Contain_State(iState, STATE_LOOP) && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i] = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i];
			XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}

_bool CVIBuffer_Point_Instance::Converge(_uint iState, _float fRenderRatio, _Vec4 vPivot, _float fTimeDelta, _Vec3 vRevolveAxis, _float fAngle, _float fTimeInterval, _float fRandomRatio, _float fAccelLimit, _float fAccelSpeed)
{
	if (1.f < fRenderRatio)
		fRenderRatio = 1.f;
	else if (fRenderRatio < 0.f)
		fRenderRatio = 0.f;
	m_iNumRender = (_uint)(m_iNumInstance * fRenderRatio);

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_bool isOver = { true };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_Vec4		vDir = vPivot - pVertices[i].vTranslation;
		vDir = XMVector3Normalize(vDir);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == Contain_State(iState, STATE_RANDOM))
		{
			_float fTime = fmod(pVertices[i].vLifeTime.y, fTimeInterval);
			if (fTime < fTimeDelta)
			{
				m_CurrentRandomDir[i] = m_NextRandomDir[i];

				m_NextRandomDir[i].x = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].y = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].z = m_pGameInstance->Get_Random(-1.f, 1.f);

				XMStoreFloat3(&m_NextRandomDir[i], XMVector3Normalize(XMLoadFloat3(&m_NextRandomDir[i])));
			}

			vDir += Get_RandomDir((_uint)i, fTime / fTimeInterval) * fRandomRatio;
			vDir.Normalize();
		}

		_Vec4 vRotateDir = {};
		if (true == Contain_State(iState, STATE_ORBIT))
		{
			_Vec4 vTargetDir = vPivot - (pVertices[i].vTranslation);
			vRotateDir = Get_RevolveDir(vTargetDir, vRevolveAxis, fAngle, fTimeDelta);
		}

		_float fAddSpeed = 1.f;
		if (true == Contain_State(iState, STATE_ACCEL))
		{
			fAddSpeed *= pVertices[i].vLifeTime.y / pVertices->vLifeTime.x * fAccelSpeed;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}
		else if (true == Contain_State(iState, STATE_DECEL))
		{
			fAddSpeed *= 1.f - (pVertices[i].vLifeTime.y / pVertices->vLifeTime.x) * fAccelSpeed;
			if (fAddSpeed < 0.f)
				fAddSpeed = 0.f;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}


		_Vec4		vMoveDir = vDir * m_pSpeed[i];

		pVertices[i].vTranslation = pVertices[i].vTranslation + (vMoveDir * fTimeDelta + vRotateDir) * fAddSpeed;
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));

		if (true == Contain_State(iState, STATE_LOOP) && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i] = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i];
			XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}

_bool CVIBuffer_Point_Instance::Spread_Independent(_uint iState, _Matrix WorldMatrix, _float fRenderRatio, _Vec4 vPivot, _float fGravity, _float fTimeDelta, _Vec3 vRevolveAxis, _float fAngle, _float fTimeInterval, _float fRandomRatio, _float fAccelLimit, _float fAccelSpeed)
{
	if (1.f < fRenderRatio)
		fRenderRatio = 1.f;
	else if (fRenderRatio < 0.f)
		fRenderRatio = 0.f;
	m_iNumRender = (_uint)(m_iNumInstance * fRenderRatio);

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_bool isOver = { true };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_Vec4 vWorldPivot = XMVector3TransformCoord(vPivot, WorldMatrix);

		if (false == m_bFirst)
		{
			XMStoreFloat4(&pVertices[i].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&pVertices[i].vTranslation), WorldMatrix));
			pVertices[i].vRight = vWorldPivot;
			pVertices[i].vUp = _Vec4(vRevolveAxis.x, vRevolveAxis.y, vRevolveAxis.z, 0.f);
		}

		_Vec4		vDir = pVertices[(_uint)i].vTranslation - vWorldPivot;
		vDir = XMVector3Normalize(vDir);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == Contain_State(iState, STATE_RANDOM))
		{
			_float fTime = fmod(pVertices[i].vLifeTime.y, fTimeInterval);
			if (fTime < fTimeDelta)
			{
				m_CurrentRandomDir[i] = m_NextRandomDir[i];

				m_NextRandomDir[i].x = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].y = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].z = m_pGameInstance->Get_Random(-1.f, 1.f);

				XMStoreFloat3(&m_NextRandomDir[i], XMVector3Normalize(XMLoadFloat3(&m_NextRandomDir[i])));
			}

			vDir += Get_RandomDir((_uint)i, fTime / fTimeInterval) * fRandomRatio;
			vDir.Normalize();
		}

		_Vec4 vRotateDir = {};
		if (true == Contain_State(iState, STATE_ORBIT))
		{
			_Vec4 vTargetDir = XMLoadFloat4(&pVertices[(_uint)i].vRight) -XMLoadFloat4(&pVertices[(_uint)i].vTranslation);
			_Vec3 Axis = _Vec3(pVertices[i].vUp.x, pVertices[i].vUp.y, pVertices[i].vUp.z);
			vRotateDir = Get_RevolveDir(vTargetDir, Axis, fAngle, fTimeDelta);
		}

		_float fAddSpeed = 1.f;
		if (true == Contain_State(iState, STATE_ACCEL))
		{
			fAddSpeed *= pVertices[i].vLifeTime.y / pVertices->vLifeTime.x * fAccelSpeed;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}
		else if (true == Contain_State(iState, STATE_DECEL))
		{
			fAddSpeed *= 1.f - (pVertices[i].vLifeTime.y / pVertices->vLifeTime.x) * fAccelSpeed;
			if (fAddSpeed < 0.f)
				fAddSpeed = 0.f;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}

		_Vec4		vMoveDir = vDir * m_pSpeed[i];
		vMoveDir.y -= fGravity * pVertices[i].vLifeTime.y;

		pVertices[i].vTranslation = pVertices[i].vTranslation + (vMoveDir * fTimeDelta + vRotateDir) * fAddSpeed;
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));

		if (true == Contain_State(iState, STATE_LOOP) && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			_Vec4 vTranslation = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			XMStoreFloat4(&pVertices[i].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&vTranslation), WorldMatrix));
		
			pVertices[i].vLifeTime.y = 0.f;
			pVertices[i].vRight = vWorldPivot;
			pVertices[i].vUp = _Vec4(vRevolveAxis.x, vRevolveAxis.y, vRevolveAxis.z, 0.f);
			XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));

		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (false == m_bFirst)
		m_bFirst = true;
	
	return isOver;
}

_bool CVIBuffer_Point_Instance::Move_Independent(_uint iState, _Matrix WorldMatrix, _float fRenderRatio, _Vec4 vDir, _float fGravity, _float fTimeDelta, _Vec4 vCenter, _Vec3 vRevolveAxis, _float fAngle, _float fTimeInterval, _float fRandomRatio, _float fAccelLimit, _float fAccelSpeed)
{
	if (1.f < fRenderRatio)
		fRenderRatio = 1.f;
	else if (fRenderRatio < 0.f)
		fRenderRatio = 0.f;
	m_iNumRender = (_uint)(m_iNumInstance * fRenderRatio);

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_bool isOver = { true };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_Vec4 vWorldPivot = XMVector3TransformCoord(vCenter, WorldMatrix);

		if (false == m_bFirst)
		{
			XMStoreFloat4(&pVertices[i].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&pVertices[i].vTranslation), WorldMatrix));
			pVertices[i].vRight = vWorldPivot;
			pVertices[i].vUp = _Vec4(vRevolveAxis.x, vRevolveAxis.y, vRevolveAxis.z, 0.f);
		}

		_Vec4 vStartDir = vDir;
		vStartDir = XMVector3Normalize(vStartDir);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == Contain_State(iState, STATE_RANDOM))
		{
			_float fTime = fmod(pVertices[i].vLifeTime.y, fTimeInterval);
			if (fTime < fTimeDelta)
			{
				m_CurrentRandomDir[i] = m_NextRandomDir[i];

				m_NextRandomDir[i].x = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].y = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].z = m_pGameInstance->Get_Random(-1.f, 1.f);

				XMStoreFloat3(&m_NextRandomDir[i], XMVector3Normalize(XMLoadFloat3(&m_NextRandomDir[i])));
			}

			vDir += Get_RandomDir((_uint)i, fTime / fTimeInterval) * fRandomRatio;
			vStartDir.Normalize();
		}

		_Vec4 vRotateDir = {};
		if (true == Contain_State(iState, STATE_ORBIT))
		{
			_Vec4 vTargetDir = XMLoadFloat4(&pVertices[(_uint)i].vRight) - XMLoadFloat4(&pVertices[(_uint)i].vTranslation);
			_Vec3 Axis = _Vec3(pVertices[i].vUp.x, pVertices[i].vUp.y, pVertices[i].vUp.z);
			vRotateDir = Get_RevolveDir(vTargetDir, Axis, fAngle, fTimeDelta);
		}

		_float fAddSpeed = 1.f;
		if (true == Contain_State(iState, STATE_ACCEL))
		{
			fAddSpeed *= pVertices[i].vLifeTime.y / pVertices->vLifeTime.x * fAccelSpeed;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}
		else if (true == Contain_State(iState, STATE_DECEL))
		{
			fAddSpeed *= 1.f - (pVertices[i].vLifeTime.y / pVertices->vLifeTime.x) * fAccelSpeed;
			if (fAddSpeed < 0.f)
				fAddSpeed = 0.f;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}

		_Vec4		vMoveDir = vStartDir * m_pSpeed[i];
		vMoveDir.y -= fGravity * pVertices[i].vLifeTime.y;

		pVertices[i].vTranslation = pVertices[i].vTranslation + (vMoveDir * fTimeDelta + vRotateDir) * fAddSpeed;
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));

		if (true == Contain_State(iState, STATE_LOOP) && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			_Vec4 vTranslation = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			XMStoreFloat4(&pVertices[i].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&vTranslation), WorldMatrix));

			pVertices[i].vLifeTime.y = 0.f;
			pVertices[i].vRight = vWorldPivot;
			pVertices[i].vUp = _Vec4(vRevolveAxis.x, vRevolveAxis.y, vRevolveAxis.z, 0.f);
			XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (false == m_bFirst)
		m_bFirst = true;

	return isOver;
}

_bool CVIBuffer_Point_Instance::Converge_Independent(_uint iState, _Matrix WorldMatrix, _float fRenderRatio, _Vec4 vPivot, _float fTimeDelta, _Vec3 vRevolveAxis, _float fAngle, _float fTimeInterval, _float fRandomRatio, _float fAccelLimit, _float fAccelSpeed)
{
	if (1.f < fRenderRatio)
		fRenderRatio = 1.f;
	else if (fRenderRatio < 0.f)
		fRenderRatio = 0.f;
	m_iNumRender = (_uint)(m_iNumInstance * fRenderRatio);

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	_bool isOver = { true };

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_Vec4 vWorldPivot = XMVector3TransformCoord(vPivot, WorldMatrix);

		if (false == m_bFirst)
		{
			XMStoreFloat4(&pVertices[i].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&pVertices[i].vTranslation), WorldMatrix));
			pVertices[i].vRight = vWorldPivot;
			pVertices[i].vUp = _Vec4(vRevolveAxis.x, vRevolveAxis.y, vRevolveAxis.z, 0.f);
		}

		_Vec4		vDir = vWorldPivot - pVertices[i].vTranslation;
		vDir = XMVector3Normalize(vDir);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == Contain_State(iState, STATE_RANDOM))
		{
			_float fTime = fmod(pVertices[i].vLifeTime.y, fTimeInterval);
			if (fTime < fTimeDelta)
			{
				m_CurrentRandomDir[i] = m_NextRandomDir[i];

				m_NextRandomDir[i].x = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].y = m_pGameInstance->Get_Random(-1.f, 1.f);
				m_NextRandomDir[i].z = m_pGameInstance->Get_Random(-1.f, 1.f);

				XMStoreFloat3(&m_NextRandomDir[i], XMVector3Normalize(XMLoadFloat3(&m_NextRandomDir[i])));
			}

			vDir += Get_RandomDir((_uint)i, fTime / fTimeInterval) * fRandomRatio;
			vDir.Normalize();
		}

		_Vec4 vRotateDir = {};
		if (true == Contain_State(iState, STATE_ORBIT))
		{
			_Vec4 vTargetDir = XMLoadFloat4(&pVertices[(_uint)i].vRight) - XMLoadFloat4(&pVertices[(_uint)i].vTranslation);
			_Vec3 Axis = _Vec3(pVertices[i].vUp.x, pVertices[i].vUp.y, pVertices[i].vUp.z);
			vRotateDir = Get_RevolveDir(vTargetDir, Axis, fAngle, fTimeDelta);
		}

		_float fAddSpeed = 1.f;
		if (true == Contain_State(iState, STATE_ACCEL))
		{
			fAddSpeed *= pVertices[i].vLifeTime.y / pVertices->vLifeTime.x * fAccelSpeed;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}
		else if (true == Contain_State(iState, STATE_DECEL))
		{
			fAddSpeed *= 1.f - (pVertices[i].vLifeTime.y / pVertices->vLifeTime.x) * fAccelSpeed;
			if (fAddSpeed < 0.f)
				fAddSpeed = 0.f;
			if (fAddSpeed < fAccelLimit)
				fAddSpeed = fAccelLimit;
		}

		_Vec4		vMoveDir = vDir * m_pSpeed[i];

		pVertices[i].vTranslation = pVertices[i].vTranslation + (vMoveDir * fTimeDelta + vRotateDir) * fAddSpeed;
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));

		if (true == Contain_State(iState, STATE_LOOP) && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			_Vec4 vTranslation = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			XMStoreFloat4(&pVertices[i].vTranslation, XMVector3TransformCoord(XMLoadFloat4(&vTranslation), WorldMatrix));

			pVertices[i].vLifeTime.y = 0.f;
			pVertices[i].vRight = vWorldPivot;
			pVertices[i].vUp = _Vec4(vRevolveAxis.x, vRevolveAxis.y, vRevolveAxis.z, 0.f);
			XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vMoveDir * fTimeDelta + vRotateDir));
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (false == m_bFirst)
		m_bFirst = true;

	return isOver;
}

void CVIBuffer_Point_Instance::Reset()
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i] = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i];
	}
	m_bFirst = false;
	m_pContext->Unmap(m_pVBInstance, 0);
}
#pragma endregion

void CVIBuffer_Point_Instance::Set_NextRandomDir()
{
	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		m_CurrentRandomDir[i] = m_NextRandomDir[i];

		m_NextRandomDir[i].x = m_pGameInstance->Get_Random(-1.f, 1.f);
		m_NextRandomDir[i].y = m_pGameInstance->Get_Random(-1.f, 1.f);
		m_NextRandomDir[i].z = m_pGameInstance->Get_Random(-1.f, 1.f);

		XMStoreFloat3(&m_NextRandomDir[i], XMVector3Normalize(XMLoadFloat3(&m_NextRandomDir[i])));
	}
}

_Vec4 CVIBuffer_Point_Instance::Get_RandomDir(_uint iIndex, _float fTime)
{
	return XMVectorLerp(XMLoadFloat3(&m_CurrentRandomDir[iIndex]), XMLoadFloat3(&m_NextRandomDir[iIndex]), fTime);
}

_Vec4 CVIBuffer_Point_Instance::Get_RevolveDir(_Vec4 vTargetDir, _Vec3 vRevolveAxis, _float fAngle, _float fTimeDelta)
{
	if (vRevolveAxis == _Vec3(0.f, 0.f, 0.f))
		return _Vec4(0.f, 0.f, 0.f, 0.f);

	_Quaternion quaternion = Quaternion::CreateFromAxisAngle(vRevolveAxis, fAngle * fTimeDelta); // 축과 각도로 쿼터니언 생성
	_Matrix RotationMatrix = Matrix::CreateFromQuaternion(quaternion);
	_vector	vRotate = XMVector3TransformNormal(vTargetDir, RotationMatrix);
	_Vec4 vResultDir = _Vec4(vRotate) - vTargetDir;

	return vResultDir;
}

void CVIBuffer_Point_Instance::Init_Particle(PARTICLE* pParticles)
{
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pParticles[i].vPosition = _float3(0.f, 0.f, 0.f);

		_float fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);
		pParticles[i].vPSize = _float2(fScale, fScale);

		pParticles[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pParticles[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pParticles[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

		pParticles[i].vTranslation = Get_RandomTranslation();
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

_float4 CVIBuffer_Point_Instance::Get_RandomTranslation()
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

	if (true == m_isClient)
	{
		if (false == m_isCloned)
		{
			Safe_Release(m_pParticleBuffer);
			Safe_Release(m_pMovementBuffer);
			Safe_Release(m_pParticleUAV);
			Safe_Release(m_pParticleSRV);
		}
	}
	else
	{
		if (true == m_isCloned)
		{
			Safe_Release(m_pParticleBuffer);
			Safe_Release(m_pMovementBuffer);
			Safe_Release(m_pParticleUAV);
			Safe_Release(m_pParticleSRV);
		}
	}

}
