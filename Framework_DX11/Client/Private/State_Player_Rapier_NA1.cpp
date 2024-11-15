#include "stdafx.h"
#include "State_Player_Rapier_NA1.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Rapier_NA1::CState_Player_Rapier_NA1(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Rapier_NA1::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_RapierNA1 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Rapier_NA1", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 30;
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Rapier_NA1::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_RapierNA1, false);

    return S_OK;
}

void CState_Player_Rapier_NA1::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();
    
    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if(KEY_TAP(KEY::LBUTTON))
            m_pPlayer->Change_State(CPlayer::RAPIER_NA2);
        else if (KEY_TAP(KEY::RBUTTON))
            m_pPlayer->Change_State(CPlayer::RAPIER_SA1);
    }
    else if (*m_pIsEndAnim)
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }
}

void CState_Player_Rapier_NA1::End_State()
{
}

CState_Player_Rapier_NA1* CState_Player_Rapier_NA1::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Rapier_NA1* pInstance = new CState_Player_Rapier_NA1(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_SA1"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Rapier_NA1::Free()
{
    __super::Free();
}