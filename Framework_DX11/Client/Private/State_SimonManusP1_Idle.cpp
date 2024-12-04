#include "stdafx.h"
#include "State_SimonManusP1_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_Idle::CState_SimonManusP1_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_SimonManusP1_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);
    
    if (pArg != nullptr)
    {
        m_fIdleTime = *static_cast<_float*>(pArg);
    }
    else
    {
        m_fIdleTime = 0;
    }


    return S_OK;
}

void CState_SimonManusP1_Idle::Update(_float fTimeDelta)
{
    m_fIdleTime += fTimeDelta;
    _float fDist = m_pMonster->Calc_Distance_XZ();
    if (m_fIdleEndDuration <= m_fIdleTime)
    {
        if (fDist >= 30.f)
        {
            return;
        }

        //m_pMonster->Change_State(CSimonManus::ATK_SWIPMULT_L);
        //return;

        if (m_iAtkCnt >= 3.f && fDist <= 20.f && 9.f < fDist)   //m_iAtkCnt >= 3.f && fDist <= 15.f && 6.f < fDist
        {
            //하이점프폴
            m_iAtkCnt = 0;
            m_pMonster->Change_State(CSimonManus::ATK_HIGHJUMPFALL);
            return;
        }
        if (fDist <= 10.f)
        {
            Calc_Act_Attack(fDist);
            return;
        }
        else if (fDist > 14.f)
        {
            m_pMonster->Change_State(CSimonManus::RUN);
            return;
        }
        else if (fDist > 9.f)
        {
            m_pMonster->Change_State(CSimonManus::WALK);
            return;
        }

        return;
    }

    _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3, fTimeDelta);
    switch (iDir)
    {
    case -1:
        m_pMonster->Change_Animation(AN_TURNLEFT, true, 0.1f, 0);
        break;

    case 0:
        m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);
        break;

    case 1:
        m_pMonster->Change_Animation(AN_TURNRIGHT, true, 0.1f, 0);
        break;

    default:
        break;
    }

}

void CState_SimonManusP1_Idle::End_State()
{
}

void CState_SimonManusP1_Idle::Calc_Act_Attack(_float fDist)
{
    if (fDist <= 6.f)
    {
        //어보이드, 점프, 스다 엘알  스탬프, 차지까지 까지
        _int iAtkNum = rand() % 5;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CSimonManus::ATK_AVOIDSWING);
            break;

        case 1:
            m_pMonster->Change_State(CSimonManus::ATK_SWINGDOWN_L);
            break;

        case 2:
            m_pMonster->Change_State(CSimonManus::ATK_SWINGDOWN_R);
            break;

        case 3:
            m_pMonster->Change_State(CSimonManus::ATK_STAMP);
            break;

        case 4:
            m_pMonster->Change_State(CSimonManus::ATK_JUMPTOSWING);
            break;


        default:
            break;
        }
        ++m_iAtkCnt;

        return;
    }
    else if (fDist <= 8.f)
    {

        _int iAtkNum = rand() % 4;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CSimonManus::ATK_STING);
            break;

        case 1:
            m_pMonster->Change_State(CSimonManus::ATK_SWINGDOWN_L);
            break;

        case 2:
            m_pMonster->Change_State(CSimonManus::ATK_SWINGDOWN_R);
            break;

        case 3:
            m_pMonster->Change_State(CSimonManus::ATK_CHARGE_SWINGDOWN);
            break;

        default:
            break;
        }
        ++m_iAtkCnt;

        return;

    }
    else
    {
        //거리가 긴 기술 스멀 엘알 체이싱 스팅
        _int iAtkNum = rand() % 3;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CSimonManus::ATK_CHASINGSWING);
            break;

        case 1:
            m_pMonster->Change_State(CSimonManus::ATK_SWIPMULT_L);
            break;

        case 2:
            m_pMonster->Change_State(CSimonManus::ATK_SWIPMULT_R);
            break;

        default:
            break;
        }

        ++m_iAtkCnt;

        return;
    }

}

CState_SimonManusP1_Idle* CState_SimonManusP1_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_Idle* pInstance = new CState_SimonManusP1_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_Idle::Free()
{
    __super::Free();
}