#include "stdafx.h"
#include "State_CarcassTail_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassTail.h"

CState_CarcassTail_Idle::CState_CarcassTail_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);

    return S_OK;
}

void CState_CarcassTail_Idle::Update(_float fTimeDelta)
{
    _float fDist = m_pMonster->Calc_Distance_XZ();
    _int iDir = 2;

    //뒤에있을때 공격도 있어서. 회전을 거리가 멀때만 하게 조정
    if (fDist >= 9.f && fDist < 30.f)
    {
       iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);

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
    
    //방향값 사용을 위해 위로 올림

    m_fIdleTime += fTimeDelta;
    if (m_fIdleEndDuration <= m_fIdleTime)
    {
        if (fDist >= 30.f)
        {
            return;
        }

        if (iDir == 0 && fDist <= 20.f && fDist >= 10.f)
        {
            _int iAtkNum = rand() % 2;
            switch (iAtkNum)
            {
            case 0:
                m_pMonster->Change_State(CCarcassTail::LEAP);
                return;
                break;

            case 1:
                m_pMonster->Change_State(CCarcassTail::LEAPTOATTACK);
                return;
                break;

            default:
                break;
            }
        }

        if (fDist <= 7.f)
        {
            Calc_Act_Attack();
            return;
        }
        else if (fDist > 15.f)
        {
            m_pMonster->Change_State(CCarcassTail::RUN);
            return;
        }
        else if (fDist > 7.f)
        {
            m_pMonster->Change_State(CCarcassTail::WALK);
            return;
        }

    }


    
}

void CState_CarcassTail_Idle::End_State()
{
    m_fIdleTime = 0.f;
}

void CState_CarcassTail_Idle::Calc_Act_Attack()
{
    //얘는 뒤에있는지 아닌지 판단해서 꼬리로 할지 앞으로 공격할지 판단해야함


    _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
    _Vec3 vLook = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK));
    _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());
    _Vec3 vTargetRight = vUp.Cross(vTargetDir);
    

    _Vec3 vCrossUp = vLook.Cross(vTargetRight);

    if (vUp.Dot(vCrossUp) >= 0)
    {
        //앞 행동 긁, 박, 연찍, 긁꼬쓸, 연긁, 연박 리프어택

        _int iAtkNum = rand() % 7;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CCarcassTail::SCRATCHING);
            break;

        case 1:
            m_pMonster->Change_State(CCarcassTail::HEADING);
            break;

        case 2:
            m_pMonster->Change_State(CCarcassTail::MULTYHITTINGDOWN);
            break;

        case 3:
            m_pMonster->Change_State(CCarcassTail::SCRATCHINGTOWIP);
            break;

        case 4:
            m_pMonster->Change_State(CCarcassTail::SCRATCHINGMULTIPLE);
            break;

        case 5:
            m_pMonster->Change_State(CCarcassTail::HEADINGMULTIPLE);
            break;

        case 6:
            m_pMonster->Change_State(CCarcassTail::LEAPATTACK);
            break;

        default:
            break;
        }

    }
    else
    {
        //뒤 행동 찍, 쓸, 연쓸

        _int iAtkNum = rand() % 3;
        switch (iAtkNum)
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