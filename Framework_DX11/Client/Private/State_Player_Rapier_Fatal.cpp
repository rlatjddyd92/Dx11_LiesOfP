#include "stdafx.h"
#include "State_Player_Rapier_Fatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Rapier_Fatal::CState_Player_Rapier_Fatal(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Rapier_Fatal::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_RapierFCA = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Rapier_FCA", 2.2f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 110;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame[0] = 5;
    m_iColliderEndFrame[0] = 10;
    m_iColliderStartFrame[1] = 16;
    m_iColliderEndFrame[1] = 23;
    m_iColliderStartFrame[2] = 36;
    m_iColliderEndFrame[2] = 41;
    m_iColliderStartFrame[3] = 45;
    m_iColliderEndFrame[3] = 48;
    m_iColliderStartFrame[4] = 78;
    m_iColliderEndFrame[4] = 90;

    return S_OK;
}

HRESULT CState_Player_Rapier_Fatal::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_RapierFCA, false);

    return S_OK;
}

void CState_Player_Rapier_Fatal::Update(_float fTimeDelta)
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

    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if (m_isInputLButton)
            m_pPlayer->Change_State(CPlayer::RAPIER_LATTACK0);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::RAPIER_CHARGE);
            else
                m_pPlayer->Change_State(CPlayer::RAPIER_RATTACK0);
        }
    }
    else if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Collider();
}

void CState_Player_Rapier_Fatal::End_State()
{
    m_pPlayer->DeActive_CurretnWeaponCollider();
}

_bool CState_Player_Rapier_Fatal::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_RapierFCA);
}

void CState_Player_Rapier_Fatal::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    _bool   isColliderActive = false;

    for (_uint i = 0; i < 5; ++i)
    {
        if (m_iColliderStartFrame[i] <= iFrame && iFrame <= m_iColliderEndFrame[i])
            isColliderActive = true;
    }

    if (isColliderActive)
        m_pPlayer->Active_CurrentWeaponCollider();
    else
        m_pPlayer->DeActive_CurretnWeaponCollider();

}

CState_Player_Rapier_Fatal* CState_Player_Rapier_Fatal::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Rapier_Fatal* pInstance = new CState_Player_Rapier_Fatal(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_Fatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Rapier_Fatal::Free()
{
    __super::Free();
}