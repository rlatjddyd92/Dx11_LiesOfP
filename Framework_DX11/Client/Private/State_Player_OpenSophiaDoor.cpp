#include "stdafx.h"
#include "State_Player_OpenSophiaDoor.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "TreasureBox.h"

CState_Player_OpenSophiaDoor::CState_Player_OpenSophiaDoor(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OpenSophiaDoor::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_DoorPush = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Prop_DoubleDoor_Push", 1.f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_OpenSophiaDoor::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_DoorPush, false, 0.8f, 316);

    return S_OK;
}

void CState_Player_OpenSophiaDoor::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        //m_pPlayer->Change_State(CPlayer::SOPHIA_WALK);
    }
}

void CState_Player_OpenSophiaDoor::End_State()
{
}

_bool CState_Player_OpenSophiaDoor::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_DoorPush);
}

CState_Player_OpenSophiaDoor* CState_Player_OpenSophiaDoor::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OpenSophiaDoor* pInstance = new CState_Player_OpenSophiaDoor(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_OpenSophiaDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OpenSophiaDoor::Free()
{
    __super::Free();
}