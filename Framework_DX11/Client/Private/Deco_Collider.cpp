#include "stdafx.h"
#include "Deco_Collider.h"
#include "GameInstance.h"
#include "Player.h"
#include "WallDeco.h"

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
    m_pWallDeco = pDesc->pWallDeco;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

   //_matrix		SocketMatrix = XMLoadFloat4x4(m_pSoketMatrix);

   // for (size_t i = 0; i < 3; i++)
   // {
   //     SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
   // }
   // m_pTransformCom->Set_WorldMatrix( SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));


    m_strObjectTag = TEXT("WallDeco_Collider");
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

    _Vec4 vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    Change_Vec4_to_Vec3(vCurPos, m_vCurPos);

    if (m_bSetRigidPos == false)
    {
        m_bSetRigidPos = true;
        m_vPrePos = m_vCurPos;
        m_pRigidBodyCom->Set_GloblePose(m_vCurPos);
    }

    //현재위치 - 과거 위치 / 시간
    Calculate_Velocity(fTimeDelta);
    m_pRigidBodyCom->Set_Velocity(m_vVelocity);

    if (m_pColliderCom != nullptr)
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

}

void CDeco_Collider::Late_Update(_float fTimeDelta)
{
    m_pRigidBodyCom->Update(fTimeDelta);
    m_vPrePos = m_vCurPos;

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
    if (pOther->Get_Tag() == TEXT("Player"))
    {
        if (m_pWallDeco->Get_IsCanHit())
        {
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
            pPlayer->Calc_DamageGain(30.f, (_Vec3)m_pTransformCom->Get_State(CTransform::STATE_POSITION));
        }
    }
  
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
     
    // 항상 마지막에 생성하기
    CRigidBody::RIGIDBODY_DESC RigidBodyDesc{};
    RigidBodyDesc.isStatic = false;
    RigidBodyDesc.isGravity = false;
    RigidBodyDesc.isUseClient = true;
    RigidBodyDesc.pOwnerTransform = m_pTransformCom;
    RigidBodyDesc.pOwnerNavigation = nullptr;

    RigidBodyDesc.pOwner = this;
    RigidBodyDesc.fStaticFriction = 0.f;
    RigidBodyDesc.fDynamicFriction = 0.f;
    RigidBodyDesc.fRestituion = 0.f;
    RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
        PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
        PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z 
        ;

    physX::GeometryBox BoxDesc;
    BoxDesc.vSize = _Vec3(1.f, 2.f, 1.f);
    RigidBodyDesc.pGeometry = &BoxDesc;

    /* FOR.Com_RigidBody */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
        TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
        return E_FAIL;

    return S_OK;
}

void CDeco_Collider::Calculate_Velocity(_float fTimeDelta)
{
    m_vVelocity = (m_vCurPos - m_vPrePos) / fTimeDelta;
   // m_vVelocity.x = 0.f;m_vVelocity.y = 0.f;m_vVelocity.z = 0.f;
}

void CDeco_Collider::Change_Vec4_to_Vec3(_Vec4 vVec4, _Vec3& vVec3)
{
    vVec3 = { vVec4.x, vVec4.y, vVec4.z };
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
    Safe_Release(m_pRigidBodyCom);
}
