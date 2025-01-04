#include "stdafx.h"
#include "State_Player_Scissor_Charge00.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Weapon_Scissor.h"

CState_Player_Scissor_Charge00::CState_Player_Scissor_Charge00(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Scissor_Charge00::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ScissorCA1 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_ScissorSword_CA1", 2.6f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 180;
    m_iSeperateFrame = 28;
    m_iCombineFrame = 127;

    m_iColliderStartFrame_Left[0] = 69;
    m_iColliderEndFrame_Left[0] = 75;
    m_iColliderStartFrame_Left[1] = 114;
    m_iColliderEndFrame_Left[1] = 117;

    m_iColliderStartFrame_Right[0] = 80;
    m_iColliderEndFrame_Right[0] = 83;
    m_iColliderStartFrame_Right[1] = 116;
    m_iColliderEndFrame_Right[1] = 119;


    m_iLeftEffectStartFrame[0] = 65;
    m_iLeftEffectEndFrame[0] = 82;
    m_iRightEffectStartFrame[0] = 71;
    m_iRightEffectEndFrame[0] = 86;

    m_iLeftEffectStartFrame[1] = 110;
    m_iLeftEffectEndFrame[1] = 121;
    m_iRightEffectStartFrame[1] = 112;
    m_iRightEffectEndFrame[1] = 124;

    m_iSwishL_Sound_Frame = 110;
    m_iSwishM_Sound_Frame = 75;
    m_iSwishS_Sound_Frame = 68;
    m_iSwishS_Sound_Frame2 = 10;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Scissor_Charge00::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorCA1, false, 0.05f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_pPlayer->Set_WeaponStrength(ATK_STRONG);

    m_isLeftActiveEffect[0] = m_isLeftActiveEffect[1] = false;
    m_isRightActiveEffect[0] = m_isRightActiveEffect[1] = false;
    m_isLeftDeActiveEffect[0] = m_isLeftDeActiveEffect[1] = false;
    m_isRightDeActiveEffect[0] = m_isRightDeActiveEffect[1] = false;

    m_bSwishL_Sound_Play = false;
    m_bSwishM_Sound_Play = false;
    m_bSwishS_Sound_Play = false;
    m_bSwishS_Sound_Play2 = false;

    return S_OK;
}

void CState_Player_Scissor_Charge00::Update(_float fTimeDelta)
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
            m_pPlayer->Change_State(CPlayer::SCISSOR_LATTACK1);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE1);
            else
            m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK1);
        }
    }
    else if (iFrame >= 195)
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
    Control_Effect(iFrame);
    Control_Sound(iFrame);
}

void CState_Player_Scissor_Charge00::End_State()
{

    m_pPlayer->Combine_Scissor(true);
}

_bool CState_Player_Scissor_Charge00::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ScissorCA1);
}

void CState_Player_Scissor_Charge00::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();
    _bool isOnLeftCollider[2] = { false, false };
    _bool isOnRightCollider[2] = { false, false };

    for (_uint i = 0; i < 2; ++i)
    {

        if (m_iColliderStartFrame_Left[i] <= iFrame && iFrame <= m_iColliderEndFrame_Left[i])
        {
            isOnLeftCollider[i] = true;
            m_pPlayer->Active_CurrentWeaponCollider(1.f, 1);
        }
        else if(!isOnLeftCollider[0] && !isOnLeftCollider[1])
        {
            m_pPlayer->DeActive_CurretnWeaponCollider(1);
        }

        if (m_iColliderStartFrame_Right[i] <= iFrame && iFrame <= m_iColliderEndFrame_Right[i])
        {
            isOnRightCollider[i] = true;
            m_pPlayer->Active_CurrentWeaponCollider(1.f, 0);
        }
        else if (!isOnRightCollider[0] && !isOnRightCollider[1])
        {
            m_pPlayer->DeActive_CurretnWeaponCollider(0);
        }
    }

}

void CState_Player_Scissor_Charge00::Control_Effect(_int iFrame)
{
    for (_uint i = 0; i < 2; ++i)
    {
        if (!m_isLeftActiveEffect[i] && m_iLeftEffectStartFrame[i] <= iFrame)
        {
            m_pPlayer->Active_WeaponEffect(CWeapon_Scissor::EFFECT_BASE, true, 1);
            m_isLeftActiveEffect[i] = true;
        }
        else if (m_isLeftActiveEffect[i] && !m_isLeftDeActiveEffect[i] && m_iLeftEffectEndFrame[i] < iFrame)
        {
            m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor::EFFECT_BASE, 1);
            m_isLeftDeActiveEffect[i] = true;
        }

        if (!m_isRightActiveEffect[i] && m_iRightEffectStartFrame[i] <= iFrame)
        {
            m_pPlayer->Active_WeaponEffect(CWeapon_Scissor::EFFECT_BASE, true, 0);
            m_isRightActiveEffect[i] = true;
        }
        else if (m_isRightActiveEffect[i] && !m_isRightDeActiveEffect[i] && m_iRightEffectEndFrame[i] < iFrame)
        {
            m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor::EFFECT_BASE, 0);
            m_isRightDeActiveEffect[i] = true;
        }
    }
}

void CState_Player_Scissor_Charge00::Control_Sound(_int iFrame)
{
    if (iFrame >= m_iSwishL_Sound_Frame && !m_bSwishL_Sound_Play)
    {
        m_bSwishL_Sound_Play = true;
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_CookingKnife_1H_L_01.wav"));
    }
    if (iFrame >= m_iSwishM_Sound_Frame && !m_bSwishM_Sound_Play)
    {
        m_bSwishM_Sound_Play = true;
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_CookingKnife_1H_M_01.wav"));
    }
    if (iFrame >= m_iSwishS_Sound_Frame && !m_bSwishS_Sound_Play)
    {
        m_bSwishS_Sound_Play = true;
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_CookingKnife_1H_S_01.wav"));
    }  
    if (iFrame >= m_iSwishS_Sound_Frame2 && !m_bSwishS_Sound_Play2)
    {
        m_bSwishS_Sound_Play2 = true;
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_CookingKnife_1H_S_01.wav"));
    }
}

CState_Player_Scissor_Charge00* CState_Player_Scissor_Charge00::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Scissor_Charge00* pInstance = new CState_Player_Scissor_Charge00(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Scissor_Charge00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Scissor_Charge00::Free()
{
    __super::Free();
}