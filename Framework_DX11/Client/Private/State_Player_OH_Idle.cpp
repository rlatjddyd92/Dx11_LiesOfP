#include "stdafx.h"
#include "State_Player_OH_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"

CState_Player_OH_Idle::CState_Player_OH_Idle(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OH_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Idle = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Idle");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_OH_Idle::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Idle, true, false);

    return S_OK;
}

void CState_Player_OH_Idle::Update(_float fTimeDelta)
{
    // ÀÌµ¿
    if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D) || KEY_HOLD(KEY::A))
    {
        if (KEY_HOLD(KEY::SPACE))
        {
            m_pPlayer->Change_State(CPlayer::WALK);
        }
        else
        {
            m_pPlayer->Change_State(CPlayer::WALK);
        }
        return;
    }

}

void CState_Player_OH_Idle::End_State()
{
}

CState_Player_OH_Idle* CState_Player_OH_Idle::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OH_Idle* pInstance = new CState_Player_OH_Idle(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_OH_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OH_Idle::Free()
{
    __super::Free();
}