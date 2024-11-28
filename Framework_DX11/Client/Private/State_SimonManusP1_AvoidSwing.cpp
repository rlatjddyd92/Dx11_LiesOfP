#include "stdafx.h"
#include "State_SimonManusP1_AvoidSwing.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_AvoidSwing::CState_SimonManusP1_AvoidSwing(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_AvoidSwing::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_AvoidSwing::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_AVOIDSWING, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP1_AvoidSwing::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }
    
    Collider_Check();

}

void CState_SimonManusP1_AvoidSwing::End_State()
{
   
}

_bool CState_SimonManusP1_AvoidSwing::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_AVOIDSWING);
}

void CState_SimonManusP1_AvoidSwing::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos >= 105.f && CurTrackPos <= 185.f)
    {
        m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
        m_pMonster->Active_CurrentWeaponCollider(1);
    }
    else
    {
        m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

CState_SimonManusP1_AvoidSwing* CState_SimonManusP1_AvoidSwing::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_AvoidSwing* pInstance = new CState_SimonManusP1_AvoidSwing(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_AvoidSwing"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_AvoidSwing::Free()
{
    __super::Free();
}