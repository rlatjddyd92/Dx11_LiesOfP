#include "stdafx.h"
#include "State_SimonManusP2_Route1.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_Route1::CState_SimonManusP2_Route1(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Route1::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_Route1::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);


    return S_OK;
}

void CState_SimonManusP2_Route1::Update(_float fTimeDelta)
{
    if (End_Check())
    {

        switch (m_iRouteTrack)
        {
        case 0:
            m_pMonster->Change_Animation(AN_ROUTE_MIDDLE, false, 0.0f, 0);
            break;

        case 1:
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.1f, 0);
            break;

        case 2:
            m_pMonster->Change_State(CSimonManus::IDLE);
            return;
            break;

        default:
            break;
        }
        ++m_iRouteTrack;
    }
}

void CState_SimonManusP2_Route1::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP2_Route1::End_Check()
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

CState_SimonManusP2_Route1* CState_SimonManusP2_Route1::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Route1* pInstance = new CState_SimonManusP2_Route1(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Route1"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Route1::Free()
{
    __super::Free();
}