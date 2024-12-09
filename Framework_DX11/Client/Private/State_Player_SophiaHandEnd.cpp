#include "stdafx.h"
#include "State_Player_SophiaHandEnd.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "TreasureBox.h"

CState_Player_SophiaHandEnd::CState_Player_SophiaHandEnd(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_SophiaHandEnd::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_SophiaHandEnd = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Idle_SophiaDead_Cine", 1.f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_SophiaHandEnd::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_SophiaHandEnd, false, 2.5f);

    return S_OK;
}

void CState_Player_SophiaHandEnd::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }
}

void CState_Player_SophiaHandEnd::End_State()
{
}

_bool CState_Player_SophiaHandEnd::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_SophiaHandEnd);
}

CState_Player_SophiaHandEnd* CState_Player_SophiaHandEnd::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_SophiaHandEnd* pInstance = new CState_Player_SophiaHandEnd(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_SophiaHandEnd"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_SophiaHandEnd::Free()
{
    __super::Free();
}