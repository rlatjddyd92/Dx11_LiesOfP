#include "stdafx.h"
#include "State_SimonManusP2_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_Idle::CState_SimonManusP2_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Idle::Initialize(_uint iStateNum, void* pArg)
{
    //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    return S_OK;
}

HRESULT CState_SimonManusP2_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP2_Idle::Update(_float fTimeDelta)
{
    m_fIdleTime += fTimeDelta;
    _float fDist = m_pMonster->Calc_Distance_XZ();
    if (m_fIdleEndDuration <= m_fIdleTime)
    {
        if (m_fIdleTime >= m_fIdleDuration + 2.f)
        {
            if (fDist <= 15.f && 9.f < fDist)
            {
                //하이점프폴
                if (m_iAtkCnt >= 3.f)
                {
                    m_pMonster->Change_State(CSimonManus::ATKP2_HIGHJUMPFALL);
                    m_iAtkCnt = 0;
                    return;
                }
                else    //스윙 점프
                {
                    m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_2);
                    return;
                }
            }
        }
        

        if (fDist <= 7.f)
        {
            Calc_Act_Attack(fDist);
            return;
        }
        else if (fDist > 9.f)
        {
            m_pMonster->Change_State(CSimonManus::RUN);
            return;
        }
        else if (fDist > 6.f)
        {
            m_pMonster->Change_State(CSimonManus::WALK);
            return;
        }

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

void CState_SimonManusP2_Idle::End_State()
{
    m_fIdleTime = 0.f;
}

void CState_SimonManusP2_Idle::Calc_Act_Attack(_float fDist)
{
    if (fDist <= 4.f)
    {
        //거리가 짧을때
        _int iAtkNum = rand() % 11;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CSimonManus::ATKP2_AVOIDSWING);
            break;

        case 1:
            m_pMonster->Change_State(CSimonManus::ATKP2_BRUTALATTACK);
            break;

        case 2:
            m_pMonster->Change_State(CSimonManus::ATKP2_STING);
            break;

        case 3:
            m_pMonster->Change_State(CSimonManus::ATKP2_WAVE);
            break;

        case 4:
            m_pMonster->Change_State(CSimonManus::ATKP2_JUMPTOATTACK);
            break;

        case 5:
            m_pMonster->Change_State(CSimonManus::ATKP2_LIGHTNINGTOWAVE);
            break;

        case 6:
            m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_0);
            break;

        case 7:
            m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_1);
            break;

        case 8:
            m_pMonster->Change_State(CSimonManus::ATKP2_STAMP);
            break;

        case 9:
            m_pMonster->Change_State(CSimonManus::ATKP2_SWINGDOWN_SWING);
            break;

        case 10:
            m_pMonster->Change_State(CSimonManus::ATKP2_SWIPMULTIPLE);
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
        //스프레드, 서몬, 볼, 콜링, 웨이브
        //체이싱, 루트 0, 1, 2, 스윕 멀티
        _int iAtkNum = rand() % 11;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CSimonManus::ATKP2_SPREADMAGIC);
            break;

        case 1:
            m_pMonster->Change_State(CSimonManus::ATKP2_SUMMONHAND);
            break;

        case 2:
            m_pMonster->Change_State(CSimonManus::ATKP2_THUNDERBALL);
            break;

        case 3:
            m_pMonster->Change_State(CSimonManus::ATKP2_THUNDERCALLING);
            break;

        case 4:
            m_pMonster->Change_State(CSimonManus::ATKP2_WAVE);
            break;

        case 5:
            m_pMonster->Change_State(CSimonManus::ATKP2_CHASINGSWING);
            break;

        case 6:
            m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_0);
            break;

        case 7:
            m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_1);
            break;

        case 8:
            m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_2);
            break;

        case 9:
            m_pMonster->Change_State(CSimonManus::ATKP2_SWIPMULTIPLE);
            break;

        default:
            break;
        }
        ++m_iAtkCnt;

        return;
    }

}

CState_SimonManusP2_Idle* CState_SimonManusP2_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Idle* pInstance = new CState_SimonManusP2_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Idle::Free()
{
    __super::Free();
}