#include "stdafx.h"
#include "State_SimonManusP2_Wave.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_Wave::CState_SimonManusP2_Wave(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Wave::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_Wave::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_MAGICWAVE, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP2_Wave::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }
}

void CState_SimonManusP2_Wave::End_State()
{

}

_bool CState_SimonManusP2_Wave::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_MAGICWAVE);
}

CState_SimonManusP2_Wave* CState_SimonManusP2_Wave::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Wave* pInstance = new CState_SimonManusP2_Wave(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Wave"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Wave::Free()
{
    __super::Free();
}