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
    m_iStateNum = iStateNum;

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
        if (!m_bPray)
        {
            if (m_pMonster->Get_Status()->fHp <= m_pMonster->Get_Status()->fMaxHp / 2.f)
            {
                m_bPray = true;
                m_pMonster->Change_State(CSimonManus::ATKP2_SUMMONHAND);
                return;
            }
        }


        //if (fDist <= 15.f && 9.f < fDist)
        //{
        //    //하이점프폴
        //    if (m_iAtkCnt >= 3.f)
        //    {
        //        m_iAtkCnt = 0;
        //        m_pMonster->Change_State(CSimonManus::ATKP2_HIGHJUMPFALL);
        //        return;
        //    }
        //    else    //스윙 점프
        //    {
        //        m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_2);
        //        return;
        //    }
        //}
        //else 
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
    if (m_iAtkTrack >= 17)
    {
        m_iAtkTrack = 0;
    }
  
    switch (m_iAtkTrack)
    {
    case 0:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_0);
        return;

    case 1:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_1);
        return;

    case 2:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_SWIPMULTIPLE);
        return;

    case 3:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_WAVE);
        return;

    case 4:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_JUMPTOATTACK);
        return;

    case 5:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_HIGHJUMPFALL);
        //m_pMonster->Change_State(CSimonManus::ATKP2_LIGHTNINGTOWAVE);
        return;

    case 6:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_AVOIDSWING);
        return;

    case 7:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_BRUTALATTACK);
        return;

    case 8:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_STAMP);
        return;

    case 9:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_SWINGDOWN_SWING);
        return;

    case 10:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_STING);
        return;

    case 11:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_THUNDERCALLING);
        return;

    case 12:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_CHASINGSWING);
        return;

    case 13:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_SPREADMAGIC);
        return;

    case 14:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_2);
        return;

    case 15:
        m_iAtkTrack++;
        m_pMonster->Change_State(CSimonManus::ATKP2_THUNDERBALL);
        return;

    default:
        break;
    }

    if (fDist <= 4.f)
    {
        //거리가 짧을때 루트 0,1 웨이브 스윕멀
        _int iAtkNum = rand() % 11;
        _bool bReCheck = { true };
        while (bReCheck)
        {
            if (iAtkNum == m_iPastNeerAtkNum)
            {
                iAtkNum = rand() % 11;
            }
            else if (iAtkNum <= 3)
            {
                if (iAtkNum == m_iPastFarAtkNum)
                {
                    iAtkNum = rand() % 11;
                }
                else
                    bReCheck = false;
            }
            else
                bReCheck = false;
        }

        m_iPastNeerAtkNum = iAtkNum;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_0);
            break;

        case 1:
            m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_1);
            break;

        case 2:
            m_pMonster->Change_State(CSimonManus::ATKP2_SWIPMULTIPLE);
            break;

        case 3:
            m_pMonster->Change_State(CSimonManus::ATKP2_WAVE);
            break;

        case 4:
            m_pMonster->Change_State(CSimonManus::ATKP2_JUMPTOATTACK);
            break;

        case 5:
            m_pMonster->Change_State(CSimonManus::ATKP2_SWIPMULTIPLE);
            //m_pMonster->Change_State(CSimonManus::ATKP2_LIGHTNINGTOWAVE);
            break;

        case 6:
            m_pMonster->Change_State(CSimonManus::ATKP2_AVOIDSWING);
            break;

        case 7:
            m_pMonster->Change_State(CSimonManus::ATKP2_BRUTALATTACK);
            break;

        case 8:
            m_pMonster->Change_State(CSimonManus::ATKP2_STAMP);
            break;

        case 9:
            m_pMonster->Change_State(CSimonManus::ATKP2_SWINGDOWN_SWING);
            break;

        case 10:
            m_pMonster->Change_State(CSimonManus::ATKP2_STING);
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
        _int iAtkNum = rand() % 10;
        _bool bReCheck = { true };
        while (bReCheck)
        {
            if (iAtkNum == m_iPastFarAtkNum)
            {
                iAtkNum = rand() % 10;
            }
            else if (iAtkNum <= 3)
            {
                if (iAtkNum == m_iPastNeerAtkNum)
                {
                    iAtkNum = rand() % 10;
                }
                else
                    bReCheck = false;
            }
            else
                bReCheck = false;
        }

        m_iPastFarAtkNum = iAtkNum;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_0);
            break;

        case 1:
            m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_1);
            break;

        case 2:
            m_pMonster->Change_State(CSimonManus::ATKP2_SWIPMULTIPLE);
            break;

        case 3:
            m_pMonster->Change_State(CSimonManus::ATKP2_WAVE);
            break;

        case 4:
            m_pMonster->Change_State(CSimonManus::ATKP2_THUNDERCALLING);
            break;

        case 5:
            m_pMonster->Change_State(CSimonManus::ATKP2_CHASINGSWING);
            break;

        case 6:
            m_pMonster->Change_State(CSimonManus::ATKP2_SPREADMAGIC);
            break;

        case 7:
            m_pMonster->Change_State(CSimonManus::ATKP2_SUMMONHAND);
            break;

        case 8:
            m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_2);
            break;

        case 9:
            m_pMonster->Change_State(CSimonManus::ATKP2_THUNDERBALL);
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