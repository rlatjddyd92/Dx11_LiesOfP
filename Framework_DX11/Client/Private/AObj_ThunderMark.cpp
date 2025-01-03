#include "stdafx.h"
#include "AObj_ThunderMark.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

CAObj_ThunderMark::CAObj_ThunderMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_ThunderMark::CAObj_ThunderMark(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_ThunderMark::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_ThunderMark::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fDamageAmount = 28.f;
    m_fLifeDuration = 2.f;

    m_pColliderCom->IsActive(true);

    m_pColliderCom->Set_Owner(this);
    
    m_pEffect->Set_Loop(true);

    m_strObjectTag = TEXT("MonsterWeapon");

    if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)
    {
        m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Ground_Loop_01.wav"), &g_fEffectVolume, true);
    }

    return S_OK;
}

void CAObj_ThunderMark::Priority_Update(_float fTimeDelta)
{
    if (!m_bExplosive)
    {
        m_pEffect->Priority_Update(fTimeDelta);
    }
    else
    {
        m_pEffectExp->Priority_Update(fTimeDelta);
    }
}

void CAObj_ThunderMark::Update(_float fTimeDelta)
{
    if (!m_bExplosive)
    {
        if (m_fLifeTime >= m_fLifeDuration)
        {
            m_pEffectExp->Reset_Effects();
            m_bExplosive = true;
            if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)
            {
                m_pSoundCom[EFF_SOUND_EFFECT1]->Stop();

                m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Ground_Explo_01.wav"), &g_fEffectVolume, false);
            }

        }
        else
        {
            m_fLifeTime += fTimeDelta;
        }

        m_pEffect->Update(fTimeDelta);
    }
    else
    {
        if (m_pEffectExp->Get_Dead())
        {
            m_isDead = true;
        }
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
        m_pEffectExp->Update(fTimeDelta);
    }


}

void CAObj_ThunderMark::Late_Update(_float fTimeDelta)
{
    if (!m_bExplosive)
    {
        m_pEffect->Late_Update(fTimeDelta);
    }
    else
    {
        m_pEffectExp->Late_Update(fTimeDelta);
        if (!m_isDead)
        {
            m_pGameInstance->Add_ColliderList(m_pColliderCom);
        }
    }
    if (m_fLifeTime < m_fLifeDuration)
    {
#ifdef DEBUG
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif // DEBUG
    }
}

HRESULT CAObj_ThunderMark::Render()
{
#ifdef DEBUG
    m_pColliderCom->Render();
#endif // DEBUG
    return S_OK;
}

HRESULT CAObj_ThunderMark::Render_LightDepth()
{
    return S_OK;
}

void CAObj_ThunderMark::OnCollisionEnter(CGameObject* pOther)
{
}

void CAObj_ThunderMark::OnCollisionStay(CGameObject* pOther)
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
            pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, _Vec3{}, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
    }
}

void CAObj_ThunderMark::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_ThunderMark::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;

    /* FOR.Com_Collider */
    CBounding_Sphere::BOUNDING_SPHERE_DESC      ColliderDesc{};
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ColliderDesc.fRadius = 0.7f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderDrag_Mark"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f));

    m_pEffectExp = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderDrag_Mark_Explosion"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f));

    return S_OK;
}

CAObj_ThunderMark* CAObj_ThunderMark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_ThunderMark* pInstance = new CAObj_ThunderMark(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_ThunderMark"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_ThunderMark::Clone(void* pArg)
{
    CAObj_ThunderMark* pInstance = new CAObj_ThunderMark(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_ThunderMark"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_ThunderMark::Free()
{
    __super::Free();

    if (true == m_isCloned)
    {
        m_pEffect->Set_Cloned(false);
        Safe_Release(m_pEffect);
        m_pEffectExp->Set_Cloned(false);
        Safe_Release(m_pEffectExp);
    }
}
