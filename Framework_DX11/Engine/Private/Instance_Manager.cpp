#include "..\Public\Component_Manager.h"
#include "Component.h"
#include "Instance_Manager.h"
#include "GameInstance.h"

CInstance_Manager::CInstance_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pGameInstance{ CGameInstance::Get_Instance() }
	,m_pDevice { pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CInstance_Manager::Initialize()
{
	m_pNonAnimInstanceShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Model_Instance.hlsl"), VTXMODELINSTANCE::Elements, VTXMODELINSTANCE::iNumElements);
	if (nullptr == m_pNonAnimInstanceShader)
		return E_FAIL;

	return S_OK;
}

CModel* CInstance_Manager::Add_NonAnimModel(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	//찾아봐라
	CModel* pInstanceModel = Find_NonAnimModel(strPrototypeTag);
	if (pInstanceModel != nullptr)
		return pInstanceModel;

	// 없으면 만들어라
	_bool isInstance = true;
	CComponent* pComponent = m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, &isInstance);
	if (nullptr == pComponent)
		return nullptr;

	pInstanceModel = dynamic_cast<CModel*>(pComponent);
	m_pNonAnimModels.emplace(strPrototypeTag, pInstanceModel);

	return pInstanceModel;
}

void CInstance_Manager::Clear()
{
	for (auto& Pair : m_pNonAnimModels)
	{
		Safe_Release(Pair.second);
	}
	m_pNonAnimModels.clear();
}

HRESULT CInstance_Manager::Draw(_uint iPass)
{
	if (FAILED(m_pNonAnimInstanceShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pNonAnimInstanceShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	for (auto& Pair : m_pNonAnimModels)
	{
		_uint		iNumMeshes = Pair.second->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(Pair.second->Bind_Material(m_pNonAnimInstanceShader, "g_DiffuseTexture", DIFFUSE, (_uint)i)))
				return E_FAIL;

			if (nullptr != Pair.second->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
			{
				if (FAILED(Pair.second->Bind_Material(m_pNonAnimInstanceShader, "g_ARMTexture", ROUGHNESS, (_uint)i)))
					return E_FAIL;
			}

			if (nullptr != Pair.second->Find_Texture((_uint)i, TEXTURE_TYPE::NORMALS))
			{
				if (FAILED(Pair.second->Bind_Material(m_pNonAnimInstanceShader, "g_NormalTexture", NORMALS, (_uint)i)))
					return E_FAIL;

				if (FAILED(m_pNonAnimInstanceShader->Begin(1)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pNonAnimInstanceShader->Begin(0)))
					return E_FAIL;
			}

			if (FAILED(Pair.second->Render_Instance((_uint)i)))
				return E_FAIL;
		}

		Pair.second->Clear_InstanceData();
	}

	return S_OK;
}

HRESULT CInstance_Manager::Draw_Shadow()
{
	if (FAILED(m_pNonAnimInstanceShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pNonAnimInstanceShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pNonAnimInstanceShader->Bind_Matrices("g_CascadeViewMatrix", m_pGameInstance->Get_CascadeViewMatirx(), 3)))
		return E_FAIL;
	if (FAILED(m_pNonAnimInstanceShader->Bind_Matrices("g_CascadeProjMatrix", m_pGameInstance->Get_CascadeProjMatirx(), 3)))
		return E_FAIL;

	for (auto& Pair : m_pNonAnimModels)
	{
		_uint		iNumMeshes = Pair.second->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pNonAnimInstanceShader->Begin(2)))
				return E_FAIL;

			if (FAILED(Pair.second->Render_Instance((_uint)i)))
				return E_FAIL;
		}

		Pair.second->Clear_InstanceData();
	}

	return S_OK;
}

CModel* CInstance_Manager::Find_NonAnimModel(const _wstring& strPrototypeTag)
{
	auto	iter = m_pNonAnimModels.find(strPrototypeTag);

	if (iter == m_pNonAnimModels.end())
		return nullptr;

	return iter->second;
}

CInstance_Manager* CInstance_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstance_Manager* pInstance = new CInstance_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CComponent_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInstance_Manager::Free()
{
	__super::Free();


	Safe_Release(m_pNonAnimInstanceShader);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
