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
    m_fIdleTime += fTimeDelta;
    _float fDist = m_pMonster->Calc_Distance_XZ();
    if (m_fIdleEndDuration <= m_fIdleTime)
    {
        if (m_pMonster->Calc_Distance_XZ() <= 7.f)
        {
            Calc_Act_Attack();
            return;
        }
        else if (fDist > 10.f)
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


    //뒤에있을때 공격도 있어서. 회전을 거리가 멀때만 하게 조정
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
}

void CState_CarcassTail_Idle::End_State()
{
    m_fIdleTime = 0.f;
}

void CState_CarcassTail_Idle::Calc_Act_Attack()
{
    //얘는 뒤에있는지 아닌지 판단해서 꼬리로 할지 앞으로 공격할지 판단해야함


    _Vec3 vPlayerUp = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP);
    _Vec3 vLook = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    _Vec3 vTargetDir = m_pMonster->Get_TargetDir();

    _Vec3 vCrossUp = vLook.Cross(vTargetDir);

    if (vPlayerUp.Dot(vCrossUp) <= 0)
    {
        //앞 행동
        _int iAtkNum = rand() % 6;
        iAtkNum = 0;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CCarcassTail::SCRATCHING);
            break;

        default:
            break;
        }

    }
    else
    {
        //뒤 행동
        _int iAtkNum = rand() % 6;
        iAtkNum = 0;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CCarcassTail::SCRATCHING);
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