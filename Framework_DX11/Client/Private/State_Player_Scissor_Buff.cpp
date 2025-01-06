#include "stdafx.h"
#include "State_Player_Scissor_Buff.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Weapon_Scissor_Handle.h"

CState_Player_Scissor_Buff::CState_Player_Scissor_Buff(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Scissor_Buff::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ScissorBuff = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_FableArts_Buff_ScissorSword", 1.8f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 145;
    m_iSeperateFrame = 20;
    m_iCombineFrame = 130;
    m_iStateNum = iStateNum;

    m_iEffectStartFrame = 35;
    m_iEffectEndFrame = 100;

    return S_OK;
}

HRESULT CState_Player_Scissor_Buff::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorBuff, false, 0.3f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_isActiveEffect = false;

    return S_OK;
}

void CState_Player_Scissor_Buff::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame < m_iChangeFrame && m_pPlayer->Get_Player_Stat().vGauge_Stamina.x > 30.f)
    {
        if (m_pPlayer->Key_Tab(KEY::LBUTTON))
        {
            m_isInputLButton = true;
            m_isInputRButton = false;
        }
        else if (m_pPlayer->Key_Tab(KEY::RBUTTON))
        {
            m_isInputRButton = true;
            m_isInputLButton = false;
            m_fRButtonTime = 0.f;
        }
        else if (m_pPlayer->Key_Hold(KEY::RBUTTON))
        {
            m_fRButtonTime += fTimeDelta;
        }
    }

    if (m_iSeperateFrame == iFrame || m_iSeperateFrame + 1 == iFrame)
    {
        m_pPlayer->Seperate_Scissor();
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("Scissor_Buff.wav"));
    }
    else if (m_iCombineFrame == iFrame || m_iCombineFrame + 1 == iFrame)
    {
        m_pPlayer->Set_AttackBuffTime(30.f);
        m_pPlayer->Combine_Scissor();
    }

    else if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if (m_pPlayer->Key_Hold(KEY::LSHIFT))
        {
            m_pPlayer->Change_State(CPlayer::OH_GUARD);
        }
        else if (m_isInputLButton)
        {
            m_pPlayer->Change_State(CPlayer::SCISSOR_LATTACK0);
        }
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE0);
            else
                m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK0);
        }


    }
    else if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Effect(iFrame);
}

void CState_Player_Scissor_Buff::End_State()
{
}

_bool CState_Player_Scissor_Buff::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ScissorBuff);
}

void CState_Player_Scissor_Buff::Control_Effect(_int iFrame)
{
    if (!m_isActiveEffect && m_iEffectStartFrame <= iFrame)
    {
        m_pPlayer->Active_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_BUFF, true, 0);
        m_pPlayer->Active_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_BUFF, true, 1);
        m_isActiveEffect = true;
    }
    else if (m_isActiveEffect && m_iEffectEndFrame < iFrame)
    {
        m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_BUFF, 0);
        m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_BUFF, 1);
    }
}

CState_Player_Scissor_Buff* CState_Player_Scissor_Buff::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Scissor_Buff* pInstance = new CState_Player_Scissor_Buff(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Scissor_Buff"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Scissor_Buff::Free()
{
    __super::Free();
}