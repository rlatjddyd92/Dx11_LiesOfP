#include "stdafx.h"
#include "State_CarcassBigA_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Idle::CState_CarcassBigA_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    return S_OK;
}

HRESULT CState_CarcassBigA_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(20, true, 0.1f, 0);
    
    return S_OK;
}

void CState_CarcassBigA_Idle::Update(_float fTimeDelta)
{
    m_fIdleTime += fTimeDelta;
    _float fDist = m_pMonster->Calc_Distance_XZ();
    if (fDist <= m_fIdleTime)
    {
        if (m_pMonster->Calc_Distance_XZ() <= 5.f)
        {
            Calc_Act_Attack();
            return;
        }
        else if (fDist > 8.f)
        {
            m_pMonster->Change_State(CCarcassBigA::RUN);
            return;
        }
        else if (fDist > 5.f)
        {
            m_pMonster->Change_State(CCarcassBigA::WALK);
            return;
        }

    }


    _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
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

void CState_CarcassBigA_Idle::End_State()
{
    m_fIdleTime = 0.f;
}

void CState_CarcassBigA_Idle::Calc_Act_Attack()
{
    if (false)//이후 체력이 일정 이하일때 어택카운트 제서 나오도록
    {
        m_pMonster->Change_State(CCarcassBigA::RAGE_ATTACK);
        return;
    }
    if (m_iAtkCnt < 3.f)
    {
        _int iAtkNum = rand() % 6;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CCarcassBigA::ATK_ROUTE_0);
            break;

        case 1:
            m_pMonster->Change_State(CCarcassBigA::ATK_ROUTE_1);
            break;

        case 2:
            m_pMonster->Change_State(CCarcassBigA::ATK_ROUTE_2);
            break;

        case 3:
            m_pMonster->Change_State(CCarcassBigA::ATK_ROUTE_3);
            break;

        case 4:
            m_pMonster->Change_State(CCarcassBigA::ATK_ROUTE_4);
            break;

        case 5:
            m_pMonster->Change_State(CCarcassBigA::WHEELWIND);
            break;

        default:
            break;
        }
        ++m_iAtkCnt;

    }
    else
    {
        //신체가 붉게 변하며 나오는 기술

        _int iAtkNum = rand() % 2;

        if (iAtkNum)
        {
            m_pMonster->Change_State(CCarcassBigA::LO_SWINGRIGHT);
        }
        else
        {
            m_pMonster->Change_State(CCarcassBigA::LT_SWINGRIGHT);
        }
        m_iAtkCnt = 0;

    }
}

CState_CarcassBigA_Idle* CState_CarcassBigA_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_Idle* pInstance = new CState_CarcassBigA_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Idle::Free()
{
    __super::Free();
}