#include "stdafx.h"
#include "State_Player_Stargazer.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Lift_Controller.h"

CState_Player_Stargazer::CState_Player_Stargazer(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Stargazer::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_Stargazer = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Stargazer_Inactive", 2.5f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Stargazer::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Stargazer, false, 0.1f);

    return S_OK;
}

void CState_Player_Stargazer::Update(_float fTimeDelta)
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

void CState_Player_Stargazer::End_State()
{
    m_pPlayer->Get_RigidBody()->Set_IsOnCell(true);
}

_bool CState_Player_Stargazer::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Stargazer);
}

CState_Player_Stargazer* CState_Player_Stargazer::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Stargazer* pInstance = new CState_Player_Stargazer(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Stargazer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Stargazer::Free()
{
    __super::Free();
}