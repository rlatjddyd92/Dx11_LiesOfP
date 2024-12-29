#include "Dissolve_Container.h"
#include "GameInstance.h"

CDissolve_Container::CDissolve_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CDissolve_Container::CDissolve_Container(const CDissolve_Container& Prototype)
    : CComponent(Prototype)
{
    m_DissolveInstances.reserve(Prototype.m_DissolveInstances.size());

    for (auto& Instances : Prototype.m_DissolveInstances)
    {
        m_DissolveInstances.emplace_back(static_cast<CVIBuffer_Dissolve_Instance*>(Instances->Clone(nullptr)));
    }
}

HRESULT CDissolve_Container::Initialize_Prototype(const DISSOLVE_CONTAINER_DESC& Desc)
{
    for (size_t i = 0; i < (size_t)Desc.iNumInstance; ++i)
    {
        m_DissolveInstances.emplace_back(Desc.pInstances[i]);
    }

    return S_OK;
}

HRESULT CDissolve_Container::Initialize(void* pArg)
{

    return S_OK;
}

HRESULT CDissolve_Container::Bind_Buffer(CShader_NonVTX* pShader, const _char* pConstantName, _uint iIndex)
{
    return m_DissolveInstances[iIndex]->Bind_Buffer(pShader, pConstantName);
}

HRESULT CDissolve_Container::Render(_uint iIndex)
{
    return m_DissolveInstances[iIndex]->Render();
}

_bool CDissolve_Container::DispatchCS(CShader_Compute* pComputeShader, CTexture* pTexture, CModel* pModel, const CVIBuffer_Dissolve_Instance::PARTICLE_MOVEMENT& MovementData, const CVIBuffer_Dissolve_Instance::DISSOLVE_DATA& DissolveData)
{
    _bool isEnd = { true };

    for (auto& Instance : m_DissolveInstances)
    {
        if(false == Instance->DispatchCS(pComputeShader, pTexture, pModel, MovementData, DissolveData))
            isEnd = false;
    }

    return isEnd;
}

CDissolve_Container* CDissolve_Container::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const DISSOLVE_CONTAINER_DESC& Desc)
{
    CDissolve_Container* pInstance = new CDissolve_Container(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(Desc)))
    {
        MSG_BOX(TEXT("Create Failed : CDissolve_Container"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CDissolve_Container::Clone(void* pArg)
{
    CDissolve_Container* pInstance = new CDissolve_Container(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CDissolve_Container"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDissolve_Container::Free()
{
    __super::Free();
}
