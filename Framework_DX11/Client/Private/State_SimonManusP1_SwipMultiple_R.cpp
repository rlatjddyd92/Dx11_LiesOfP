#include "stdafx.h"
#include "State_SimonManusP1_SwipMultiple_R.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_SwipMultiple_R::CState_SimonManusP1_SwipMultiple_R(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_SwipMultiple_R::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_SwipMultiple_R::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);
    m_bSwing_Sound = false;
    m_bStamp_Sound = false;
    m_bResetCheck = false;
    m_bSwing = false;

    return S_OK;
}

void CState_SimonManusP1_SwipMultiple_R::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        switch (m_iRouteTrack)
        {
        case 0:
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.2f, 0);
            m_bSwing = false;
            m_bSwing_Sound = false;
            break;

        case 1:

            m_pMonster->Change_State(CSimonManus::IDLE);
            return;
            break;

        default:
            break;
        }
        ++m_iRouteTrack;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
    Effect_Check(CurTrackPos);
}

void CState_SimonManusP1_SwipMultiple_R::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP1_SwipMultiple_R::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_ROUTE_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_FIRST);
        }
        break;

    case 1:
        if ((AN_ROUTE_LAST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_LAST);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_SimonManusP1_SwipMultiple_R::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 85.f && CurTrackPos <= 205.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else
    {
        if (CurTrackPos >= 20.f && CurTrackPos <= 80.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }

        if (!m_bResetCheck && 40.f <= CurTrackPos && CurTrackPos >= 50.f)
        {

            m_pMonster->Reset_WeaponOverlapCheck();
            m_bResetCheck = true;
        }
    }

}

void CState_SimonManusP1_SwipMultiple_R::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 85.f && CurTrackPos <= 205.f))
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
    else
    {
        if ((CurTrackPos >= 20.f && CurTrackPos <= 80.f))
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

void CState_SimonManusP1_SwipMultiple_R::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bStamp_Sound)
        {
            if (CurTrackPos >= 74.f && CurTrackPos <= 80.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
                m_bStamp_Sound = true;
            }
        }

        if (!m_bSwing_Sound)
        {
            if (CurTrackPos >= 80.f && CurTrackPos <= 90.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"), true);
                m_bSwing_Sound = true;
            }
            if ((CurTrackPos >= 135.f && CurTrackPos <= 145.f))
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));
                m_bSwing_Sound = true;
            }
        }
        else
        {
            if (CurTrackPos >= 95.f && CurTrackPos <= 100.f)
            {
                m_bSwing_Sound = false;
            }
        }

    }
    else
    {
        if (!m_bSwing_Sound)
        {
            if ((CurTrackPos >= 20.f && CurTrackPos <= 30.f) ||
                (CurTrackPos >= 60.f && CurTrackPos <= 70.f))
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));
                m_bSwing_Sound = true;
            }
        }
        else
        {
            if ((CurTrackPos > 30.f && CurTrackPos <= 50.f))
            {
                m_bSwing_Sound = false;
            }
        }

    }
}

CState_SimonManusP1_SwipMultiple_R* CState_SimonManusP1_SwipMultiple_R::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_SwipMultiple_R* pInstance = new CState_SimonManusP1_SwipMultiple_R(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_SwipMultiple_R"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_SwipMultiple_R::Free()
{
    __super::Free();
}