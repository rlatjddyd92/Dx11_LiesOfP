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

    //m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_FX_Rapier_1H_B_FableArts_Start_01.wav"));
    //m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_SK_FX_Rapier_1H_B_FableArts_Motor_03.wav"));

    m_isActiveEffect[0] = false;
    m_isActiveEffect[1] = false;
    m_isActiveEffect[2] = false;
    m_isActiveEffect[3] = false;

    //m_pPlayer->Active_Effect(TEXT("Player_Attack_Rapier_StormStab_First_Ready"));

    m_bSwishL_Sound_Play = false;
    m_bSwishM_Sound_Play = false;
    m_bSwishS_Sound_Play = false;
    m_bSwishM_Sound_Play2 = false;

    m_pPlayer->Set_WeaponStrength(ATK_STRONG);

    m_pPlayer->SetUp_Monster_Fatal();

    return S_OK;
}

void CState_Player_Scissor_Fatal::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();


    if (End_Check())
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
            if (i == 4)
            {
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