#include "stdafx.h"
#include "State_Player_Flame_ParryAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Flame_ParryAttack::CState_Player_Flame_ParryAttack(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Flame_ParryAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ParryAttack = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_FableArts_AssaultUp_FlameSword_End", 3.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 100;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame = 30;
    m_iColliderEndFrame = 35;

    m_iSwishL_SoundFrame = 25;

    return S_OK;
}

HRESULT CState_Player_Flame_ParryAttack::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ParryAttack, false, 0.f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_isSwishL_PlaySound = false;

    return S_OK;
}

void CState_Player_Flame_ParryAttack::Update(_float fTimeDelta)
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
        if (m_isInputLButton)
            m_pPlayer->Change_State(CPlayer::FLAME_LATTACK1);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::FLAME_CHARGE0);
            else
                m_pPlayer->Change_State(CPlayer::FLAME_RATTACK0);
        }
    }
    else if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }

    if (iFrame >= m_iSwishL_SoundFrame && !m_isSwishL_PlaySound)
    {
        m_isSwishL_PlaySound = true;
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_FlameSword_2H_L_01.wav"));
    }

    Control_Collider();
}

void CState_Player_Flame_ParryAttack::End_State()
{
}

_bool CState_Player_Flame_ParryAttack::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ParryAttack);
}

void CState_Player_Flame_ParryAttack::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iColliderStartFrame <= iFrame && iFrame <= m_iColliderEndFrame)
        m_pPlayer->Active_CurrentWeaponCollider();
    else
        m_pPlayer->DeActive_CurretnWeaponCollider();
}

CState_Player_Flame_ParryAttack* CState_Player_Flame_ParryAttack::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Flame_ParryAttack* pInstance = new CState_Player_Flame_ParryAttack(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Flame_ParryAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Flame_ParryAttack::Free()
{
    __super::Free();
}