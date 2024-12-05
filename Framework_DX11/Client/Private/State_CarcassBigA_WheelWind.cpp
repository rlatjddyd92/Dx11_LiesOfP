#include "stdafx.h"
#include "State_CarcassBigA_WheelWind.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassBigA_WheelWind::CState_CarcassBigA_WheelWind(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_WheelWind::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassBigA_WheelWind::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_SWINGRIGHT, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CarcassBigA_WheelWind::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    Collider_Check();

}

void CState_CarcassBigA_WheelWind::End_State()
{
}

_bool CState_CarcassBigA_WheelWind::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_SWINGRIGHT);
}

void CState_CarcassBigA_WheelWind::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos >= 85.f && CurTrackPos <= 155.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1.3, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(1);
    }
}

CState_CarcassBigA_WheelWind* CState_CarcassBigA_WheelWind::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_WheelWind* pInstance = new CState_CarcassBigA_WheelWind(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_WheelWind::Free()
{
    __super::Free();
}