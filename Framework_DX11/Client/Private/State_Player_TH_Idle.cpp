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
    m_pPlayer->Change_Animation_Boundry(m_iAnimation_Idle, true, 0.3f);
    m_pPlayer->Change_Animation(m_iAnimation_Idle, true, 0.3f);

    m_fRButtonTime = 0.f;

    return S_OK;
}

void CState_Player_TH_Idle::Update(_float fTimeDelta)
{
    if (m_pPlayer->Key_Hold(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CPlayer::TH_GUARD);
    }
    else if (m_pPlayer->Key_Hold(KEY::W) || m_pPlayer->Key_Hold(KEY::S) || m_pPlayer->Key_Hold(KEY::D) || m_pPlayer->Key_Hold(KEY::A))
    {
        if (m_pPlayer->Get_IsLockOn())
            m_pPlayer->Change_State(CPlayer::TH_WALK);
        else
            m_pPlayer->Change_State(CPlayer::TH_RUN);
    }
    else if (m_pPlayer->Key_Tab(KEY::SPACE))
    {
        m_pPlayer->Change_State(CPlayer::TH_DASH);
    }
    else if (m_pPlayer->Key_Tab(KEY::LBUTTON))
    {
        m_pPlayer->Change_State(CPlayer::FLAME_LATTACK0);
    }
    else if (m_pPlayer->Key_Hold(KEY::RBUTTON))
    {
        m_fRButtonTime += fTimeDelta;
        if (m_fRButtonTime > 0.15f)
        {
            m_pPlayer->Change_State(CPlayer::FLAME_CHARGE0);
        }
    }
    else if (m_pPlayer->Key_Tab(KEY::F))
    {
        if (m_pPlayer->Check_Region_Fable01())
        {
            m_pPlayer->Change_State(CPlayer::FLAME_FABLE);
        }
    }
    else if (m_pPlayer->Key_Tab(KEY::R))
    {
        SPECIAL_ITEM eNow = GET_GAMEINTERFACE->Get_Now_Select_Item();
        if (SPECIAL_ITEM::SP_PULSE_BATTERY == eNow)
        {
            m_pPlayer->Change_State(CPlayer::HEAL);
        }
        else if (SPECIAL_ITEM::SP_GRINDER == eNow)
        {
            m_pPlayer->Change_State(CPlayer::GRINDER);
        }
        else if (SPECIAL_ITEM::SP_GRANADE == eNow
            || SPECIAL_ITEM::SP_THERMITE == eNow
            || SPECIAL_ITEM::SP_THROW_BATTERY == eNow)
        {
            m_pPlayer->Change_State(CPlayer::THROW_ITEM);
        }
        else if (SPECIAL_ITEM::SP_TELEPOT == eNow)
        {
            //UI¸¦ ¶ç¿ìÀÚ
            m_pPlayer->Change_State(CPlayer::TELEPORT);
        }
        else if (SPECIAL_ITEM::SP_DEAD == eNow)
        {
            //UI¸¦ ¶ç¿ìÀÚ
            m_pPlayer->Change_State(CPlayer::DIE);
        }
    }
    else if (m_pPlayer->Key_Tab(KEY::TAPKEY))
    {
        m_pPlayer->Change_State(CPlayer::CHANGEWEP);
    }
    else if (m_pPlayer->Key_Hold(KEY::CTRL))
    {
        if (m_pPlayer->Get_ArmGuage_Ptr()->x >= 10.f)
            m_pPlayer->Change_State(CPlayer::ARM_START);
    }

    if (m_pPlayer->Key_Away(KEY::RBUTTON))
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