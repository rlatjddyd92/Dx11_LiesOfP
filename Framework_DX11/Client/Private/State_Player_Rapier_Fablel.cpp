#include "stdafx.h"
#include "State_Player_Rapier_Fablel.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon_Rapier.h"

#include "Effect_Manager.h"

CState_Player_Rapier_Fable::CState_Player_Rapier_Fable(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Rapier_Fable::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_RapierFCA = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Rapier_FCA", 3.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 110;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame[0] = 5;
    m_iColliderEndFrame[0] = 10;
    m_iColliderStartFrame[1] = 16;
    m_iColliderEndFrame[1] = 23;
    m_iColliderStartFrame[2] = 36;
    m_iColliderEndFrame[2] = 41;
    m_iColliderStartFrame[3] = 45;
    m_iColliderEndFrame[3] = 48;
    m_iColliderStartFrame[4] = 78;
    m_iColliderEndFrame[4] = 90;

    return S_OK;
}

HRESULT CState_Player_Rapier_Fable::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_RapierFCA, false, 0.05f);

    m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_FX_Rapier_1H_B_FableArts_Start_01.wav"));
    m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_SK_FX_Rapier_1H_B_FableArts_Motor_03.wav"));

    m_isActiveEffect[0] = m_isActiveEffect[1] = false;
    m_isActiveFableEffect[0] = m_isActiveFableEffect[1] = false;

    m_pPlayer->Decrease_Region(3);
    //m_pPlayer->Active_Effect(TEXT("Player_Attack_Rapier_StormStab_First_Ready"));
    
    for (_uint i = 0; i < 5; ++i)
    {
        m_isPlaySound[i] = false;
    }

    m_pPlayer->Set_WeaponStrength(ATK_STRONG);

    return S_OK;
}

void CState_Player_Rapier_Fable::Update(_float fTimeDelta)
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
            m_pPlayer->Change_State(CPlayer::RAPIER_LATTACK0);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::RAPIER_CHARGE);
            else
                m_pPlayer->Change_State(CPlayer::RAPIER_RATTACK0);
        }
    }
    else if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Collider(iFrame);
    Control_Sound(iFrame);
    Control_Effect(iFrame);
}

void CState_Player_Rapier_Fable::End_State()
{
    m_pPlayer->DeActive_WeaponEffect(CWeapon_Rapier::EFFECT_STORMSTAB2);
    m_pPlayer->DeActive_CurretnWeaponCollider();
}

_bool CState_Player_Rapier_Fable::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_RapierFCA);
}

void CState_Player_Rapier_Fable::Control_Collider(_int iFrame)
{
    _bool   isColliderActive = false;

    for (_uint i = 0; i < 5; ++i)
    {
        if (m_iColliderStartFrame[i] <= iFrame && iFrame <= m_iColliderEndFrame[i])
            isColliderActive = true;
    }

    if (isColliderActive)
        m_pPlayer->Active_CurrentWeaponCollider();
    else
        m_pPlayer->DeActive_CurretnWeaponCollider();

}

void CState_Player_Rapier_Fable::Control_Sound(_int iFrame)
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
    else if ((iFrame == m_iColliderStartFrame[3] || iFrame == m_iColliderStartFrame[3] + 1) && !m_isPlaySound[3])
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Dagger_1H_S_02.wav"));
        m_isPlaySound[3] = true;
    }
    else if ((iFrame == m_iColliderStartFrame[4] || iFrame == m_iColliderStartFrame[4] + 1) && !m_isPlaySound[4])
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Dagger_1H_S_03.wav"));
        m_isPlaySound[4] = true;
    }
}

void CState_Player_Rapier_Fable::Control_Effect(_int iFrame)
{
    if (!m_isActiveEffect[0] && (iFrame == m_iColliderStartFrame[0] || iFrame == m_iColliderStartFrame[0] + 1))
    {
        m_pPlayer->Active_WeaponEffect(CWeapon_Rapier::EFFECT_STORMSTAB1);
        m_isActiveEffect[0] = true;
    }
    else if (!m_isActiveEffect[1] && (iFrame == m_iColliderStartFrame[3] || iFrame == m_iColliderStartFrame[3] + 1))
    {
        m_pPlayer->DeActive_WeaponEffect(CWeapon_Rapier::EFFECT_STORMSTAB1);
        m_pPlayer->Active_WeaponEffect(CWeapon_Rapier::EFFECT_STORMSTAB2);
        m_isActiveEffect[1] = true;
    }

    if (!m_isActiveFableEffect[0] && iFrame > 0)
    {
        const _Matrix* SocketMatrix = m_pPlayer->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");

        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Rapier_StormStab_First_Ready"), m_pPlayer->Get_Transform()->Get_WorldMatrix_Ptr(),
            SocketMatrix);

        m_isActiveFableEffect[0] = true;
    }
    else if (!m_isActiveFableEffect[1] && iFrame > 73)
    {
        const _Matrix* SocketMatrix = m_pPlayer->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");

        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Rapier_StormStab_Second_Ready"), m_pPlayer->Get_Transform()->Get_WorldMatrix_Ptr(),
            SocketMatrix);

        m_isActiveFableEffect[1] = true;
    }
}

CState_Player_Rapier_Fable* CState_Player_Rapier_Fable::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Rapier_Fable* pInstance = new CState_Player_Rapier_Fable(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_Fable"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Rapier_Fable::Free()
{
    __super::Free();
}