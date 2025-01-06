#include "stdafx.h"
#include "State_CarcassBigA_AttackRoute_4.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_AttackRoute_4::CState_CarcassBigA_AttackRoute_4(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_AttackRoute_4::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
   // FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassBigA_AttackRoute_4::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f);
    m_bSwingSound = false;

    return S_OK;
}

void CState_CarcassBigA_AttackRoute_4::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        if (m_pMonster->Get_TargetDead())
        {
            m_pMonster->Change_State(CCarcassBigA::IDLE);
            return;
        }

        ++m_iRouteTrack;
        m_bSwingSound = false;
        if (m_iRouteTrack == 1)
        {
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.1f);
        }
        else if (m_iRouteTrack == 2)
        {
            m_pMonster->Change_State(CCarcassBigA::IDLE);
            return;
        }
    }

    if (m_iRouteTrack == 0 && CurTrackPos >= 55.f && CurTrackPos <= 140.f)
    {
        _Vec3 vDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

        m_pMonster->Get_RigidBody()->Set_Velocity(XMVector3Normalize(vDir) * m_fImpactSpeed);
    }
    else
    {
        if (CurTrackPos <= 30.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.5, fTimeDelta);
        }
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassBigA_AttackRoute_4::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(1);
    m_pMonster->DeActive_CurrentWeaponCollider(2);
    m_iRouteTrack = 0;
}

_bool CState_CarcassBigA_AttackRoute_4::End_Check()
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

void CState_CarcassBigA_AttackRoute_4::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 55.f && CurTrackPos <= 140.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.4f, 2, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(2);
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
            m_pMonster->DeActive_CurrentWeaponCollider(1);
        }
    }

}

void CState_CarcassBigA_AttackRoute_4::Sound_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        //ÀÓÆÑÆ®
        if ((CurTrackPos >= 27.f && CurTrackPos <= 30.f) ||
            (CurTrackPos >= 76.f && CurTrackPos <= 80.f) ||
            (CurTrackPos >= 108.f && CurTrackPos <= 115.f) ||
            (CurTrackPos >= 126.f && CurTrackPos <= 130.f))
        {
            if (!m_bStepSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_FS_Carcass_OneArmed_Stone_04.wav"), false);
                m_bStepSound = true;
            }
        }
        else
        {
            m_bStepSound = false;
        }
    }
    else
    {
        if( !m_bSwingSound)
        {
            if (CurTrackPos >= 90.f && CurTrackPos <= 105.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Carcass_OneArmed_SK_WS_Blunt_01.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
}

CState_CarcassBigA_AttackRoute_4* CState_CarcassBigA_AttackRoute_4::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_AttackRoute_4* pInstance = new CState_CarcassBigA_AttackRoute_4(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_AttackRoute_4"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_AttackRoute_4::Free()
{
    __super::Free();
}