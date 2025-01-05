#include "stdafx.h"
#include "State_Player_Arm_Thrust.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"

CState_Player_Arm_Thrust::CState_Player_Arm_Thrust(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Arm_Thrust::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_ArmThrust = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Arm_GuardEx_WP_Thrust", 2.f);

    m_iStateNum = iStateNum;

    m_iColliderStartFrame = 16;
    m_iColliderEndFrame = 20;

    return S_OK;
}

HRESULT CState_Player_Arm_Thrust::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ArmThrust, false);

    m_isPlaySound = false;

    return S_OK;
}

void CState_Player_Arm_Thrust::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::ARM_LOOP);
        return;
    }


    Control_Collider();
}

void CState_Player_Arm_Thrust::End_State()
{
}

_bool CState_Player_Arm_Thrust::End_Check()
{
    return (m_pPlayer->Get_EndAnim(m_iAnimation_ArmThrust) || m_pPlayer->Get_EndAnim(m_iAnimation_ArmThrust, true));
}

void CState_Player_Arm_Thrust::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iColliderStartFrame <= iFrame && iFrame <= m_iColliderEndFrame)
        m_pPlayer->Active_CurrentWeaponCollider();
    else
        m_pPlayer->DeActive_CurretnWeaponCollider();
}

void CState_Player_Arm_Thrust::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if ((iFrame == m_iColliderStartFrame || iFrame == m_iColliderStartFrame + 1) && !m_isPlaySound)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Glaive_P_B_SS_02.wav"));
        m_isPlaySound = true;
    }
}

CState_Player_Arm_Thrust* CState_Player_Arm_Thrust::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Arm_Thrust* pInstance = new CState_Player_Arm_Thrust(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Arm_Thrust"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Arm_Thrust::Free()
{
    __super::Free();
}
