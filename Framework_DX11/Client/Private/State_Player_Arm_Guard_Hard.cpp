#include "stdafx.h"
#include "State_Player_Arm_Guard_Hard.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"

CState_Player_Arm_Guard_Hard::CState_Player_Arm_Guard_Hard(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Arm_Guard_Hard::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_Arm_Guard_Hard = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Arm_GuardEx_Guard_Strong", 1.8f);

    m_iStateNum = iStateNum;

    m_iCounterStartFrame = 20;
    m_iCounterStartFrame = 45;

    return S_OK;
}

HRESULT CState_Player_Arm_Guard_Hard::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Arm_Guard_Hard, false, 0.f, 0, true, true);

    m_isAwayCtrlKey = false;
    m_fCtrlKeyTime = 0.f;

    return S_OK;
}

void CState_Player_Arm_Guard_Hard::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_pPlayer->Key_Away(KEY::CTRL) || m_pPlayer->Key_None(KEY::CTRL))
    {
        m_isAwayCtrlKey = true;
        m_fCtrlKeyTime = 0.f;
    }
    else if (m_pPlayer->Key_Hold(KEY::CTRL))
    {
        m_fCtrlKeyTime += fTimeDelta;
    }

    if (m_iCounterStartFrame <= iFrame && m_iCounterEndFrame <= iFrame)
    {
        if (m_isAwayCtrlKey && m_fCtrlKeyTime > 0.5f)
        {
            m_pPlayer->Change_State(CPlayer::ARM_COUNTER);
            return;
        }
    }

    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::ARM_LOOP);
        return;
    }

}

void CState_Player_Arm_Guard_Hard::End_State()
{
}

_bool CState_Player_Arm_Guard_Hard::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Arm_Guard_Hard);
}

CState_Player_Arm_Guard_Hard* CState_Player_Arm_Guard_Hard::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Arm_Guard_Hard* pInstance = new CState_Player_Arm_Guard_Hard(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Arm_Guard_Hard"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Arm_Guard_Hard::Free()
{
    __super::Free();
}
