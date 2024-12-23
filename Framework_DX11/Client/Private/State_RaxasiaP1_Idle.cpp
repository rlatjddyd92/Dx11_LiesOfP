#include "stdafx.h"
#include "State_RaxasiaP1_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "GameInterface_Controller.h"

CState_RaxasiaP1_Idle::CState_RaxasiaP1_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP1_Idle::Start_State(void* pArg)
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

void CState_RaxasiaP1_Idle::Update(_float fTimeDelta)
{
    _float fDist = m_pMonster->Calc_Distance_XZ();
    if (!m_bFirstMeetCheck)
    {
        _Vec3 vTargetPos = m_pMonster->Get_TargetPos();
        _Vec3 vMonsterPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        if (fDist <= 25.f && abs(vTargetPos.y - vMonsterPos.y) <= 5.f)
        {
            GET_GAMEINTERFACE->Set_OnOff_OrthoUI(true, m_pMonster);
            m_bFirstMeetCheck = true;
        }
        else
        {
            return;
        }
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

void CState_RaxasiaP1_Idle::End_State()
{
}

void CState_RaxasiaP1_Idle::Calc_Act_Attack()
{
    if (m_iAtkTrack >= 7)
    {
        m_iAtkTrack = 0;
    }
    m_iAtkTrack = 1;
    switch (m_iAtkTrack)
    {
    case 0:
        m_pMonster->Change_State(CRaxasia::ATK_DASHUPPER);
        m_fNeedDist_ForAttack = 7.5f;
        break;

    case 1:
        m_pMonster->Change_State(CRaxasia::ATK_GROUNDSLASH);
        m_fNeedDist_ForAttack = 3.5f;
        break;

    case 2:
        m_pMonster->Change_State(CRaxasia::ATK_KICKSTING);
        m_fNeedDist_ForAttack = 3.5f;
        break;

    case 3:
        m_pMonster->Change_State(CRaxasia::ATK_LINKEDATTACK);
        m_fNeedDist_ForAttack = 4.5f;
        break;

    case 4:
        m_pMonster->Change_State(CRaxasia::ATK_REPETUPPERSLASH);
        m_fNeedDist_ForAttack = 6.5f;
        break;

    case 5:
        m_pMonster->Change_State(CRaxasia::ATK_TRIPLESTING);
        m_fNeedDist_ForAttack = 6.5f;
        break;

    case 6:
        m_pMonster->Change_State(CRaxasia::ATK_STING_ANDSPREAD);
        m_fNeedDist_ForAttack = 8.f;
        break;

    default:
        break;
    }
    ++m_iAtkTrack;
}

CState_RaxasiaP1_Idle* CState_RaxasiaP1_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_Idle* pInstance = new CState_RaxasiaP1_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_Idle::Free()
{
    __super::Free();
}