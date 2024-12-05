#include "stdafx.h"
#include "State_CarcassTail_TailSwingDown.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassTail_TailSwingDown::CState_CarcassTail_TailSwingDown(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_TailSwingDown::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_TailSwingDown::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_TAILSWINGDOWN, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CarcassTail_TailSwingDown::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(-m_pMonster->Get_TargetDir(), 1.5f, fTimeDelta);

    Collider_Check();

}

void CState_CarcassTail_TailSwingDown::End_State()
{
}

_bool CState_CarcassTail_TailSwingDown::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_TAILSWINGDOWN);
}

void CState_CarcassTail_TailSwingDown::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if ((CurTrackPos >= 65.f && CurTrackPos <= 72.f) || 
        (CurTrackPos >= 110.f && CurTrackPos <= 115.f) ||
        (CurTrackPos >= 250.f && CurTrackPos <= 258.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.4, 2, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        m_pMonster->Active_CurrentWeaponCollider(1.4, 4, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(2);
    }
}

CState_CarcassTail_TailSwingDown* CState_CarcassTail_TailSwingDown::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_TailSwingDown* pInstance = new CState_CarcassTail_TailSwingDown(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_TailSwingDown"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_TailSwingDown::Free()
{
    __super::Free();
}