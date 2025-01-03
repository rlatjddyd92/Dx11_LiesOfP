#include "stdafx.h"
#include "AObj_Stomp.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

CAObj_Stomp::CAObj_Stomp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_Stomp::CAObj_Stomp(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_Stomp::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_Stomp::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
    m_pTransformCom->Look_Dir(_Vec4{pDesc->vDir});

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fDamageAmount = 320.f;
    m_fLifeDuration = 0.2f;
    m_pColliderCom->IsActive(true);

    m_strObjectTag = TEXT("MonsterWeapon");

    return S_OK;
}

void CAObj_Stomp::Priority_Update(_float fTimeDelta)
{
    //m_pEffect->Priority_Update(fTimeDelta);
}

void CAObj_Stomp::Update(_float fTimeDelta)
{
    if (m_fLifeTime >= m_fLifeDuration)
    {
        m_isDead = true;
    }
    else
    {
        m_fLifeTime += fTimeDelta;
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

    //m_pEffect->Update(fTimeDelta);
}

void CAObj_Stomp::Late_Update(_float fTimeDelta)
{
    //m_pEffect->Late_Update(fTimeDelta);
    if (m_fLifeTime < m_fLifeDuration)
    {
        m_pGameInstance->Add_ColliderList(m_pColliderCom);
#ifdef DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif // DEBUG
    }
}

HRESULT CAObj_Stomp::Render()
{
    //if (FAILED(__super::Render()))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CAObj_Stomp::Render_LightDepth()
{
    //if (FAILED(__super::Render_LightDepth()))
    //    return E_FAIL;

    return S_OK;
}

void CAObj_Stomp::OnCollisionEnter(CGameObject* pOther)
{
}

void CAObj_Stomp::OnCollisionStay(CGameObject* pOther)
{
    //pOther check
    if (pOther->Get_Tag() == TEXT("Player"))
    {
        _bool bOverlapCheck = false;
        for (auto& pObj : m_DamagedObjects)
        {
            if (pObj == pOther)
            {
                bOverlapCheck = true;
                break;
            }
        }

        if (!bOverlapCheck)
        {
            m_DamagedObjects.push_back(pOther);
            pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, _Vec3{}, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_STRONG);
        }
    }
}

void CAObj_Stomp::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_Stomp::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;

    /* FOR.Com_Collider */		//Body
    CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(2.f, 0.1f, 2.f);
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    //const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    //
    //m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_ChargeStamp2"), pParetnMatrix,
    //    nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));
    //
    //m_pEffect->Reset_Effects();

    return S_OK;
}

CAObj_Stomp* CAObj_Stomp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_Stomp* pInstance = new CAObj_Stomp(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_Stomp"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_Stomp::Clone(void* pArg)
{
    CAObj_Stomp* pInstance = new CAObj_Stomp(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_Stomp"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_Stomp::Free()
{
    __super::Free();
    //if (nullptr != m_pEffect)
    //{
    //    m_pEffect->Set_Cloned(false);
    //}
    //Safe_Release(m_pEffect);
}
