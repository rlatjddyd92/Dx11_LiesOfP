#include "stdafx.h"
#include "State_SimonManusP2_Sting.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_Sting::CState_SimonManusP2_Sting(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Sting::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_Sting::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_STINGATTACK, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP2_Sting::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    Collider_Check();

}

void CState_SimonManusP2_Sting::End_State()
{

}

_bool CState_SimonManusP2_Sting::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_STINGATTACK);
}

void CState_SimonManusP2_Sting::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if ((CurTrackPos >= 75.f && CurTrackPos <= 105.f) ||
        (CurTrackPos >= 175.f && CurTrackPos <= 225.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

CState_SimonManusP2_Sting* CState_SimonManusP2_Sting::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Sting* pInstance = new CState_SimonManusP2_Sting(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Sting"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Sting::Free()
{
    __super::Free();
}