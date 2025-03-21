#include "stdafx.h"
#include "State_Player_Arm_Guard_Heavy.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"

CState_Player_Arm_Guard_Heavy::CState_Player_Arm_Guard_Heavy(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Arm_Guard_Heavy::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_Arm_Guard_Heavy = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Arm_GuardEx_Guard_Heavy", 1.8f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Arm_Guard_Heavy::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Arm_Guard_Heavy, false, 0.f, 0, true, true);

    return S_OK;
}

void CState_Player_Arm_Guard_Heavy::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::ARM_LOOP);
        return;
    }

}

void CState_Player_Arm_Guard_Heavy::End_State()
{
}

_bool CState_Player_Arm_Guard_Heavy::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Arm_Guard_Heavy);
}

CState_Player_Arm_Guard_Heavy* CState_Player_Arm_Guard_Heavy::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Arm_Guard_Heavy* pInstance = new CState_Player_Arm_Guard_Heavy(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Arm_Guard_Heavy"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Arm_Guard_Heavy::Free()
{
    __super::Free();
}
