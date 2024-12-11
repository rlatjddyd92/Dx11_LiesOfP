#include "stdafx.h"
#include "State_Player_Arm_Parry.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Weapon.h"

CState_Player_Arm_Parry::CState_Player_Arm_Parry(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Arm_Parry::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Arm_Parry = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Arm_GuardEx_Parry", 4.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    m_iSoundFrame = 25;

    return S_OK;
}

HRESULT CState_Player_Arm_Parry::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Arm_Parry, false, 0.05f);

    return S_OK;
}

void CState_Player_Arm_Parry::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (25 <= iFrame && iFrame < 45)
    {
        m_pPlayer->Set_IsParry(true);
    }

    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::ARM_LOOP);
        return;
    }

    Control_Sound();
}

void CState_Player_Arm_Parry::End_State()
{
    m_pPlayer->Set_IsParry(false);
}

_bool CState_Player_Arm_Parry::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Arm_Parry);
}

void CState_Player_Arm_Parry::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if ((iFrame == m_iSoundFrame || iFrame == m_iSoundFrame + 1) && !m_isPlaySound)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_FX_Rapier_1H_H_FableArts_Parry_01.wav"));
        m_isPlaySound = true;
    }
    else
    {
        m_isPlaySound = false;
    }
}

CState_Player_Arm_Parry* CState_Player_Arm_Parry::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Arm_Parry* pInstance = new CState_Player_Arm_Parry(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Arm_Parry"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Arm_Parry::Free()
{
    __super::Free();
}