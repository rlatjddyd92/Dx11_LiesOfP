#include "stdafx.h"
#include "State_Player_Scissor_RAttack01.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Weapon.h"

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

    return S_OK;
}

HRESULT CState_Player_Scissor_RAttack01::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorSA2, false, 0.3f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_pPlayer->Set_WeaponStrength(ATK_NORMAL);
    m_pPlayer->Set_WeaponEffectType(CWeapon::ATK_EFFECT_GENERAL);

    return S_OK;
}

void CState_Player_Scissor_RAttack01::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame < m_iChangeFrame)
    {
        if (KEY_TAP(KEY::LBUTTON))
        {
            m_isInputLButton = true;
            m_isInputRButton = false;
        }
        else if (KEY_TAP(KEY::RBUTTON))
        {
            m_isInputRButton = true;
            m_isInputLButton = false;
            m_fRButtonTime = 0.f;
        }
        else if (KEY_HOLD(KEY::RBUTTON))
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
                 m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE1);
             else
                 m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK0);
         }
     }
     else if (iFrame >= 130)
     {
         if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D) || KEY_HOLD(KEY::A))
         {
             m_pPlayer->Change_State(CPlayer::OH_RUN);
         }
     }
     else if (End_Check())
     {
         m_pPlayer->Change_State(CPlayer::OH_IDLE);
     }

     Control_Collider();
}

void CState_Player_Scissor_RAttack01::End_State()
{
    m_pPlayer->Set_WeaponEffectType(CWeapon::ATK_EFFECT_NOTHING);
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
            m_pPlayer->Active_CurrentWeaponCollider(3.f, 1);
        }
        else
        {
            m_pPlayer->DeActive_CurretnWeaponCollider(1);
        }

        if (m_iColliderStartFrameRight <= iFrame && iFrame <= m_iColliderEndFrameRight)
        {
            m_pPlayer->Active_CurrentWeaponCollider(3.f, 0);
        }
        else
        {
            m_pPlayer->DeActive_CurretnWeaponCollider(0);
        }
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