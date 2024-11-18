#include "stdafx.h"
#include "State_Player_Scissor_Fatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Scissor_Fatal::CState_Player_Scissor_Fatal(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Scissor_Fatal::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ScissorCombos[COMBO1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_FableArts_Combo_ScissorSword1", 2.f);
    m_iAnimation_ScissorCombos[COMBO2] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_FableArts_Combo_ScissorSword2", 2.f);
    m_iAnimation_ScissorCombos[COMBO3] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_FableArts_Combo_ScissorSword3", 2.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame[COMBO1] = 145;
    m_iChangeFrame[COMBO2] = 65;
    m_iChangeFrame[COMBO3] = 175;

    m_iComboFrame[COMBO1] = 30;
    m_iComboFrame[COMBO2] = 30;

    m_iSeperateFrame[COMBO1] = 1;
    m_iSeperateFrame[COMBO2] = 0;
    m_iSeperateFrame[COMBO3] = 0;

    m_iCombineFrame[COMBO1] = 87;
    m_iCombineFrame[COMBO2] = 81;
    m_iCombineFrame[COMBO3] = 50;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Scissor_Fatal::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorCombos[COMBO1], false, 0.1f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_iComboIndex = 0;
    m_isChangeAnim[0] = false;
    m_isChangeAnim[1] = false;
    m_isChangeAnim[2] = false;

    return S_OK;
}

void CState_Player_Scissor_Fatal::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();
    _uint iCurrentAnim = m_pPlayer->Get_Model()->Get_CurrentAnimationIndex();

    if (iFrame < m_iChangeFrame[m_iComboIndex])
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
        else if (KEY_TAP(KEY::F) && !m_isChangeAnim[m_iComboIndex])
        {
            m_isInputFButton = true;
            m_isInputLButton = false;
            m_isInputRButton = false;
            m_fRButtonTime = 0.f;
        }
    }

    if (m_iSeperateFrame[m_iComboIndex] == iFrame)
    {
        m_pPlayer->Seperate_Scissor();
    }
    else if (m_iCombineFrame[m_iComboIndex] == iFrame)
    {
        m_pPlayer->Combine_Scissor();
    }

    if (!m_isInputFButton)
    {
        if (m_iChangeFrame[m_iComboIndex] < iFrame && iFrame < m_iChangeFrame[m_iComboIndex] + 15)
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
        if (m_iComboFrame[m_iComboIndex] < iFrame && iFrame < m_iComboFrame[m_iComboIndex] + 10 )
        {
            if (!m_isChangeAnim[m_iComboIndex] && iCurrentAnim == m_iAnimation_ScissorCombos[m_iComboIndex])
            {
                m_isChangeAnim[m_iComboIndex] = true;
                m_isInputFButton = false;
                m_pPlayer->Change_Animation(m_iAnimation_ScissorCombos[++m_iComboIndex], false, 0.1f);
            }
        }
    }
    
    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }
}

void CState_Player_Scissor_Fatal::End_State()
{
    m_pPlayer->Combine_Scissor();
}

_bool CState_Player_Scissor_Fatal::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ScissorCombos[m_iComboIndex]);
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