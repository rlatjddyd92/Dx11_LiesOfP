#include "..\Public\VIBuffer_Point_Instance.h"

#include "GameInstance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing { pDevice, pContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance & Prototype)
	: CVIBuffer_Instancing { Prototype }
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient)
{
	m_isClient = isClient;

	if(true == m_isClient)
	{
#pragma region CLIENT
		if (FAILED(__super::Initialize_Desc(Desc)))
			return E_FAIL;

		m_iNumVertexBuffers = 2;
		m_iNumVertices = 1;
		m_iVertexStride = sizeof(VTXPOINT);
		m_iNumIndices = m_iNumInstance;
		m_iIndexStride = 2;
		m_eIndexFormat = DXGI_FORMAT_R16_UINT;
		m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		m_iInstanceStride = sizeof(VTXPOINTINSTANCE);
		m_iIndexCountPerInstance = 1;

#pragma region VERTEX_BUFFER
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;

		/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
		VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

		_float	fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);

		pVertices->vPosition = _float3(0.f, 0.f, 0.f);
		pVertices->vPSize = _float2(fScale, fScale);


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
		m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_InstanceBufferDesc.MiscFlags = 0;
		m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

		m_pInstanceVertices = new VTXPOINTINSTANCE[m_iNumInstance];
		ZeroMemory(m_pInstanceVertices, sizeof(VTXPOINTINSTANCE) * m_iNumInstance);

		VTXPOINTINSTANCE* pInstanceVertices = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices);

		for (size_t i = 0; i < m_iNumInstance; i++)
		{
			pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
			pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
			pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

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

			pInstanceVertices[i].vTranslation = _float4(fX, fY, fZ, 1.f);
			pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
			pInstanceVertices[i].vColor =
				_float4(m_pGameInstance->Get_Random(m_vMinColor.x, m_vMaxColor.x),
					m_pGameInstance->Get_Random(m_vMinColor.y, m_vMaxColor.y),
					m_pGameInstance->Get_Random(m_vMinColor.z, m_vMaxColor.z),
					m_pGameInstance->Get_Random(m_vMinColor.w, m_vMaxColor.w));
		}

		ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
		m_InstanceInitialData.pSysMem = m_pInstanceVertices;

#pragma endregion

#pragma endregion
	}
	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void * pArg)
{
	if (false == m_isClient)
	{
#pragma region TOOL
		INSTANCE_DESC* pDesc = static_cast<INSTANCE_DESC*>(pArg);

		if (FAILED(__super::Initialize_Desc(*pDesc)))
			return E_FAIL;

		m_iNumVertexBuffers = 2;
		m_iNumVertices = 1;
		m_iVertexStride = sizeof(VTXPOINT);
		m_iNumIndices = m_iNumInstance;
		m_iIndexStride = 2;
		m_eIndexFormat = DXGI_FORMAT_R16_UINT;
		m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		m_iInstanceStride = sizeof(VTXPOINTINSTANCE);
		m_iIndexCountPerInstance = 1;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	_float	fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);

	pVertices->vPosition = _float3(0.f, 0.f, 0.f);
	pVertices->vPSize = _float2(fScale, fScale);


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
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
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXPOINTINSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXPOINTINSTANCE) * m_iNumInstance);

	VTXPOINTINSTANCE* pInstanceVertices = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

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

		pInstanceVertices[i].vTranslation = _float4(fX, fY, fZ, 1.f);
		pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
		pInstanceVertices[i].vColor =
			_float4(m_pGameInstance->Get_Random(m_vMinColor.x, m_vMaxColor.x),
				m_pGameInstance->Get_Random(m_vMinColor.y, m_vMaxColor.y),
				m_pGameInstance->Get_Random(m_vMinColor.z, m_vMaxColor.z),
				m_pGameInstance->Get_Random(m_vMinColor.w, m_vMaxColor.w));
	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;
#pragma endregion
#pragma endregion
	}

#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

#pragma endregion

	m_CurrentRandomDir.resize(m_iNumInstance);
	m_NextRandomDir.resize(m_iNumInstance);
	Set_NextRandomDir();

	return S_OK;
}

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

}
