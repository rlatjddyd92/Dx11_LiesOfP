#include "stdafx.h"

#include "Camera_Manager.h"
#include "Camera.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCamera_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

void CCamera_Manager::Add_Camera(const _wstring& strCameraTag, CCamera* pCamera)
{
	if (nullptr != Find_Camera(strCameraTag))
		return;

	pCamera->IsActive(false);
	m_Cameras.emplace(strCameraTag, pCamera);
	Safe_AddRef(pCamera);
}

HRESULT CCamera_Manager::Change_Camera(const _wstring& strCameraTag)
{
	CCamera* pCamera = Find_Camera(strCameraTag);
	if (nullptr == pCamera)
		return E_FAIL;

	if (nullptr != m_pCurrentCamera)
	{
		m_pCurrentCamera->IsActive(false);
		Safe_Release(m_pCurrentCamera);
	}

	pCamera->IsActive(true);
	Safe_AddRef(pCamera);
	m_pCurrentCamera = pCamera;

}

CCamera* CCamera_Manager::Find_Camera(const _wstring& strCameraTag)
{
	auto	iter = m_Cameras.find(strCameraTag);

	if (iter == m_Cameras.end())
		return nullptr;

	return iter->second;
}

void CCamera_Manager::Clear()
{
	if (m_Cameras.size() > 0)
	{
		for (auto& Pair : m_Cameras)
			Safe_Release(Pair.second);
		m_Cameras.clear();
	}
}

void CCamera_Manager::Free()
{
	__super::Free();

	Clear();
	Safe_Release(m_pCurrentCamera);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
