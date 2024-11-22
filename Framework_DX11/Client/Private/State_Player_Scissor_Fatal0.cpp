#include "stdafx.h"
#include "State_Player_Scissor_Fatal0.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Scissor_Fatal0::CState_Player_Scissor_Fatal0(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Scissor_Fatal0::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ScissorCombos1 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_FableArts_Combo_ScissorSword1", 2.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 145;
    m_iComboFrame = 30;
    m_iSeperateFrame = 1;
    m_iCombineFrame = 87;

    m_iColliderStartFrame = 18;
    m_iColliderEndFrame = 22;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Scissor_Fatal0::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorCombos1, false, 0.1f);

    m_isInputLButton = false;
    m_isInputFButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    return S_OK;
}

void CState_Player_Scissor_Fatal0::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();
    _uint iCurrentAnim = m_pPlayer->Get_Model()->Get_CurrentAnimationIndex();

    if (iFrame < m_iChangeFrame)
    {
        if (KEY_TAP(KEY::LBUTTON))
        {
            m_isInputLButton = true;
            m_isInputRButton = false;
            m_isInputFButton = false;
        }
        else if (KEY_TAP(KEY::RBUTTON))
        {
            m_isInputRButton = true;
            m_isInputLButton = false;
            m_isInputFButton = false;
            m_fRButtonTime = 0.f;
        }
        else if (KEY_HOLD(KEY::RBUTTON))
        {
            m_fRButtonTime += fTimeDelta;
        }
        else if (KEY_TAP(KEY::F))
        {
            m_isInputFButton = true;
            m_isInputLButton = false;
            m_isInputRButton = false;
            m_fRButtonTime = 0.f;
        }
    }

    if (m_iSeperateFrame == iFrame)
    {
        m_pPlayer->Seperate_Scissor();
    }
    else if (m_iCombineFrame == iFrame)
    {
        m_pPlayer->Combine_Scissor();
    }

    if (!m_isInputFButton)
    {
        if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
        {
            if (m_isInputLButton)
                m_pPlayer->Change_State(CPlayer::SCISSOR_LATTACK0);
            else if (m_isInputRButton)
            {
                if (m_fRButtonTime > 0.15f)
                    m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE0);
                else
                    m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK0);
            }
        }
    }
    else if (m_isInputFButton)
    {
        if (m_iComboFrame < iFrame && iFrame < m_iComboFrame + 10)
        {
            m_pPlayer->Change_State(CPlayer::SCISSOR_FATAL1);
        }
    }

    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Collider();
}

void CState_Player_Scissor_Fatal0::End_State()
{
    m_pPlayer->DeActive_CurretnWeaponCollider();
}

_bool CState_Player_Scissor_Fatal0::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ScissorCombos1);
}

void CState_Player_Scissor_Fatal0::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iColliderStartFrame <= iFrame && iFrame <= m_iColliderEndFrame)
    {
        m_pPlayer->Active_CurrentWeaponCollider(3.f, 0);
        m_pPlayer->Active_CurrentWeaponCollider(3.f, 1);
    }
    else
    {
        m_pPlayer->DeActive_CurretnWeaponCollider(0);
        m_pPlayer->DeActive_CurretnWeaponCollider(1);
    }
}

CState_Player_Scissor_Fatal0* CState_Player_Scissor_Fatal0::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Scissor_Fatal0* pInstance = new CState_Player_Scissor_Fatal0(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Scissor_Fatal0"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Scissor_Fatal0::Free()
{
    __super::Free();
}