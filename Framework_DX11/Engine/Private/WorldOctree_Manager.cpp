#include "WorldOctree_Manager.h"
#include "WorldOctree.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

CWorldOctree_Manager::CWorldOctree_Manager()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CWorldOctree_Manager::Initialize()
{
    return S_OK;
}

void CWorldOctree_Manager::Update()
{
	m_FrustumCulledNodes.clear();

	//절두체 컬링으로 그려질 노드 판단
	if (m_pWorldOctree != nullptr)
		m_pWorldOctree->Culling(m_pGameInstance, m_FrustumCulledNodes);
}

void CWorldOctree_Manager::Create_Octree(_Vec3 vMinPos, _Vec3 vMaxPos)
{
	_int iDepth = -1;
	vector<CGameObject*> ObjectList = m_pGameInstance->Find_Layer((_uint)3, TEXT("Layer_Map"))->Get_ObjectList();
	m_pWorldOctree = CWorldOctree::Create(vMinPos, vMaxPos, ObjectList, &iDepth);
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

CWorldOctree_Manager* CWorldOctree_Manager::Create()
{
	CWorldOctree_Manager* pInstance = new CWorldOctree_Manager();

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

	Safe_Release(m_pGameInstance);
}
