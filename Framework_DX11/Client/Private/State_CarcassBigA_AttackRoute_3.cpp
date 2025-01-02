#include "stdafx.h"
#include "State_CarcassBigA_AttackRoute_3.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_AttackRoute_3::CState_CarcassBigA_AttackRoute_3(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_AttackRoute_3::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 1.f;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassBigA_AttackRoute_3::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    m_fIdleTime = m_fIdleDuration;
    m_bSwingSound = false;
    return S_OK;
}

void CState_CarcassBigA_AttackRoute_3::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (!m_isDelayed)
    {
        if (m_iRouteTrack == 1)
        {
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.1f, 0, true);
        }

        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwingSound = false;

            if (m_iRouteTrack >= 2)
            {
                m_pMonster->Change_State(CCarcassBigA::IDLE);
                return;
            }
            m_fIdleTime = 0.f;
            m_isDelayed = true;
        }
    }
    else
    {
        m_fIdleTime += fTimeDelta;

        if (m_fIdleTime >= m_fIdleDuration)
        {
            m_isDelayed = false;
        }
        _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3, fTimeDelta);
        switch (iDir)
        {
        case -1:
            m_pMonster->Change_Animation(30, true, 0.1f);
            break;

        case 0:
            m_pMonster->Change_Animation(20, true, 0.1f);
            break;

        case 1:
            m_pMonster->Change_Animation(31, true, 0.1f);
            break;

        default:
            break;
        }
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassBigA_AttackRoute_3::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_CarcassBigA_AttackRoute_3::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_ROUTE_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_FIRST);
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

void CState_CarcassBigA_AttackRoute_3::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 50.f && CurTrackPos <= 70.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.4f, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
    else
    {
        if (CurTrackPos >= 90.f && CurTrackPos <= 105.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
}

void CState_CarcassBigA_AttackRoute_3::Sound_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 50.f && CurTrackPos <= 70.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Carcass_OneArmed_SK_WS_Blunt_01.wav"), false);
                m_bSwingSound = true;
            }
        }

        if (!m_bStampSound)
        {
            if (CurTrackPos >= 70.f && CurTrackPos <= 75.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Carcass_OneArmed_SK_Impact_Ground_01.wav"), false);
                m_bStampSound = true;
            }
        }
    }
    else
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 90.f && CurTrackPos <= 105.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Carcass_OneArmed_SK_WS_Blunt_01.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
}

CState_CarcassBigA_AttackRoute_3* CState_CarcassBigA_AttackRoute_3::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_AttackRoute_3* pInstance = new CState_CarcassBigA_AttackRoute_3(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_AttackRoute_3"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_AttackRoute_3::Free()
{
    __super::Free();
}