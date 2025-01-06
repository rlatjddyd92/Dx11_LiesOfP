#include "stdafx.h"
#include "State_RaxasiaP1_RepetUpperSlash.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP1_RepetUpperSlash::CState_RaxasiaP1_RepetUpperSlash(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_RepetUpperSlash::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_RepetUpperSlash::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_UPPERONCE_RIGHT, false, 0.1f, 0);

    m_bSwingSound = false;

    m_bSwing = false;
    m_bDrag = false;

    return S_OK;
}

void CState_RaxasiaP1_RepetUpperSlash::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);

    Route_Controll(CurTrackPos, fTimeDelta);

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP1_RepetUpperSlash::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
}

_bool CState_RaxasiaP1_RepetUpperSlash::End_Check()
{
    _bool   bEndCheck{false};
    if (m_iRouteTrack == 0)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_UPPERONCE_RIGHT);
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 5)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_UPPERTWICE);
    }
    else if (m_iRouteTrack >= 6)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_LAST);
    }

    return  bEndCheck;
}

void CState_RaxasiaP1_RepetUpperSlash::Route_Controll(_double CurTrackPos, _float fTimeDelta)
{
    switch (m_iRouteTrack)
    {
    case 0://1
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwing = false;
            m_bSwingSound = false;
            m_bDrag = false;
            m_pMonster->Change_Animation(AN_UPPERTWICE, false, 0.02f, 0);
        }

        if (CurTrackPos <= 90.f ||
            CurTrackPos >= 130.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    case 1://23
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwing = false;
            m_bSwingSound = false;
            m_bDrag = false;
            m_pMonster->Change_Animation(AN_UPPERTWICE, false, 0.02f, 0, true, true);
        }

        if (!m_bSpeedController)
        {
            if (CurTrackPos <= 25.f)
            {
                m_bSpeedController = true;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_UPPERTWICE, (double)0.5);
            }
        }
        else
        {
            m_bSpeedController = false;
            m_pMonster->Get_Model()->Set_SpeedRatio(AN_UPPERTWICE, (double)1.3);
        }

        break;

    case 2://45
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwing = false;
            m_bSwingSound = false;
            m_bDrag = false;
            m_pMonster->Change_Animation(AN_UPPERTWICE, false, 0.02f, 0, true, true);
        }

        if (!m_bSpeedController)
        {
            if (CurTrackPos >= 45.f && CurTrackPos <= 70.f)
            {
                m_bSpeedController = true;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_UPPERTWICE, (double)0.5);
            }
        }
        else if(CurTrackPos >= 70.f)
        {
            m_bSpeedController = false;
            m_pMonster->Get_Model()->Set_SpeedRatio(AN_UPPERTWICE, (double)1.6);
        }

        break;

    case 3://67
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwing = false;
            m_bSwingSound = false;
            m_bDrag = false;
            m_pMonster->Change_Animation(AN_UPPERTWICE, false, 0.02f, 0, true, true);
        }

        break;

    case 4://89
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwing = false;
            m_bSwingSound = false;
            m_bDrag = false;
            m_pMonster->Change_Animation(AN_UPPERTWICE, false, 0.02f, 0, true, true);
        }

        break;

    case 5://10 11
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwing = false;
            m_bSwingSound = false;
            m_bDrag = false;
            m_pMonster->Change_Animation(AN_LINKED_LAST, false, 0.02f, 0, true, true);
        }

        if (!m_bSpeedController)
        {
            if (CurTrackPos >= 45.f && CurTrackPos <= 70.f)
            {
                m_bSpeedController = true;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_UPPERTWICE, (double)1);
            }
        }

        break;

    case 6://12 13
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
        }

        if (CurTrackPos <= 25.f ||
            (CurTrackPos <= 65.f && CurTrackPos <= 75.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    default:
        break;
    }
    if (m_iRouteTrack >= 1 && m_iRouteTrack <= 5)
    {
        if (CurTrackPos <= 15.f ||
            (CurTrackPos <= 50.f && CurTrackPos <= 60.f) ||
            CurTrackPos >= 95.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

    }

}

void CState_RaxasiaP1_RepetUpperSlash::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 105.f && CurTrackPos <= 125.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 5)
    {
        if ((CurTrackPos >= 25.f && CurTrackPos <= 40.f) ||
            (CurTrackPos >= 70.f && CurTrackPos <= 85.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
    else if (m_iRouteTrack == 6)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 60.f) ||
            (CurTrackPos >= 80.f && CurTrackPos <= 95.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
}

void CState_RaxasiaP1_RepetUpperSlash::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 105.f && CurTrackPos <= 125.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }

        if ((CurTrackPos >= 114.f && CurTrackPos <= 117.f))
        {
            if (!m_bDrag)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_DRAG, true);
                m_bDrag = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_DRAG);
            m_bDrag = false;
        }

    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 5)
    {
        if ((CurTrackPos >= 25.f && CurTrackPos <= 40.f) ||
            (CurTrackPos >= 70.f && CurTrackPos <= 85.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
            m_bSwing = false;
        }

        if ((CurTrackPos >= 30.f && CurTrackPos <= 33.f) ||
            (CurTrackPos >= 72.f && CurTrackPos <= 74.f))
        {
            if (!m_bDrag)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_DRAG, true);
                m_bDrag = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_DRAG);
            m_bDrag = false;
        }
    }
    else if (m_iRouteTrack == 6)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 60.f) ||
            (CurTrackPos >= 80.f && CurTrackPos <= 95.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
            m_bSwing = false;
        }
    }

}

void CState_RaxasiaP1_RepetUpperSlash::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 105.f && CurTrackPos <= 125.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 5)
    {
        if ((CurTrackPos >= 25.f && CurTrackPos <= 40.f) ||
            (CurTrackPos >= 70.f && CurTrackPos <= 85.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }
        else
        {
            m_bSwingSound = false;
        }
    }
    else if (m_iRouteTrack == 6)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 60.f) ||
            (CurTrackPos >= 80.f && CurTrackPos <= 95.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }
        else
        {
            m_bSwingSound = false;
        }
    }

}

CState_RaxasiaP1_RepetUpperSlash* CState_RaxasiaP1_RepetUpperSlash::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_RepetUpperSlash* pInstance = new CState_RaxasiaP1_RepetUpperSlash(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_RepetUpperSlash"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_RepetUpperSlash::Free()
{
    __super::Free();
}