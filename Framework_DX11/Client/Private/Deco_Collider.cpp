#include "stdafx.h"
#include "Deco_Collider.h"
#include "GameInstance.h"

CDeco_Collider::CDeco_Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CDeco_Collider::CDeco_Collider(const CDeco_Collider& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CDeco_Collider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDeco_Collider::Initialize(void* pArg)
{
    DECO_COLLIDER_DESC* pDesc = static_cast<DECO_COLLIDER_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    return S_OK;
}

void CDeco_Collider::Priority_Update(_float fTimeDelta)
{
}

void CDeco_Collider::Update(_float fTimeDelta)
{
}

void CDeco_Collider::Late_Update(_float fTimeDelta)
{
}

HRESULT CDeco_Collider::Render()
{
    return S_OK;
}

void CDeco_Collider::OnCollisionEnter(CGameObject* pOther)
{
}

void CDeco_Collider::OnCollisionStay(CGameObject* pOther)
{
}

void CDeco_Collider::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CDeco_Collider::Ready_Components()
{
    return S_OK;
}

CDeco_Collider* CDeco_Collider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDeco_Collider* pInstance = new CDeco_Collider(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDeco_Collider"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDeco_Collider::Clone(void* pArg)
{
    CDeco_Collider* pInstance = new CDeco_Collider(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDeco_Collider"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDeco_Collider::Free()
{
    __super::Free();
    Safe_Release(m_pColliderCom);
}
