#include "Collider_Manager.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"

#include "Collider.h"

CCollider_Manager::CCollider_Manager()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollider_Manager::Initialize()
{
	return S_OK;
}

void CCollider_Manager::Update()
{
	for (auto& pCollider1 : m_ColliderList)
	{
		for (auto& pCollider2 : m_ColliderList)
		{
			if (pCollider1 == pCollider2 || (nullptr == pCollider1 || nullptr == pCollider2))
				continue;

			CGameObject* pGameObject1 = pCollider1->Get_GameObject();
			CGameObject* pGameObject2 = pCollider2->Get_GameObject();

			if (nullptr == pGameObject1 || nullptr == pGameObject2)
				continue;

			//만약 충돌체크가 꺼져있으면
			if (!pCollider1->IsActive() || !pCollider2->IsActive())
			{
				//충돌 체크가 꺼져있는데 목록에 들어있네?
				if (pCollider1->GetCollisionSet().find(pGameObject2) != pCollider1->GetCollisionSet().end())
				{
					pGameObject1->OnCollisionExit(pGameObject2);
					pGameObject2->OnCollisionExit(pGameObject1);
					pCollider1->GetCollisionSet().erase(pGameObject2);
					pCollider2->GetCollisionSet().erase(pGameObject1);
				}
				continue;
			}

			//충돌했다
			if (pCollider1->Intersect(pCollider2) && pCollider2->Intersect(pCollider1))
			{
				//충돌했는데 충돌 목록에 없다
				//막 충돌한상태다
				if (pCollider1->GetCollisionSet().find(pGameObject2) == pCollider1->GetCollisionSet().end()
					&& pCollider2->GetCollisionSet().find(pGameObject1) == pCollider2->GetCollisionSet().end())
				{
					pGameObject1->OnCollisionEnter(pGameObject2);
					pGameObject2->OnCollisionEnter(pGameObject1);
					pCollider1->GetCollisionSet().insert(pGameObject2);
					pCollider2->GetCollisionSet().insert(pGameObject1);
				}
				//충돌했는데 충돌 목록에 있다
				//충돌중이다
				else if (pCollider1->GetCollisionSet().find(pGameObject2) != pCollider1->GetCollisionSet().end()
					&& pCollider2->GetCollisionSet().find(pGameObject1) != pCollider2->GetCollisionSet().end())
				{
					pGameObject1->OnCollisionStay(pGameObject2);
					pGameObject2->OnCollisionStay(pGameObject1);
				}
			}
			//충돌을 안 했어
			else
			{
				//근데 목록에 있네
				if (pCollider1->GetCollisionSet().find(pGameObject2) != pCollider1->GetCollisionSet().end()
					|| pCollider2->GetCollisionSet().find(pGameObject1) != pCollider2->GetCollisionSet().end())
				{
					pGameObject1->OnCollisionExit(pGameObject2);
					pGameObject2->OnCollisionExit(pGameObject1);
					pCollider1->GetCollisionSet().erase(pGameObject2);
					pCollider2->GetCollisionSet().erase(pGameObject1);
				}
			}

		}

	}

	for (auto& pCollider : m_ColliderList)
	{
		Safe_Release(pCollider);
	}
	m_ColliderList.clear();
}

HRESULT CCollider_Manager::Add_ColliderList(CCollider* pCollider)
{
	if (nullptr == pCollider)
		return E_FAIL;

	m_ColliderList.emplace_back(pCollider);
	Safe_AddRef(pCollider);

	return S_OK;
}

CCollider_Manager* CCollider_Manager::Create()
{
	CCollider_Manager* pInstance = new CCollider_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (auto& pCollider : m_ColliderList)
	{
		Safe_Release(pCollider);
	}
	m_ColliderList.clear();
}
