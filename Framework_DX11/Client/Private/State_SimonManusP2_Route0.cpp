#include "stdafx.h"
#include "State_SimonManusP2_Route0.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_Route0::CState_SimonManusP2_Route0(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Route0::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_Route0::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    m_bSwingSound = false;
    m_bSwing = false;

    return S_OK;
}

void CState_SimonManusP2_Route0::Update(_float fTimeDelta)
{
    if (End_Check())
    {

        switch (m_iRouteTrack)
        {
        case 0:
            m_bSwingSound = false;
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.0f, 0);
            m_bSwing = false;
            break;

        case 1:
            m_pMonster->Change_State(CSimonManus::IDLE);
            return;
            break;

        default:
            break;
        }
        ++m_iRouteTrack;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_SimonManusP2_Route0::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP2_Route0::End_Check()
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

void CState_SimonManusP2_Route0::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0) //AN_ROUTE_FIRST, 쓰러지면서 하는 스윙
    {
        if (CurTrackPos >= 60 && CurTrackPos <= 85.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else       //어보이드 스윙
    {
        if (CurTrackPos >= 120 && CurTrackPos <= 180.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.1f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_SimonManusP2_Route0::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 60 && CurTrackPos <= 85.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CSimonManus::P2_TRAIL);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P2_TRAIL);
        }
    }
    else
    {
        if (CurTrackPos >= 120 && CurTrackPos <= 180.f)
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CSimonManus::P2_TRAIL);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P2_TRAIL);
        }
    }
}

void CState_SimonManusP2_Route0::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0) //AN_ROUTE_FIRST, 쓰러지면서 하는 스윙
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 70 && CurTrackPos <= 75.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));

                m_bSwingSound = true;
            }
        }
    }
    else       //어보이드 스윙
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 160 && CurTrackPos <= 165.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));

                m_bSwingSound = true;
            }
        }
    }
}

CState_SimonManusP2_Route0* CState_SimonManusP2_Route0::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Route0* pInstance = new CState_SimonManusP2_Route0(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Route0"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Route0::Free()
{
    __super::Free();
}