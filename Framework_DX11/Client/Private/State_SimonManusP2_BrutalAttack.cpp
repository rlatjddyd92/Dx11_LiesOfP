#include "stdafx.h"
#include "State_SimonManusP2_BrutalAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_BrutalAttack::CState_SimonManusP2_BrutalAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_BrutalAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_BrutalAttack::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_BRUTALATTACK, false, 0.1f, 0);
    m_bSwingSound = false;
    m_bStampSound = false;
    m_bSwing = false;

    return S_OK;
}

void CState_SimonManusP2_BrutalAttack::Update(_float fTimeDelta)
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

void CState_SimonManusP2_BrutalAttack::End_State()
{

}

_bool CState_SimonManusP2_BrutalAttack::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_BRUTALATTACK);
}

void CState_SimonManusP2_BrutalAttack::Collider_Check(_double CurTrackPos)
{
    if (CurTrackPos >= 135.f && CurTrackPos <= 150.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

void CState_SimonManusP2_BrutalAttack::Effect_Check(_double CurTrackPos)
{
}

void CState_SimonManusP2_BrutalAttack::Control_Sound(_double CurTrackPos)
{
    if (!m_bSwingSound)
    {
        if ((CurTrackPos >= 100.f && CurTrackPos <= 105.f) ||
            (CurTrackPos >= 157.f && CurTrackPos <= 162.f) ||
            (CurTrackPos >= 222.f && CurTrackPos <= 227.f) ||
            (CurTrackPos >= 240.f && CurTrackPos <= 245.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));

            m_bSwingSound = true;
        }
    }
    else
    {
        if ((CurTrackPos > 105.f && CurTrackPos <= 110.f) ||
            (CurTrackPos > 162.f && CurTrackPos <= 167.f) ||
            (CurTrackPos > 227.f && CurTrackPos <= 232.f))
        {
            m_bSwingSound = false;
        }
    }

    if (!m_bStampSound)
    {
        if ((CurTrackPos >= 346.f && CurTrackPos <= 351.f) ||
            (CurTrackPos >= 410.f && CurTrackPos <= 415.f) ||
            (CurTrackPos >= 569.f && CurTrackPos <= 574.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
            m_bStampSound = true;
        }
    }
    else
    {
        if ((CurTrackPos > 351.f && CurTrackPos <= 356.f) ||
            (CurTrackPos > 415.f && CurTrackPos <= 420.f))
        {
            m_bSwingSound = false;
        }
    }

}

CState_SimonManusP2_BrutalAttack* CState_SimonManusP2_BrutalAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_BrutalAttack* pInstance = new CState_SimonManusP2_BrutalAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_BrutalAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_BrutalAttack::Free()
{
    __super::Free();
}