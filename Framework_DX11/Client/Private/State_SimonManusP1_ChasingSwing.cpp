#include "stdafx.h"
#include "State_SimonManusP1_ChasingSwing.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_ChasingSwing::CState_SimonManusP1_ChasingSwing(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_ChasingSwing::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_ChasingSwing::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_CHASINGSWING_START, false,  0.1f, 0);

    m_bSwingSound = false;
    m_bStampSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_SimonManusP1_ChasingSwing::Update(_float fTimeDelta)
{
    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            ++m_iRouteTrack;
            if (!m_pMonster->Get_EffectsLoop(CSimonManus::P1_TRAIL))
            {
                m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
            }
            m_pMonster->Change_Animation(AN_CHASINGSWING_LOOP, true, 0.f, 0);
        }
        break;

    case 1:
        if (m_fCurrentTime >= 6.f)
        {
            ++m_iRouteTrack;
            m_pMonster->Change_Animation(AN_CHASINGSWING_END, false, 0.f, 0);
        }
        else if (m_fCurrentTime >= m_fChaseDuration)
        {
            if (m_pMonster->Calc_Distance_XZ() <= 7.f)
            {
                ++m_iRouteTrack;
                m_pMonster->Change_Animation(AN_CHASINGSWING_END, false, 0.f, 0);
            }
        }
        else
            m_fCurrentTime += fTimeDelta;

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        break;

    case 2:
        if (End_Check())
        {
            m_iRouteTrack = 0;
            m_pMonster->Change_State(CSimonManus::IDLE);
            return;
        }
        break;

    default:
        break;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_SimonManusP1_ChasingSwing::End_State()
{
}

_bool CState_SimonManusP1_ChasingSwing::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_CHASINGSWING_START) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_CHASINGSWING_START);
        }
        break;

    case 1:
        if ((AN_CHASINGSWING_LOOP) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_CHASINGSWING_LOOP);
        }
        break;

    case 2:
        if ((AN_CHASINGSWING_END) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_CHASINGSWING_END);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_SimonManusP1_ChasingSwing::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 140.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if ((CurTrackPos <= 53.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_SimonManusP1_ChasingSwing::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 140.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if ((CurTrackPos <= 50.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
        }
    }
}

void CState_SimonManusP1_ChasingSwing::Control_Sound(_double CurTrackPos)
{
    //100 35
    if (!m_bStampSound)
    {
        if (CurTrackPos >= 35.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
            m_bStampSound = true;
        }
    }

    if (m_iRouteTrack == 0)
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 140.f && CurTrackPos <= 145.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"), true);
                m_bSwingSound = true;
            }
        }
        else if(CurTrackPos >= 147.f)
        {
            m_bSwingSound = false;
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 100.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));
                m_bSwingSound = true;
            }
        }
    }

}

CState_SimonManusP1_ChasingSwing* CState_SimonManusP1_ChasingSwing::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_ChasingSwing* pInstance = new CState_SimonManusP1_ChasingSwing(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_ChasingSwing"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_ChasingSwing::Free()
{
    __super::Free();
}