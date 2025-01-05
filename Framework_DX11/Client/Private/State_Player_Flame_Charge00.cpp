#include "stdafx.h"
#include "State_Player_Flame_Charge00.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include"Weapon_FlameSword.h"

CState_Player_Flame_Charge00::CState_Player_Flame_Charge00(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Flame_Charge00::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_FlameCA1 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_FlameSword_CA1", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 150;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame[0] = 83;
    m_iColliderEndFrame[0] = 88;
    m_iColliderStartFrame[1] = 120;
    m_iColliderEndFrame[1] = 130;

    m_iEffectStartFrame[0] = 80;
    m_iEffectEndFrame[0] = 90;
    m_iEffectStartFrame[1] = 118;
    m_iEffectEndFrame[1] = 134;

    m_iSwishM_SoundFrame = 80;
    m_iSwishL_SoundFrame = 120;

    return S_OK;
}

HRESULT CState_Player_Flame_Charge00::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_FlameCA1, false, 0.15f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_isActiveEffect[0] = m_isDeActiveEffect[0] = false;
    m_isActiveEffect[1] = m_isDeActiveEffect[1] = false;

    m_isSwishM_PlaySound = false;
    m_isSwishL_PlaySound = false;

    m_pPlayer->Set_WeaponStrength(ATK_STRONG);
    m_pPlayer->Get_CurrentWeapon()->Set_DamageAmount(50.f);

    return S_OK;
}

void CState_Player_Flame_Charge00::Update(_float fTimeDelta)
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
            m_pPlayer->Change_State(CPlayer::TH_GUARD);
        }
        else if (m_isInputLButton)
        {
            m_pPlayer->Change_State(CPlayer::FLAME_LATTACK0);
        }
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::FLAME_CHARGE1);
            else
                m_pPlayer->Change_State(CPlayer::FLAME_RATTACK1);
        }
    }
    else if (iFrame >= 210)
    {
        if (m_pPlayer->Key_Hold(KEY::W) || m_pPlayer->Key_Hold(KEY::S) || m_pPlayer->Key_Hold(KEY::D) || m_pPlayer->Key_Hold(KEY::A))
        {
            m_pPlayer->Change_State(CPlayer::TH_RUN);
        }
    }
    else if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }

    if (iFrame >= m_iSwishM_SoundFrame && !m_isSwishM_PlaySound)
    {
        m_isSwishM_PlaySound = true;
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_FlameSword_2H_M_01.wav"));
    }
    if (iFrame >= m_iSwishL_SoundFrame && !m_isSwishL_PlaySound)
    {
        m_isSwishL_PlaySound = true;
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_FlameSword_2H_L_01.wav"));
    }

    Control_Collider();
    Control_Effect(iFrame);
}

void CState_Player_Flame_Charge00::End_State()
{
    m_pPlayer->DeActive_CurretnWeaponCollider();
}

_bool CState_Player_Flame_Charge00::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_FlameCA1);
}

void CState_Player_Flame_Charge00::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    _bool   isColliderActive = false;

    for (_uint i = 0; i < 2; ++i)
    {
        if (m_iColliderStartFrame[i] <= iFrame && iFrame <= m_iColliderEndFrame[i])
        {
            if (i == 1)
            {
                m_pPlayer->Get_CurrentWeapon()->Set_DamageAmount(70.f);
            }
            isColliderActive = true;
        }
    }

    if (isColliderActive)
    {
        if (m_pPlayer->Active_CurrentWeaponCollider())
        {
            m_pPlayer->Decrease_Stamina(45.f);
        }
    }
    else
        m_pPlayer->DeActive_CurretnWeaponCollider();
}

void CState_Player_Flame_Charge00::Control_Effect(_int iFrame)
{
    for (_uint i = 0; i < 2; ++i)
    {
        if (!m_isActiveEffect[i] && m_iEffectStartFrame[i] <= iFrame)
        {
            m_pPlayer->Active_WeaponEffect(CWeapon_FlameSword::EFFECT_BASE);
            m_isActiveEffect[i] = true;
        }
        else if (m_isActiveEffect[i] && !m_isDeActiveEffect[i] && m_iEffectEndFrame[i] < iFrame)
        {
            m_pPlayer->DeActive_WeaponEffect(CWeapon_FlameSword::EFFECT_BASE);
            m_isDeActiveEffect[i] = true;
        }
    }
}

CState_Player_Flame_Charge00* CState_Player_Flame_Charge00::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Flame_Charge00* pInstance = new CState_Player_Flame_Charge00(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Flame_Charge00::Free()
{
    __super::Free();
}