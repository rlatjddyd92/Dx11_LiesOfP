#include "stdafx.h"
#include "State_Player_Scissor_Fatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"

CState_Player_Scissor_Fatal::CState_Player_Scissor_Fatal(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Scissor_Fatal::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ScissorFatal = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Fatal_Light_Swing", 2.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 200;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame[0] = 14;
    m_iColliderEndFrame[0] = 21;
    m_iColliderStartFrame[1] = 27;
    m_iColliderEndFrame[1] = 34;
    m_iColliderStartFrame[2] = 48;
    m_iColliderEndFrame[2] = 55;
    m_iColliderStartFrame[3] = 87;
    m_iColliderEndFrame[3] = 93;

    m_iSwishL_Sound_Frame = 85;
    m_iSwishM_Sound_Frame = 14;
    m_iSwishS_Sound_Frame = 27;
    m_iSwishM_Sound_Frame2 =47;

    return S_OK;
}

HRESULT CState_Player_Scissor_Fatal::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorFatal, false, 0.05f);

    m_isActiveEffect[0] = false;
    m_isActiveEffect[1] = false;
    m_isActiveEffect[2] = false;
    m_isActiveEffect[3] = false;

    m_bSwishL_Sound_Play = false;
    m_bSwishM_Sound_Play = false;
    m_bSwishS_Sound_Play = false;
    m_bSwishM_Sound_Play2 = false;

    m_pPlayer->Set_WeaponStrength(ATK_STRONG);
    m_pPlayer->Get_CurrentWeapon()->Set_DamageAmount(30.f);

    m_pPlayer->SetUp_Monster_Fatal();

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    return S_OK;
}

void CState_Player_Scissor_Fatal::Update(_float fTimeDelta)
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
        else if (m_pPlayer->Key_Tab(KEY::RBUTTON))
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
        else if (m_pPlayer->Key_Hold(KEY::W) || m_pPlayer->Key_Hold(KEY::S) || m_pPlayer->Key_Hold(KEY::D) || m_pPlayer->Key_Hold(KEY::A))
        {
            m_pPlayer->Change_State(CPlayer::OH_RUN);
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

void CState_Player_Scissor_Fatal::End_State()
{
    m_pPlayer->DeActive_CurretnWeaponCollider();
}

_bool CState_Player_Scissor_Fatal::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ScissorFatal);
}

void CState_Player_Scissor_Fatal::Control_Collider(_int iFrame)
{
    _bool   isColliderActive = false;

    for (_uint i = 0; i < 4; ++i)
    {
        if (m_iColliderStartFrame[i] <= iFrame && iFrame <= m_iColliderEndFrame[i])
        {
            if (i == 3)
            {
                m_pPlayer->Get_CurrentWeapon()->Set_DamageAmount(50.f);
                m_pPlayer->Set_WeaponStrength(ATK_LAST);
            }

            isColliderActive = true;
        }
    }

    if (isColliderActive)
        m_pPlayer->Active_CurrentWeaponCollider();
    else
        m_pPlayer->DeActive_CurretnWeaponCollider();

}

void CState_Player_Scissor_Fatal::Control_Sound(_int iFrame)
{
    if (iFrame >= m_iSwishL_Sound_Frame && !m_bSwishL_Sound_Play)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Dagger_1H_L_01.wav"));
        m_bSwishL_Sound_Play = true;
    }
    if (iFrame >= m_iSwishM_Sound_Frame && !m_bSwishM_Sound_Play)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Dagger_1H_M_01.wav"));
        m_bSwishM_Sound_Play = true;
    }
    if (iFrame >= m_iSwishM_Sound_Frame2 && !m_bSwishM_Sound_Play2)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Dagger_1H_M_01.wav"));
        m_bSwishM_Sound_Play2 = true;
    }
    if (iFrame >= m_iSwishS_Sound_Frame && !m_bSwishS_Sound_Play)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Dagger_1H_S_01.wav"));
        m_bSwishS_Sound_Play = true;
    }
  
}

void CState_Player_Scissor_Fatal::Control_Effect(_int iFrame)
{
    if (!m_isActiveEffect[0] && (iFrame == m_iColliderStartFrame[0] || iFrame == m_iColliderStartFrame[0] + 1))
    {
       // m_pPlayer->Active_Effect(CPlayer::EFFECT_RAPIER_TRAIL_FIRST);
        m_isActiveEffect[0] = true;
    }
    else if (!m_isActiveEffect[1] && (iFrame == m_iColliderStartFrame[3] || iFrame == m_iColliderStartFrame[3] + 1))
    {
        //m_pPlayer->DeActive_Effect(CPlayer::EFFECT_RAPIER_TRAIL_FIRST);
        //m_pPlayer->Active_Effect(CPlayer::EFFECT_RAPIER_TRAIL_SECOND);
        m_isActiveEffect[1] = true;
    }
}

CState_Player_Scissor_Fatal* CState_Player_Scissor_Fatal::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Scissor_Fatal* pInstance = new CState_Player_Scissor_Fatal(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Scissor_Fatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Scissor_Fatal::Free()
{
    __super::Free();
}