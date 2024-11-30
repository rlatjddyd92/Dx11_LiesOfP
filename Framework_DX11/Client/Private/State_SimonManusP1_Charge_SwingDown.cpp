#include "stdafx.h"
#include "State_SimonManusP1_Charge_SwingDown.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_Charge_SwingDown::CState_SimonManusP1_Charge_SwingDown(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_Charge_SwingDown::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_Charge_SwingDown::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_CHARGESWINGDOWN, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP1_Charge_SwingDown::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    Collider_Check();

}

void CState_SimonManusP1_Charge_SwingDown::End_State()
{

}

_bool CState_SimonManusP1_Charge_SwingDown::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_CHARGESWINGDOWN);
}

void CState_SimonManusP1_Charge_SwingDown::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if ((CurTrackPos >= 225.f && CurTrackPos <= 245.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

CState_SimonManusP1_Charge_SwingDown* CState_SimonManusP1_Charge_SwingDown::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_Charge_SwingDown* pInstance = new CState_SimonManusP1_Charge_SwingDown(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_Charge_SwingDown"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_Charge_SwingDown::Free()
{
    __super::Free();
}