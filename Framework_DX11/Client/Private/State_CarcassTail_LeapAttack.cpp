#include "stdafx.h"
#include "State_CarcassTail_LeapAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassTail_LeapAttack::CState_CarcassTail_LeapAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_LeapAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 1.f;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassTail_LeapAttack::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    m_fIdleTime = m_fIdleDuration;
    return S_OK;
}

void CState_CarcassTail_LeapAttack::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        if (m_iRouteTrack == 0)
        {
            m_pMonster->Change_Animation(AN_ROUTE_MIDDLE, false, 0.1f, 0);
        }
        else if (m_iRouteTrack == 1)
        {
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.1f, 0);
        }
        else if (m_iRouteTrack == 2)
        {
            m_pMonster->Change_State(CCarcassBigA::IDLE);
            return;
        }
        ++m_iRouteTrack;
    }

    Collider_Check();

}

void CState_CarcassTail_LeapAttack::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_CarcassTail_LeapAttack::End_Check()
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
        if ((AN_ROUTE_MIDDLE) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_MIDDLE);
        }
        break;

    case 2:
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

void CState_CarcassTail_LeapAttack::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 50.f && CurTrackPos <= 70.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1, 1);
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
            m_pMonster->Active_CurrentWeaponCollider(1, 1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
}

CState_CarcassTail_LeapAttack* CState_CarcassTail_LeapAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_LeapAttack* pInstance = new CState_CarcassTail_LeapAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_LeapAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_LeapAttack::Free()
{
    __super::Free();
}