#include "stdafx.h"
#include "State_Player_Teleport.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "SteppingStone.h"

CState_Player_Teleport::CState_Player_Teleport(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Teleport::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_TeleportStart = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Teleport_Start", 2.5f);
    m_iAnimation_TeleportEnd = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Teleport_End", 2.5f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Teleport::Start_State(void* pArg)
{
    m_pSteppingStone = static_cast<CSteppingStone*>(pArg);

    m_pPlayer->Change_Animation(m_iAnimation_TeleportStart, false, 0.f);


    m_isEnd_Teleport = false;

    return S_OK;
}

void CState_Player_Teleport::Update(_float fTimeDelta)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _int iFrame = m_pPlayer->Get_Frame();

    if (iCurAnim == m_iAnimation_TeleportStart)
    {
        if (iFrame >= 150)
        {
            m_pSteppingStone->Change_Player_Pos();
            m_isEnd_Teleport = true;
        }
        else if (m_isEnd_Teleport)
        {
            m_isEnd_Teleport = false;
            m_pPlayer->Change_Animation(m_iAnimation_TeleportEnd, false, 0.3f);
        }
        else if (iFrame > 80 && !m_isEnd_Teleport)
        {
            m_pPlayer->Set_IsTeleport(true);
        }
    }
    else if (iCurAnim == m_iAnimation_TeleportEnd)
    {
        if (iFrame > 10)
        {
            m_pPlayer->Set_IsTeleport(false);
        }
        if (End_Check())
        {
            _uint iWeponType = m_pPlayer->Get_WeaponType();

            if (iWeponType < 2)
                m_pPlayer->Change_State(CPlayer::OH_IDLE);
            else
                m_pPlayer->Change_State(CPlayer::TH_IDLE);
        }
    }
}

void CState_Player_Teleport::End_State()
{
    m_pSteppingStone = nullptr;
}

_bool CState_Player_Teleport::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_TeleportEnd);
}

CState_Player_Teleport* CState_Player_Teleport::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Teleport* pInstance = new CState_Player_Teleport(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Teleport"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Teleport::Free()
{
    __super::Free();
}