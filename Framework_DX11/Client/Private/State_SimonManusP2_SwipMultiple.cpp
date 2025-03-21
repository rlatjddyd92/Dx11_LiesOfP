#include "stdafx.h"
#include "State_SimonManusP2_SwipMultiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_SwipMultiple::CState_SimonManusP2_SwipMultiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_SwipMultiple::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_SwipMultiple::Start_State(void* pArg)
{
    int iCnt = rand() % 2;

    switch (iCnt)
    {
    case 0:
        m_iCurStartAnim = AN_ROUTE_FIRST_L;
        break;

    case 1:
        m_iCurStartAnim = AN_ROUTE_FIRST_R;
        break;

    default:
        break;
    }

    m_pMonster->Change_Animation(m_iCurStartAnim, false, 0.1f, 0);

    m_bSwing_Sound = false;
    m_bStamp_Sound = false;
    m_bSwing = false;
    m_bResetList = false;

    return S_OK;
}

void CState_SimonManusP2_SwipMultiple::Update(_float fTimeDelta)
{
    if (End_Check())
    {

        switch (m_iRouteTrack)
        {
        case 0:
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.0f, 0);
            m_bSwing = false;

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
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
}

void CState_SimonManusP2_SwipMultiple::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP2_SwipMultiple::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((m_iCurStartAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iCurStartAnim);
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


void CState_SimonManusP2_SwipMultiple::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (m_iCurStartAnim == AN_ROUTE_FIRST_L)
        {
            if (CurTrackPos >= 135.f && CurTrackPos <= 165.f)
            {
                m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
            }
            else
            {
                m_pMonster->DeActive_CurrentWeaponCollider(0);
            }
        }
        else
        {
            if (CurTrackPos >= 170 && CurTrackPos <= 195.f)
            {
                m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
            }
            else
            {
                m_pMonster->DeActive_CurrentWeaponCollider(0);
            }
        }
    }
    else
    {
        if (CurTrackPos >= 20.f && CurTrackPos <= 80.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }

        if (!m_bResetList)
        {
            if (CurTrackPos >= 40.f)
            {
                m_bResetList = true;
                m_pMonster->Reset_WeaponOverlapCheck();
            }
        }
    }
}

void CState_SimonManusP2_SwipMultiple::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (m_iCurStartAnim == AN_ROUTE_FIRST_L)
        {
            if (CurTrackPos >= 135.f && CurTrackPos <= 165.f)
            {
                if (!m_bSwing)
                {
                    m_pMonster->Active_Effect(CSimonManus::P2_TRAIL);
                    m_pMonster->Active_Effect(CSimonManus::SWING_DRAG_REVERSE);
                    m_bSwing = true;
                }
            }
            else
            {
                m_pMonster->DeActive_Effect(CSimonManus::P2_TRAIL);
                m_pMonster->DeActive_Effect(CSimonManus::SWING_DRAG_REVERSE);
            }
        }
        else
        {
            if (CurTrackPos >= 170 && CurTrackPos <= 195.f)
            {
                if (!m_bSwing)
                {
                    m_pMonster->Active_Effect(CSimonManus::P2_TRAIL);
                    m_pMonster->Active_Effect(CSimonManus::SWING_DRAG);
                    m_bSwing = true;
                }
            }
            else
            {
                m_pMonster->DeActive_Effect(CSimonManus::P2_TRAIL);
                m_pMonster->DeActive_Effect(CSimonManus::SWING_DRAG);
            }
        }
    }
    else
    {
        if (CurTrackPos >= 20.f && CurTrackPos <= 80.f)
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CSimonManus::P2_TRAIL);
                m_pMonster->Active_Effect(CSimonManus::SWING_DRAG);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P2_TRAIL);
            m_pMonster->DeActive_Effect(CSimonManus::SWING_DRAG);
        }

    }
}

void CState_SimonManusP2_SwipMultiple::Control_Sound(_double CurTrackPos)
{
    // �ִ������� ������
    if (m_iRouteTrack == 0)
    {
        _double StampTime{};
        _double SwingTime{};
        _double SlideTime{};
        if (m_iCurStartAnim == AN_ROUTE_FIRST_L)
        {
            StampTime = 54.f;
            SlideTime = 61.f;
            SwingTime = 141.f;
        }
        else
        {
            StampTime = 74.f;
            SlideTime = 81.f;
            SwingTime = 178.f;
        }
        if (!m_bStamp_Sound)
        {
            if (CurTrackPos >= StampTime && CurTrackPos <= StampTime + 5.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
                m_bStamp_Sound = true;
            }
        }

        if (!m_bSwing_Sound)
        {
            if (CurTrackPos >= SlideTime && CurTrackPos <= SlideTime + 5.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"), true);
                m_bSwing_Sound = true;
            }
            if ((CurTrackPos >= SwingTime && CurTrackPos <= SwingTime + 5.f))
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
            if ((CurTrackPos >= 27.f && CurTrackPos <= 32.f) ||
                (CurTrackPos >= 64.f && CurTrackPos <= 69.f))
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));
                m_bSwing_Sound = true;
            }
        }
        else
        {
            if ((CurTrackPos > 33.f && CurTrackPos <= 40.f))
            {
                m_bSwing_Sound = false;
            }
        }

    }
}

CState_SimonManusP2_SwipMultiple* CState_SimonManusP2_SwipMultiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_SwipMultiple* pInstance = new CState_SimonManusP2_SwipMultiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_SwipMultiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_SwipMultiple::Free()
{
    __super::Free();
}