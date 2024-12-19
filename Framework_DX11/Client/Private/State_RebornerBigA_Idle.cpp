#include "stdafx.h"
#include "State_RebornerBigA_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "RebornerBigA.h"

#include "GameInterface_Controller.h"

CState_RebornerBigA_Idle::CState_RebornerBigA_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerBigA_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RebornerBigA_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);

    return S_OK;
}

void CState_RebornerBigA_Idle::Update(_float fTimeDelta)
{
    m_fIdleTime += fTimeDelta;
    _float fDist = m_pMonster->Calc_Distance_XZ();
    if (!m_bFirstMeetCheck)
    {
        _Vec3 vTargetPos = m_pMonster->Get_TargetPos();
        _Vec3 vMonsterPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        if (fDist <= 15.f && abs(vTargetPos.y - vMonsterPos.y) <= 5.f)
        {
            m_bFirstMeetCheck = true;
    
            GET_GAMEINTERFACE->Set_OnOff_OrthoUI(true, m_pMonster);
        }
        else
        {
            return;
        }
    }
    if (m_fIdleEndDuration <= m_fIdleTime)
    {
        if (fDist >= 30.f)
        {
            return;
        }

        if (fDist <= 7.f)
        {
            Calc_Act_Attack();
            return;
        }
        else if (fDist > 10.f)
        {
            m_pMonster->Change_State(CRebornerBigA::RUN);
            return;
        }
        else if (fDist > 7.f)
        {
            m_pMonster->Change_State(CRebornerBigA::WALK);
            return;
        }

    }

}

void CState_RebornerBigA_Idle::End_State()
{
    m_fIdleTime = 0.f;
}

void CState_RebornerBigA_Idle::Calc_Act_Attack()
{
    //µÚÀÏ¶§
    _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
    _Vec3 vRight = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
    _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());


    _Vec3 vCrossUp = vRight.Cross(vTargetDir);

    if (vCrossUp.y > 0)
    {
        m_pMonster->Change_State(CRebornerBigA::SLASHJUMP);
        return;
    }
    else
    {
        if (m_iAtkCnt >= 4)
        {
            m_iAtkCnt = 0;
        }
        switch (m_iAtkCnt)
        {
        case 0:
            m_pMonster->Change_State(CRebornerBigA::SWINGMULTIPLE);
            break;

        case 1:
            m_pMonster->Change_State(CRebornerBigA::RUSHSTING);
            break;

        case 2:
            m_pMonster->Change_State(CRebornerBigA::SLASHTWICE);
            break;

        case 3:
            m_pMonster->Change_State(CRebornerBigA::GUARDSTING);
            break;

        default:
            break;
        }
        ++m_iAtkCnt;
    }
}

CState_RebornerBigA_Idle* CState_RebornerBigA_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerBigA_Idle* pInstance = new CState_RebornerBigA_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerBigA_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerBigA_Idle::Free()
{
    __super::Free();
}