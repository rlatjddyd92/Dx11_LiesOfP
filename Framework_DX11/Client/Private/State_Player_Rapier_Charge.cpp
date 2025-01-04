#include "stdafx.h"
#include "State_Player_Rapier_Charge.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"

CState_Player_Rapier_Charge::CState_Player_Rapier_Charge(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Rapier_Charge::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_RapierCA1 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Rapier_CA1", 2.3f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 140;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame[0] = 60;
    m_iColliderEndFrame[0] = 65;
    m_iColliderStartFrame[1] = 70;
    m_iColliderEndFrame[1] = 74;
    m_iColliderStartFrame[2] = 105;
    m_iColliderEndFrame[2] = 120;

    return S_OK;
}

HRESULT CState_Player_Rapier_Charge::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_RapierCA1, false);

    for (_uint i = 0; i < 3; ++i)
    {
        m_isPlaySound[i] = false;
    }

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_pPlayer->Set_WeaponStrength(ATK_STRONG);
    m_pPlayer->Get_CurrentWeapon()->Set_DamageAmount(31.f);

    return S_OK;
}

void CState_Player_Rapier_Charge::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if (m_pPlayer->Key_Hold(KEY::LSHIFT))
        {
            m_pPlayer->Change_State(CPlayer::OH_GUARD);
        }
        else if (m_isInputLButton)
            m_pPlayer->Change_State(CPlayer::RAPIER_LATTACK0);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::RAPIER_CHARGE);
            else
                m_pPlayer->Change_State(CPlayer::RAPIER_RATTACK0);
        }
        else if (m_pPlayer->Key_Hold(KEY::W) || m_pPlayer->Key_Hold(KEY::S) || m_pPlayer->Key_Hold(KEY::D) || m_pPlayer->Key_Hold(KEY::A))
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
}

void CState_Player_Rapier_Charge::End_State()
{
}

_bool CState_Player_Rapier_Charge::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_RapierCA1);
}

void CState_Player_Rapier_Charge::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    _bool   isColliderActive = false;

    for (_uint i = 0; i < 3; ++i)
    {
        if (m_iColliderStartFrame[i] <= iFrame && iFrame <= m_iColliderEndFrame[i])
        {
            if (i == 2)
            {
                m_pPlayer->Get_CurrentWeapon()->Set_DamageAmount(38.f);
            }
            else
            {
                m_pPlayer->Get_CurrentWeapon()->Set_DamageAmount(31.f);
            }

            isColliderActive = true;
        }
    }

    if (isColliderActive)
    {
        if (m_pPlayer->Active_CurrentWeaponCollider())
            m_pPlayer->Decrease_Stamina(20.f);
    }
    else
    {
        m_pPlayer->DeActive_CurretnWeaponCollider();
    }
}

void CState_Player_Rapier_Charge::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

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

CState_Player_Rapier_Charge* CState_Player_Rapier_Charge::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Rapier_Charge* pInstance = new CState_Player_Rapier_Charge(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Rapier_Charge::Free()
{
    __super::Free();
}