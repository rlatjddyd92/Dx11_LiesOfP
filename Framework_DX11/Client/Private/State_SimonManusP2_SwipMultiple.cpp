#include "stdafx.h"
#include "State_SimonManusP2_SwipMultiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_SwipMultiple::CState_SimonManusP2_SwipMultiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_SwipMultiple::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_SwipMultiple::Start_State(void* pArg)
{
    int iCnt = rand() % 2;

    switch (iCnt)
    {
    case 0:
        m_iCurStartAnim = AN_ROUTE_FIRST_L;
        break;

    case 1:
        m_iCurStartAnim = AN_ROUTE_FIRST_R;
        break;

    default:
        break;
    }

    m_pMonster->Change_Animation(m_iCurStartAnim, false, 0.1f, 0);


    return S_OK;
}

void CState_SimonManusP2_SwipMultiple::Update(_float fTimeDelta)
{
    if (End_Check())
    {

        switch (m_iRouteTrack)
        {
        case 0:
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.0f, 0);

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

}

void CState_SimonManusP2_SwipMultiple::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP2_SwipMultiple::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((m_iCurStartAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iCurStartAnim);
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


void CState_SimonManusP2_SwipMultiple::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (m_iCurStartAnim == AN_ROUTE_FIRST_L)
        {
            if (CurTrackPos >= 135.f && CurTrackPos <= 165.f)
            {
                m_pMonster->Active_CurrentWeaponCollider(1);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
        else
        {
            if (CurTrackPos >= 170 && CurTrackPos <= 195.f)
            {
                m_pMonster->Active_CurrentWeaponCollider(1);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
    }
    else
    {
        if (CurTrackPos >= 20.f && CurTrackPos <= 80.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_SimonManusP2_SwipMultiple::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (m_iCurStartAnim == AN_ROUTE_FIRST_L)
        {
            if (CurTrackPos >= 135.f && CurTrackPos <= 165.f)
            {
                if (!m_pMonster->Get_EffectsLoop(CSimonManus::P1_TRAIL))
                {
                    m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
                }
            }
            else
            {
                m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
            }
        }
        else
        {
            if (CurTrackPos >= 170 && CurTrackPos <= 195.f)
            {
                if (!m_pMonster->Get_EffectsLoop(CSimonManus::P1_TRAIL))
                {
                    m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
                }
            }
            else
            {
                m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
            }
        }
    }
    else
    {
        if (CurTrackPos >= 20.f && CurTrackPos <= 80.f)
        {
            if (!m_pMonster->Get_EffectsLoop(CSimonManus::P1_TRAIL))
            {
                m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
        }
    }
}

void CState_SimonManusP2_SwipMultiple::Control_Sound(_double CurTrackPos)
{
}

CState_SimonManusP2_SwipMultiple* CState_SimonManusP2_SwipMultiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_SwipMultiple* pInstance = new CState_SimonManusP2_SwipMultiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_SwipMultiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_SwipMultiple::Free()
{
    __super::Free();
}