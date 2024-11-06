#include "Effect_Base.h"

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
    if(FAILED(__super::Initialize(pArg)))
        return E_FAIL;

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

void CEffect_Base::Free()
{
    __super::Free();

}
