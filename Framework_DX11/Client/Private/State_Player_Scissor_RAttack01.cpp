#include "stdafx.h"
#include "State_Player_Scissor_RAttack01.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Weapon_Scissor.h"

CState_Player_Scissor_RAttack01::CState_Player_Scissor_RAttack01(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Scissor_RAttack01::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ScissorSA2 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_ScissorSword_SA2", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 52;
    m_iSeperateFrame = 1;
    m_iCombineFrame = 45;

    m_iStateNum = iStateNum;

    m_iColliderStartFrameLeft = 33;
    m_iColliderEndFrameLeft = 40;
    m_iColliderStartFrameRight = 10;
    m_iColliderEndFrameRight = 17;

    m_iLeftEffectStartFrame = 30;
    m_iLeftEffectEndFrame = 43;
    m_iRightEffectStartFrame = 8;
    m_iRightEffectEndFrame = 21;

    m_iSwishL_SoundFrame = 10;
    m_iSwishM_SoundFrame = 33;

    return S_OK;
}

HRESULT CState_Player_Scissor_RAttack01::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorSA2, false, 0.3f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_pPlayer->Set_WeaponStrength(ATK_NORMAL);
    m_pPlayer->Get_CurrentWeapon()->Set_DamageAmount(25.f);

    m_isLeftActiveEffect = false;
    m_isRightActiveEffect = false;

    m_isPlaySound_SwishL = false;
    m_isPlaySound_SwishM = false;

    return S_OK;
}

void CState_Player_Scissor_RAttack01::Update(_float fTimeDelta)
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
    }
    else if (m_iCombineFrame == iFrame || m_iCombineFrame + 1 == iFrame)
    {
        m_pPlayer->Combine_Scissor();
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
                 m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE1);
             else
                 m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK0);
         }
     }
     else if (iFrame >= 130)
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

     if (iFrame >= m_iSwishL_SoundFrame && !m_isPlaySound_SwishL)
     {
         m_isPlaySound_SwishL = true;
         m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_CookingKnife_1H_L_01.wav"));
     }
     if (iFrame >= m_iSwishM_SoundFrame && !m_isPlaySound_SwishM)
     {
         m_isPlaySound_SwishM = true;
         m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_CookingKnife_1H_M_01.wav"));
     }

     Control_Collider();
     Control_Effect(iFrame);
}

void CState_Player_Scissor_RAttack01::End_State()
{
    m_pPlayer->Combine_Scissor(true);
}

_bool CState_Player_Scissor_RAttack01::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ScissorSA2);
}

void CState_Player_Scissor_RAttack01::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iCombineFrame >= iFrame)
    {
        if (m_iColliderStartFrameLeft <= iFrame && iFrame <= m_iColliderEndFrameLeft)
        {
            if (m_pPlayer->Active_CurrentWeaponCollider(1.f, 1))
            {
                m_pPlayer->Decrease_Stamina(45.f);
            }
        }
        else
        {
            m_pPlayer->DeActive_CurretnWeaponCollider(1);
        }

        if (m_iColliderStartFrameRight <= iFrame && iFrame <= m_iColliderEndFrameRight)
        {
            if (m_pPlayer->Active_CurrentWeaponCollider(1.f, 0))
            {
                m_pPlayer->Decrease_Stamina(45.f);
            }
        }
        else
        {
            m_pPlayer->DeActive_CurretnWeaponCollider(0);
        }
    }
}

void CState_Player_Scissor_RAttack01::Control_Effect(_int iFrame)
{
    if (!m_isLeftActiveEffect && m_iLeftEffectStartFrame <= iFrame)
    {
        m_pPlayer->Active_WeaponEffect(CWeapon_Scissor::EFFECT_BASE, true, 1);
        m_isLeftActiveEffect = true;
    }
    else if (m_isLeftActiveEffect && m_iLeftEffectEndFrame < iFrame)
    {
        m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor::EFFECT_BASE, 1);
    }

    if (!m_isRightActiveEffect && m_iRightEffectStartFrame <= iFrame)
    {
        m_pPlayer->Active_WeaponEffect(CWeapon_Scissor::EFFECT_BASE, true, 0);
        m_isRightActiveEffect = true;
    }
    else if (m_isRightActiveEffect && m_iRightEffectEndFrame < iFrame)
    {
        m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor::EFFECT_BASE, 0);
    }
}

CState_Player_Scissor_RAttack01* CState_Player_Scissor_RAttack01::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Scissor_RAttack01* pInstance = new CState_Player_Scissor_RAttack01(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Scissor_RAttack01"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Scissor_RAttack01::Free()
{
    __super::Free();
}