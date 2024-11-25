#include "stdafx.h"
#include "State_SimonManusP1_SwipMultiple_R.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_SwipMultiple_R::CState_SimonManusP1_SwipMultiple_R(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_SwipMultiple_R::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_SwipMultiple_R::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP1_SwipMultiple_R::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        switch (m_iRouteTrack)
        {
        case 0:
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.2f, 0);

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

    Collider_Check();
}

void CState_SimonManusP1_SwipMultiple_R::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP1_SwipMultiple_R::End_Check()
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

void CState_SimonManusP1_SwipMultiple_R::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 130.f && CurTrackPos <= 155.f)
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

CState_SimonManusP1_SwipMultiple_R* CState_SimonManusP1_SwipMultiple_R::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_SwipMultiple_R* pInstance = new CState_SimonManusP1_SwipMultiple_R(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_SwipMultiple_R"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_SwipMultiple_R::Free()
{
    __super::Free();
}