#include "stdafx.h"
#include "State_SimonManusP2_HighJumpFall.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "EffectObject.h"
#include "Effect_Manager.h"
#include "AttackObject.h"

CState_SimonManusP2_HighJumpFall::CState_SimonManusP2_HighJumpFall(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_HighJumpFall::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_HighJumpFall::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_HIGHJUMPFALL, false, 0.1f, 0);

    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);
    m_isRimLight = false;

    m_bStartSpot = true;
    m_bStompAttack = false;

    return S_OK;
}

void CState_SimonManusP2_HighJumpFall::Update(_float fTimeDelta)
{
    m_fCurrentTime += fTimeDelta;

    _double CurTrackPos{};
    CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (!m_isRimLight)
    {
        if (CurTrackPos >= 100.f)
        {
            m_isRimLight = true;
            m_pMonster->On_PowerAttack(true);
        }
    }

    if (CurTrackPos >= 200.f && CurTrackPos < 230.f) //점프 이후 공중 체공 + 플레이어방향 회전
    {
        m_vTargetDir = m_pMonster->Get_TargetDir();
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 2.5f, fTimeDelta);
    }
    else if (CurTrackPos >= 230.f && CurTrackPos <= 245.f) //땅 찍기까지
    {
        if (m_bStartSpot)
        {
            _float fLength = m_vTargetDir.Length();
            if (fLength > 12.f)
            {
                fLength = 12.f;
            }

            m_vTargetDir = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK)) * fLength;
            m_bStartSpot = false;
        }

        _Vec3 vMove = m_vTargetDir * (((_float)CurTrackPos - 230.f) / 15.f);
        m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vFlyMoveStack) / fTimeDelta);
        m_vFlyMoveStack = vMove;
    }


    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Update_Rimlight(fTimeDelta, CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_SimonManusP2_HighJumpFall::End_State()
{
    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);
    m_pMonster->On_PowerAttack(false);
    m_pMonster->Set_RimLightColor(m_vRimLightColor);
}

_bool CState_SimonManusP2_HighJumpFall::End_Check()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
    return m_pMonster->Get_EndAnim(AN_HIGHJUMPFALL);
}

void CState_SimonManusP2_HighJumpFall::Collider_Check(_double CurTrackPos)
{
    if (CurTrackPos >= 135.f && CurTrackPos <= 150.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(0);
    }

    if (!m_bStompAttack)
    {
        if (CurTrackPos >= 255.f)
        {
            CAttackObject::ATKOBJ_DESC Desc;
            Desc.vPos = _Vec3{ m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION) };
            Desc.vDir = _Vec3{ m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK) };

            Desc.pOwner = m_pMonster;

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_Stomp"), &Desc);
            m_bStompAttack = true;
        }
    }

}

void CState_SimonManusP2_HighJumpFall::Effect_Check(_double CurTrackPos)
{
    if (CurTrackPos >= 135.f && CurTrackPos <= 150.f)
    {
        if (!m_bStampEffect)
        {
            _float4x4 WorldMat{};
            _Vec3 vPos = { 0.f, 0.f, -1.75f };
            XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(6) * (*m_pMonster->Get_WeaponWorldMat())));

            vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
            vPos.y = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;

            CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("SimonManus_Attack_Stamp"),
                vPos, _Vec3{ 0.f, 0.f, 1.f });

            m_bStampEffect = true;
        }
    }
}

void CState_SimonManusP2_HighJumpFall::Update_Rimlight(_float fTimeDelta, _double CurTrackPos)
{
    if (m_isRimLight)
    {
        if (CurTrackPos < 240.f)
        {
            m_vRimLightColor.x = max(m_vRimLightColor.x + 0.5f * fTimeDelta, 1.f);
            m_vRimLightColor.w = max(m_vRimLightColor.w - 0.6f * fTimeDelta, 0.1f);
        }
        else
        {
            m_vRimLightColor.x = max(m_vRimLightColor.x - 0.7f * fTimeDelta, 0.f);
            m_vRimLightColor.w = min(m_vRimLightColor.w + 0.7f * fTimeDelta, 0.5f);
        }

        if (m_vRimLightColor.x == 0.f && m_vRimLightColor.w == 0.5f)
            m_pMonster->On_PowerAttack(false);

        m_pMonster->Set_RimLightColor(m_vRimLightColor);
    }
}

void CState_SimonManusP2_HighJumpFall::Control_Sound(_double CurTrackPos)
{
    if (!m_bStampSound)
    {
        if ((CurTrackPos >= 150.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"));
            m_bStampSound = true;
        }
    }

    if (!m_bLandSound)
    {
        if (CurTrackPos >= 245.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_MT_Dust_M_02.wav"));
            m_bLandSound = true;
        }
    }
}

CState_SimonManusP2_HighJumpFall* CState_SimonManusP2_HighJumpFall::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_HighJumpFall* pInstance = new CState_SimonManusP2_HighJumpFall(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_HighJumpFall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_HighJumpFall::Free()
{
    __super::Free();
}