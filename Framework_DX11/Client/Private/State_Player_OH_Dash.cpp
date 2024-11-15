#include "stdafx.h"
#include "State_Player_OH_Dash.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_OH_Dash::CState_Player_OH_Dash(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OH_Dash::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Dash[DASH_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_Normal_F", 2.f);
    m_iAnimation_Dash[DASH_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_Normal_B", 2.f);
    m_iAnimation_Dash[DASH_FOCUS_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_FocusHurt_F", 2.f);
    m_iAnimation_Dash[DASH_FOCUS_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_FocusHurt_B", 2.f);
    m_iAnimation_Dash[DASH_FOCUS_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_FocusHurt_L", 2.f);
    m_iAnimation_Dash[DASH_FOCUS_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_FocusHurt_R", 2.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;
    m_fMoveSpeed = 2.f;

    return S_OK;
}

HRESULT CState_Player_OH_Dash::Start_State(void* pArg)
{
    Select_DashAnimation();
    //m_pPlayer->Get_RigidBody()->Set_Friction(_float3(10.f, 0.f, 10.f));
    return S_OK;
}

void CState_Player_OH_Dash::Update(_float fTimeDelta)
{
    //if (*m_pIsEndAnim)
    //{
    //    m_pPlayer->Change_State(CPlayer::WALK);
    //}
}



void CState_Player_OH_Dash::End_State()
{
}

void CState_Player_OH_Dash::Select_DashAnimation()
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
        if (KEY_HOLD(KEY::W))
        {
            m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_F], false);
            return;
        }

        m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_B], false);
    }

}

CState_Player_OH_Dash* CState_Player_OH_Dash::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OH_Dash* pInstance = new CState_Player_OH_Dash(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_OH_Dash"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OH_Dash::Free()
{
    __super::Free();
}