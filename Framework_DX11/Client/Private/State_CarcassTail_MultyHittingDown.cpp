#include "stdafx.h"
#include "State_CarcassTail_MultyHittingDown.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassTail_MultyHittingDown::CState_CarcassTail_MultyHittingDown(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_MultyHittingDown::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_MultyHittingDown::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_MULTYHITTINGDOWN, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CarcassTail_MultyHittingDown::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos < 75.f)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.5f, fTimeDelta);
    }

    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    Collider_Check();

}

void CState_CarcassTail_MultyHittingDown::End_State()
{
}

_bool CState_CarcassTail_MultyHittingDown::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_MULTYHITTINGDOWN);
}

void CState_CarcassTail_MultyHittingDown::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if ((CurTrackPos >= 85.f && CurTrackPos <= 95.f) ||
        (CurTrackPos >= 165.f && CurTrackPos <= 175.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1, 0);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(0);
    }

    if ((CurTrackPos >= 120.f && CurTrackPos <= 129.f) ||
        (CurTrackPos >= 195.f && CurTrackPos <= 204.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1, 1);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(1);
    }
}

CState_CarcassTail_MultyHittingDown* CState_CarcassTail_MultyHittingDown::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_MultyHittingDown* pInstance = new CState_CarcassTail_MultyHittingDown(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_MultyHittingDown"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_MultyHittingDown::Free()
{
    __super::Free();
}