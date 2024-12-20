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

void CState_SimonManusP2_Idle::End_State()
{
    m_fIdleTime = 0.f;
}

void CState_SimonManusP2_Idle::Calc_Act_Attack()
{
    if (m_iAtkTrack >= 16)
    {
        m_iAtkTrack = 0;
    }
  
    switch (m_iAtkTrack)
    {
    case 0:
        m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_0);
        m_fNeedDist_ForAttack = 8.f;
        break;

    case 1:
        m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_1);
        m_fNeedDist_ForAttack = 8.f;
        break;

    case 2:
        m_pMonster->Change_State(CSimonManus::ATKP2_SWIPMULTIPLE);
        m_fNeedDist_ForAttack = 12.f;
        break;

    case 3:
        m_pMonster->Change_State(CSimonManus::ATKP2_WAVE);
        m_fNeedDist_ForAttack = 8.f;
        break;

    case 4:
        m_pMonster->Change_State(CSimonManus::ATKP2_JUMPTOATTACK);
        m_fNeedDist_ForAttack = 15.f;
        break;

    case 5:
        m_pMonster->Change_State(CSimonManus::ATKP2_HIGHJUMPFALL);
        m_fNeedDist_ForAttack = 6.f;
        //m_pMonster->Change_State(CSimonManus::ATKP2_LIGHTNINGTOWAVE);
        break;

    case 6:
        m_pMonster->Change_State(CSimonManus::ATKP2_AVOIDSWING);
        m_fNeedDist_ForAttack = 6.f;
        break;

    case 7:
        m_pMonster->Change_State(CSimonManus::ATKP2_BRUTALATTACK);
        m_fNeedDist_ForAttack = 5.5f;
        break;

    case 8:
        m_pMonster->Change_State(CSimonManus::ATKP2_STAMP);
        m_fNeedDist_ForAttack = 6.5f;
        break;

    case 9:
        m_pMonster->Change_State(CSimonManus::ATKP2_SWINGDOWN_SWING);
        m_fNeedDist_ForAttack = 7.5f;
        break;

    case 10:
        m_pMonster->Change_State(CSimonManus::ATKP2_STING);
        m_fNeedDist_ForAttack = 20.f;
        break;

    case 11:
        m_pMonster->Change_State(CSimonManus::ATKP2_THUNDERCALLING);
        m_fNeedDist_ForAttack = 10.5f;
        break;

    case 12:
        m_pMonster->Change_State(CSimonManus::ATKP2_CHASINGSWING);
        m_fNeedDist_ForAttack = 12.5f;
        break;

    case 13:
        m_pMonster->Change_State(CSimonManus::ATKP2_SPREADMAGIC);
        m_fNeedDist_ForAttack = 8.f;
        break;

    case 14:
        m_pMonster->Change_State(CSimonManus::ATKP2_ROUTE_2);
        m_fNeedDist_ForAttack = 20.f;
        break;

    case 15:
        m_pMonster->Change_State(CSimonManus::ATKP2_THUNDERBALL);
        m_fNeedDist_ForAttack = 8.f;
        break;

    default:
        break;
    }
    ++m_iAtkTrack;
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