#include "stdafx.h"
#include "Dissolve_Effect.h"
#include "GameInstance.h"

CDissolve_Effect::CDissolve_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CDissolve_Effect::CDissolve_Effect(const CDissolve_Effect& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CDissolve_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDissolve_Effect::Initialize(void* pArg)
{
    DISSOLVE_EFFECT_DESC* pDesc = static_cast<DISSOLVE_EFFECT_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(*pDesc)))
        return E_FAIL;

    m_pTarget_ModelCom = pDesc->pTarget_ModelCom;
    Safe_AddRef(m_pTarget_ModelCom);

    m_pTarget_TransformCom = pDesc->pTarget_TransformCom;
    Safe_AddRef(m_pTarget_TransformCom);

    return S_OK;
}

void CDissolve_Effect::Priority_Update(_float fTimeDelta)
{
}

void CDissolve_Effect::Update(_float fTimeDelta)
{
}

void CDissolve_Effect::Late_Update(_float fTimeDelta)
{
}

HRESULT CDissolve_Effect::Render()
{
    return S_OK;
}

void CDissolve_Effect::Reset()
{
    m_pVIBufferCom->ResetCS(m_pResetCS);
}

void CDissolve_Effect::Set_On(_bool bOn)
{
    if (m_bOn != bOn)
    {
        m_bOn = bOn;
        if (true == m_bOn)
        {
            Reset();
        }
    }
}

HRESULT CDissolve_Effect::Ready_Components(const DISSOLVE_EFFECT_DESC& Desc)
{
    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.strVIBufferTag,
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxDiffuseInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    
    /* FOR.Com_Compute_Reset */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Compute_Dissolve_Reset"),
        TEXT("Com_Compute_Reset"), reinterpret_cast<CComponent**>(&m_pResetCS))))
        return E_FAIL;

    return S_OK;
}


void CDissolve_Effect::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pResetCS);

    Safe_Release(m_pTarget_ModelCom);
    Safe_Release(m_pTarget_TransformCom);
}
