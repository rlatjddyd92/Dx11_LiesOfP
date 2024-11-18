#include "stdafx.h"
#include "State_Player_Hit.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Hit::CState_Player_Hit(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Hit::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Hit[HIT_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_B", 2.5f);
    m_iAnimation_Hit[HIT_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_L", 2.5f);
    m_iAnimation_Hit[HIT_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_R", 2.5f);
    m_iAnimation_Hit[HIT_FB] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_FtoB", 2.5f);
    m_iAnimation_Hit[HIT_LR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_LtoR", 2.5f);
    m_iAnimation_Hit[HIT_RL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_RtoL", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Hit::Start_State(void* pArg)
{
    _uint* iHitIndex = static_cast<_uint*>(pArg);

    m_pPlayer->Change_Animation(m_iAnimation_Hit[*iHitIndex], false);

    m_pPlayer->Set_IsGuard(true);

    return S_OK;
}

void CState_Player_Hit::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }
}

void CState_Player_Hit::End_State()
{
    m_pPlayer->Set_IsGuard(false);
}

_bool CState_Player_Hit::End_Check()
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((m_iAnimation_Hit[HIT_B]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_B]);
    }
    else if ((m_iAnimation_Hit[HIT_L]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_L]);
    }
    else if ((m_iAnimation_Hit[HIT_R]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_R]);
    }
    else if ((m_iAnimation_Hit[HIT_FB]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_FB]);
    }
    else if ((m_iAnimation_Hit[HIT_LR]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_LR]);
    }
    else if ((m_iAnimation_Hit[HIT_RL]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_RL]);
    }
    else
    {

    }
    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

CState_Player_Hit* CState_Player_Hit::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Hit* pInstance = new CState_Player_Hit(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Hit"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Hit::Free()
{
    __super::Free();
}