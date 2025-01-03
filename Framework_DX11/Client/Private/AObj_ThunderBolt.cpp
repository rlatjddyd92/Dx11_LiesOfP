#include "stdafx.h"
#include "AObj_ThunderBolt.h"

#include "Raxasia.h"
#include "GameInstance.h"

#include "Effect_Manager.h"

CAObj_ThunderBolt::CAObj_ThunderBolt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_ThunderBolt::CAObj_ThunderBolt(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_ThunderBolt::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_ThunderBolt::Initialize(void* pArg)
{
    GAMEOBJECT_DESC GameObjDesc{};
    if (FAILED(__super::Initialize(&GameObjDesc)))
        return E_FAIL;

    ATKOBJ_DESC* pDesc = static_cast<ATKOBJ_DESC*>(pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

    m_pCopyRaxasia = static_cast<CRaxasia*>(pDesc->pOwner);

    m_vMoveDir = pDesc->vDir;
    m_vTargetPos = pDesc->vTargetPos;
    m_iStateTrack = pDesc->iTrack_State;

    m_vMoveDir.Normalize();


    m_pTransformCom->LookAt_Dir(_Vec4{ m_vMoveDir });

    if (FAILED(Ready_Components()))
        return E_FAIL;
    
    m_fDamageAmount = 20.f;
    m_fLifeDuration = 0.3f;
    m_fSpeed = 20.f;
    m_bCounter = false;

    m_fLifeTime = { 0.f };
    m_fLifeDuration = { 10.f };

    m_pColliderCom->IsActive(false);

    m_pColliderCom->Set_Owner(this);

    m_strObjectTag = TEXT("MonsterWeapon");

    if (m_iStateTrack == 0)
    {
        if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)
        {
            m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_Raxasia_SK_PJ_BladeMissile_01.wav"), &g_fEffectVolume);
        }
    }

    return S_OK;
}

void CAObj_ThunderBolt::Priority_Update(_float fTimeDelta)
{
    for (auto& pEffect : m_pEffects)
    {
        if (!pEffect->Get_Dead())
            pEffect->Priority_Update(fTimeDelta);
    }
}

void CAObj_ThunderBolt::Update(_float fTimeDelta)
{
    switch (m_iStateTrack)
    {
    case 0:         //번개
    {
        m_fTimer += fTimeDelta;
        if (m_fTimer >= 0.5f)
        {
            m_pEffects[m_iStateTrack]->Set_Loop(false);
            ++m_iStateTrack;
            m_pEffects[m_iStateTrack]->Reset_Effects();
            return;
        }
        break;
    }
    case 1:         //생성
    {
        if (!m_pEffects[m_iStateTrack]->Get_Loop())
        {
            ++m_iStateTrack;
            m_pColliderCom->IsActive(true);
            m_pEffects[m_iStateTrack]->Set_Loop(true);


            return;
        }
        break;
    }
    case 2:         //추적
    {
        _Vec3 vTargetPos = {};
        if (m_bCounter)
        {

            vTargetPos = _Vec3{ m_pCopyRaxasia->Calc_CenterPos(true) };            
        }
        else
        {
            vTargetPos = _Vec3{ m_pCopyRaxasia->Get_TargetPos() };
        }


    //    <<<<<< < HEAD
    //        vTargetPos = (_Vec3)m_pCopyRaxasia->Calc_CenterPos(true);
    //}
    //    else
    //    {
    //        vTargetPos = (_Vec3)m_pCopyRaxasia->Get_TargetPos();
    //        ====== =

        _Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

        _Vec3 vDir = { vTargetPos - vPos };
        _Vec3 vDirNoHeight = { vTargetPos };

        vDirNoHeight.y = 0.f;
        _Vec3 vCopyPos = vPos;
        vCopyPos.y = 0.f;

        vDirNoHeight = vDirNoHeight - vCopyPos;

        _Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
        _Vec3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);


        _Vec3 vDirCtr = vRight.Cross(vDirNoHeight);

        if (vDirCtr.y <= 0.f)
        {
            vLook.Normalize();
            vDir.Normalize();
            vLook += (vDir * 3.f);
            vLook.Normalize();
            m_vMoveDir = vLook;
        }

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + (m_vMoveDir * m_fSpeed * fTimeDelta));

        break;
    }
    case 3:         //피격
        if (m_pEffects[m_iStateTrack]->Get_Dead())
        {
            m_isDead = true;
            if (m_pSoundCom[EFF_SOUND_EFFECT2] != nullptr)
            {
                m_pSoundCom[EFF_SOUND_EFFECT2]->Stop();
            }
            return;
        }
        break;

    default:
        break;
    }


    if (m_fLifeDuration <= m_fLifeTime)
    {
        m_isDead = true;
    }
    else
    {
        m_fLifeTime += fTimeDelta;
    }
    if (!m_bCounter)
    {
        _Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        if (vPos.y <= m_vTargetPos.y)
        {
            m_isDead = true;
            CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_ThunderBolt_Impact_Ground "),
                _Vec3{ m_pTransformCom->Get_State(CTransform::STATE_POSITION) }, _Vec3{ m_pTransformCom->Get_State(CTransform::STATE_LOOK) });
        }
    }
    

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

    for (auto& pEffect : m_pEffects)
    {
        if (!pEffect->Get_Dead())
            pEffect->Update(fTimeDelta);
    }
}

void CAObj_ThunderBolt::Late_Update(_float fTimeDelta)
{
    for (auto& pEffect : m_pEffects)
    {
        if (!pEffect->Get_Dead())
            pEffect->Late_Update(fTimeDelta);
    }
    if (m_fLifeTime < m_fLifeDuration && !m_bImpact)
    {
        m_pGameInstance->Add_ColliderList(m_pColliderCom);
#ifdef DEBUG
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif // DEBUG
    }
}

HRESULT CAObj_ThunderBolt::Render()
{
#ifdef DEBUG
    m_pColliderCom->Render();
#endif // DEBUG
    return S_OK;
}

HRESULT CAObj_ThunderBolt::Render_LightDepth()
{
    return S_OK;
}

void CAObj_ThunderBolt::OnCollisionEnter(CGameObject* pOther)
{
    //pOther check
    if (pOther->Get_Tag() == TEXT("Player") || (m_bCounter && pOther->Get_Tag() == TEXT("Monster")))
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
            _bool bAttackCheck = pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, _Vec3{}, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);

            if (bAttackCheck)
            {
                //적중시
                m_pEffects[m_iStateTrack]->Set_Loop(false);
                m_iStateTrack = STATE_IMPACT;
                m_pEffects[STATE_IMPACT]->Reset_Effects();
                m_bImpact = true;
                if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)
                {
                    m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_Raxasia_SK_PJ_BladeMissile_Hit_03.wav"), &g_fEffectVolume);
                }

            }
            else
            {
                //패링시
                m_bCounter = true;//노말이랑 임팩트 카운터로 변경시키기 룩 방향도 돌려야 함

                _float fvariableX = m_pGameInstance->Get_Random(0.f, 2.f) - 1.f;
                _float fvariableY = m_pGameInstance->Get_Random(0.f, 1.f);

                _Vec3 vTargetPos = m_pCopyRaxasia->Get_Transform()->Get_State(CTransform::STATE_POSITION);
                _Vec3 vDir = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
                vDir.Normalize();

                _Vec3 vUp = { 0.f, 1.f, 0.f };
                _Vec3 vRight = vUp.Cross(vDir);
                vUp = vDir.Cross(vRight);

                vDir += (vUp * fvariableY) + (vRight * fvariableX);
                vDir.Normalize();

                m_pTransformCom->Look_Dir(_Vec4{ vDir });

                m_pEffects[STATE_NORMAL]->Set_Cloned(false);
                Safe_Release(m_pEffects[STATE_NORMAL]);

                m_pEffects[STATE_IMPACT]->Set_Cloned(false);
                Safe_Release(m_pEffects[STATE_IMPACT]);

                //m_fHeightGap *= -1.f;
                m_fDamageAmount = 5.f;
                m_strObjectTag = TEXT("PlayerWeapon");
                m_fLifeTime = 0.f;
                m_pEffects[STATE_NORMAL] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderBolt_Counter"), m_pTransformCom->Get_WorldMatrix_Ptr(),
                    nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));
                m_pEffects[STATE_NORMAL]->Set_Loop(true);

                m_pEffects[STATE_IMPACT] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderBolt_Counter_Impact"), m_pTransformCom->Get_WorldMatrix_Ptr(),
                    nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

                if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)
                {
                    m_pSoundCom[EFF_SOUND_EFFECT1]->Play2D(TEXT("SE_NPC_Raxasia_SK_PJ_Reflection_PerfectGuard_03.wav"), &g_fEffectVolume);
                }

                if (m_pSoundCom[EFF_SOUND_EFFECT2] != nullptr)
                {
                    m_pSoundCom[EFF_SOUND_EFFECT2]->Play2D(TEXT("SE_NPC_Raxasia_SK_PJ_Reflection_PerfectGuard_Loop_01.wav"), &g_fEffectVolume);
                }

            }
        }
    }
}

void CAObj_ThunderBolt::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_ThunderBolt::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_ThunderBolt::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;

    /* FOR.Com_EffectSound */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
        TEXT("Com_EffectSound_2"), reinterpret_cast<CComponent**>(&m_pSoundCom[EFF_SOUND_EFFECT2]))))
        return E_FAIL;
    m_pSoundCom[EFF_SOUND_EFFECT2]->Set_Owner(this);

    /* FOR.Com_Collider */
    CBounding_Sphere::BOUNDING_SPHERE_DESC      ColliderDesc{};
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ColliderDesc.fRadius = 2.f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pColliderCom->Set_Owner(this);

    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_pEffects[STATE_SIGN] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderBolt_Sign"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));
    m_pEffects[STATE_SIGN]->Set_Loop(false);

    m_pEffects[STATE_CREATE] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderBolt_Create"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffects[STATE_NORMAL] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderBolt"), m_pTransformCom->Get_WorldMatrix_Ptr(),
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffects[STATE_IMPACT] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderBolt_Impact"), m_pTransformCom->Get_WorldMatrix_Ptr(),
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));


    if (m_iStateTrack == 0 || m_iStateTrack == 2)
    {
        m_pEffects[m_iStateTrack]->Set_Loop(true);
    }
    else
    {
        m_pEffects[m_iStateTrack]->Reset_Effects();
    }

    return S_OK;
}

CAObj_ThunderBolt* CAObj_ThunderBolt::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_ThunderBolt* pInstance = new CAObj_ThunderBolt(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_ThunderBolt"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_ThunderBolt::Clone(void* pArg)
{
    CAObj_ThunderBolt* pInstance = new CAObj_ThunderBolt(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_ThunderBolt"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_ThunderBolt::Free()
{
    __super::Free();

    if (true == m_isCloned)
    {
        for (_int i = 0; i < STATE_END; ++i)
        {
            m_pEffects[i]->Set_Cloned(false);
            Safe_Release(m_pEffects[i]);
        }
    }
}
