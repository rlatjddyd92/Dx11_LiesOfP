#include "WorldOctree_Manager.h"
#include "WorldOctree.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

CWorldOctree_Manager::CWorldOctree_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CWorldOctree_Manager::Initialize()
{
    return S_OK;
}

void CWorldOctree_Manager::Update()
{
	if (m_bActive_Octree == false)
		return;

	m_FrustumCulledNodes.clear();

	//절두체 컬링으로 그려질 노드 판단
	if (m_pWorldOctree != nullptr)
		m_pWorldOctree->Culling(m_pGameInstance, m_FrustumCulledNodes);
}

void CWorldOctree_Manager::Create_Octree(_Vec3 vMinPos, _Vec3 vMaxPos)
{
	_int iDepth = -100;
	vector<CGameObject*> ObjectList = m_pGameInstance->Find_Layer((_uint)3, TEXT("Layer_Map"))->Get_ObjectList();
	m_pWorldOctree = CWorldOctree::Create(m_pDevice, m_pContext, vMinPos, vMaxPos, ObjectList, &iDepth);
}

_bool CWorldOctree_Manager::Is_In_FrustumCulledOctree(vector<_int>& pWorldOctreeIndex)
{
	for (size_t i = 0; i < pWorldOctreeIndex.size(); ++i)
	{
		if (m_FrustumCulledNodes.find(pWorldOctreeIndex[i]) != m_FrustumCulledNodes.end())
		{
			return true;
		}
	}
	return false;
}

void CWorldOctree_Manager::Render()
{
#ifdef _DEBUG
	if (m_pWorldOctree)
		m_pWorldOctree->Render();
#endif
}

CWorldOctree_Manager* CWorldOctree_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWorldOctree_Manager* pInstance = new CWorldOctree_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CWorldOctree_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWorldOctree_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pWorldOctree);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
