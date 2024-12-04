#include "stdafx.h"
#include "State_SimonManusP1_HighJumpFall.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "EffectObject.h"

CState_SimonManusP1_HighJumpFall::CState_SimonManusP1_HighJumpFall(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_HighJumpFall::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_HighJumpFall::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_HIGHJUMPFALL, false, 0.1f, 0);
    m_bResetRim = false;

    m_fGoalRimAlpha = 0.1f;
    m_fCurtRimAlpha = 1.f;

    //m_pMonster->Set_RimLightColor(_Vec4{0.9f, 0.f, 0.f, 1.f});
    m_bStartSpot = true;
    m_bStampSound = false;
    m_bLandSound = false;

    return S_OK;
}

void CState_SimonManusP1_HighJumpFall::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos >= 200.f && CurTrackPos < 230.f) //점프 이후 공중 체공 + 플레이어방향 회전
    {
        m_vTargetDir = m_pMonster->Get_TargetDir();
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 2.2f, fTimeDelta);
    }
    else if(CurTrackPos >= 230.f && CurTrackPos <= 245.f) //땅 찍기까지
    {
        if (m_bStartSpot)
        {
            _float fLength = m_vTargetDir.Length();
            m_vTargetDir = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK)) * fLength;
            m_bStartSpot = false;
        }

        _Vec3 vMove = m_vTargetDir * (((_float)CurTrackPos - 230.f) / 15.f);
        m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vFlyMoveStack) / fTimeDelta);
        m_vFlyMoveStack = vMove;
    }

    if (!m_bResetRim)
    {
        if (CurTrackPos >= 250.f)
        {
            m_fGoalRimAlpha = 1.f;
            m_bResetRim = true;
        }
    }

    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Update_Rimlight();
    Control_Sound(CurTrackPos);

}

void CState_SimonManusP1_HighJumpFall::End_State()
{
    m_vFlyMoveStack = _vector{0, 0, 0, 0};
    m_pMonster->Set_RimLightColor(_Vec4{ 0.f, 0.f, 0.f, 1.f });
}

_bool CState_SimonManusP1_HighJumpFall::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_HIGHJUMPFALL);
}

void CState_SimonManusP1_HighJumpFall::Collider_Check(_double CurTrackPos)
{

    if ((CurTrackPos >= 140.f && CurTrackPos <= 150.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

void CState_SimonManusP1_HighJumpFall::Effect_Check(_double CurTrackPos)
{
    if (!m_bStampEffect)
    {
        if ((CurTrackPos >= 150.f))
        {
            CEffectObject::EFFECTOBJ_DESC Desc{};
            Desc.fLifeDuration = 1.5f;
            Desc.strEffectTag = TEXT("SimonManus_Attack_Stamp");
            _float4x4 WorldMat{};

            XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(6) * (*m_pMonster->Get_WeaponWorldMat())));
            Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42, WorldMat._43 };

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_SpotEffect"), &Desc);

            m_bStampEffect = true;
        }
    }
}

void CState_SimonManusP1_HighJumpFall::Update_Rimlight()
{
    if (m_fCurtRimAlpha != m_fGoalRimAlpha)
    {
        m_fCurtRimAlpha += (m_fGoalRimAlpha - m_fCurtRimAlpha) / 20;
        m_pMonster->Set_RimLightColor(_Vec4{ 0.9f, 0.f, 0.f, m_fCurtRimAlpha });
        if (abs(m_fGoalRimAlpha - m_fCurtRimAlpha) < 0.1f)
        {
            m_fCurtRimAlpha = m_fGoalRimAlpha;
            if (m_fGoalRimAlpha == 1.f)
            {
                m_pMonster->Set_RimLightColor(_Vec4{ 0.f, 0.f, 0.f, 1.f });
            }
        }
    }
}

void CState_SimonManusP1_HighJumpFall::Control_Sound(_double CurTrackPos)
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

CState_SimonManusP1_HighJumpFall* CState_SimonManusP1_HighJumpFall::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_HighJumpFall* pInstance = new CState_SimonManusP1_HighJumpFall(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_HighJumpFall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_HighJumpFall::Free()
{
    __super::Free();
}