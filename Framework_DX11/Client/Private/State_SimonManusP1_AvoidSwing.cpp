#include "stdafx.h"
#include "State_SimonManusP1_AvoidSwing.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_AvoidSwing::CState_SimonManusP1_AvoidSwing(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_AvoidSwing::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_AvoidSwing::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_AVOIDSWING, false, 0.1f, 0);
    m_bSwing_Sound = false;
    m_bSwing = false;
    return S_OK;
}

void CState_SimonManusP1_AvoidSwing::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
}

void CState_SimonManusP1_AvoidSwing::End_State()
{
   
}

_bool CState_SimonManusP1_AvoidSwing::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_AVOIDSWING);
}

void CState_SimonManusP1_AvoidSwing::Collider_Check(_double CurTrackPos)
{
    if (CurTrackPos >= 105.f && CurTrackPos <= 185.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1.1f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

void CState_SimonManusP1_AvoidSwing::Effect_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 115.f && CurTrackPos <= 180.f))
    {
        if (!m_bSwing)
        {
            m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
            m_pMonster->Active_Effect(CSimonManus::SWING_DRAG);
            m_bSwing = true;
        }
    }
    else
    {
        m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
        m_pMonster->DeActive_Effect(CSimonManus::SWING_DRAG);
    }
}

void CState_SimonManusP1_AvoidSwing::Control_Sound(_double CurTrackPos)
{
    if (!m_bSwing_Sound)
    {
        if (CurTrackPos >= 80.f && CurTrackPos <= 90.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
            m_bSwing_Sound = true;
        }
        if (CurTrackPos >= 140.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));
            m_bSwing_Sound = true;
        }
    }
    else
    {
        if (CurTrackPos > 90.f && CurTrackPos <= 100.f)
        {
            m_bSwing_Sound = false;
        }
    }
}

CState_SimonManusP1_AvoidSwing* CState_SimonManusP1_AvoidSwing::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_AvoidSwing* pInstance = new CState_SimonManusP1_AvoidSwing(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_AvoidSwing"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_AvoidSwing::Free()
{
    __super::Free();
}