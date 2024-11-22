#include "stdafx.h"
#include "State_Player_Flame_RAttack01.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Flame_RAttack01::CState_Player_Flame_RAttack01(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Flame_RAttack01::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_AseaultUp = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_FableArts_AssaultUp_FlameSword_End", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 100;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame = 30;
    m_iColliderEndFrame = 35;

    return S_OK;
}

HRESULT CState_Player_Flame_RAttack01::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_AseaultUp, false, 0.3f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_iColliderStartFrame = 30;
    m_iColliderEndFrame = 35;

    return S_OK;
}

void CState_Player_Flame_RAttack01::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame < m_iChangeFrame)
    {
        if (KEY_TAP(KEY::LBUTTON))
        {
            m_isInputLButton = true;
            m_isInputRButton = false;
        }
        else if (KEY_TAP(KEY::RBUTTON))
        {
            m_isInputRButton = true;
            m_isInputLButton = false;
            m_fRButtonTime = 0.f;
        }
        else if (KEY_HOLD(KEY::RBUTTON))
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
    else if (*m_pIsEndAnim)
    {
        m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }

    Control_Collider();
}

void CState_Player_Flame_RAttack01::End_State()
{
}

_bool CState_Player_Flame_RAttack01::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_AseaultUp);
}

void CState_Player_Flame_RAttack01::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iColliderStartFrame <= iFrame && iFrame <= m_iColliderEndFrame)
        m_pPlayer->Active_CurrentWeaponCollider();
    else
        m_pPlayer->DeActive_CurretnWeaponCollider();
}

CState_Player_Flame_RAttack01* CState_Player_Flame_RAttack01::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Flame_RAttack01* pInstance = new CState_Player_Flame_RAttack01(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Flame_RAttack01"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Flame_RAttack01::Free()
{
    __super::Free();
}