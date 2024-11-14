#include "stdafx.h"
#include "State_Player_OH_Guard.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_OH_Guard::CState_Player_OH_Guard(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OH_Guard::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Guard[GUARD_N] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Guard_Idle", 2.5f);
    m_iAnimation_Guard[GUARD_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Guard_Walk_B", 2.5f);
    m_iAnimation_Guard[GUARD_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Guard_Walk_F", 2.5f);
    m_iAnimation_Guard[GUARD_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Guard_Walk_L", 2.5f);
    m_iAnimation_Guard[GUARD_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Guard_Walk_R", 2.5f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_OH_Guard::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Guard[GUARD_N], true, false);

    return S_OK;
}

void CState_Player_OH_Guard::Update(_float fTimeDelta)
{
    if (KEY_NONE(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CPlayer::IDLE);
    }
}

void CState_Player_OH_Guard::End_State()
{
}

_bool CState_Player_OH_Guard::Move(_float fTimeDelta)
{
    _bool isMoving = false;
    m_vMoveDir = _Vec4(0.f, 0.f, 0.f, 0.f);

    _Vec4 vCameraLook = m_pPlayer->Get_Camera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    _Vec4 vCameraRight = m_pPlayer->Get_Camera()->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
    vCameraLook.y = 0.f;
    vCameraRight.y = 0.f;

    vCameraLook.Normalize();
    vCameraRight.Normalize();

    if (KEY_HOLD(KEY::W))
    {
        m_vMoveDir += vCameraLook;
        isMoving = true;
    }
    else if (KEY_HOLD(KEY::S))
    {
        m_vMoveDir -= vCameraLook;
        isMoving = true;
    }

    if (KEY_HOLD(KEY::D))
    {
        m_vMoveDir += vCameraRight;

        isMoving = true;
    }

    if (KEY_HOLD(KEY::A))
    {
        m_vMoveDir -= vCameraRight;

        isMoving = true;
    }

    m_vMoveDir.Normalize();

    _Vec4 vLocalMoveDir = { m_vMoveDir.Dot(vCameraRight), 0.f, m_vMoveDir.Dot(vCameraLook), 0.f };
    vLocalMoveDir.Normalize();

    //// 방향을 기반으로 애니메이션 선정
    //if (vLocalMoveDir.z > 0.5f)
    //{
    //    if (vLocalMoveDir.x > 0.5f)
    //        m_pPlayer->Change_Animation(m_iAnimation_RUN[RUN_FR], true, false);
    //    else if (vLocalMoveDir.x < 0)
    //        m_pPlayer->Change_Animation(m_iAnimation_RUN[RUN_FL], true, false);
    //    else
    //        m_pPlayer->Change_Animation(m_iAnimation_RUN[RUN_F], true, false);
    //}
    //else if (vLocalMoveDir.z < 0.5f)
    //{
    //    if (vLocalMoveDir.x > 0.5f)
    //        m_pPlayer->Change_Animation(m_iAnimation_RUN[RUN_BR], true, false);
    //    else if (vLocalMoveDir.x < 0)
    //        m_pPlayer->Change_Animation(m_iAnimation_RUN[RUN_BL], true, false);
    //    else
    //        m_pPlayer->Change_Animation(m_iAnimation_RUN[RUN_B], true, false);
    //}
    //else
    //{
    //    if (vLocalMoveDir.x > 0.5f)
    //        m_pPlayer->Change_Animation(m_iAnimation_RUN[RUN_R], true, false);
    //    else if (vLocalMoveDir.x < 0.5f)
    //        m_pPlayer->Change_Animation(m_iAnimation_RUN[RUN_L], true, false);
    //}


    if (m_vMoveDir.Length() > 0.f)
    {
        m_pPlayer->Move_Dir(m_vMoveDir, m_fMoveSpeed, fTimeDelta);
    }

    return isMoving;
}

CState_Player_OH_Guard* CState_Player_OH_Guard::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OH_Guard* pInstance = new CState_Player_OH_Guard(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_OH_Guard"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OH_Guard::Free()
{
    __super::Free();
}