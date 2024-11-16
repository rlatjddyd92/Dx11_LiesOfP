#include "stdafx.h"
#include "State_Player_TH_Dash.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_TH_Dash::CState_Player_TH_Dash(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_TH_Dash::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Dash[DASH_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Dash_Normal_F", 3.5f);
    m_iAnimation_Dash[DASH_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Dash_Normal_B", 3.5f);
    m_iAnimation_Dash[DASH_FOCUS_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Dash_FocusHurt_F", 3.5f);
    m_iAnimation_Dash[DASH_FOCUS_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Dash_FocusHurt_B", 3.5f);
    m_iAnimation_Dash[DASH_FOCUS_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Dash_FocusHurt_L", 3.5f);
    m_iAnimation_Dash[DASH_FOCUS_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Dash_FocusHurt_R", 3.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_TH_Dash::Start_State(void* pArg)
{
    Select_DashAnimation();

    m_pPlayer->Set_IsInvicible(true);
    m_pPlayer->Set_MoveSpeed(5.f);

    //m_pPlayer->Get_RigidBody()->Set_Friction(_float3(10.f, 0.f, 10.f));
    *m_pResetRootMove = true;

    return S_OK;
}

void CState_Player_TH_Dash::Update(_float fTimeDelta)
{
    if (*m_pIsEndAnim)
    {
        m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }
}



void CState_Player_TH_Dash::End_State()
{
    m_pPlayer->Set_IsInvicible(false);
}

void CState_Player_TH_Dash::Select_DashAnimation()
{
    if (m_pPlayer->Get_IsLockOn())
    {
        if (KEY_HOLD(KEY::W))
        {
            m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_FOCUS_F], false);
        }
        else if (KEY_HOLD(KEY::S))
        {
            m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_FOCUS_B], false);
        }
        else if (KEY_HOLD(KEY::A))
        {
            m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_FOCUS_L], false);
        }
        else if (KEY_HOLD(KEY::D))
        {
            m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_FOCUS_R], false);
        }
    }
    else
    {
        if (m_pFsm->Get_PrevState() == CPlayer::OH_WALK)
        {
            m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_F], false);
            return;
        }

        m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_B], false);
    }

}

CState_Player_TH_Dash* CState_Player_TH_Dash::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_TH_Dash* pInstance = new CState_Player_TH_Dash(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_TH_Dash"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_TH_Dash::Free()
{
    __super::Free();
}