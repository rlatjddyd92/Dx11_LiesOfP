#include "stdafx.h"
#include "State_SimonManusP1_SwingMultiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManusP1.h"

CState_SimonManusP1_SwingMultiple::CState_SimonManusP1_SwingMultiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_SwingMultiple::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    CSimonManusP1::FSMSTATE_DESC* pDesc = static_cast<CSimonManusP1::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_SwingMultiple::Start_State(void* pArg)
{
    m_pMonster->Look_Player();

    m_pMonster->Change_Animation(AN_SWINGMULTIPLE, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP1_SwingMultiple::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManusP1::IDLE);
    }

}

void CState_SimonManusP1_SwingMultiple::End_State()
{
    m_iAnimCnt = 0;//혹시 완료되지 않고 변하는 경우에 대비
    m_fParalizeTime = 0.f;
    *m_pResetRootMove = true;
}

_bool CState_SimonManusP1_SwingMultiple::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_SWINGMULTIPLE);
}

CState_SimonManusP1_SwingMultiple* CState_SimonManusP1_SwingMultiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_SwingMultiple* pInstance = new CState_SimonManusP1_SwingMultiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_SwingMultiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_SwingMultiple::Free()
{
    __super::Free();
}