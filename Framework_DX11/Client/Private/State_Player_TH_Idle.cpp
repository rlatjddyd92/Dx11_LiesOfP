#include "stdafx.h"
#include "State_Player_TH_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"

CState_Player_TH_Idle::CState_Player_TH_Idle(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_TH_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Idle = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Idle");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_TH_Idle::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation_Boundry(m_iAnimation_Idle, true);
    m_pPlayer->Change_Animation(m_iAnimation_Idle, true);

    m_fRButtonTime = 0.f;

    return S_OK;
}

void CState_Player_TH_Idle::Update(_float fTimeDelta)
{
    if (KEY_HOLD(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CPlayer::TH_GUARD);
    }
    else if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D) || KEY_HOLD(KEY::A))
    {
        m_pPlayer->Change_State(CPlayer::TH_RUN);
    }
    else if (KEY_TAP(KEY::SPACE))
    {
        m_pPlayer->Change_State(CPlayer::TH_DASH);
    }
    else if (KEY_TAP(KEY::LBUTTON))
    {
        m_pPlayer->Change_State(CPlayer::FLAME_LATTACK0);
    }
    else if (KEY_HOLD(KEY::RBUTTON))
    {
        m_fRButtonTime += fTimeDelta;
        if(m_fRButtonTime > 0.15f)
            m_pPlayer->Change_State(CPlayer::FLAME_RATTACK0);
    }
    else if (KEY_TAP(KEY::F))
    {
        m_pPlayer->Change_State(CPlayer::RAPIER_FATAL);
    }
    else if (KEY_TAP(KEY::R))
    {
        m_pPlayer->Change_State(CPlayer::HEAL);
    }
    else if (KEY_TAP(KEY::TAPKEY))
    {
        m_pPlayer->Change_State(CPlayer::CHANGEWEP);
    }
    
    if (KEY_AWAY(KEY::RBUTTON))
    {
        m_fRButtonTime = 0.f;
        m_pPlayer->Change_State(CPlayer::FLAME_RATTACK0);
    }
}

void CState_Player_TH_Idle::End_State()
{
}

CState_Player_TH_Idle* CState_Player_TH_Idle::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_TH_Idle* pInstance = new CState_Player_TH_Idle(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_TH_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_TH_Idle::Free()
{
    __super::Free();
}