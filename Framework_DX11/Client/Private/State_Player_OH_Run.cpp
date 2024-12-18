#include "stdafx.h"
#include "State_Player_OH_Run.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

#include "GameInterface_Controller.h"

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

    m_pPlayer->Set_MoveSpeed(3.5f);

    m_isInputSpace = false;
    m_fSpaceTime = 0.f;

    m_isTurnOver = false;

    m_vFirstMoveDir = _Vec4(0.f, 0.f, 0.f, 0.f);


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
        if (m_isTurnOver)
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        return;
    }

    if (m_fSpaceTime > 0.15f)
    {
        if (m_pPlayer->Key_None(KEY::SPACE))
        {
            if (m_isTurnOver)
                m_pPlayer->Change_State(CPlayer::OH_DASH);
        }

        if (m_pPlayer->Key_Hold(KEY::SPACE))
        {
            if (m_isTurnOver)
                m_pPlayer->Change_State(CPlayer::OH_SPRINT);
        }
    }
    else if (m_pPlayer->Key_Hold(KEY::LSHIFT))
    {
        if (m_isTurnOver)
            m_pPlayer->Change_State(CPlayer::OH_GUARD);
    }
    else if (m_pPlayer->Key_Tab(KEY::R))
    {
        if (m_isTurnOver)
        {
            SPECIAL_ITEM eNow = GET_GAMEINTERFACE->Get_Now_Select_Item();
            if (SPECIAL_ITEM::SP_PULSE_BATTERY == eNow)
            {
                m_pPlayer->Change_State(CPlayer::HEAL);
            }
            else if (SPECIAL_ITEM::SP_GRINDER == eNow)
            {
                m_pPlayer->Change_State(CPlayer::GRINDER);
            }
        }
    }
    else if (m_pPlayer->Key_Tab(KEY::TAPKEY))
    {
        m_pPlayer->Change_State(CPlayer::CHANGEWEP);
    }
    else if (m_pPlayer->Key_Hold(KEY::CTRL))
    {
        m_pPlayer->Change_State(CPlayer::ARM_START);
    }

    if (m_pPlayer->Key_Tab(KEY::SPACE))
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

    if (m_pPlayer->Key_Hold(KEY::W) || m_pPlayer->Key_Tab(KEY::W) || m_pPlayer->Key_Away(KEY::W))
    {
        m_vMoveDir += vCameraLook;
        isMoving = true;
    }
    else if (m_pPlayer->Key_Hold(KEY::S) || m_pPlayer->Key_Tab(KEY::S) || m_pPlayer->Key_Away(KEY::S))
    {
        m_vMoveDir -= vCameraLook;
        isMoving = true;
    }

    if (m_pPlayer->Key_Hold(KEY::D) || m_pPlayer->Key_Tab(KEY::D) || m_pPlayer->Key_Away(KEY::D))
    {
        m_vMoveDir += vCameraRight;

        isMoving = true;
    }

    if (m_pPlayer->Key_Hold(KEY::A) || m_pPlayer->Key_Tab(KEY::A) || m_pPlayer->Key_Away(KEY::A))
    {
        m_vMoveDir -= vCameraRight;

        isMoving = true;
    }

    m_vMoveDir.Normalize();

    if (m_vFirstMoveDir.Length() == 0.f && isMoving)
    {
        m_vFirstMoveDir = m_vMoveDir;
    }

    _Vec4 vLocalMoveDir = { m_vMoveDir.Dot(vCameraRight), 0.f, m_vMoveDir.Dot(vCameraLook), 0.f };
    vLocalMoveDir.Normalize();

    if(!m_isTurnOver)
        m_isTurnOver = m_pPlayer->Turn_Lerp(m_vFirstMoveDir, fTimeDelta);

    if (m_vMoveDir.Length() > 0.f && m_isTurnOver)
    {
        m_pPlayer->Move_Dir(m_vMoveDir, fTimeDelta, m_isTurnOver);
        m_pPlayer->Change_Animation(m_iAnimation_Run[RUN_F], true, 0.2f);
    }

    return isMoving;
}

void CState_Player_OH_Run::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if ((iFrame == m_iFootStepFrame[0] || iFrame == m_iFootStepFrame[0] + 1) && !m_isPlaySound[0])
    {
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Run_01.wav"));
        m_isPlaySound[0] = true;
    }
    else if ((iFrame == m_iFootStepFrame[1] || iFrame == m_iFootStepFrame[1] + 1) && !m_isPlaySound[1])
    {
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Run_02.wav"));
        m_isPlaySound[1] = true;
    }
    else if (iFrame < 5)
    {
        m_isPlaySound[0] = false;
        m_isPlaySound[1] = false;
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