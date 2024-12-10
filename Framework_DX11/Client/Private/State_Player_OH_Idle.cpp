#include "stdafx.h"
#include "State_Player_OH_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"

#include "GameInterface_Controller.h"

CState_Player_OH_Idle::CState_Player_OH_Idle(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OH_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Idle = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Idle");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_OH_Idle::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation_Boundry(m_iAnimation_Idle, true, 0.3f);
    m_pPlayer->Change_Animation(m_iAnimation_Idle, true, 0.3f);

    m_fRButtonTime = 0.f;

    return S_OK;
}

void CState_Player_OH_Idle::Update(_float fTimeDelta)
{
    if (KEY_HOLD(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CPlayer::OH_GUARD);
    }
    else if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D) || KEY_HOLD(KEY::A))
    {
        if(m_pPlayer->Get_IsLockOn())
            m_pPlayer->Change_State(CPlayer::OH_WALK);
        else
            m_pPlayer->Change_State(CPlayer::OH_RUN);
    }
    else if (KEY_TAP(KEY::SPACE))
    {
        m_pPlayer->Change_State(CPlayer::OH_DASH);
    }
    else if (KEY_TAP(KEY::LBUTTON))
    {
        if(m_pPlayer->Get_WeaponType() == CPlayer::WEP_RAPIER)
            m_pPlayer->Change_State(CPlayer::RAPIER_LATTACK0);
        else if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_SCISSOR)
            m_pPlayer->Change_State(CPlayer::SCISSOR_LATTACK0);
    }
    else if (KEY_HOLD(KEY::RBUTTON))
    {
        m_fRButtonTime += fTimeDelta;
        if (m_fRButtonTime > 0.15f)
        {
            if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_RAPIER)
                m_pPlayer->Change_State(CPlayer::RAPIER_CHARGE);
            else if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_SCISSOR)
                m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE0);
        }
    }
    else if (KEY_TAP(KEY::F))
    {
        if (m_pPlayer->Check_Region_Fable01())
        {
            if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_RAPIER)
                m_pPlayer->Change_State(CPlayer::RAPIER_FABALE);
            else if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_SCISSOR)
                m_pPlayer->Change_State(CPlayer::SCISSOR_FABAL0);
        }
    }
    else if (KEY_TAP(KEY::R))
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
    }
    else if (KEY_TAP(KEY::TAPKEY))
    {
        m_pPlayer->Change_State(CPlayer::CHANGEWEP);
    }
    else if (KEY_HOLD(KEY::CTRL))
    {
        m_pPlayer->Change_State(CPlayer::ARM_START);
    }
    
    if (KEY_AWAY(KEY::RBUTTON))
    {
        m_fRButtonTime = 0.f;
        if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_RAPIER)
            m_pPlayer->Change_State(CPlayer::RAPIER_RATTACK0);
        else if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_SCISSOR)
            m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK0);
    }
}

void CState_Player_OH_Idle::End_State()
{
}

CState_Player_OH_Idle* CState_Player_OH_Idle::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OH_Idle* pInstance = new CState_Player_OH_Idle(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_TH_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OH_Idle::Free()
{
    __super::Free();
}