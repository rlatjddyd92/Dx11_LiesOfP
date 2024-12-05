#include "stdafx.h"
#include "State_SimonManusP2_Sting.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_Sting::CState_SimonManusP2_Sting(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Sting::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_Sting::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_STINGATTACK, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP2_Sting::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
}

void CState_SimonManusP2_Sting::End_State()
{

}

_bool CState_SimonManusP2_Sting::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_STINGATTACK);
}

void CState_SimonManusP2_Sting::Collider_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 75.f && CurTrackPos <= 105.f) ||
        (CurTrackPos >= 175.f && CurTrackPos <= 225.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.1, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

void CState_SimonManusP2_Sting::Control_Sound(_double CurTrackPos)
{
    if (!m_bStingSound)
    {
        if ((CurTrackPos >= 105.f && CurTrackPos <= 110.f) ||
            (CurTrackPos >= 175.f && CurTrackPos <= 180.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
            m_bStingSound = true;
        }
    }
    else
    {
        if (CurTrackPos > 115.f && CurTrackPos <= 120.f)
        {
            m_bStingSound = false;
        }
    }
}

CState_SimonManusP2_Sting* CState_SimonManusP2_Sting::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Sting* pInstance = new CState_SimonManusP2_Sting(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Sting"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Sting::Free()
{
    __super::Free();
}