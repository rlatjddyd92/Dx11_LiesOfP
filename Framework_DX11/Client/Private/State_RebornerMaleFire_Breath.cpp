#include "stdafx.h"
#include "State_RebornerMaleFire_Breath.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "RebornerBigA.h"

CState_RebornerMaleFire_Breath::CState_RebornerMaleFire_Breath(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerMaleFire_Breath::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RebornerMaleFire_Breath::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_LINKED_START, false, 0.1f, 0, true);
    m_iRouteTrack = 0;

    m_bSwingSound = { false }; 
    m_bBreath = false;

    return S_OK;
}

void CState_RebornerMaleFire_Breath::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (End_Check())
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

        if ((CurTrackPos >= 20.f && CurTrackPos <= 60.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
        }
    }



    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);
    Effect_Check(CurTrackPos);

}

void CState_RebornerMaleFire_Breath::End_State()
{
    m_pMonster->DeActive_Effect(0);

    m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT1);
    m_pMonster->DeActive_CurrentWeaponCollider(0);
    m_pMonster->DeActive_Debuff(0);
}

_bool CState_RebornerMaleFire_Breath::End_Check()
{
    _bool   bEndCheck = false;

    if (m_iRouteTrack == 0)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_START);
    }
    else
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_LAST);
    }
    return bEndCheck;
}

void CState_RebornerMaleFire_Breath::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 60.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.0f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
            m_pMonster->Active_Debuff(0, 1.f);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
            m_pMonster->DeActive_Debuff(0);
        }
    }
    else
    {
        if (CurTrackPos <= 15.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.0f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }

}

void CState_RebornerMaleFire_Breath::Sound_Check(_double CurTrackPos)
{
    //if (m_iRouteTrack == 0)
    //{
    //    if (CurTrackPos >= 55.f && CurTrackPos <= 70.f)
    //    {
    //        if (!m_bSwingSound)
    //        {
    //            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Staff_03.wav"), false);
    //            m_bSwingSound = true;
    //        }
    //    }
    //}
    //else
    //{
    //    if (CurTrackPos >= 40.f && CurTrackPos <= 58.f)
    //    {
    //        if (!m_bSwingSound)
    //        {
    //            m_bSwingSound = true;
    //        }
    //    }
    //}
}

void CState_RebornerMaleFire_Breath::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bBreath)
        {
            if (CurTrackPos >= 60.f)//SE_NPC_SK_FX_Fire_L
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Fire_L.wav"), false);
                m_pMonster->Active_Effect(0);
                m_bBreath = true;
            }
        }
    }
    else
    {
        if (m_bBreath)
        {
            if (CurTrackPos >= 15.f)
            {
                m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT1);
                m_pMonster->DeActive_Effect(0);
                m_bBreath = false;
            }
        }
    }
}

CState_RebornerMaleFire_Breath* CState_RebornerMaleFire_Breath::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerMaleFire_Breath* pInstance = new CState_RebornerMaleFire_Breath(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerMaleFire_Breath"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerMaleFire_Breath::Free()
{
    __super::Free();
}