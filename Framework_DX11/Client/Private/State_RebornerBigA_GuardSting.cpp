#include "stdafx.h"
#include "State_RebornerBigA_GuardSting.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "RebornerBigA.h"

CState_RebornerBigA_GuardSting::CState_RebornerBigA_GuardSting(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerBigA_GuardSting::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RebornerBigA_GuardSting::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_LINKED_START, false, 0.1f, 0, true);
    m_iRouteTrack = 0;

    m_bSwingSound = { false };

    return S_OK;
}

void CState_RebornerBigA_GuardSting::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    _float fDist = m_pMonster->Calc_Distance_XZ();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos <= 240.f)
        {
            if (fDist <= 4.f)
            {
                ++m_iRouteTrack;
                m_bSwingSound = false;
                m_pMonster->Change_Animation(AN_LINKED_MIDDLE, false, 0.1f, 0, true);
                return;
            }
        }
        else if (End_Check())
        {
            m_pMonster->Change_State(CMonster::IDLE);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
    }
    else if (m_iRouteTrack == 1)
    {
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwingSound = false;
            m_pMonster->Change_Animation(AN_LINKED_LAST, false, 0.4f, 50, true);
            return;
        }

        if ((CurTrackPos <= 15.f) ||
            (CurTrackPos >= 80.f && CurTrackPos <= 100.f))
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
    }



    Collider_Check(CurTrackPos);

}

void CState_RebornerBigA_GuardSting::End_State()
{
}

_bool CState_RebornerBigA_GuardSting::End_Check()
{
    _bool   bEndCheck = { false };

    if (m_iRouteTrack == 0)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_START);
    }
    else if (m_iRouteTrack == 1)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_MIDDLE);
    }
    else
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_LAST);
    }

    return bEndCheck;
}

void CState_RebornerBigA_GuardSting::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 25.f && CurTrackPos <= 40.f) ||
            (CurTrackPos >= 100.f && CurTrackPos <= 125.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if (CurTrackPos >= 55.f && CurTrackPos <= 65.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }

}

void CState_RebornerBigA_GuardSting::Sound_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 25.f && CurTrackPos <= 40.f) ||
            (CurTrackPos >= 100.f && CurTrackPos <= 125.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Staff_03.wav"), false);
                m_bSwingSound = true;
            }
        }
        else
        {
            m_bSwingSound = false;
        }
    }
    else if (m_iRouteTrack == 2)
    {

    }
}

CState_RebornerBigA_GuardSting* CState_RebornerBigA_GuardSting::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerBigA_GuardSting* pInstance = new CState_RebornerBigA_GuardSting(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerBigA_GuardSting"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerBigA_GuardSting::Free()
{
    __super::Free();
}