#include "stdafx.h"
#include "State_CarcassTail_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassTail.h"

#include "GameInterface_Controller.h"

CState_CarcassTail_Idle::CState_CarcassTail_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_pDiscover = m_pMonster->Get_bDicover();
    m_pFirstMeetCheck = m_pMonster->Get_FirstMeetCheck();

    return S_OK;
}

HRESULT CState_CarcassTail_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);

    m_bRunning = false;
    m_bWalk = false;

    return S_OK;
}

void CState_CarcassTail_Idle::Update(_float fTimeDelta)
{
    _int iDir = 2;
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
                _int iDirCheck = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 0.6f, fTimeDelta);
                switch (iDirCheck)
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
        if (fDist >= 9.f && fDist < 30.f || m_fLinkedTailCtrl >= 3)
        {
            //뒤에있을때 공격도 있어서. 회전을 거리가 멀때만 하게 조정
            iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);

            switch (iDir)
            {
            case -1:
                m_pMonster->Change_Animation(AN_TURN_LEFT, true, 0.1f);
                break;

            case 0:
                m_pMonster->Change_Animation(AN_IDLE, true, 0.1f);
                m_fLinkedTailCtrl = 0;
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

void CState_CarcassTail_Idle::End_State()
{
    m_fIdleTime = 0.f;
}

void CState_CarcassTail_Idle::Calc_Act_Attack(_float fTimeDelta, _float fDist)
{
    //얘는 뒤에있는지 아닌지 판단해서 꼬리로 할지 앞으로 공격할지 판단해야함


    _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
    _Vec3 vRight = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
    _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());


    _Vec3 vCrossUp = vRight.Cross(vTargetDir);

    if (vCrossUp.y < 0)
    {
        //앞 행동 긁, 박, 연찍, 긁꼬쓸, 연긁, 연박 리프어택

        if (m_iAtkCnt >= 9)
        {
            m_iAtkCnt = 0;
        }

        m_iAtkCnt = 4;
        switch (m_iAtkCnt)
        {
        case 0:
            m_pMonster->Change_State(CCarcassTail::SCRATCHING);
            m_fNeedDist_ForAttack = 4.5f;
            break;

        case 1:
            m_pMonster->Change_State(CCarcassTail::HEADING);
            m_fNeedDist_ForAttack = 10.f;
            break;

        case 2:
            m_pMonster->Change_State(CCarcassTail::LEAP);
            m_fNeedDist_ForAttack = 4.5f;
            break;

        case 3:
            m_pMonster->Change_State(CCarcassTail::SCRATCHINGTOWIP);
            m_fNeedDist_ForAttack = 4.5f;
            break;

        case 4:
            m_pMonster->Change_State(CCarcassTail::SCRATCHINGMULTIPLE);
            m_fNeedDist_ForAttack = 5.f;
            break;

        case 5:
            m_pMonster->Change_State(CCarcassTail::HEADINGMULTIPLE);
            m_fNeedDist_ForAttack = 12.f;
            break;

        case 6:
            m_pMonster->Change_State(CCarcassTail::LEAPATTACK);
            m_fNeedDist_ForAttack = 4.5f;
            break;

        case 7:
            m_pMonster->Change_State(CCarcassTail::MULTYHITTINGDOWN);
            m_fNeedDist_ForAttack = 15.f;
            break;

        case 8:
            m_pMonster->Change_State(CCarcassTail::LEAPTOATTACK);
            m_fNeedDist_ForAttack = 4.5f;
            break;

        default:
            break;
        }
        ++m_iAtkCnt;

    }
    else
    {
        if (m_fLinkedTailCtrl >= 3.f)
        {
            return;
        }
        //뒤 행동 찍, 쓸, 연쓸
        if (m_iTailAtkCnt >= 3)
        {
            m_iTailAtkCnt = 0;
        }

        ++m_fLinkedTailCtrl;
        
        switch (m_iTailAtkCnt)
        {
        case 0:
            m_pMonster->Change_State(CCarcassTail::TAILSWINGDOWN);
            break;

        case 1:
            m_pMonster->Change_State(CCarcassTail::TAILSWIP);
            break;

        case 2:
            m_pMonster->Change_State(CCarcassTail::TAILSWIPMULTIPLE);
            break;

        default:
            break;
        }
        ++m_iTailAtkCnt;
    }
}

CState_CarcassTail_Idle* CState_CarcassTail_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_Idle* pInstance = new CState_CarcassTail_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_Idle::Free()
{
    __super::Free();
}