#include "stdafx.h"
#include "State_SimonManusP1_StingAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_StingAttack::CState_SimonManusP1_StingAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_StingAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_StingAttack::Start_State(void* pArg)
{
    m_pMonster->Look_Player();

    m_pMonster->Change_Animation(AN_STINGATTACK, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP1_StingAttack::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

}

void CState_SimonManusP1_StingAttack::End_State()
{
    m_iAnimCnt = 0;//혹시 완료되지 않고 변하는 경우에 대비
    m_fParalizeTime = 0.f;
}

_bool CState_SimonManusP1_StingAttack::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_STINGATTACK);
}

CState_SimonManusP1_StingAttack* CState_SimonManusP1_StingAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_StingAttack* pInstance = new CState_SimonManusP1_StingAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_StingAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_StingAttack::Free()
{
    __super::Free();
}