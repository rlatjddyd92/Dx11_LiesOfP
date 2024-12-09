#include "stdafx.h"
#include "State_Player_SophiaHand.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "TreasureBox.h"

CState_Player_SophiaHand::CState_Player_SophiaHand(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_SophiaHand::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_SophiaHand = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hand_SophiaDead_Cine", 1.f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_SophiaHand::Start_State(void* pArg)
{
    m_pPlayer->Get_Model()->ReadyDenyNextTranslate(4);
    m_pPlayer->Change_Animation(m_iAnimation_SophiaHand, false, 0.f);

    m_pPlayer->Get_RigidBody()->Set_GloblePose(_Vec3(15.955,-88.788,12.195));

    return S_OK;
}

void CState_Player_SophiaHand::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::SOPHIA_HANDEND);
    }
}

void CState_Player_SophiaHand::End_State()
{
}

_bool CState_Player_SophiaHand::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_SophiaHand);
}

CState_Player_SophiaHand* CState_Player_SophiaHand::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_SophiaHand* pInstance = new CState_Player_SophiaHand(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_SophiaHand"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_SophiaHand::Free()
{
    __super::Free();
}