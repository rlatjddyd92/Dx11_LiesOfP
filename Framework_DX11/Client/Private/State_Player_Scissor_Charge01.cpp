#include "stdafx.h"
#include "State_Player_Scissor_Charge01.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Weapon_Scissor.h"

CState_Player_Scissor_Charge01::CState_Player_Scissor_Charge01(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Scissor_Charge01::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ScissorCA2 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_ScissorSword_CA2", 2.6f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 180;
    m_iSeperateFrame = 30;
    m_iCombineFrame = 145;

    m_iStateNum = iStateNum;

    m_iColliderStartFrame_Left[0] = 94;
    m_iColliderEndFrame_Left[0] = 99;
    m_iColliderStartFrame_Left[1] = 136;
    m_iColliderEndFrame_Left[1] = 142;

    m_iColliderStartFrame_Right[0] = 94;
    m_iColliderEndFrame_Right[0] = 99;
    m_iColliderStartFrame_Right[1] = 132;
    m_iColliderEndFrame_Right[1] = 139;

    m_iLeftEffectStartFrame[0] = 90;
    m_iLeftEffectEndFrame[0] = 102;
    m_iRightEffectStartFrame[0] = 90;
    m_iRightEffectEndFrame[0] = 102;

    m_iLeftEffectStartFrame[1] = 130;
    m_iLeftEffectEndFrame[1] = 143;
    m_iRightEffectStartFrame[1] = 130;
    m_iRightEffectEndFrame[1] = 142;

    return S_OK;
}

HRESULT CState_Player_Scissor_Charge01::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorCA2, false, 0.05f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_pPlayer->Set_WeaponStrength(ATK_STRONG);

    m_isLeftActiveEffect[0] = m_isLeftActiveEffect[1] = false;
    m_isRightActiveEffect[0] = m_isRightActiveEffect[1] = false;
    m_isLeftDeActiveEffect[0] = m_isLeftDeActiveEffect[1] = false;
    m_isRightDeActiveEffect[0] = m_isRightDeActiveEffect[1] = false;

    return S_OK;
}

void CState_Player_Scissor_Charge01::Update(_float fTimeDelta)
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
        if (m_isInputLButton)
            m_pPlayer->Change_State(CPlayer::SCISSOR_LATTACK0);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE0);
            else
            m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK1);
        }
    }
    else if (iFrame >= 225)
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
}

void CState_Player_Scissor_Charge01::End_State()
{

    m_pPlayer->Combine_Scissor(true);
}

_bool CState_Player_Scissor_Charge01::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ScissorCA2);
}

void CState_Player_Scissor_Charge01::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();
    _bool isOnLeftCollider[2] = { false, false };
    _bool isOnRightCollider[2] = { false, false };

    for (_uint i = 0; i < 2; ++i)
    {
        if (m_iColliderStartFrame_Left[i] <= iFrame && iFrame <= m_iColliderEndFrame_Left[i])
        {
            isOnLeftCollider[i] = true;
            m_pPlayer->Active_CurrentWeaponCollider(3.f, 1);
        }
        else if (!isOnLeftCollider[0] && !isOnLeftCollider[1])
        {
            m_pPlayer->DeActive_CurretnWeaponCollider(1);
        }

        if (m_iColliderStartFrame_Right[i] <= iFrame && iFrame <= m_iColliderEndFrame_Right[i])
        {
            isOnRightCollider[i] = true;
            m_pPlayer->Active_CurrentWeaponCollider(3.f, 0);
        }
        else if (!isOnRightCollider[0] && !isOnRightCollider[1])
        {
            m_pPlayer->DeActive_CurretnWeaponCollider(0);
        }
    }

}

void CState_Player_Scissor_Charge01::Control_Effect(_int iFrame)
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

CState_Player_Scissor_Charge01* CState_Player_Scissor_Charge01::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Scissor_Charge01* pInstance = new CState_Player_Scissor_Charge01(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Scissor_Charge01"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Scissor_Charge01::Free()
{
    __super::Free();
}