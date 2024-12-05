#include "stdafx.h"
#include "AObj_GodHands.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

CAObj_GodHands::CAObj_GodHands(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_GodHands::CAObj_GodHands(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_GodHands::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_GodHands::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fDamageAmount = 20.f;
    m_fLifeDuration = 0.6f;
    m_pColliderCom->IsActive(true);

    m_pColliderCom->Set_Owner(this);

    m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"), &g_fEffectVolume);

    m_strObjectTag = TEXT("MonsterWeapon");

    return S_OK;
}

void CAObj_GodHands::Priority_Update(_float fTimeDelta)
{
    m_pEffect->Priority_Update(fTimeDelta);
}

void CAObj_GodHands::Update(_float fTimeDelta)
{
    if (m_fLifeTime >= m_fLifeDuration)
    {
        if (m_pEffect->Get_Dead())
        {
            m_isDead = true;
        }
    }
    else
    {
        m_fLifeTime += fTimeDelta;
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

    m_pEffect->Update(fTimeDelta);
}

void CAObj_GodHands::Late_Update(_float fTimeDelta)
{
    m_pEffect->Late_Update(fTimeDelta);
    if (m_fLifeTime < m_fLifeDuration)
    {
        m_pGameInstance->Add_ColliderList(m_pColliderCom);
#ifdef DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif // DEBUG
    }
}

HRESULT CAObj_GodHands::Render()
{
    //if (FAILED(__super::Render()))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CAObj_GodHands::Render_LightDepth()
{
    //if (FAILED(__super::Render_LightDepth()))
    //    return E_FAIL;

    return S_OK;
}

void CAObj_GodHands::OnCollisionEnter(CGameObject* pOther)
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

void CAObj_GodHands::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_GodHands::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_GodHands::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;

    /* FOR.Com_Collider */
    CBounding_Sphere::BOUNDING_SPHERE_DESC      ColliderDesc{};
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ColliderDesc.fRadius = 3.f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_ChargeStamp2"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffect->Reset_Effects();

    return S_OK;
}

CAObj_GodHands* CAObj_GodHands::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_GodHands* pInstance = new CAObj_GodHands(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_GodHands"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_GodHands::Clone(void* pArg)
{
    CAObj_GodHands* pInstance = new CAObj_GodHands(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_GodHands"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_GodHands::Free()
{
    __super::Free();

    m_pEffect->Set_Cloned(false);
    Safe_Release(m_pEffect);
}
