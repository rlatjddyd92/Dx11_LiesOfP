#include "stdafx.h"
#include "Item_Dropped.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect_Container.h"

CItem_Dropped::CItem_Dropped(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CItem_Dropped::CItem_Dropped(const CItem_Dropped& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CItem_Dropped::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItem_Dropped::Initialize(void* pArg)
{
    ITEM_DROPPED_DESC* pDesc = static_cast<ITEM_DROPPED_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

    m_iItemIndex = pDesc->iItemIndex;
	m_strObjectTag = TEXT("Item_Dropped");
    _Vec3 vPos = { pDesc->vParentPos.x,pDesc->vParentPos.y, pDesc->vParentPos.z };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

    m_Effect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("DropItem_Active"), m_pTransformCom->Get_WorldMatrix_Ptr(), nullptr, _Vec3(0.f, 0.3f, 0.f ));
    m_Effect->Set_Loop(true);

	return S_OK;
}

void CItem_Dropped::Priority_Update(_float fTimeDelta)
{
    m_Effect->Priority_Update(fTimeDelta);
}

void CItem_Dropped::Update(_float fTimeDelta)
{
    if (m_pColliderCom != nullptr)
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

    m_Effect->Update(fTimeDelta);
}

void CItem_Dropped::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    m_pGameInstance->Add_ColliderList(m_pColliderCom);
    m_Effect->Late_Update(fTimeDelta);
#ifdef _DEBUG
    if (m_pColliderCom != nullptr)
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CItem_Dropped::Render()
{
    return S_OK;
}

void CItem_Dropped::OnCollisionEnter(CGameObject* pOther)
{
}

void CItem_Dropped::OnCollisionStay(CGameObject* pOther)
{
    if (pOther->Get_Tag() == TEXT("Player"))
    {
    }
}

void CItem_Dropped::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CItem_Dropped::Ready_Components()
{
    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.f, 1.f, 1.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    return S_OK;
}

CItem_Dropped* CItem_Dropped::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CItem_Dropped* pInstance = new CItem_Dropped(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CItem_Dropped"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItem_Dropped::Clone(void* pArg)
{
    CItem_Dropped* pInstance = new CItem_Dropped(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CItem_Dropped"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItem_Dropped::Free()
{
    __super::Free();

    if (true == m_isCloned)
    {
        m_Effect->Set_Cloned(false);
        Safe_Release(m_Effect);
    }
    Safe_Release(m_pColliderCom);
}
