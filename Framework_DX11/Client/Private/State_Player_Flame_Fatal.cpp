#include "stdafx.h"
#include "State_Player_Flame_Fatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"

CState_Player_Flame_Fatal::CState_Player_Flame_Fatal(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Flame_Fatal::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_FlameFatal = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Fatal_Middle_Swing", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 210;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame[0] = 25;
    m_iColliderEndFrame[0] = 35;
    m_iColliderStartFrame[1] = 60;
    m_iColliderEndFrame[1] = 70;
    m_iColliderStartFrame[2] = 120;
    m_iColliderEndFrame[2] = 130;

    m_iSwishL_Sound_Frame = 120;
    m_iSwishM_Sound_Frame = 23;
    m_iSwishS_Sound_Frame = 60;


    return S_OK;
}

HRESULT CState_Player_Flame_Fatal::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_FlameFatal, false, 0.05f);

    //m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_FX_Rapier_1H_B_FableArts_Start_01.wav"));
    //m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_SK_FX_Rapier_1H_B_FableArts_Motor_03.wav"));

    m_isActiveEffect[0] = false;
    m_isActiveEffect[1] = false;
    m_isActiveEffect[2] = false;

    //m_pPlayer->Active_Effect(TEXT("Player_Attack_Rapier_StormStab_First_Ready"));

    for (_uint i = 0; i < 3; ++i)
    {
        m_isPlaySound[i] = false;
    }

     m_isSwishL_Play_Sound = false;
     m_isSwishM_Play_Sound = false;
     m_isSwishS_Play_Sound = false;

    m_pPlayer->Set_WeaponStrength(ATK_STRONG);

    m_pPlayer->SetUp_Monster_Fatal();

    return S_OK;
}

void CState_Player_Flame_Fatal::Update(_float fTimeDelta)
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

void CState_Player_Flame_Fatal::End_State()
{
    m_pPlayer->DeActive_CurretnWeaponCollider();
}

_bool CState_Player_Flame_Fatal::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_FlameFatal);
}

void CState_Player_Flame_Fatal::Control_Collider(_int iFrame)
{
    _bool   isColliderActive = false;

    for (_uint i = 0; i < 3; ++i)
    {
        if (m_iColliderStartFrame[i] <= iFrame && iFrame <= m_iColliderEndFrame[i])
        {
            if (i == 2)
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

void CState_Player_Flame_Fatal::Control_Sound(_int iFrame)
{
    if ((iFrame == m_iSwishL_Sound_Frame || iFrame == m_iSwishL_Sound_Frame + 1) && !m_isSwishL_Play_Sound)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_FlameSword_2H_L_01.wav"));
        m_isSwishL_Play_Sound = true;
    }
    else  if ((iFrame == m_iSwishM_Sound_Frame || iFrame == m_iSwishM_Sound_Frame + 1) && !m_isSwishM_Play_Sound)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_FlameSword_2H_M_01.wav"));
        m_isSwishM_Play_Sound = true;
    }
    else  if ((iFrame == m_iSwishS_Sound_Frame || iFrame == m_iSwishS_Sound_Frame + 1) && !m_isSwishS_Play_Sound)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_FlameSword_2H_S_01.wav"));
        m_isSwishS_Play_Sound = true;
    }
}

void CState_Player_Flame_Fatal::Control_Effect(_int iFrame)
{
    if (!m_isActiveEffect[0] && (iFrame == m_iColliderStartFrame[0] || iFrame == m_iColliderStartFrame[0] + 1))
    {
        m_isActiveEffect[0] = true;
    }
    else if (!m_isActiveEffect[1] && (iFrame == m_iColliderStartFrame[3] || iFrame == m_iColliderStartFrame[3] + 1))
    {
        m_isActiveEffect[1] = true;
    }
}

CState_Player_Flame_Fatal* CState_Player_Flame_Fatal::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Flame_Fatal* pInstance = new CState_Player_Flame_Fatal(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Flame_Fatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Flame_Fatal::Free()
{
    __super::Free();
}