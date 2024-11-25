#include "stdafx.h"
#include "State_Player_Rapier_Charge.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Rapier_Charge::CState_Player_Rapier_Charge(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Rapier_Charge::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_RapierCA1 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Rapier_CA1", 2.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 140;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame[0] = 60;
    m_iColliderEndFrame[0] = 65;
    m_iColliderStartFrame[1] = 70;
    m_iColliderEndFrame[1] = 74;
    m_iColliderStartFrame[2] = 105;
    m_iColliderEndFrame[2] = 120;

    return S_OK;
}

HRESULT CState_Player_Rapier_Charge::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_RapierCA1, false);

    return S_OK;
}

void CState_Player_Rapier_Charge::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if (KEY_TAP(KEY::LBUTTON))
            m_pPlayer->Change_State(CPlayer::RAPIER_LATTACK0);
        else if (KEY_TAP(KEY::RBUTTON))
            m_pPlayer->Change_State(CPlayer::RAPIER_RATTACK0);
    }
    else if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Collider();
}

void CState_Player_Rapier_Charge::End_State()
{
}

_bool CState_Player_Rapier_Charge::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_RapierCA1);
}

void CState_Player_Rapier_Charge::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    _bool   isColliderActive = false;

    for (_uint i = 0; i < 3; ++i)
    {
        if (m_iColliderStartFrame[i] <= iFrame && iFrame <= m_iColliderEndFrame[i])
            isColliderActive = true;
    }

    if (isColliderActive)
        m_pPlayer->Active_CurrentWeaponCollider();
    else
        m_pPlayer->DeActive_CurretnWeaponCollider();
}

CState_Player_Rapier_Charge* CState_Player_Rapier_Charge::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Rapier_Charge* pInstance = new CState_Player_Rapier_Charge(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Rapier_Charge::Free()
{
    __super::Free();
}