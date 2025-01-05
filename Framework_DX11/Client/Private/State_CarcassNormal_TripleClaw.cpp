#include "stdafx.h"
#include "State_CarcassNormal_TripleClaw.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "CarcassNormal.h"

CState_CarcassNormal_TripleClaw::CState_CarcassNormal_TripleClaw(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassNormal_TripleClaw::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassNormal_TripleClaw::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_LINKED_START, false, 0.1f, 0, true);
    m_iRouteTrack = 0;

    m_bSwingSound = false;

    return S_OK;
}

void CState_CarcassNormal_TripleClaw::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 90.f)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(95, true, 0.5f, 0, true);
                m_pMonster->Change_State(CCarcassNormal::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwingSound = false;
            m_pMonster->Change_Animation(AN_LINKED_MIDDLE, false, 0.1f, 0, true);
            return;
        }
        if (CurTrackPos <= 45.f || 
            (CurTrackPos >= 80.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if (CurTrackPos >= 65.f)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(95, true, 0.5f, 0, true);
                m_pMonster->Change_State(CCarcassNormal::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwingSound = false;
            m_pMonster->Change_Animation(AN_LINKED_LAST, false, 0.1f, 0, true);
            return;
        }
        if (CurTrackPos <= 15.f ||
            (CurTrackPos >= 55.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }
    }
    else
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CCarcassNormal::IDLE);
            return;
        }

        if ((CurTrackPos <= 45.f) ||
            (CurTrackPos >= 75.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }
    }



    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassNormal_TripleClaw::End_State()
{
}

_bool CState_CarcassNormal_TripleClaw::End_Check()
{

    return m_pMonster->Get_EndAnim(AN_LINKED_LAST);;
}

void CState_CarcassNormal_TripleClaw::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 55.f && CurTrackPos <= 70.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.1f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
    else if(m_iRouteTrack == 1)
    {
        if (CurTrackPos >= 25.f && CurTrackPos <= 45.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else
    {
        if (CurTrackPos >= 55.f && CurTrackPos <= 68.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }

}

void CState_CarcassNormal_TripleClaw::Sound_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 55.f && CurTrackPos <= 70.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Nail_01.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 25.f && CurTrackPos <= 45.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Nail_01.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
    else
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 55.f && CurTrackPos <= 68.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Nail_01.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
}

CState_CarcassNormal_TripleClaw* CState_CarcassNormal_TripleClaw::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassNormal_TripleClaw* pInstance = new CState_CarcassNormal_TripleClaw(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassNormal_TripleClaw"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassNormal_TripleClaw::Free()
{
    __super::Free();
}