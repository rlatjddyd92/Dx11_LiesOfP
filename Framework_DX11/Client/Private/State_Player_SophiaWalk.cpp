#include "stdafx.h"
#include "State_Player_SophiaWalk.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "TreasureBox.h"

CState_Player_SophiaWalk::CState_Player_SophiaWalk(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_SophiaWalk::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    //m_iAnimation_SophiaWalk = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Walking_SophiaDead_Cine", 1.f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_SophiaWalk::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_SophiaWalk, false, 0.1f);

    return S_OK;
}

void CState_Player_SophiaWalk::Update(_float fTimeDelta)
{
    //if (End_Check())
    //{
    //    _uint iWeponType = m_pPlayer->Get_WeaponType();

    //    if (iWeponType < 2)
    //        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    //    else
    //        m_pPlayer->Change_State(CPlayer::TH_IDLE);
    //}
}

void CState_Player_SophiaWalk::End_State()
{
}

_bool CState_Player_SophiaWalk::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_SophiaWalk);
}

CState_Player_SophiaWalk* CState_Player_SophiaWalk::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_SophiaWalk* pInstance = new CState_Player_SophiaWalk(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_SophiaWalk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_SophiaWalk::Free()
{
    __super::Free();
}