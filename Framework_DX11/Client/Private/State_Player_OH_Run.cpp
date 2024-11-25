#include "stdafx.h"
#include "State_Player_OH_Run.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_OH_Run::CState_Player_OH_Run(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OH_Run::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Run[RUN_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Run_B", 2.5f);
    m_iAnimation_Run[RUN_BL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Run_BL", 2.5f);
    m_iAnimation_Run[RUN_BR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Run_BR", 2.5f);
    m_iAnimation_Run[RUN_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Run_F", 2.5f);
    m_iAnimation_Run[RUN_FL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Run_FL", 2.5f);
    m_iAnimation_Run[RUN_FR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Run_FR", 2.5f);
    m_iAnimation_Run[RUN_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Run_L", 2.5f);
    m_iAnimation_Run[RUN_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Run_R", 2.5f);

    m_iStateNum = iStateNum;

    m_iFootStepFrame[0] = 17;
    m_iFootStepFrame[1] = 40;

    return S_OK;
}

HRESULT CState_Player_OH_Run::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Run[RUN_F], true, 0.1f);

    m_pPlayer->Set_MoveSpeed(3.f);

    m_isInputSpace = false;
    m_fSpaceTime = 0.f;

    m_isPlaySound = false;

    return S_OK;
}

void CState_Player_OH_Run::Update(_float fTimeDelta)
{
    if (m_isInputSpace)
    {
        m_fSpaceTime += fTimeDelta;
    }

    if (false == Move(fTimeDelta))
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
        return;
    }

    if (m_fSpaceTime > 0.15f)
    {
        if (KEY_NONE(KEY::SPACE))
        {
            m_pPlayer->Change_State(CPlayer::OH_DASH);
        }

        if (KEY_HOLD(KEY::SPACE))
        {
            m_pPlayer->Change_State(CPlayer::OH_SPRINT);
        }
    }
    else if (KEY_HOLD(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CPlayer::OH_GUARD);
    }
    else if (KEY_TAP(KEY::R))
    {
        m_pPlayer->Change_State(CPlayer::HEAL);
    }

    if (KEY_TAP(KEY::SPACE))
        m_isInputSpace = true;

    Control_Sound();
}

void CState_Player_OH_Run::End_State()
{
}

_bool CState_Player_OH_Run::Move(_float fTimeDelta)
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
        m_pPlayer->Move_Dir(m_vMoveDir, fTimeDelta);
    }

    return isMoving;
}

void CState_Player_OH_Run::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame == m_iFootStepFrame[0] && !m_isPlaySound)
    {
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Run_01.wav"));
        m_isPlaySound = true;
    }
    else if (iFrame == m_iFootStepFrame[1] && !m_isPlaySound)
    {
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Run_02.wav"));
        m_isPlaySound = true;
    }
    else
    {
        m_isPlaySound = false;
    }

}

CState_Player_OH_Run* CState_Player_OH_Run::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OH_Run* pInstance = new CState_Player_OH_Run(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_OH_Run"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OH_Run::Free()
{
    __super::Free();
}