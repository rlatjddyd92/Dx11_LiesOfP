#include "stdafx.h"
#include "State_Player_Flame_LAttack00.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Weapon_FlameSword.h"

CState_Player_Flame_LAttack00::CState_Player_Flame_LAttack00(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Flame_LAttack00::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_FlameNA3 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_FlameSword_NA3", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 55;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame = 33;
    m_iColliderEndFrame = 38;

    m_iEffectStartFrame = 30;
    m_iEffectEndFrame = 42;

    return S_OK;
}

HRESULT CState_Player_Flame_LAttack00::Start_State(void* pArg)
{
    if (m_pFsm->Get_PrevState() == CPlayer::TH_IDLE)
        m_pPlayer->Change_Animation(m_iAnimation_FlameNA3, false, 0.f);
    else
        m_pPlayer->Change_Animation(m_iAnimation_FlameNA3, false, 0.1f, 7);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_isPlaySound = false;

    m_isActiveEffect = false;

    m_pPlayer->Set_WeaponStrength(ATK_WEAK);
    m_pPlayer->Get_CurrentWeapon()->Set_DamageAmount(30.f);

    return S_OK;
}

void CState_Player_Flame_LAttack00::Update(_float fTimeDelta)
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

    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if (m_pPlayer->Key_Hold(KEY::LSHIFT))
        {
            m_pPlayer->Change_State(CPlayer::TH_GUARD);
        }
        else if (m_isInputLButton)
        {
            m_pPlayer->Change_State(CPlayer::FLAME_LATTACK1);
        }
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::FLAME_CHARGE1);
            else
                m_pPlayer->Change_State(CPlayer::FLAME_RATTACK1);
        }
    }
    else if (iFrame >= 100)
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

    Control_Collider();
    Control_Sound();
    Control_Effect(iFrame);
}

void CState_Player_Flame_LAttack00::End_State()
{
    m_pPlayer->DeActive_WeaponEffect(CWeapon_FlameSword::EFFECT_BASE);
    m_pPlayer->DeActive_CurretnWeaponCollider();

}

_bool CState_Player_Flame_LAttack00::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_FlameNA3);
}

void CState_Player_Flame_LAttack00::Control_Collider()
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
    {
        m_pPlayer->DeActive_CurretnWeaponCollider();
    }
}

void CState_Player_Flame_LAttack00::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if ((iFrame == m_iColliderStartFrame || iFrame == m_iColliderStartFrame + 1) && !m_isPlaySound)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Glaive_P_B_M_01.wav"));
        m_isPlaySound = true;
    }
}

void CState_Player_Flame_LAttack00::Control_Effect(_int iFrame)
{
    if (!m_isActiveEffect && m_iEffectStartFrame <= iFrame)
    {
        m_pPlayer->Active_WeaponEffect(CWeapon_FlameSword::EFFECT_BASE);
        m_isActiveEffect = true;
    }
    else if (m_isActiveEffect && m_iEffectEndFrame < iFrame)
    {
        m_pPlayer->DeActive_WeaponEffect(CWeapon_FlameSword::EFFECT_BASE);
    }
}

CState_Player_Flame_LAttack00* CState_Player_Flame_LAttack00::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Flame_LAttack00* pInstance = new CState_Player_Flame_LAttack00(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Flame_LAttack00::Free()
{
    __super::Free();
}