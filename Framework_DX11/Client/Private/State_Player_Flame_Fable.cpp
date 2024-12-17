#include "stdafx.h"
#include "State_Player_Flame_Fable.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Weapon_FlameSword.h"

CState_Player_Flame_Fable::CState_Player_Flame_Fable(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Flame_Fable::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_FlameFable = m_pPlayer->Get_Model()->Find_AnimationIndex("As_Pino_FableArts_TripleSlash", 3.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 180;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame[0] = 22;
    m_iColliderEndFrame[0] = 32;
    m_iColliderStartFrame[1] = 50;
    m_iColliderEndFrame[1] = 57;
    m_iColliderStartFrame[2] = 80;
    m_iColliderEndFrame[2] = 90;

    m_iEffectStartFrame[0] = 35;
    m_iEffectEndFrame[0] = 68;
    m_iEffectStartFrame[1] = 78;
    m_iEffectEndFrame[1] = 93;

    return S_OK;
}

HRESULT CState_Player_Flame_Fable::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_FlameFable, false, 0.05f);

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

    m_isActiveEffect[0] = m_isDeActiveEffect[0] = false;
    m_isActiveEffect[1] = m_isDeActiveEffect[1] = false;

    return S_OK;
}

void CState_Player_Flame_Fable::Update(_float fTimeDelta)
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

    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if (m_isInputLButton)
            m_pPlayer->Change_State(CPlayer::FLAME_LATTACK0);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::FLAME_CHARGE0);
            else
                m_pPlayer->Change_State(CPlayer::FLAME_RATTACK0);
        }
    }
    else if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }

    Control_Collider(iFrame);
    Control_Sound(iFrame);
    Control_Effect(iFrame);
}

void CState_Player_Flame_Fable::End_State()
{
    m_pPlayer->DeActive_CurretnWeaponCollider();
}

_bool CState_Player_Flame_Fable::End_Check()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame > 181)
    {
        return true;
    }

    return false;
}

void CState_Player_Flame_Fable::Control_Collider(_int iFrame)
{
    _bool   isColliderActive = false;

    for (_uint i = 0; i < 3; ++i)
    {
        if (m_iColliderStartFrame[i] <= iFrame && iFrame <= m_iColliderEndFrame[i])
            isColliderActive = true;
    }

    if (isColliderActive)
    {
        m_pPlayer->Active_CurrentWeaponCollider();
    }
    else
    {
        m_pPlayer->DeActive_CurretnWeaponCollider();
    }

}

void CState_Player_Flame_Fable::Control_Sound(_int iFrame)
{
    if ((iFrame == m_iColliderStartFrame[0] || iFrame == m_iColliderStartFrame[0] + 1) && !m_isPlaySound[0])
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Dagger_1H_S_01.wav"));
        m_isPlaySound[0] = true;
    }
    else if ((iFrame == m_iColliderStartFrame[1] || iFrame == m_iColliderStartFrame[1] + 1) && !m_isPlaySound[1])
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Dagger_1H_S_02.wav"));
        m_isPlaySound[1] = true;
    }
    else if ((iFrame == m_iColliderStartFrame[2] || iFrame == m_iColliderStartFrame[2] + 1) && !m_isPlaySound[2])
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Dagger_1H_S_01.wav"));
        m_isPlaySound[2] = true;
    }
}

void CState_Player_Flame_Fable::Control_Effect(_int iFrame)
{
    if (!m_isActiveEffect[0] && m_iEffectStartFrame[0] <= iFrame)
    {
        m_pPlayer->Active_WeaponEffect(CWeapon_FlameSword::EFFECT_STORMSLASH1);
        m_isActiveEffect[0] = true;
    }
    else if (m_isActiveEffect[0] && !m_isDeActiveEffect[0] && m_iEffectEndFrame[0] < iFrame)
    {
        m_pPlayer->DeActive_WeaponEffect(CWeapon_FlameSword::EFFECT_STORMSLASH1);
        m_isDeActiveEffect[0] = true;
    }

    if (!m_isActiveEffect[1] && m_iEffectStartFrame[1] <= iFrame)
    {
        m_pPlayer->Active_WeaponEffect(CWeapon_FlameSword::EFFECT_STORMSLASH2);
        m_isActiveEffect[1] = true;
    }
    else if (m_isActiveEffect[1] && !m_isDeActiveEffect[1] && m_iEffectEndFrame[1] < iFrame)
    {
        m_pPlayer->DeActive_WeaponEffect(CWeapon_FlameSword::EFFECT_STORMSLASH2);
        m_isDeActiveEffect[1] = true;
    }
}

CState_Player_Flame_Fable* CState_Player_Flame_Fable::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Flame_Fable* pInstance = new CState_Player_Flame_Fable(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Flame_Fable"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Flame_Fable::Free()
{
    __super::Free();
}