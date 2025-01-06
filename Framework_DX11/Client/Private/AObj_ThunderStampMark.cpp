#include "stdafx.h"
#include "AObj_ThunderStampMark.h"

#include "GameInstance.h"
#include "Pawn.h"

#include "Effect_Manager.h"

CAObj_ThunderStampMark::CAObj_ThunderStampMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_ThunderStampMark::CAObj_ThunderStampMark(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_ThunderStampMark::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_ThunderStampMark::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pOwner = pDesc->pOwner;
    Safe_AddRef(m_pOwner);

    if (pDesc->iTrack_State == 1)
    {
        m_bExplosive = true;
        m_pEffect->Set_Loop(false);
        m_pEffectExp->Reset_Effects();
    }
    else
    {
        m_pEffect->Set_Loop(true);
    }

    m_fDamageAmount = 250.f;
    m_fLifeDuration = 1.f;

    m_pColliderCom->IsActive(true);

    m_pColliderCom->Set_Owner(this);

    m_strObjectTag = TEXT("MonsterWeapon");

    if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)
    {
        m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Ground_Loop_01.wav"), &g_fEffectVolume, true);
    }

    return S_OK;
}

void CAObj_ThunderStampMark::Priority_Update(_float fTimeDelta)
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

void CAObj_ThunderStampMark::Update(_float fTimeDelta)
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
            }

            if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)
            {
                m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_Raxasia_SK_PJ_Thunder_Explo_03.wav"), &g_fEffectVolume, false);
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
        m_pEffectExp->Update(fTimeDelta);
    }

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

}

void CAObj_ThunderStampMark::Late_Update(_float fTimeDelta)
{
    if (m_pOwner->Get_Dead())
    {
        m_isDead = true;
    }
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

    if (m_fLifeTime < m_fLifeDuration && m_bExplosive)
    {
#ifdef DEBUG
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif // DEBUG
    }
}

HRESULT CAObj_ThunderStampMark::Render()
{
#ifdef DEBUG
    m_pColliderCom->Render();
#endif // DEBUG
    return S_OK;
}

HRESULT CAObj_ThunderStampMark::Render_LightDepth()
{
    return S_OK;
}

void CAObj_ThunderStampMark::OnCollisionEnter(CGameObject* pOther)
{
    if (m_pOwner->Get_IsDieState())
    {
        return;
    }
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
            pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, _Vec3{}, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
    }
}

void CAObj_ThunderStampMark::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_ThunderStampMark::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_ThunderStampMark::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;

    /* FOR.Com_Collider */
    CBounding_Sphere::BOUNDING_SPHERE_DESC      ColliderDesc{};
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ColliderDesc.fRadius = 2.f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderStamp_Mark"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f));

    m_pEffectExp = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderStamp_Mark_Explosion"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f));

    return S_OK;
}

CAObj_ThunderStampMark* CAObj_ThunderStampMark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_ThunderStampMark* pInstance = new CAObj_ThunderStampMark(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_ThunderStampMark"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_ThunderStampMark::Clone(void* pArg)
{
    CAObj_ThunderStampMark* pInstance = new CAObj_ThunderStampMark(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_ThunderStampMark"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_ThunderStampMark::Free()
{
    Safe_Release(m_pOwner);

    __super::Free();

    if (true == m_isCloned)
    {
        m_pEffect->Set_Cloned(false);
        Safe_Release(m_pEffect);
        m_pEffectExp->Set_Cloned(false);
        Safe_Release(m_pEffectExp);
    }
}
