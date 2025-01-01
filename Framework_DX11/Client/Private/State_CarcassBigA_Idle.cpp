#include "stdafx.h"
#include "State_CarcassBigA_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

#include "GameInterface_Controller.h"

CState_CarcassBigA_Idle::CState_CarcassBigA_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_pDiscover = m_pMonster->Get_bDicover();

    return S_OK;
}

HRESULT CState_CarcassBigA_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(20, true, 0.1f, 0);
    
    m_bRunning = false;
    m_bWalk = false;

    return S_OK;
}

void CState_CarcassBigA_Idle::Update(_float fTimeDelta)
{
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
                if (!m_bFirstMeetCheck)
                {
                    GET_GAMEINTERFACE->Set_OnOff_OrthoUI(true, m_pMonster);

                    m_bFirstMeetCheck = true;
                }

                _float fRadian = vTargetDir.Dot(vLook);
                if (fRadian >= XMConvertToRadians(60.f))
                {
                    (*m_pDiscover) = true;
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
            m_bFirstMeetCheck = false;
            (*m_pDiscover) = false;
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
            _Vec3 vDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

            m_pMonster->Get_RigidBody()->Set_Velocity(XMVector3Normalize(vDir) * m_fRunSpeed);
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
            _Vec3 vDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

            m_pMonster->Get_RigidBody()->Set_Velocity(XMVector3Normalize(vDir) * m_fWalkSpeed);
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

void CState_CarcassBigA_Idle::End_State()
{
    m_fIdleTime = 0.f;
}

void CState_CarcassBigA_Idle::Calc_Act_Attack()
{
    if (m_iAtkCnt >= 10)
    {
        m_iAtkCnt = 0;
    }
    //m_iAtkCnt = 6;
    switch (m_iAtkCnt)
    {
    case 0:
        m_pMonster->Change_State(CCarcassBigA::ATK_ROUTE_0);
        m_fNeedDist_ForAttack = 3.f;
        break;

    case 1:
        m_pMonster->Change_State(CCarcassBigA::ATK_ROUTE_1);
        m_fNeedDist_ForAttack = 3.f;
        break;

    case 2:
        m_pMonster->Change_State(CCarcassBigA::ATK_ROUTE_2);
        m_fNeedDist_ForAttack = 3.f;
        break;

    case 3:
        m_pMonster->Change_State(CCarcassBigA::ATK_ROUTE_3);
        m_fNeedDist_ForAttack = 3.f;
        break;

    case 4:
        m_pMonster->Change_State(CCarcassBigA::LO_SWINGRIGHT);
        m_fNeedDist_ForAttack = 6.f;
        break;

    case 5:
        m_pMonster->Change_State(CCarcassBigA::ATK_ROUTE_4);
        m_fNeedDist_ForAttack = 3.f;
        break;

    case 6:
        m_pMonster->Change_State(CCarcassBigA::WHEELWIND);
        m_fNeedDist_ForAttack = 6.f;
        break;

    case 7:
        m_pMonster->Change_State(CCarcassBigA::ATK_IMPACT);
        m_fNeedDist_ForAttack = 2.5f;
        break;

    case 8:
        m_pMonster->Change_State(CCarcassBigA::RAGE_ATTACK);
        m_fNeedDist_ForAttack = 3.f;
        break;

    case 9:
        m_pMonster->Change_State(CCarcassBigA::LT_SWINGRIGHT);
        m_fNeedDist_ForAttack = 3.f;
        break;


    default:
        break;
    }
    ++m_iAtkCnt;
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