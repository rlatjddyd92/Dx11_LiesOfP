#include "stdafx.h"
#include "State_Player_Arm_ParryBomb.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Weapon.h"

CState_Player_Arm_ParryBomb::CState_Player_Arm_ParryBomb(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Arm_ParryBomb::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Arm_ParryBomb = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Arm_GuardEx_Explosion", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    m_iSoundFrame = 30;

    return S_OK;
}

HRESULT CState_Player_Arm_ParryBomb::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Arm_ParryBomb, false, 0.f);

    return S_OK;
}

void CState_Player_Arm_ParryBomb::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::ARM_LOOP);
        return;
    }

    //Control_Sound();
}

void CState_Player_Arm_ParryBomb::End_State()
{
    m_pPlayer->Set_IsParry(false);
}

_bool CState_Player_Arm_ParryBomb::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Arm_ParryBomb);
}

void CState_Player_Arm_ParryBomb::Control_Sound()
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

CState_Player_Arm_ParryBomb* CState_Player_Arm_ParryBomb::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Arm_ParryBomb* pInstance = new CState_Player_Arm_ParryBomb(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Arm_ParryBomb"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Arm_ParryBomb::Free()
{
    __super::Free();
}