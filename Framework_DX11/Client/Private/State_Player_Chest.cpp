#include "stdafx.h"
#include "State_Player_Chest.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Chest::CState_Player_Chest(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Chest::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_Chest = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Prop_TreasureChest_Open", 2.f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Chest::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Chest, false, 0.f);

    return S_OK;
}

void CState_Player_Chest::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        _uint iWeponType = m_pPlayer->Get_WeaponType();

        if (iWeponType < 2)
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        else
            m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }
}

void CState_Player_Chest::End_State()
{
    m_pPlayer->Get_RigidBody()->Set_IsOnCell(true);
}

_bool CState_Player_Chest::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Chest);
}

CState_Player_Chest* CState_Player_Chest::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Chest* pInstance = new CState_Player_Chest(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Chest"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Chest::Free()
{
    __super::Free();
}