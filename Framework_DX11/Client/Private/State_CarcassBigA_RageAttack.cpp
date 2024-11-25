#include "stdafx.h"
#include "State_CarcassBigA_RageAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassBigA_RageAttack::CState_CarcassBigA_RageAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_RageAttack::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassBigA_RageAttack::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_RAGEATTACK, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CarcassBigA_RageAttack::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    Collider_Check();

}

void CState_CarcassBigA_RageAttack::End_State()
{
}

_bool CState_CarcassBigA_RageAttack::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_RAGEATTACK);
}

void CState_CarcassBigA_RageAttack::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if ((CurTrackPos >= 90.f && CurTrackPos <= 115.f) ||
        (CurTrackPos >= 135.f && CurTrackPos <= 165.f) ||
        (CurTrackPos >= 225.f && CurTrackPos <= 245.f) ||
        (CurTrackPos >= 275.f && CurTrackPos <= 295.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1, 0);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(0);
    }

    if ((CurTrackPos >= 82.f && CurTrackPos <= 100.f) ||
        (CurTrackPos >= 150.f && CurTrackPos <= 165.f) ||
        (CurTrackPos >= 185.f && CurTrackPos <= 195.f) ||
        (CurTrackPos >= 205.f && CurTrackPos <= 225.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1, 1);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(1);
    }
}

CState_CarcassBigA_RageAttack* CState_CarcassBigA_RageAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_RageAttack* pInstance = new CState_CarcassBigA_RageAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_RageAttack::Free()
{
    __super::Free();
}