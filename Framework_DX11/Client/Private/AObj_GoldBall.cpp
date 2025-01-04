#include "stdafx.h"
#include "AObj_GoldBall.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

#include "EffectObject.h"

CAObj_GoldBall::CAObj_GoldBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_GoldBall::CAObj_GoldBall(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_GoldBall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_GoldBall::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

    m_vMoveDir = pDesc->vDir;

    m_bSoundControl = pDesc->bSoundControl;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fDamageAmount = 270.f;

    m_fLifeDuration = 2.f;
    m_fSpeed = 15.f;

    m_pColliderCom->IsActive(true);

    m_pColliderCom->Set_Owner(this);

    m_strObjectTag = TEXT("MonsterWeapon");

    if (m_bSoundControl)
    {
        if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)
        {
            m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_SimonManus_SK_PJ_Ergo_Direct_01.wav"), &g_fEffectVolume);
        }
    }

    return S_OK;
}

void CAObj_GoldBall::Priority_Update(_float fTimeDelta)
{
    m_pEffect->Priority_Update(fTimeDelta);
}

void CAObj_GoldBall::Update(_float fTimeDelta)
{
    if (m_fLifeTime >= m_fLifeDuration)
    {
        if (!m_bDelCheck)
        {
            m_pEffect->Set_Loop(false);
            m_bDelCheck = true;
        }
        if (m_pEffect->Get_Dead())
        {
            m_isDead = true;
        }


    }
    else
    {
        m_fLifeTime += fTimeDelta;
    }

    //¿òÁ÷ÀÓ
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vMoveDir * m_fSpeed * fTimeDelta);


    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

    m_pEffect->Update(fTimeDelta);
}

void CAObj_GoldBall::Late_Update(_float fTimeDelta)
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

HRESULT CAObj_GoldBall::Render()
{
    //if (FAILED(__super::Render()))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CAObj_GoldBall::Render_LightDepth()
{
    //if (FAILED(__super::Render_LightDepth()))
    //    return E_FAIL;

    return S_OK;
}

void CAObj_GoldBall::OnCollisionEnter(CGameObject* pOther)
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
            m_isDead = true;

            CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("SimonManus_Attack_GoldBall_Impact"),
                _Vec3{ m_pTransformCom->Get_State(CTransform::STATE_POSITION) }, _Vec3{ m_pTransformCom->Get_State(CTransform::STATE_LOOK) });
        }
        m_pEffect->Set_Loop(false);
    }
}

void CAObj_GoldBall::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_GoldBall::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_GoldBall::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;

    /* FOR.Com_Collider */
    CBounding_Sphere::BOUNDING_SPHERE_DESC      ColliderDesc{};
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ColliderDesc.fRadius = 0.5f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_GoldBall"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffect->Set_Loop(true);

    return S_OK;
}

CAObj_GoldBall* CAObj_GoldBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_GoldBall* pInstance = new CAObj_GoldBall(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_ThunderSpear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_GoldBall::Clone(void* pArg)
{
    CAObj_GoldBall* pInstance = new CAObj_GoldBall(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_GoldBall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_GoldBall::Free()
{
    __super::Free();

    if (true == m_isCloned)
    {
        m_pEffect->Set_Cloned(false);
        Safe_Release(m_pEffect);
    }
}
