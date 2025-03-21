#include "stdafx.h"
#include "State_CarcassNormal_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassNormal.h"

#include "GameInterface_Controller.h"

CState_CarcassNormal_Idle::CState_CarcassNormal_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassNormal_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_pDiscover = m_pMonster->Get_bDicover();
    m_pFirstMeetCheck = m_pMonster->Get_FirstMeetCheck();

    return S_OK;
}

HRESULT CState_CarcassNormal_Idle::Start_State(void* pArg)
{
    m_pMonster->Get_Model()->Set_RemoteTuning(true);
    m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);

    m_bWalk = false;
    m_bRunning = false;

    return S_OK;
}

void CState_CarcassNormal_Idle::Update(_float fTimeDelta)
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
            (*m_pFirstMeetCheck) = false;
            (*m_pDiscover) = false;
            return;
        }

        if (fDist <= m_fNeedDist_ForAttack)
        {
            Calc_Act_Attack(fTimeDelta, fDist);
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

void CState_CarcassNormal_Idle::End_State()
{
    m_pMonster->Get_Model()->Set_RemoteTuning(false);
    m_fIdleTime = 0.f;
}

void CState_CarcassNormal_Idle::Calc_Act_Attack(_float fTimeDelta, _float fDist)
{
    //���϶�
    _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
    _Vec3 vRight = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
    _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());


    _Vec3 vCrossUp = vRight.Cross(vTargetDir);

    if (vCrossUp.y > 0)
    {
        if (fDist <= 6.f)
        {
            m_pMonster->Change_State(CCarcassNormal::HEADINGMULTIPLE);
            return;
        }
        else
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3, fTimeDelta);
        }
    }
    else
    {
        if (m_iAtkCnt >= 5)
        {
            m_iAtkCnt = 0;
        }

        switch (m_iAtkCnt)
        {
        case 0:
            m_pMonster->Change_State(CCarcassNormal::TRIPLECLAW);
            m_fNeedDist_ForAttack = 2.f;
            break;

        case 1:
            m_pMonster->Change_State(CCarcassNormal::CLAWMULTIPLE);
            m_fNeedDist_ForAttack = 7.f;
            break;

        case 2:
            m_pMonster->Change_State(CCarcassNormal::CLAWRUSH);
            m_fNeedDist_ForAttack = 3.f;
            break;

        case 3:
            m_pMonster->Change_State(CCarcassNormal::TRIPLECLAW_2);
            m_fNeedDist_ForAttack = 2.5f;
            break;

        case 4:
            m_pMonster->Change_State(CCarcassNormal::BITE);
            m_fNeedDist_ForAttack = 3.f;
            break;

        default:
            break;
        }
        ++m_iAtkCnt;
    }
}

CState_CarcassNormal_Idle* CState_CarcassNormal_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassNormal_Idle* pInstance = new CState_CarcassNormal_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassNormal_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassNormal_Idle::Free()
{
    __super::Free();
}