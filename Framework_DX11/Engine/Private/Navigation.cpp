#include "..\Public\Navigation.h"
#include "Cell.h"

#include<filesystem>
#include<fstream>
#include <locale>
#include <codecvt>
#include "Shader.h"
#include "GameInstance.h"
#include "Transform.h"

_Matrix CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{
	m_eComponentType = NAVIGATION;
}

CNavigation::CNavigation(const CNavigation & Prototype)
	: CComponent{ Prototype }
	, m_Cells { Prototype.m_Cells}

#ifdef _DEBUG
	, m_pShader { Prototype.m_pShader }
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif

	m_eComponentType = NAVIGATION;
}

_int CNavigation::Get_Cell_AreaNum(_int iIndex)
{
	return m_Cells[iIndex]->Get_AreaNum();
}

//HRESULT CNavigation::Initialize_Prototype(const _wstring& strNavigationDataFile)
//{
//	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
//
//	_ulong			dwByte = {};
//
//	HANDLE			hFile = CreateFile(strNavigationDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//	if (0 == hFile)
//		return E_FAIL;
//
//	_float3			vPoints[3];
//
//	while (true)
//	{
//		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//		if (0 == dwByte)
//			break;
//
//		CCell::CELL_DESC desc = {};
//		desc.iIndex = (_int)m_Cells.size();
//
//		CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoints, &desc);
//		if (nullptr == pCell)
//			return E_FAIL;
//
//		m_Cells.emplace_back(pCell);
//	}	
//	
//	CloseHandle(hFile);
//
//	if (FAILED(SetUp_Neighbors()))
//		return E_FAIL;
//
//#ifdef _DEBUG
//
//	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
//	if (nullptr == m_pShader)
//		return E_FAIL;
//
//#endif
//
//	return S_OK;
//}
HRESULT CNavigation::Initialize_Prototype(const _wstring& strNavigationDataFile)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	// wstring을 string으로 변환
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string narrowStr = converter.to_bytes(strNavigationDataFile);

	// 변환된 narrow string을 const char 배열로 복사
	const char* cFile = narrowStr.c_str();

	ifstream fin(cFile, ios::in | ios::binary);


	//	fin.open("../Bin/Map_Data.txt");
	if (!fin.is_open())    // 파일 열었다면
	{
		return E_FAIL;
	}

	_int iCellCout = { 0 };
	//getline(fin, line);
	//LayerCout = std::stoi(line);
	fin.read(reinterpret_cast<char*>(&iCellCout), sizeof(iCellCout));

	_float3 vPos[3] = {};

	for (_int i = 0; i < iCellCout; ++i)
	{
		_uint iRoomNum = { };
		fin.read(reinterpret_cast<char*>(&iRoomNum), sizeof(iRoomNum));

		_uint iCellType = { };
		fin.read(reinterpret_cast<char*>(&iCellType), sizeof(iCellType));

		for (int j = 0; j < 3; ++j)
		{
			fin.read(reinterpret_cast<char*>(&vPos[j].x), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&vPos[j].y), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&vPos[j].z), sizeof(_float));
		}

		CCell::CELL_DESC pDesc = {};
		pDesc.iIndex = (_int)m_Cells.size();
		pDesc.iAreaNum = iRoomNum;
 		pDesc.iCellTypeNum = iCellType;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPos, &pDesc);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);
	}

	fin.close();

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC*		pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentIndex;

	return S_OK;
}

void CNavigation::Update(_fmatrix TerrainWorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, TerrainWorldMatrix);
}

_bool CNavigation::isMove(_Vec3 vPosition)
{
	_Vec3		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_int			iNeighborIndex = { -1 };

	/* 원래 있던 삼각형 안에서 움직인거야. */
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex, &m_vOutLine))
	{
		return true;
	}

	/* 원래 있던 삼각형을 벗어난거야. */
	else
	{
		/* 나간쪽에 이웃이 있다라면. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex || m_Cells[iNeighborIndex]->Get_CellTypeNum() == 100 || m_Cells[iNeighborIndex]->Get_CellTypeNum() == m_iExceptCellNum)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex, &m_vOutLine))
				{
					break;
				}
			}
			
			m_iCurrentCellIndex = iNeighborIndex;

			_int iCellAreaNum = m_Cells[iNeighborIndex]->Get_AreaNum();
			if (iCellAreaNum != 0)	//공용 공간은 방 바꼈다고 판단 X
				m_iCurrentAreaIndex = iCellAreaNum;

			return true;
		}

		/* 나간쪽에 이웃이 없다라면. */
		else
			return false;
	}		
}

_float CNavigation::SetUp_OnCell(CTransform* pTransform, _float fOffset, _float fTimeDelta)
{
	if (m_iCurrentCellIndex < 0 || m_iCurrentCellIndex >= m_Cells.size())
		return pTransform->Get_State(CTransform::STATE_POSITION).y;

	_vector      vLocalPos = XMVector3TransformCoord(pTransform->Get_State(CTransform::STATE_POSITION), XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_vector vPointA = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_A);
	_vector vPointB = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_B);
	_vector vPointC = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_C);

	XMFLOAT3 pA, pB, pC;
	XMStoreFloat3(&pA, vPointA);
	XMStoreFloat3(&pB, vPointB);
	XMStoreFloat3(&pC, vPointC);

	XMFLOAT3 v1 = { pB.x - pA.x, pB.y - pA.y, pB.z - pA.z };
	XMFLOAT3 v2 = { pC.x - pA.x, pC.y - pA.y, pC.z - pA.z };

	XMFLOAT3 normal;
	normal.x = v1.y * v2.z - v1.z * v2.y;
	normal.y = v1.z * v2.x - v1.x * v2.z;
	normal.z = v1.x * v2.y - v1.y * v2.x;

	_float A = normal.x;
	_float B = normal.y;
	_float C = normal.z;
	_float D = -(A * pA.x + B * pA.y + C * pA.z);

	XMFLOAT3 playerPos;
	XMStoreFloat3(&playerPos, vLocalPos);
	_float x = playerPos.x;
	_float z = playerPos.z;

	_float y = -(A * x + C * z + D) / B + fOffset;

	_Vec3 vWorldPos = _Vec3(x, y + fOffset, z);

	_Vec3 vCurrentPos = pTransform->Get_State(CTransform::STATE_POSITION);

	//y = m_pGameInstance->Lerp(vCurrentPos.y, y, 0.7f);

	pTransform->Set_State(CTransform::STATE_POSITION, vWorldPos);

	return y;
}

void CNavigation::Move_to_Cell(CRigidBody* pRigidBody, _int iIndex)
{
	_Vec3 vAverage = m_Cells[iIndex]->Get_Point(CCell::POINT_A) + m_Cells[iIndex]->Get_Point(CCell::POINT_B) + m_Cells[iIndex]->Get_Point(CCell::POINT_C);
	vAverage /= 3.f;

	pRigidBody->Set_GloblePose(vAverage);
	m_iCurrentCellIndex = iIndex;
	m_iCurrentAreaIndex = m_Cells[iIndex]->Get_AreaNum();
}

void CNavigation::Research_Cell(_Vec3 vNewPos, _uint* iIndex)
{
	if (m_Cells.size() <= 0) //Cell이 없는 경우
	{
		return;
	}

	for (_uint i = 0; i < m_Cells.size(); ++i)
	{
		CCell* pCell = m_Cells[i];
		if (nullptr == pCell)
			continue;

		_int			iNeighborIndex = { -1 };

		if (true == m_Cells[i]->isIn(XMLoadFloat3(&vNewPos), &iNeighborIndex))
		{
			//y축 비교
			_Vec3 vPointA = m_Cells[i]->Get_Point(CCell::POINT_A);
			_Vec3 vPointB = m_Cells[i]->Get_Point(CCell::POINT_B);
			_Vec3 vPointC = m_Cells[i]->Get_Point(CCell::POINT_C);

			_float fCellBigY = max(vPointA.y, vPointB.y);
			fCellBigY = max(fCellBigY, vPointC.y);

			_float fCellSmallY = min(vPointA.y, vPointB.y);
			fCellSmallY = min(fCellSmallY, vPointC.y);

			//약간 느슨하게 검사
			if (vNewPos.y <= fCellBigY + 4.f && vNewPos.y >= fCellSmallY - 4.f)
			{
				m_iCurrentCellIndex = i;
				m_iCurrentAreaIndex = m_Cells[i]->Get_AreaNum();
				if(iIndex != nullptr)
					*iIndex = i;
				return;
			}
			else
				continue;
		}
	}

	if (iIndex != nullptr)
		*iIndex = -1;
}

_float CNavigation::Get_CellPosY(CTransform* pTransform, _float fOffset)
{
	if (m_iCurrentCellIndex < 0 || m_iCurrentCellIndex >= m_Cells.size())
		return 100.f;

	_vector      vLocalPos = XMVector3TransformCoord(pTransform->Get_State(CTransform::STATE_POSITION), XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_vector vPointA = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_A);
	_vector vPointB = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_B);
	_vector vPointC = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_C);

	XMFLOAT3 pA, pB, pC;
	XMStoreFloat3(&pA, vPointA);
	XMStoreFloat3(&pB, vPointB);
	XMStoreFloat3(&pC, vPointC);

	XMFLOAT3 v1 = { pB.x - pA.x, pB.y - pA.y, pB.z - pA.z };
	XMFLOAT3 v2 = { pC.x - pA.x, pC.y - pA.y, pC.z - pA.z };

	XMFLOAT3 normal;
	normal.x = v1.y * v2.z - v1.z * v2.y;
	normal.y = v1.z * v2.x - v1.x * v2.z;
	normal.z = v1.x * v2.y - v1.y * v2.x;

	_float A = normal.x;
	_float B = normal.y;
	_float C = normal.z;
	_float D = -(A * pA.x + B * pA.y + C * pA.z);

	XMFLOAT3 playerPos;
	XMStoreFloat3(&playerPos, vLocalPos);
	_float x = playerPos.x;
	_float z = playerPos.z;

	_float y = -(A * x + C * z + D) / B + fOffset;

	return y;
}


#ifdef _DEBUG

HRESULT CNavigation::Render()
{

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_Vec4		vColor = -1 == m_iCurrentCellIndex ? _float4(0.f, 1.f, 0.f, 1.f) : _float4(1.f, 0.f, 0.f, 1.f);
	_Matrix		WorldMatrix = m_WorldMatrix;
	
	if (-1 != m_iCurrentCellIndex)
		WorldMatrix._42 += 0.1f;	

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

	m_pShader->Begin(0);


	if (-1 == m_iCurrentCellIndex)
	{
		for (auto& pCell : m_Cells)
			pCell->Render();
	}
	else
	{
		m_Cells[m_iCurrentCellIndex]->Render();
	}



	return S_OK;
}

#endif

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _wstring & strNavigationDataFile)
{
	CNavigation*		pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFile)))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*		pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
