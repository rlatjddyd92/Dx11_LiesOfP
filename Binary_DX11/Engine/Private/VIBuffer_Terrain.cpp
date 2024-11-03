#include "..\Public\VIBuffer_Terrain.h"
#include "GameInstance.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	: CVIBuffer{ Prototype }
	, m_iNumVerticesX{ Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ{ Prototype.m_iNumVerticesZ }
	,m_bCreate{ Prototype.m_bCreate }

{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
#pragma region HeightMap 읽어오기
	_ulong		dwByte = {};

	// HeightMap 열기
	HANDLE		hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	// bmp 파일 구조를 나타냄
	// BITMAPFILEHEADER -> BITMAPINFOHEADER -> Pixel 순서로 읽어와야함
	BITMAPFILEHEADER			fh{};
	BITMAPINFOHEADER			ih{};

	ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);
	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	_uint* pPixel = new _uint[m_iNumVerticesX * m_iNumVerticesZ];
	ReadFile(hFile, pPixel, sizeof(_uint) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);

	CloseHandle(hFile);
#pragma endregion


	m_iNumVertexBuffers = 1;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;		// 정점이 2*2면 사각형이 한 개 -> 인덱스는 6개 형식으로 계산
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 정점들의 위치를 저장해주기 위함
	m_pVerticesPos = new _float3[m_iNumVertices];

#pragma region VERTEX_BUFFER
	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint			iIndex = _uint(i * m_iNumVerticesX + j);

			// y 즉 Height값은 픽셀의 RGB에 0x000000ff를 &연산을 통해서 B값만 남김 -> 제일 편해서
			// 10을 나누어서 높이를 어느정도 낮춤
			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(_float(j), (pPixel[iIndex] & 0x000000ff) / 10.0f, _float(i));
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);	// 법선 벡터는 나중에 계산함
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}
#pragma endregion

#pragma region INDEX_BUFFER

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint			iIndex = _uint(i * m_iNumVerticesX + j);

			_uint			iIndices[] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			// 삼각형의 두 변을 나타내는 벡터와 두 변에 대한 면 법선 벡터
			_vector		vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			// 삼각형의 면 법선을 구하기 위해서는 삼각형 두 변에 놓인 벡터를 구한 후 외적을 해야한다
			// 첫 번째 삼각형의 두 변의 벡터를 구한 후 외적 -> 면 법선을 구함
			vSourDir = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			// 구한 Normal을 vNormal에 더해줌 (누적)
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			// 두 번째 삼각형의 두 변의 벡터를 구한 후 외적 -> 면 법선을 구함
			vSourDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			// 구한 Normal을 vNormal에 더해줌 (누적)
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
		}
	}

	// 누적된 vNormal 값을 Normalize하며 정규화 시킴
	for (size_t i = 0; i < m_iNumVertices; i++)
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
#pragma endregion

	// vertex와 index에 관한 내용을 다 채워주고 나서 구조체에 대한 정보를 채워주어야함
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pPixel);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	m_bCreate = true;

	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	if (m_bCreate)
	{
		if (nullptr != pArg)
		{
			memcpy(&m_tVIBuffer_Terrain_Desc, pArg, sizeof(VIBUFFER_TERRAIN_DESC));

			m_iNumVerticesX = m_tVIBuffer_Terrain_Desc.iNumVerticesX;
			m_iNumVerticesZ = m_tVIBuffer_Terrain_Desc.iNumVerticesZ;
		}

		m_iNumVertexBuffers = 1;
		m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
		m_iVertexStride = sizeof(VTXNORTEX);
		m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
		m_iIndexStride = 4;
		m_eIndexFormat = DXGI_FORMAT_R32_UINT;
		m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// 정점들의 위치를 저장해주기 위함
		m_pVerticesPos = new _float3[m_iNumVertices];

#pragma region VERTEX_BUFFER
		/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
		VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

		for (size_t i = 0; i < m_iNumVerticesZ; i++)
		{
			for (size_t j = 0; j < m_iNumVerticesX; j++)
			{
				_uint			iIndex = _uint(i * m_iNumVerticesX + j);

				// y 즉 Height값은 픽셀의 RGB에 0x000000ff를 &연산을 통해서 B값만 남김 -> 제일 편해서
				// 10을 나누어서 높이를 어느정도 낮춤
				pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(_float(j), 0.f, _float(i));
				pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);	// 법선 벡터는 나중에 계산함
				pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			}
		}
#pragma endregion

#pragma region INDEX_BUFFER

		_uint* pIndices = new _uint[m_iNumIndices];
		ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

		_uint		iNumIndices = { 0 };

		for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
		{
			for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
			{
				_uint			iIndex = _uint(i * m_iNumVerticesX + j);

				_uint			iIndices[] = {
					iIndex + m_iNumVerticesX,
					iIndex + m_iNumVerticesX + 1,
					iIndex + 1,
					iIndex
				};

				// 삼각형의 두 변을 나타내는 벡터와 두 변에 대한 면 법선 벡터
				_vector		vSourDir, vDestDir, vNormal;

				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[1];
				pIndices[iNumIndices++] = iIndices[2];

				// 삼각형의 면 법선을 구하기 위해서는 삼각형 두 변에 놓인 벡터를 구한 후 외적을 해야한다
				// 첫 번째 삼각형의 두 변의 벡터를 구한 후 외적 -> 면 법선을 구함
				vSourDir = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
				vDestDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

				// 구한 Normal을 vNormal에 더해줌 (누적)
				XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[2];
				pIndices[iNumIndices++] = iIndices[3];

				// 두 번째 삼각형의 두 변의 벡터를 구한 후 외적 -> 면 법선을 구함
				vSourDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
				vDestDir = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

				// 구한 Normal을 vNormal에 더해줌 (누적)
				XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
			}
		}

		// 누적된 vNormal 값을 Normalize하며 정규화 시킴
		for (size_t i = 0; i < m_iNumVertices; i++)
			XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
#pragma endregion

		// vertex와 index에 관한 내용을 다 채워주고 나서 구조체에 대한 정보를 채워주어야함
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;


		//m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 동적버퍼로 생성한다. */
		//m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		//m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
	

		ZeroMemory(&m_InitialData, sizeof m_InitialData);
		m_InitialData.pSysMem = pVertices;

		/* 정점버퍼를 생성한다. */
		if (FAILED(__super::Create_Buffer(&m_pVB)))
			return E_FAIL;

		/* 인덱스버퍼의 내용을 채워주곡 */
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iIndexStride;

		ZeroMemory(&m_InitialData, sizeof m_InitialData);
		m_InitialData.pSysMem = pIndices;

		/* 인덱스버퍼를 생성한다. */
		if (FAILED(__super::Create_Buffer(&m_pIB)))
			return E_FAIL;

		Safe_Delete_Array(pIndices);
		Safe_Delete_Array(pVertices);
	}
	return S_OK;
}

_bool CVIBuffer_Terrain::isPicking(const _fmatrix& WorldMatrix, _float3* pOut)
{
	/* 마우스 레이와 레이포스를 로컬로 던졌다. */
	m_pGameInstance->Transform_MouseRay_ToLocalSpace(WorldMatrix);

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = _uint(i * m_iNumVerticesX + j);

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX, /* 왼위*/
				iIndex + m_iNumVerticesX + 1, /* 오위 */
				iIndex + 1, /* 오아 */
				iIndex /* 왼아 */
			};

			/* 오른쪽 위 삼각형 */
			if (true == m_pGameInstance->isPicked_InLocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
				XMLoadFloat3(&m_pVerticesPos[iIndices[1]]),
				XMLoadFloat3(&m_pVerticesPos[iIndices[2]]),
				pOut))
				goto Compute_WorldPos;


			/* 왼쪽 아래 삼각형 */
			if (true == m_pGameInstance->isPicked_InLocalSpace(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
				XMLoadFloat3(&m_pVerticesPos[iIndices[2]]),
				XMLoadFloat3(&m_pVerticesPos[iIndices[3]]),
				pOut))
				goto Compute_WorldPos;
		}
	}
	return false;

Compute_WorldPos:
	_vector vOut = XMLoadFloat3(pOut);
	vOut = XMVector3TransformCoord(vOut, WorldMatrix);
	XMStoreFloat3(pOut, vOut);

	return true;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticeX, _uint iNumVerticeZ)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumVerticeX, iNumVerticeZ)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	if (m_bCreate)
		Safe_Delete_Array(m_pVerticesPos);
}
