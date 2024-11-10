#include "stdafx.h"
#include "Effect_Base.h"
#include "GameInstance.h"

CEffect_Base::CEffect_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CEffect_Base::CEffect_Base(const CEffect_Base& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CEffect_Base::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Base::Initialize(void* pArg)
{
    EFFECT_BASE_DESC* pDesc = static_cast<EFFECT_BASE_DESC*>(pArg);

    if(FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pParentMatrix = pDesc->pParentMatrix;

    return S_OK;
}

void CEffect_Base::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Base::Update(_float fTimeDelta)
{
}

void CEffect_Base::Late_Update(_float fTimeDelta)
{
}

HRESULT CEffect_Base::Render()
{
    return S_OK;
}

void CEffect_Base::Reset()
{
}


HRESULT CEffect_Base::Bind_WorldMatrix(CShader* pShader, const _char* pConstantName)
{
    if (FAILED(pShader->Bind_Matrix(pConstantName, &m_WorldMatrix)))
        return E_FAIL;

    return S_OK;
}

void CEffect_Base::Free()
{
    __super::Free();

}
