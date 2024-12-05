#include "stdafx.h"
#include "State_Player_Rapier_ParryAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"

CState_Player_Rapier_ParryAttack::CState_Player_Rapier_ParryAttack(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Rapier_ParryAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_RapierParryAtk[0] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Rapier_SA1", 3.f);
    m_iAnimation_RapierParryAtk[1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Rapier_SS", 3.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 65;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame[0] = 7;
    m_iColliderEndFrame[0] = 20;
    m_iColliderStartFrame[1] = 12;
    m_iColliderEndFrame[1] = 20;

    return S_OK;
}

HRESULT CState_Player_Rapier_ParryAttack::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_RapierParryAtk[0], false, 0.f);

    m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_FX_Rapier_1H_B_FableArts_Start_01.wav"));
    m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_SK_FX_Rapier_1H_B_FableArts_Motor_03.wav"));

    m_isActiveEffect[0] = false;
    m_isActiveEffect[1] = false;

    m_pPlayer->Decrease_Region(3);
    //m_pPlayer->Active_Effect(TEXT("Player_Attack_Rapier_StormStab_First_Ready"));

    for (_uint i = 0; i < 5; ++i)
    {
        m_isPlaySound[i] = false;
    }

    m_pPlayer->Set_WeaponStrength(ATK_STRONG);

    return S_OK;
}

void CState_Player_Rapier_ParryAttack::Update(_float fTimeDelta)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame < m_iChangeFrame && iCurAnim == m_iAnimation_RapierParryAtk[1])
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

    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15 && iCurAnim == m_iAnimation_RapierParryAtk[1])
    {
        if (m_isInputLButton)
            m_pPlayer->Change_State(CPlayer::RAPIER_LATTACK0);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::RAPIER_CHARGE);
            else
                m_pPlayer->Change_State(CPlayer::RAPIER_RATTACK0);
        }
    }
    else if (iCurAnim == m_iAnimation_RapierParryAtk[0] && iFrame > 45 )
    {
        m_pPlayer->Change_Animation(m_iAnimation_RapierParryAtk[1], false, 0.f);
    }
    else if (End_Check())
    {
        if (m_iAnimation_RapierParryAtk[1] == iCurAnim)
        {
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        }
    }

    Control_Collider(iFrame);
    Control_Sound(iFrame);
    Control_Effect(iFrame);
}

void CState_Player_Rapier_ParryAttack::End_State()
{
    m_pPlayer->DeActive_Effect(CPlayer::EFFECT_RAPIER_TRAIL_SECOND);
    m_pPlayer->DeActive_CurretnWeaponCollider();
}

_bool CState_Player_Rapier_ParryAttack::End_Check()
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };

    if (m_iAnimation_RapierParryAtk[0] == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_RapierParryAtk[0]);
    }
    else if (m_iAnimation_RapierParryAtk[1] == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_RapierParryAtk[1]);
    }

    return bEndCheck;
}

void CState_Player_Rapier_ParryAttack::Control_Collider(_int iFrame)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _bool   isColliderActive = false;

    for (_uint i = 0; i < 2; ++i)
    {
        if (iCurAnim == m_iAnimation_RapierParryAtk[i])
        {
            if (m_iColliderStartFrame[i] <= iFrame && iFrame <= m_iColliderEndFrame[i])
                isColliderActive = true;
        }
    }

    if (isColliderActive)
        m_pPlayer->Active_CurrentWeaponCollider();
    else
        m_pPlayer->DeActive_CurretnWeaponCollider();

}

void CState_Player_Rapier_ParryAttack::Control_Sound(_int iFrame)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();

    if (iCurAnim == m_iAnimation_RapierParryAtk[0] && (iFrame == m_iColliderStartFrame[0] || iFrame == m_iColliderStartFrame[0] + 1) && !m_isPlaySound[0])
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Dagger_1H_S_01.wav"));
        m_isPlaySound[0] = true;
    }
    else if (iCurAnim == m_iAnimation_RapierParryAtk[1] && (iFrame == m_iColliderStartFrame[1] || iFrame == m_iColliderStartFrame[1] + 1) && !m_isPlaySound[1])
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Dagger_1H_S_02.wav"));
        m_isPlaySound[1] = true;
    }
}

void CState_Player_Rapier_ParryAttack::Control_Effect(_int iFrame)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();

    if (!m_isActiveEffect[0] && iCurAnim == m_iAnimation_RapierParryAtk[0] && (iFrame == m_iColliderStartFrame[0] || iFrame == m_iColliderStartFrame[0] + 1))
    {
        m_pPlayer->Active_Effect(CPlayer::EFFECT_RAPIER_TRAIL_FIRST);
        m_isActiveEffect[0] = true;
    }
    else if (!m_isActiveEffect[1] && iCurAnim == m_iAnimation_RapierParryAtk[1] && (iFrame == m_iColliderStartFrame[3] || iFrame == m_iColliderStartFrame[3] + 1))
    {
        m_pPlayer->DeActive_Effect(CPlayer::EFFECT_RAPIER_TRAIL_FIRST);
        m_pPlayer->Active_Effect(CPlayer::EFFECT_RAPIER_TRAIL_SECOND);
        m_isActiveEffect[1] = true;
    }
}

CState_Player_Rapier_ParryAttack* CState_Player_Rapier_ParryAttack::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Rapier_ParryAttack* pInstance = new CState_Player_Rapier_ParryAttack(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_ParryAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Rapier_ParryAttack::Free()
{
    __super::Free();
}