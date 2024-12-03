#include "..\Public\Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

LIGHT_DESC * CLight_Manager::Get_LightDesc(_uint iIndex) 
{
	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;	

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Initialize()
{

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC & LightDesc)
{
	CLight*		pLight = CLight::Create(LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	if (LIGHT_DESC::TYPE::TYPE_DIRECTIONAL == LightDesc.eType) {

		m_vDirectionLightDir = _Vec3(LightDesc.vDirection.x, LightDesc.vDirection.y, LightDesc.vDirection.z);
	}

	return S_OK;
}

HRESULT CLight_Manager::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer, _bool isOnPBR)
{
	for (auto& pLight : m_Lights)
		pLight->Render(pShader, pVIBuffer, isOnPBR);

	return S_OK;
}

void CLight_Manager::Delete_Light(_int iIndex)
{
	if (m_Lights[iIndex] != nullptr && iIndex < m_Lights.size())
	{
		Safe_Release(m_Lights[iIndex]);
		m_Lights.erase(remove(m_Lights.begin(), m_Lights.end(), m_Lights[iIndex]), m_Lights.end());
	}
	//remove와 erase의 단점을 보완하기 위해 둘 다 사용
}

_int CLight_Manager::Find_Light_Index(_Vec4 vPos)
{
	int iIndex = 0;
	for (auto& pLight : m_Lights)
	{
		LIGHT_DESC* pDesc = pLight->Get_LightDesc();
		if (pDesc->vPosition.x == vPos.x && pDesc->vPosition.y == vPos.y && pDesc->vPosition.z == vPos.z && pDesc->eType != LIGHT_DESC::TYPE::TYPE_DIRECTIONAL)
		{
			return iIndex;
		}
		else
			iIndex++;
	}

	return -1;
}

_int CLight_Manager::Find_DirectionLight()
{
	_int iIndex = 0;
	for (auto& pLight : m_Lights)
	{
		if (pLight->Get_LightDesc()->eType == LIGHT_DESC::TYPE::TYPE_DIRECTIONAL)
			return iIndex;

		++iIndex;
	}

	return -1;
}

CLight_Manager * CLight_Manager::Create()
{
	CLight_Manager*		pInstance = new CLight_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLight_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLight_Manager::Free()
{
	__super::Free();

	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();	
}