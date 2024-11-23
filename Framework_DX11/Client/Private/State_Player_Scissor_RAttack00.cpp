#include "stdafx.h"
#include "State_Player_Scissor_RAttack00.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Scissor_RAttack00::CState_Player_Scissor_RAttack00(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Scissor_RAttack00::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ScissorSA1 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_ScissorSword_SA1", 1.7f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 55;
    m_iSeperateFrame = 10;
    m_iCombineFrame = 43;

    m_iStateNum = iStateNum;

    m_iColliderStartFrameLeft = 15;
    m_iColliderEndFrameLeft = 20;

    m_iColliderStartFrameRight = 30;
    m_iColliderEndFrameRight = 35;

    return S_OK;
}

HRESULT CState_Player_Scissor_RAttack00::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorSA1, false, 0.3f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    return S_OK;
}

void CState_Player_Scissor_RAttack00::Update(_float fTimeDelta)
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

    if (m_iSeperateFrame == iFrame)
    {
        m_pPlayer->Seperate_Scissor();
    }
    else if (m_iCombineFrame == iFrame)
    {
        m_pPlayer->Combine_Scissor();
    }
    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if (m_isInputLButton)
            m_pPlayer->Change_State(CPlayer::SCISSOR_LATTACK1);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE1);
            else
                m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK1);
        }
    }
    else if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Collider();
}

void CState_Player_Scissor_RAttack00::End_State()
{
}

_bool CState_Player_Scissor_RAttack00::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ScissorSA1);
}

void CState_Player_Scissor_RAttack00::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iCombineFrame >= iFrame)
    {
            if (m_iColliderStartFrameLeft <= iFrame && iFrame <= m_iColliderEndFrameLeft)
            {
                m_pPlayer->Active_CurrentWeaponCollider(3.f, 1);
            }
            else
            {
                m_pPlayer->DeActive_CurretnWeaponCollider(1);
            }

            if (m_iColliderStartFrameRight <= iFrame && iFrame <= m_iColliderEndFrameRight)
            {
                m_pPlayer->Active_CurrentWeaponCollider(3.f, 0);
            }
            else
            {
                m_pPlayer->DeActive_CurretnWeaponCollider(0);
            }
    }
}

CState_Player_Scissor_RAttack00* CState_Player_Scissor_RAttack00::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Scissor_RAttack00* pInstance = new CState_Player_Scissor_RAttack00(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Scissor_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Scissor_RAttack00::Free()
{
    __super::Free();
}