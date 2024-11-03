#include "Picking_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "Mesh.h"

CPicking_Manager::CPicking_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
}

HRESULT CPicking_Manager::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	m_hWnd = hWnd;

	m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;

	return S_OK;
}

void CPicking_Manager::Update()
{
	POINT			ptMouse = m_pGameInstance->Get_MosePos();

	_vector     vMousePos;

	// 투영 스페이스 마우스 좌표
	vMousePos = XMVectorSet(_float(ptMouse.x / (m_iWinSizeX * 0.5f) - 1), _float(ptMouse.y / (m_iWinSizeY * -0.5f) + 1), 0.f, 1.f);

	// 뷰 스페이스 마우스 좌표
	_matrix		ProjMatrixInv;
	ProjMatrixInv = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ);
	vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInv);

	_vector						vRayPos = {};
	_vector						vRayDir = {};


	// 뷰스페이스 상의 마우스 레이와 레이포스
	vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vRayDir = vMousePos - vRayPos;

	// 월드스페이스 상의 마우스 레이와 레이포스
	_matrix		ViewMatrixInv;
	ViewMatrixInv = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
	vRayPos = XMVector3TransformCoord(vRayPos, ViewMatrixInv);
	vRayDir = XMVector3TransformNormal(vRayDir, ViewMatrixInv);

	vRayDir = XMVector3Normalize(vRayDir);
	
	XMStoreFloat3(&m_vRayDir, vRayDir);
	XMStoreFloat3(&m_vRayPos, vRayPos);


	for (auto& PickingObject : m_ModelPickingObjects)
	{
		if (KEY_AWAY(KEY::LBUTTON))
		{
			IsPicking_Mesh(PickingObject);
		}
		Safe_Release(PickingObject);
	}
	m_ModelPickingObjects.clear();
}

void CPicking_Manager::Transform_ToLocalSpace(const _matrix WorldMatrixInverse)
{
	_vector						vRayPos_InLocalSpace = {};
	_vector						vRayDir_InLocalSpace = {};

	vRayPos_InLocalSpace = XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), WorldMatrixInverse);
	vRayDir_InLocalSpace = XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldMatrixInverse);

	vRayDir_InLocalSpace = XMVector3Normalize(vRayDir_InLocalSpace);

	XMStoreFloat3(&m_vRayPos_InLocalSpace, vRayPos_InLocalSpace);
	XMStoreFloat3(&m_vRayDir_InLocalSpace, vRayDir_InLocalSpace);
}

_bool CPicking_Manager::isPicked_InWorldSpace(_float3& vPointA, _float3& vPointB, _float3& vPointC, _float3* pOut)
{
	//_float		fU{}, fV{}, fDist{};

	//if (TRUE == D3DXIntersectTri(&vPointA, &vPointB, &vPointC, &m_vRayPos, &m_vRayDir, &fU, &fV, &fDist))
	//{
	//	*pOut = m_vRayPos + m_vRayDir * fDist;
	//	return true;
	//}

	return false;
}

_bool CPicking_Manager::isPicked_InLocalSpace(const _fvector& vPointA, const _fvector& vPointB, const _fvector& vPointC, _float3* pOut)
{
	_float		fDist{};

	_vector		vRayPos_InLocalSpace = XMLoadFloat3(&m_vRayPos_InLocalSpace);
	_vector		vRayDir_InLocalSpace = XMLoadFloat3(&m_vRayDir_InLocalSpace);

	if (TRUE == TriangleTests::Intersects(vRayPos_InLocalSpace, vRayDir_InLocalSpace, vPointA, vPointB, vPointC, fDist))
	{
		XMStoreFloat3(pOut, vRayPos_InLocalSpace + vRayDir_InLocalSpace * fDist);
		return true;
	}

	return false;
}

void CPicking_Manager::Add_ModelPickingList(CGameObject* pObj)
{
	if (nullptr != pObj)
	{
		m_ModelPickingObjects.emplace_back(pObj);
		Safe_AddRef(pObj);
	}
}

void CPicking_Manager::IsPicking_Mesh(CGameObject* pObj)
{
	CModel*				pModel = dynamic_cast<CModel*>(pObj->Find_Component(MODEL));
	if (nullptr == pModel)
		return;

	vector<CMesh*>&		Meshes = pModel->Get_Meshes();
	CTransform*			pTransform = pObj->Get_Transform();

	for (auto& Mesh : Meshes)
	{
		if (Mesh->isPicking(pTransform->Get_WorldMatrix(), &m_vModelPickingPos))
		{
			if (nullptr != m_pModelPickObject)
			{
				m_pModelPickObject->Set_IsPicked(false);
				Safe_Release(m_pModelPickObject);
			}

			m_pModelPickObject = pObj;
			Safe_AddRef(m_pModelPickObject);

			pObj->Set_IsPicked(true);
		}
	}
}

CPicking_Manager* CPicking_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	CPicking_Manager* pInstance = new CPicking_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
	{
		MSG_BOX(TEXT("Failed to Created : CPicking_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking_Manager::Free()
{
	__super::Free();

	for (auto& Object : m_ModelPickingObjects)
		Safe_Release(Object);
	m_ModelPickingObjects.clear();

	Safe_Release(m_pModelPickObject);
}
