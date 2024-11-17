#include "stdafx.h"
#include "State_Player_Parry.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Parry::CState_Player_Parry(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Parry::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Parry = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_P_Parry_Guard", 2.7f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Parry::Start_State(void* pArg)
{
    m_iAnimation_Parry = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_P_Parry_Guard", 4.5f);
    m_pPlayer->Change_Animation(m_iAnimation_Parry, false);


    return S_OK;
}

void CState_Player_Parry::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (30 <= iFrame && iFrame < 40)
    {
        m_pPlayer->Set_IsParry(false);
    }
    else if (*m_pIsEndAnim)
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }
}

void CState_Player_Parry::End_State()
{
    m_pPlayer->Set_IsParry(false);
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