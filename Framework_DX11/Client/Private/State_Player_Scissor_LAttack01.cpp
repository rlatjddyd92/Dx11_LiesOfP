#include "stdafx.h"
#include "State_Player_Scissor_LAttack01.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon_Scissor.h"

CState_Player_Scissor_LAttack01::CState_Player_Scissor_LAttack01(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Scissor_LAttack01::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ScissorNA2 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_ScissorSword_NA2", 2.3f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 45;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame = 27;
    m_iColliderEndFrame = 32;

    m_iEffectStartFrame = 25;
    m_iEffectEndFrame = 32;

    return S_OK;
}

HRESULT CState_Player_Scissor_LAttack01::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorNA2, false, 0.1f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_isPlaySound = false;
    m_isActiveEffect = false;

    m_pPlayer->Set_WeaponStrength(ATK_WEAK);

    return S_OK;
}

void CState_Player_Scissor_LAttack01::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame < m_iChangeFrame)
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

    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if (m_pPlayer->Key_Hold(KEY::LSHIFT))
        {
            m_pPlayer->Change_State(CPlayer::OH_GUARD);
        }
        else if (m_isInputLButton)
            m_pPlayer->Change_State(CPlayer::SCISSOR_LATTACK0);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE0);
            else
                m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK0);
        }
    }
    else if (iFrame >= 125)
    {
        if (m_pPlayer->Key_Hold(KEY::W) || m_pPlayer->Key_Hold(KEY::S) || m_pPlayer->Key_Hold(KEY::D) || m_pPlayer->Key_Hold(KEY::A))
        {
            m_pPlayer->Change_State(CPlayer::OH_RUN);
        }
    }
    else if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Collider();
    Control_Sound();
    Control_Effect(iFrame);

}

void CState_Player_Scissor_LAttack01::End_State()
{
    m_pPlayer->DeActive_CurretnWeaponCollider();
}

_bool CState_Player_Scissor_LAttack01::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ScissorNA2);
}

void CState_Player_Scissor_LAttack01::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iColliderStartFrame <= iFrame && iFrame <= m_iColliderEndFrame)
    {
        if (m_pPlayer->Active_CurrentWeaponCollider())
        {
            m_pPlayer->Decrease_Stamina(45.f);
        }
    }
    else
        m_pPlayer->DeActive_CurretnWeaponCollider();
}

void CState_Player_Scissor_LAttack01::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if ((iFrame == m_iColliderStartFrame || iFrame == m_iColliderStartFrame + 1) && !m_isPlaySound)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Glaive_P_B_SS_02.wav"));
        m_isPlaySound = true;
    }
}

void CState_Player_Scissor_LAttack01::Control_Effect(_int iFrame)
{
    if (!m_isActiveEffect && m_iEffectStartFrame <= iFrame)
    {
        m_pPlayer->Active_WeaponEffect(CWeapon_Scissor::EFFECT_BASE);
        m_isActiveEffect = true;
    }
    else if (m_isActiveEffect && m_iEffectEndFrame < iFrame)
    {
        m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor::EFFECT_BASE);
    }
}

CState_Player_Scissor_LAttack01* CState_Player_Scissor_LAttack01::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Scissor_LAttack01* pInstance = new CState_Player_Scissor_LAttack01(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Scissor_LAttack01::Free()
{
    __super::Free();
}