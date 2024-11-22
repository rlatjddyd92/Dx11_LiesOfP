#include "stdafx.h"
#include "State_CarcassBigA_AttackRoute_0.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_AttackRoute_0::CState_CarcassBigA_AttackRoute_0(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_AttackRoute_0::Initialize(_uint iStateNum, void* pArg)
{
    //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pResetRootMove = pDesc->pIsResetRootMove;

    return S_OK;
}

HRESULT CState_CarcassBigA_AttackRoute_0::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f);

    m_fIdleTime = m_fIdleDuration;

    m_isDelayed = false;

    return S_OK;
}

void CState_CarcassBigA_AttackRoute_0::Update(_float fTimeDelta)
{
    if (!m_isDelayed)
    {
        if(m_iRouteTrack == 1)
        {
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.1f, 0, true);
        }

        if (End_Check())
        {
            ++m_iRouteTrack;

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
    

}

void CState_CarcassBigA_AttackRoute_0::End_State()
{
    m_iRouteTrack = 0;
    m_fIdleTime = 0.f;
    *m_pResetRootMove = true;
}

_bool CState_CarcassBigA_AttackRoute_0::End_Check()
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

CState_CarcassBigA_AttackRoute_0* CState_CarcassBigA_AttackRoute_0::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_AttackRoute_0* pInstance = new CState_CarcassBigA_AttackRoute_0(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_AttackRoute_0"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_AttackRoute_0::Free()
{
    __super::Free();
}