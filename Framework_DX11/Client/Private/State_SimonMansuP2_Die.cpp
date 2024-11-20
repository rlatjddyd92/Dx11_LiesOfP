#include "stdafx.h"
#include "State_SimonMansuP2_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonMansuP2_Die::CState_SimonMansuP2_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonMansuP2_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    return S_OK;
}

HRESULT CState_SimonMansuP2_Die::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_DIE, true, 0.1f, 0);

    return S_OK;
}

void CState_SimonMansuP2_Die::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        //¸ó½ºÅÍ »ç¸Á
        m_pMonster->Change_State(CSimonManus::IDLE);
    }

}

void CState_SimonMansuP2_Die::End_State()
{
}

_bool CState_SimonMansuP2_Die::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_DIE);
}

CState_SimonMansuP2_Die* CState_SimonMansuP2_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonMansuP2_Die* pInstance = new CState_SimonMansuP2_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonMansuP2_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonMansuP2_Die::Free()
{
    __super::Free();
}