#include "stdafx.h"
#include "State_CurruptedStrongArm_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "CurruptedStrongArm_Puppet.h"

#include "GameInterface_Controller.h"

CState_CurruptedStrongArm_Idle::CState_CurruptedStrongArm_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CurruptedStrongArm_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_pDiscover = m_pMonster->Get_bDicover();
    m_pFirstMeetCheck = m_pMonster->Get_FirstMeetCheck();

    return S_OK;
}

HRESULT CState_CurruptedStrongArm_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);

    m_bRunning = false;
    m_bWalk = false;
    m_bWalkBack = false;

    return S_OK;
}

void CState_CurruptedStrongArm_Idle::Update(_float fTimeDelta)
{
    if (m_pMonster->Get_TargetDead())
    {
        m_pMonster->Change_Animation(AN_IDLE, true, 0.1f);
        return;
    }

    _float fDist = m_pMonster->Calc_Distance_XZ();
    if (!(*m_pDiscover))
    {
        _Vec3 vTargetPos = m_pMonster->Get_TargetPos();
        _Vec3 vMonsterPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);

        if (abs(vTargetPos.y - vMonsterPos.y) <= 3.f)
        {
            _Vec3 vTargetDir = m_pMonster->Get_TargetDir();
            _Vec3 vLook = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);
            if (fDist <= m_fDiscoverDist)
            {
                _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 0.6f, fTimeDelta);
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
            else
            {
                m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);
            }

            if (fDist <= m_fRecognitionDist)
            {
                if (!(*m_pFirstMeetCheck))
                {
                    GET_GAMEINTERFACE->Set_OnOff_OrthoUI(true, m_pMonster);

                    (*m_pFirstMeetCheck) = true;
                }

                _float fRadian = vTargetDir.Dot(vLook);
                if (fRadian >= XMConvertToRadians(60.f))
                {
                    *m_pDiscover = true;
                    return;
                }

            }
        }
        else
        {
            return;
        }
    }
    else if (m_fIdleEndDuration <= m_fIdleTime)
    {
        if (fDist >= 30.f)
        {
            (*m_pFirstMeetCheck) = false;
            *m_pDiscover = false;
            return;
        }

        if (fDist <= m_fNeedDist_ForAttack && fDist >= m_fNeedLowerDist_ForAttack)
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
        else
        {
            if (!m_bWalkBack)
            {
                m_pMonster->Change_Animation(AN_WALK_BACK, true, 0.1f, 0);
                m_bWalkBack = true;
            }
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.5f, fTimeDelta);
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

void CState_CurruptedStrongArm_Idle::End_State()
{
    m_fIdleTime = 0.f;
}

void CState_CurruptedStrongArm_Idle::Calc_Act_Attack()
{
    if (m_iAtkCnt >= 3)
    {
        m_iAtkCnt = 0;
    }
    switch (m_iAtkCnt)
    {
    case 0:
        m_pMonster->Change_State(CCurruptedStrongArm_Puppet::SWIPATTACK);
        m_fNeedLowerDist_ForAttack = 4.f;
        m_fNeedDist_ForAttack = 7.f;
        break;

    case 1:
        m_pMonster->Change_State(CCurruptedStrongArm_Puppet::STINGTWICE);
        m_fNeedLowerDist_ForAttack = 0.f;
        m_fNeedDist_ForAttack = 3.5f;
        break;

    case 2:
        m_pMonster->Change_State(CCurruptedStrongArm_Puppet::JUMP_PUNCH);
        m_fNeedLowerDist_ForAttack = 0.f;
        m_fNeedDist_ForAttack = 3.f;
        break;

    default:
        break;
    }
    ++m_iAtkCnt;
}

CState_CurruptedStrongArm_Idle* CState_CurruptedStrongArm_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CurruptedStrongArm_Idle* pInstance = new CState_CurruptedStrongArm_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CurruptedStrongArm_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CurruptedStrongArm_Idle::Free()
{
    __super::Free();
}