#include "stdafx.h"
#include "State_SimonManusP1_SwingMultiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_SwingMultiple::CState_SimonManusP1_SwingMultiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_SwingMultiple::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_SwingMultiple::Start_State(void* pArg)
{
    m_pMonster->Look_Player();

    m_pMonster->Change_Animation(AN_SWINGMULTIPLE, false, 0.1f, 0);

    m_bSwing = false;
    m_bStampSound = false;
    m_bSwipSound = false;
    return S_OK;
}

void CState_SimonManusP1_SwingMultiple::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
}

void CState_SimonManusP1_SwingMultiple::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
    m_iAnimCnt = 0;//혹시 완료되지 않고 변하는 경우에 대비
}

_bool CState_SimonManusP1_SwingMultiple::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_SWINGMULTIPLE);
}

void CState_SimonManusP1_SwingMultiple::Collider_Check(_double CurTrackPos)
{

    if ((CurTrackPos >= 85.f && CurTrackPos <= 115.f) ||
        (CurTrackPos >= 155.f && CurTrackPos <= 170.f) ||
        (CurTrackPos >= 205.f && CurTrackPos <= 275.f) ||
        (CurTrackPos >= 330.f && CurTrackPos <= 345.f) ||
        (CurTrackPos >= 385.f && CurTrackPos <= 415.f) ||
        (CurTrackPos >= 550.f && CurTrackPos <= 565.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(0);
    }
}

void CState_SimonManusP1_SwingMultiple::Effect_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 90.f && CurTrackPos <= 116.f) ||
        (CurTrackPos >= 150.f && CurTrackPos <= 190.f) ||
        (CurTrackPos >= 180.f && CurTrackPos <= 280.f))
    {
        if (!m_bSwing)
        {
            m_bSwing = true;
            m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
            m_pMonster->Active_Effect(CSimonManus::SWING_DRAG);
        }
    }
    else if((CurTrackPos > 116.f && CurTrackPos <= 122.f) ||
        (CurTrackPos > 190.f && CurTrackPos <= 195.f) ||
        (CurTrackPos > 280.f && CurTrackPos <= 285.f))
    {
        m_bSwing = false;
        m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
        m_pMonster->DeActive_Effect(CSimonManus::SWING_DRAG);
    }
}
void CState_SimonManusP1_SwingMultiple::Control_Sound(_double CurTrackPos)
{
    if (!m_bSwipSound)
    {
        if ((CurTrackPos >= 100.f && CurTrackPos <= 105.f) ||
            (CurTrackPos >= 157.f && CurTrackPos <= 162.f) ||
            (CurTrackPos >= 225.f && CurTrackPos <= 230.f) ||
            (CurTrackPos >= 240.f && CurTrackPos <= 245.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));
            m_bSwipSound = true;
        }
    }
    else if((CurTrackPos >= 110.f && CurTrackPos <= 115.f) ||
        (CurTrackPos > 162.f && CurTrackPos <= 172.f) ||
        (CurTrackPos >= 235.f && CurTrackPos <= 240.f))
    {
       m_bSwipSound = false;
    }

    if (!m_bStampSound)
    {
        if ((CurTrackPos >= 349.f && CurTrackPos <= 354.f) ||
            (CurTrackPos >= 410.f && CurTrackPos <= 415.f) ||
            (CurTrackPos >= 567.f && CurTrackPos <= 572.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
            m_bStampSound = true;
        }
        else if ((CurTrackPos >= 355.f && CurTrackPos <= 360.f) ||
            (CurTrackPos >= 416.f && CurTrackPos <= 420.f))
        {
            m_bStampSound = false;
        }

    }

}
CState_SimonManusP1_SwingMultiple* CState_SimonManusP1_SwingMultiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_SwingMultiple* pInstance = new CState_SimonManusP1_SwingMultiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_SwingMultiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_SwingMultiple::Free()
{
    __super::Free();
}