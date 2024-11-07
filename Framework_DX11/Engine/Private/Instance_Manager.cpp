#include "..\Public\Component_Manager.h"
#include "Component.h"
#include "Instance_Manager.h"

Instance_Manager::Instance_Manager()
{
}

HRESULT Instance_Manager::Initialize(_uint iNumLevels)
{
	
	return S_OK;
}

HRESULT Instance_Manager::Add_NonAnimModel(const _wstring& strPrototypeTag, CModel* pModel)
{
	if (nullptr != Find_NonAnimModel(strPrototypeTag))
		return S_OK;

	m_pNonAnimModles.emplace(strPrototypeTag, pModel);
	Safe_AddRef(pModel);

	return S_OK;
}

void Instance_Manager::Clear()
{
	for (auto& Pair : m_pNonAnimModles)
		Safe_Release(Pair.second);

	m_pNonAnimModles.clear();
}

CModel* Instance_Manager::Find_NonAnimModel(const _wstring& strPrototypeTag)
{
	auto	iter = m_pNonAnimModles.find(strPrototypeTag);

	if (iter == m_pNonAnimModles.end())
		return nullptr;

	return iter->second;
}

Instance_Manager* Instance_Manager::Create(_uint iNumLevels)
{
	Instance_Manager* pInstance = new Instance_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CComponent_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Instance_Manager::Free()
{
	__super::Free();


	for (auto& Pair : m_pNonAnimModles)
		Safe_Release(Pair.second);
	m_pNonAnimModles.clear();

}
