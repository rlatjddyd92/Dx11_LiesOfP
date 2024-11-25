#include "stdafx.h"
#include "State_CarcassBigA_LTSwingRight.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassBigA_LTSwingRight::CState_CarcassBigA_LTSwingRight(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_LTSwingRight::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 35;
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassBigA_LTSwingRight::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_LTSWINGRIGHT, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CarcassBigA_LTSwingRight::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    Collider_Check();

}

void CState_CarcassBigA_LTSwingRight::End_State()
{
}

_bool CState_CarcassBigA_LTSwingRight::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_LTSWINGRIGHT);
}

void CState_CarcassBigA_LTSwingRight::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos >= 75.f && CurTrackPos <= 95.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1, 0);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(0);
    }
}

CState_CarcassBigA_LTSwingRight* CState_CarcassBigA_LTSwingRight::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_LTSwingRight* pInstance = new CState_CarcassBigA_LTSwingRight(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_LTSwingRight::Free()
{
    __super::Free();
}