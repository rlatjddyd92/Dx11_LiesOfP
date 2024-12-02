#include "stdafx.h"
#include "State_SimonManusP2_AvoidSwing.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_AvoidSwing::CState_SimonManusP2_AvoidSwing(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_AvoidSwing::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_AvoidSwing::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_AVOIDSWING, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP2_AvoidSwing::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);

}

void CState_SimonManusP2_AvoidSwing::End_State()
{

}

_bool CState_SimonManusP2_AvoidSwing::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_AVOIDSWING);
}

void CState_SimonManusP2_AvoidSwing::Collider_Check(_double CurTrackPos)
{
    if (CurTrackPos >= 120.f && CurTrackPos <= 180.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

void CState_SimonManusP2_AvoidSwing::Effect_Check(_double CurTrackPos)
{
    if (CurTrackPos >= 120.f && CurTrackPos <= 180.f)
    {
        if (!m_pMonster->Get_EffectsLoop(CSimonManus::P1_TRAIL))
        {
            m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
        }
    }
    else
    {
        m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
    }
}

CState_SimonManusP2_AvoidSwing* CState_SimonManusP2_AvoidSwing::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_AvoidSwing* pInstance = new CState_SimonManusP2_AvoidSwing(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_AvoidSwing"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_AvoidSwing::Free()
{
    __super::Free();
}