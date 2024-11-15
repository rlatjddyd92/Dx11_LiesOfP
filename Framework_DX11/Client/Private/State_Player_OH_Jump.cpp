#include "stdafx.h"
#include "State_Player_OH_Jump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"

CState_Player_OH_Jump::CState_Player_OH_Jump(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OH_Jump::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_JumpUp = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Jump_Up");
    m_iAnimation_JumpFall = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Jump_Fall_Loop");
    m_iAnimation_JumpLand = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Jump_Land_Light");

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_OH_Jump::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_JumpUp, true, 0.1f);

    return S_OK;
}

void CState_Player_OH_Jump::Update(_float fTimeDelta)
{
    if (KEY_HOLD(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CPlayer::OH_GUARD);
    }
    else if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D) || KEY_HOLD(KEY::A))
    {
        if (KEY_HOLD(KEY::SPACE))
        {
            m_pPlayer->Change_State(CPlayer::OH_RUN);
        }
        else
        {
            m_pPlayer->Change_State(CPlayer::OH_WALK);
        }
        return;
    }

}

void CState_Player_OH_Jump::End_State()
{
}

CState_Player_OH_Jump* CState_Player_OH_Jump::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OH_Jump* pInstance = new CState_Player_OH_Jump(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_OH_Jump"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OH_Jump::Free()
{
    __super::Free();
}