#include "stdafx.h"
#include "State_RaxasiaP2_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "GameInterface_Controller.h"

CState_RaxasiaP2_Idle::CState_RaxasiaP2_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);
    
    m_bFirstAct = false;
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

void CState_RaxasiaP2_Idle::Update(_float fTimeDelta)
{
    _float fDist = m_pMonster->Calc_Distance_XZ();

    if (m_fIdleEndDuration <= m_fIdleTime)
    {
        if (m_bFirstAct)
        {
            m_bFirstAct = false;
            m_pMonster->Change_State(CRaxasia::ATKP2_DECLAREWAR);
            return;
        }

        if (fDist <= m_fNeedDist_ForAttack)
        {
            Calc_Act_Attack();
            return;
        }
        else if (fDist > m_fNeedDist_ForAttack + m_fRunningWeights || m_bRunning)
        {
            if (!m_bRunning)
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
                m_pMonster->Change_Animation(AN_WALK, true, 0.1f, 0);
                m_bWalk = true;
            }
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.5, fTimeDelta);
            
            return;
        }

    }
    else
    {
        _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);
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
        m_fIdleTime += fTimeDelta;
    }

}

void CState_RaxasiaP2_Idle::End_State()
{
}

void CState_RaxasiaP2_Idle::Calc_Act_Attack()
{
    if (m_iAtkTrack >= 10)
    {
        m_iAtkTrack = 0;
    }

    m_iAtkTrack = 3;

    switch (m_iAtkTrack)
    {
    case 0:
        m_pMonster->Change_State(CRaxasia::ATKP2_WAVESTING);    //
        m_fNeedDist_ForAttack = 12.f;
        break;

    case 1:
        m_pMonster->Change_State(CRaxasia::ATKP2_RUNNING);      //
        m_fNeedDist_ForAttack = 20.f;
        break;

    case 2:
        m_pMonster->Change_State(CRaxasia::ATKP2_STEPJUMP);     //
        m_fNeedDist_ForAttack = 15.f;
        break;

    case 3:
        m_pMonster->Change_State(CRaxasia::ATKP2_TELE);         
        m_fNeedDist_ForAttack = 16.f;
        break;

    case 4:
        m_pMonster->Change_State(CRaxasia::ATKP2_TELEPORTATTACK);
        m_fNeedDist_ForAttack = 20.f;
        break;

    case 5:
        m_pMonster->Change_State(CRaxasia::ATKP2_JUMPSTAMP);
        m_fNeedDist_ForAttack = 13.f;
        break;

    case 6:
        m_pMonster->Change_State(CRaxasia::ATKP2_GUARD);
        m_fNeedDist_ForAttack = 15.f;
        break;

    case 7:
        m_pMonster->Change_State(CRaxasia::ATKP2_CHARGERUSH);
        m_fNeedDist_ForAttack = 12.f;
        break;

    case 8:
        m_pMonster->Change_State(CRaxasia::ATKP2_SHIELDRUSH);
        m_fNeedDist_ForAttack = 8.f;
        break;

    case 9:
        m_pMonster->Change_State(CRaxasia::ATKP2_SHIELDLINKED);
        m_fNeedDist_ForAttack = 22.f;
        break;

    default:
        break;
    }
    ++m_iAtkTrack;
}

CState_RaxasiaP2_Idle* CState_RaxasiaP2_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Idle* pInstance = new CState_RaxasiaP2_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Idle::Free()
{
    __super::Free();
}