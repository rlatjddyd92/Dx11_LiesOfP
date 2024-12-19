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

    return S_OK;
}

HRESULT CState_CarcassNormal_Idle::Start_State(void* pArg)
{
    m_pMonster->Get_Model()->Set_RemoteTuning(true);
    m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);

    return S_OK;
}

void CState_CarcassNormal_Idle::Update(_float fTimeDelta)
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
            m_pMonster->Change_State(CCarcassNormal::RUN);
            return;
        }
        else if (fDist > 7.f)
        {
            m_pMonster->Change_State(CCarcassNormal::WALK);
            return;
        }

    }

}

void CState_CarcassNormal_Idle::End_State()
{
    m_pMonster->Get_Model()->Set_RemoteTuning(false);
    m_fIdleTime = 0.f;
}

void CState_CarcassNormal_Idle::Calc_Act_Attack()
{
    //µÚÀÏ¶§
    _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
    _Vec3 vRight = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
    _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());


    _Vec3 vCrossUp = vRight.Cross(vTargetDir);

    if (vCrossUp.y > 0)
    {
        m_pMonster->Change_State(CCarcassNormal::HEADINGMULTIPLE);
        return;
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
            break;

        case 1:
            m_pMonster->Change_State(CCarcassNormal::CLAWMULTIPLE);
            break;

        case 2:
            m_pMonster->Change_State(CCarcassNormal::CLAWRUSH);
            break;

        case 3:
            m_pMonster->Change_State(CCarcassNormal::TRIPLECLAW_2);
            break;

        case 4:
            m_pMonster->Change_State(CCarcassNormal::BITE);
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