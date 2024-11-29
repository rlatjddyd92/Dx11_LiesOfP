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

    m_pParentWorldMatrix = pDesc->pParentWorldMatrix;
    m_pSoketMatrix = pDesc->pSoketMatrix;

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
    _matrix		SocketMatrix = XMLoadFloat4x4(m_pSoketMatrix);

    for (size_t i = 0; i < 3; i++)
    {
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
    }
    m_pTransformCom->Set_WorldMatrix( SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));

    if (m_pColliderCom != nullptr)
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CDeco_Collider::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
    if (m_pColliderCom != nullptr)
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
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
    /* For.Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.f, 1.f, 1.f);
    ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);
     
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
