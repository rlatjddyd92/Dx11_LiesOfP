#include "stdafx.h"
#include "State_SimonManusP1_ChasingSwing.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManusP1.h"

CState_SimonManusP1_ChasingSwing::CState_SimonManusP1_ChasingSwing(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_ChasingSwing::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    CSimonManusP1::FSMSTATE_DESC* pDesc = static_cast<CSimonManusP1::FSMSTATE_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;

    return S_OK;
}

HRESULT CState_SimonManusP1_ChasingSwing::Start_State(void* pArg)
{
    *m_pResetRootMove = false;

    m_pMonster->Change_Animation(AN_CHASINGSWING_START - (m_iAnimCnt), 0.f, 0);

    return S_OK;
}

void CState_SimonManusP1_ChasingSwing::Update(_float fTimeDelta)
{
    if (*m_pIsEndAnim)
    {
        if (m_iAnimCnt < 2)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_CHASINGSWING_START - (m_iAnimCnt), 0.f, 0);
        }
        else
        {
            m_iAnimCnt = 0;
            m_pMonster->Change_State(CSimonManusP1::IDLE);
        }
    }

}

void CState_SimonManusP1_ChasingSwing::End_State()
{
    m_iAnimCnt = 0;
    *m_pResetRootMove = true;
}

CState_SimonManusP1_ChasingSwing* CState_SimonManusP1_ChasingSwing::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_ChasingSwing* pInstance = new CState_SimonManusP1_ChasingSwing(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_ChasingSwing"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_ChasingSwing::Free()
{
    __super::Free();
}