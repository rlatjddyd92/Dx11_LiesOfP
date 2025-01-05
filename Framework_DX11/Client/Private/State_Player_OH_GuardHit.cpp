#include "stdafx.h"
#include "State_Player_OH_GuardHit.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_OH_GuardHit::CState_Player_OH_GuardHit(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OH_GuardHit::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_GuardHit_Normal = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_GuardHit_Normal", 2.5f);
    m_iAnimation_GuardHit_Strong = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_GuardHit_Strong", 2.5f);
    m_iAnimation_GuardHit_Break = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_GuardHit_Break", 2.5f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_OH_GuardHit::Start_State(void* pArg)
{
    _bool* pIsStrong = static_cast<_bool*>(pArg);
    m_isBreak = false;

    if (m_pPlayer->Get_Player_Stat().vGauge_Stamina.x <= 0)
    {
        m_isBreak = true;
        m_pPlayer->Change_Animation(m_iAnimation_GuardHit_Break, false, 0.f, 0, true, true);
    }
    else if (*pIsStrong)
    {
        m_pPlayer->Change_Animation(m_iAnimation_GuardHit_Strong, false, 0.f, 0, true, true);
    }
    else
    {
        m_pPlayer->Change_Animation(m_iAnimation_GuardHit_Normal, false, 0.f, 0, true, true);
    }

    m_pPlayer->Set_IsGuard(true, false);

    return S_OK;
}

void CState_Player_OH_GuardHit::Update(_float fTimeDelta)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();

    if (m_pPlayer->Get_EndAnim(iCurAnim))
    {
        if (m_isBreak)
        {
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        }
        else
        {
            m_pPlayer->Change_State(CPlayer::OH_GUARD);
        }
    }
}

void CState_Player_OH_GuardHit::End_State()
{
    if (m_isBreak)
        m_pPlayer->Set_IsGuard(false);
}

CState_Player_OH_GuardHit* CState_Player_OH_GuardHit::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OH_GuardHit* pInstance = new CState_Player_OH_GuardHit(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_OH_GuardHit"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OH_GuardHit::Free()
{
    __super::Free();
}