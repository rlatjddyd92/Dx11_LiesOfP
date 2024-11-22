#include "stdafx.h"
#include "State_SimonManusP2_SpreadMagic.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_SpreadMagic::CState_SimonManusP2_SpreadMagic(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_SpreadMagic::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_SpreadMagic::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_STINGATTACK, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP2_SpreadMagic::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }
}

void CState_SimonManusP2_SpreadMagic::End_State()
{

}

_bool CState_SimonManusP2_SpreadMagic::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_STINGATTACK);
}

CState_SimonManusP2_SpreadMagic* CState_SimonManusP2_SpreadMagic::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_SpreadMagic* pInstance = new CState_SimonManusP2_SpreadMagic(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_SpreadMagic"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_SpreadMagic::Free()
{
    __super::Free();
}