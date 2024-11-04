#include "..\Public\Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.emplace_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto pGameObject = m_GameObjects.begin(); pGameObject != m_GameObjects.end(); )
	{
		if ((*pGameObject)->Get_Dead())
		{
			Safe_Release(*pGameObject);
			pGameObject = m_GameObjects.erase(pGameObject);
		}
		else
		{
			if (nullptr != *pGameObject && (*pGameObject)->IsActive())
				(*pGameObject)->Priority_Update(fTimeDelta);

			++pGameObject;
		}
	}

	return S_OK;
}

HRESULT CLayer::Update(_float fTimeDelta)
{
	for (auto pGameObject = m_GameObjects.begin(); pGameObject != m_GameObjects.end(); )
	{
		if ((*pGameObject)->Get_Dead())
		{
			Safe_Release(*pGameObject);
			pGameObject = m_GameObjects.erase(pGameObject);
		}
		else
		{
			if (nullptr != *pGameObject && (*pGameObject)->IsActive())
				(*pGameObject)->Update(fTimeDelta);

			++pGameObject;
		}
	}

	return S_OK;
}

HRESULT CLayer::Late_Update(_float fTimeDelta)
{
	for (auto pGameObject = m_GameObjects.begin(); pGameObject != m_GameObjects.end(); )
	{
		if ((*pGameObject)->Get_Dead())
		{
			Safe_Release(*pGameObject);
			pGameObject = m_GameObjects.erase(pGameObject);
		}
		else
		{
			if (nullptr != *pGameObject && (*pGameObject)->IsActive())
				(*pGameObject)->Late_Update(fTimeDelta);

			++pGameObject;
		}
	}

	return S_OK;
}

CComponent * CLayer::Find_Component(const _wstring & strComponentTag, _uint iIndex, _uint iPartObjIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_Component(strComponentTag, iPartObjIndex);	
}

CGameObject* CLayer::Find_Object(_uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter);
}

CLayer * CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);
	m_GameObjects.clear();
}

