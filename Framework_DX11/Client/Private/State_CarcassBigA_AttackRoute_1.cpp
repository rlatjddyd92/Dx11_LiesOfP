#include "stdafx.h"
#include "State_CarcassBigA_AttackRoute_1.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_AttackRoute_1::CState_CarcassBigA_AttackRoute_1(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_AttackRoute_1::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassBigA_AttackRoute_1::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0, true);

    m_iRouteTrack = 0;
    m_bSwingSound = false;
    m_bStampSound = false;

    return S_OK;
}

void CState_CarcassBigA_AttackRoute_1::Update(_float fTimeDelta)
{

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        //플레이어 사망시 아이들로 되돌아가기
        if (m_pMonster->Get_TargetDead())
        {
            m_pMonster->Change_State(CCarcassBigA::IDLE);
            return;
        }

        ++m_iRouteTrack;
        m_bSwingSound = false;

        if (m_iRouteTrack == 1)
        {
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.1f, 0, true);
        }
        else if (m_iRouteTrack >= 2)
        {
            m_pMonster->Change_State(CCarcassBigA::IDLE);
            return;
        }
    }


    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 60.f && CurTrackPos <= 90.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);
        }
        break;
    case 1:
        if (CurTrackPos >= 65.f && CurTrackPos <= 95.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);
        }
        break;
    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassBigA_AttackRoute_1::End_State()
{

    m_pMonster->DeActive_CurrentWeaponCollider(1);
    m_iRouteTrack = 0;
}

_bool CState_CarcassBigA_AttackRoute_1::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_FIRST);
        break;

    case 1:
        bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_LAST);
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_CarcassBigA_AttackRoute_1::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 110.f && CurTrackPos <= 130.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(1);
        }
    }
    else
    {
        if (CurTrackPos >= 115.f && CurTrackPos <= 140.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(1);
        }
    }
}

void CState_CarcassBigA_AttackRoute_1::Sound_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 110.f && CurTrackPos <= 130.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Carcass_OneArmed_SK_WS_Blunt_01.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
    else
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 115.f && CurTrackPos <= 140.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Carcass_OneArmed_SK_WS_Blunt_01.wav"), false);
                m_bSwingSound = true;
            }
        }

        if (!m_bStampSound)
        {
            if (CurTrackPos >= 140.f && CurTrackPos <= 145.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Carcass_OneArmed_SK_Impact_Ground_01.wav"), false);
                m_bStampSound = true;
            }
        }
    }
}

CState_CarcassBigA_AttackRoute_1* CState_CarcassBigA_AttackRoute_1::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_AttackRoute_1* pInstance = new CState_CarcassBigA_AttackRoute_1(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_AttackRoute_1"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_AttackRoute_1::Free()
{
    __super::Free();
}