#include "stdafx.h"
#include "AObj_Thunder.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

CAObj_Thunder::CAObj_Thunder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_Thunder::CAObj_Thunder(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_Thunder::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_Thunder::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fDamageAmount = 20.f;
    m_fLifeDuration = 0.4f;
    m_fAttackTime = 1.5f;

    m_pColliderCom->IsActive(false);

    m_pColliderCom->Set_Owner(this);

    m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"), &g_fEffectVolume);

    m_strObjectTag = TEXT("MonsterWeapon");

    return S_OK;
}

void CAObj_Thunder::Priority_Update(_float fTimeDelta)
{
    if (!m_bAttack)
    {
        m_pSignEffect->Priority_Update(fTimeDelta);
    }
    else
    {
        m_pEffect->Priority_Update(fTimeDelta);
    }
}

void CAObj_Thunder::Update(_float fTimeDelta)
{
    if (!m_bAttack)
    {
        if (m_fLifeTime >= m_fAttackTime)
        {
            m_fLifeTime = 0.f;
            m_bAttack = true;
            m_pColliderCom->IsActive(true);
            m_pSignEffect->Set_Loop(false);
            m_pEffect->Reset_Effects();
        }
        else
            m_fLifeTime += fTimeDelta;
    }
    else
    {
        if (m_pEffect->Get_Dead())
        {
            m_isDead = true;
        }
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

    if (!m_bAttack)
    {
        m_pSignEffect->Update(fTimeDelta);
    }
    else
    {
        m_pEffect->Update(fTimeDelta);
    }
}

void CAObj_Thunder::Late_Update(_float fTimeDelta)
{
    if (!m_bAttack)
    {
        m_pSignEffect->Late_Update(fTimeDelta);
    }
    else
    {
        m_pEffect->Late_Update(fTimeDelta);
    }

    if (m_fLifeTime < m_fLifeDuration)
    {
        m_pGameInstance->Add_ColliderList(m_pColliderCom);
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
    }
}

HRESULT CAObj_Thunder::Render()
{
    //if (FAILED(__super::Render()))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CAObj_Thunder::Render_LightDepth()
{
    //if (FAILED(__super::Render_LightDepth()))
    //    return E_FAIL;

    return S_OK;
}

void CAObj_Thunder::OnCollisionEnter(CGameObject* pOther)
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
            pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio);
        }
    }
}

void CAObj_Thunder::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_Thunder::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_Thunder::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;

    /* FOR.Com_Collider */
    CBounding_Sphere::BOUNDING_SPHERE_DESC      ColliderDesc{};
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ColliderDesc.fRadius = 1.5f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_LightningDown"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffect->Reset_Effects();

    m_pSignEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_LightningSign"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));


    return S_OK;
}

CAObj_Thunder* CAObj_Thunder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_Thunder* pInstance = new CAObj_Thunder(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_Thunder"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_Thunder::Clone(void* pArg)
{
    CAObj_Thunder* pInstance = new CAObj_Thunder(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_Thunder"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_Thunder::Free()
{
    __super::Free();

    Safe_Release(m_pEffect);
}
