#include "stdafx.h"
#include "AObj_BlackBall.h"

#include "GameInstance.h"
#include "Pawn.h"

#include "Effect_Manager.h"

CAObj_BlackBall::CAObj_BlackBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAttackObject{ pDevice, pContext }
{
}

CAObj_BlackBall::CAObj_BlackBall(const CGameObject& Prototype)
    : CAttackObject{ Prototype }
{
}

HRESULT CAObj_BlackBall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAObj_BlackBall::Initialize(void* pArg)
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

    m_bSoundControl = pDesc->bSoundControl;

    m_vMoveDir = pDesc->vDir;
    m_vMoveDir.Normalize();

    m_pTransformCom->Look_Dir(_Vec4{ m_vMoveDir });


    m_fLifeDuration = 0.6f;

    m_fSpeed = 5.f;

    m_fThrowTime = 1.5f;
    m_fDelayTime = 3.f;

    m_pCopyPlayerTransformCom = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag));

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

void CAObj_BlackBall::Priority_Update(_float fTimeDelta)
{
    for (auto& pEffect : m_pEffects)
    {
        if (!pEffect->Get_Dead())
            pEffect->Priority_Update(fTimeDelta);
    }
}

void CAObj_BlackBall::Update(_float fTimeDelta)
{
    if (!m_bShoted)
    {
        m_fLifeTime += fTimeDelta;
        switch (m_iThrowPhase)
        {
        case 0:
            //이동
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, 
                _Vec3{ m_pTransformCom->Get_State(CTransform::STATE_POSITION) + 
                m_vMoveDir * m_fSpeed * fTimeDelta});

            if (m_fLifeTime >= m_fThrowTime)
            {
                ++m_iThrowPhase;
                m_fLifeTime = 0.f;

                m_pEffects[STATE_START]->Set_Loop(false);
                m_pEffects[STATE_MIDDLE]->Reset_Effects();
                m_pEffects[STATE_LAST]->Set_Loop(true);
            }
            break;

        case 1:
            if (m_fLifeTime >= m_fDelayTime)
            {
                m_fLifeTime = 0.f;
                m_pEffects[STATE_LAST]->Set_Loop(false);
                m_bShoted = true;
            }
            break;

        default:
            break;
        }
    }
    else 
    {
        if (m_bEnd)
        {
            if (m_pEffects[STATE_LAST]->Get_Dead())
            {
                m_isDead = true;
            }
        }
        else
        {
            if (m_pSoundCom[EFF_SOUND_EFFECT1] != nullptr)
            {
                m_pSoundCom[EFF_SOUND_EFFECT1]->Stop();
            }

            //골드볼 생성
            CAttackObject::ATKOBJ_DESC Desc;
            _float4x4 WorldMat{};
            Desc.vPos = _Vec3{ m_pTransformCom->Get_State(CTransform::STATE_POSITION) };

            _Vec3 vTargetPos = m_pCopyPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
            vTargetPos.y += 1.5f;

            Desc.vDir = _Vec3{ ( vTargetPos - Desc.vPos) };
            Desc.vDir.Normalize();

            Desc.bSoundControl = m_bSoundControl;

            Desc.pOwner = m_pOwner;

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack_Extra"), TEXT("Prototype_GameObject_GoldBall"), &Desc);
            
            m_bEnd = true;

            return;
        }
    }

    for (auto& pEffect : m_pEffects)
    {
        if (!pEffect->Get_Dead())
            pEffect->Update(fTimeDelta);
    }
}

void CAObj_BlackBall::Late_Update(_float fTimeDelta)
{
    if (m_pOwner->Get_Dead())
    {
        m_isDead = true;
    }

    for (auto& pEffect : m_pEffects)
    {
        if (!pEffect->Get_Dead())
            pEffect->Late_Update(fTimeDelta);
    }
}

HRESULT CAObj_BlackBall::Render()
{
    //if (FAILED(__super::Render()))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CAObj_BlackBall::Render_LightDepth()
{
    //if (FAILED(__super::Render_LightDepth()))
    //    return E_FAIL;

    return S_OK;
}

void CAObj_BlackBall::OnCollisionEnter(CGameObject* pOther)
{
}

void CAObj_BlackBall::OnCollisionStay(CGameObject* pOther)
{
}

void CAObj_BlackBall::OnCollisionExit(CGameObject* pOther)
{
}

HRESULT CAObj_BlackBall::Ready_Components()
{
    if (FAILED(__super::Ready_Components()))
        return E_FAIL;


    const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    //루프 리셋 루프
    m_pEffects[STATE_START] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_BlackBall_Throw"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffects[STATE_MIDDLE] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_BlackBall_Impact"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffects[STATE_LAST] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_BlackBall_Small"), pParetnMatrix,
        nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

    m_pEffects[STATE_START]->Set_Loop(true);


    return S_OK;
}

CAObj_BlackBall* CAObj_BlackBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAObj_BlackBall* pInstance = new CAObj_BlackBall(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAObj_BlackBall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAObj_BlackBall::Clone(void* pArg)
{
    CAObj_BlackBall* pInstance = new CAObj_BlackBall(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAObj_BlackBall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAObj_BlackBall::Free()
{
    Safe_Release(m_pOwner);

    __super::Free();

    if (true == m_isCloned)
    {
        for (auto& pEffect : m_pEffects)
        {
            pEffect->Set_Cloned(false);
            Safe_Release(pEffect);
        }
    }
}
