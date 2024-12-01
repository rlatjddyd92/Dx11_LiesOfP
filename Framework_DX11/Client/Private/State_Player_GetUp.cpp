#include "stdafx.h"
#include "State_Player_GetUp.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "TreasureBox.h"

CState_Player_GetUp::CState_Player_GetUp(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_GetUp::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_GetUp[GETUP_BACK] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Getup_B", 2.f);
    m_iAnimation_GetUp[GETUP_FRONT] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Getup_F", 2.f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_GetUp::Start_State(void* pArg)
{
    _int* iType = static_cast<_int*>(pArg);

    m_pPlayer->Change_Animation(m_iAnimation_GetUp[*iType], false, 0.05f);

    return S_OK;
}

void CState_Player_GetUp::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        _uint iWeponType = m_pPlayer->Get_WeaponType();

        if (iWeponType < 2)
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        else
            m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }
}

void CState_Player_GetUp::End_State()
{
    m_pPlayer->Get_RigidBody()->Set_IsOnCell(true);
}

_bool CState_Player_GetUp::End_Check()
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };

    if (m_iAnimation_GetUp[GETUP_FRONT] == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_GetUp[GETUP_FRONT]);
    }
    else if (m_iAnimation_GetUp[GETUP_BACK] == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_GetUp[GETUP_BACK]);
    }

    return bEndCheck;
}

CState_Player_GetUp* CState_Player_GetUp::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_GetUp* pInstance = new CState_Player_GetUp(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_GetUp"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_GetUp::Free()
{
    __super::Free();
}