#include "stdafx.h"
#include "State_SimonManusP1_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "GameInterface_Controller.h"

CState_SimonManusP1_Idle::CState_SimonManusP1_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_pFirstMeetCheck = m_pMonster->Get_FirstMeetCheck();

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

    m_bRunning = false;
    m_bWalk = false;

    return S_OK;
}

void CState_SimonManusP1_Idle::Update(_float fTimeDelta)
{
    if (m_pMonster->Get_TargetDead())
    {
        m_pMonster->Change_Animation(AN_IDLE, true, 0.1f);
        return;
    }

    _float fDist = m_pMonster->Calc_Distance_XZ();
    if (!(*m_pFirstMeetCheck))
    {
        return;
    }
    else if (m_fIdleEndDuration <= m_fIdleTime)
    {

        if (fDist <= m_fNeedDist_ForAttack)
        {
            Calc_Act_Attack();
            return;
        }
        else if (fDist > m_fNeedDist_ForAttack + m_fRunningWeights || m_bRunning)
        {
            if (!m_bRunning ) // || (m_pMonster->Get_Model()->Get_CurrentAnimationIndex() != AN_RUN)
            {
                m_pMonster->Change_Animation(AN_RUN, true, 0.1f, 0);
                m_bRunning = true;
            }
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
            
            return;
        }
        else if (fDist > m_fNeedDist_ForAttack)
        {
            if (!m_bWalk)
            {
                m_pMonster->Change_Animation(AN_WALK_F, true, 0.1f, 0);
                m_bWalk = true;
            }
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.5, fTimeDelta);
            
            return;
        }

    }
    else
    {
        _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);


        if (fDist <= m_fNeedDist_ForAttack)
        {
            if (m_pMonster->Get_CurrentAnimIndex() != AN_WALK_B)
            {
                m_pMonster->Change_Animation(AN_WALK_B, true, 0.1f, 0);
            }
        }
        else
        {
            switch (iDir)
            {
            case -1:
                m_pMonster->Change_Animation(AN_TURN_LEFT, true, 0.1f);
                break;

            case 0:
                m_pMonster->Change_Animation(AN_IDLE, true, 0.1f);
                break;

            case 1:
                m_pMonster->Change_Animation(AN_TURN_RIGHT, true, 0.1f);
                break;

            default:
                break;
            }
        }

        m_fIdleTime += fTimeDelta;

    }


}

void CState_SimonManusP1_Idle::End_State()
{
}

void CState_SimonManusP1_Idle::Calc_Act_Attack()
{
    if (m_iAtkTrack >= 10)
    {
        m_iAtkTrack = 0;
    }

    //if (m_iAtkTrack == 5)
    //{
    //    m_iAtkTrack = 9;
    //}
    //else
    //{
    //    m_iAtkTrack = 4;
    //}
    switch (m_iAtkTrack)
    {
    case 0:
        m_pMonster->Change_State(CSimonManus::ATK_SWINGDOWN_L);
        m_fNeedDist_ForAttack = 5.5f;
        break;

    case 1:
        m_pMonster->Change_State(CSimonManus::ATK_SWINGDOWN_R);
        m_fNeedDist_ForAttack = 7.5f;
        break;

    case 2:
        m_pMonster->Change_State(CSimonManus::ATK_STAMP);
        m_fNeedDist_ForAttack = 8.f;
        break;

    case 3:
        m_pMonster->Change_State(CSimonManus::ATK_JUMPTOSWING);
        m_fNeedDist_ForAttack = 9.f;
        break;

    case 4:
        m_pMonster->Change_State(CSimonManus::ATK_STING);
        m_fNeedDist_ForAttack = 4.5f;
        break;

    case 5:
        m_pMonster->Change_State(CSimonManus::ATK_CHARGE_SWINGDOWN);
        m_fNeedDist_ForAttack = 15.f;
        break;

    case 6:
        m_pMonster->Change_State(CSimonManus::ATK_CHASINGSWING);
        m_fNeedDist_ForAttack = 12.5f;
        break;

    case 7:
        m_pMonster->Change_State(CSimonManus::ATK_SWIPMULT_L);
        m_fNeedDist_ForAttack = 12.5f;
        break;

    case 8:
        m_pMonster->Change_State(CSimonManus::ATK_SWIPMULT_R);
        m_fNeedDist_ForAttack = 15.f;
        break;

    case 9:
        m_pMonster->Change_State(CSimonManus::ATK_HIGHJUMPFALL);
        m_fNeedDist_ForAttack = 5.5f;
        break;

    default:
        break;
    }
    m_iAtkTrack++;
    return;


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