#include "stdafx.h"
#include "AObj_Wave.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

CAObj_Wave::CAObj_Wave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_Wave::CAObj_Wave(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_Wave::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_Wave::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

    m_vMoveDir = pDesc->vDir;

    m_pTransformCom->LookAt_Dir(_Vec4{ m_vMoveDir });

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fDamageAmount = 20.f;
    m_fLifeDuration = 3.f;
    m_fSpeed = 10.f;
    
    m_pColliderCom->IsActive(true);

    m_pColliderCom->Set_Owner(this);

    m_strObjectTag = TEXT("MonsterWeapon");

    m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_SimonManus_SK_PJ_Ergo_Retspuken_04.wav"), &g_fEffectVolume);

    return S_OK;
}

void CAObj_Wave::Priority_Update(_float fTimeDelta)
{
    m_pEffect->Priority_Update(fTimeDelta);
}

void CAObj_Wave::Update(_float fTimeDelta)
{
    if (m_fLifeTime >= m_fLifeDuration)
    {
        if (m_pEffect->Get_Dead())
        {
            m_pEffect->Set_Loop(false);
        }
        else if (m_pEffect->Get_Dead())
        {
            m_isDead = true;
            m_pSoundCom[EFF_SOUND_EFFECT1]->Stop();
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

void CAObj_Wave::Late_Update(_float fTimeDelta)
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

HRESULT CAObj_Wave::Render()
{
    //if (FAILED(__super::Render()))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CAObj_Wave::Render_LightDepth()
{
    //if (FAILED(__super::Render_LightDepth()))
    //    return E_FAIL;

    return S_OK;
}

void CAObj_Wave::OnCollisionEnter(CGameObject* pOther)
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
        m_pEffect->Set_Loop(false);
    }
}

void CAObj_Wave::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_Wave::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_Wave::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;

    /* FOR.Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC      ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.f, 0.6f, 1.f);
    ColliderDesc.vCenter = _float3(0.f, 0.3f, 0.f);
    ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_Wave"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffect->Set_Loop(true);

    return S_OK;
}

CAObj_Wave* CAObj_Wave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_Wave* pInstance = new CAObj_Wave(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_Wave"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_Wave::Clone(void* pArg)
{
    CAObj_Wave* pInstance = new CAObj_Wave(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_Wave"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_Wave::Free()
{
    __super::Free();

    Safe_Release(m_pEffect);
}
