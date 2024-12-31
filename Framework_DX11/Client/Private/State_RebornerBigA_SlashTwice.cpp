#include "stdafx.h"
#include "State_RebornerBigA_SlashTwice.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "RebornerBigA.h"

CState_RebornerBigA_SlashTwice::CState_RebornerBigA_SlashTwice(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerBigA_SlashTwice::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RebornerBigA_SlashTwice::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_LINKED_START, false, 0.1f, 0, true);
    m_iRouteTrack = 0;
    
    m_bSwingSound = { false };

    return S_OK;
}

void CState_RebornerBigA_SlashTwice::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 80.f)
        {
            ++m_iRouteTrack;
            m_bSwingSound = false;
            m_pMonster->Change_Animation(AN_LINKED_LAST, false, 0.1f, 0, true);
            return;
        }
        if (CurTrackPos <= 45.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
        }
    }
    else
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CMonster::IDLE);
            return;
        }

        if ((CurTrackPos <= 30.f) ||
            (CurTrackPos >= 65.f && CurTrackPos <= 80.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
        }
    }



    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_RebornerBigA_SlashTwice::End_State()
{
}

_bool CState_RebornerBigA_SlashTwice::End_Check()
{

    return m_pMonster->Get_EndAnim(AN_LINKED_LAST);;
}

void CState_RebornerBigA_SlashTwice::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 55.f && CurTrackPos <= 70.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.1f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else
    {
        if (CurTrackPos >= 40.f && CurTrackPos <= 58.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }

}

void CState_RebornerBigA_SlashTwice::Sound_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 55.f && CurTrackPos <= 70.f)
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Staff_03.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
    else
    {
        if (CurTrackPos >= 40.f && CurTrackPos <= 58.f)
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Staff_03.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
}

CState_RebornerBigA_SlashTwice* CState_RebornerBigA_SlashTwice::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerBigA_SlashTwice* pInstance = new CState_RebornerBigA_SlashTwice(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerBigA_SlashTwice"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerBigA_SlashTwice::Free()
{
    __super::Free();
}