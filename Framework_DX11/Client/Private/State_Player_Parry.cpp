#include "stdafx.h"
#include "State_Player_Parry.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"

CState_Player_Parry::CState_Player_Parry(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Parry::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Parry = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_P_Parry_Guard", 4.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    m_iSoundFrame = 30;

    return S_OK;
}

HRESULT CState_Player_Parry::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Parry, false, 0.05f);


    return S_OK;
}

void CState_Player_Parry::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (30 <= iFrame && iFrame < 40)
    {
        m_pPlayer->Set_IsParry(true);
    }
    else
    {
        m_pPlayer->Set_IsParry(false);
    }

    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Sound();
}

void CState_Player_Parry::End_State()
{
    m_pPlayer->Set_IsParry(false);
}

_bool CState_Player_Parry::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Parry);
}

void CState_Player_Parry::Control_Sound()
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

CState_Player_Parry* CState_Player_Parry::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Parry* pInstance = new CState_Player_Parry(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Parry"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Parry::Free()
{
    __super::Free();
}