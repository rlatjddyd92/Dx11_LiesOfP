#include "stdafx.h"
#include "State_Player_TH_Sprint.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_TH_Sprint::CState_Player_TH_Sprint(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_TH_Sprint::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Sprint = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Sprint", 2.5f);
    m_iAnimation_SprintEnd = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Sprint_Stop", 3.5f);

    m_iStateNum = iStateNum;

    m_iFootStepFrame[0] = 12;
    m_iFootStepFrame[1] = 27;

    return S_OK;
}

HRESULT CState_Player_TH_Sprint::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Sprint, true, 0.05f);

    m_pPlayer->Set_MoveSpeed(4.5f);

    m_isSprintEnd = false;
    m_fSprintEndTime = 0.f;

    return S_OK;
}

void CState_Player_TH_Sprint::Update(_float fTimeDelta)
{
    if (m_isSprintEnd)
        m_fSprintEndTime += fTimeDelta;

    m_pPlayer->Decrease_Stamina(2.f);

    if (false == Move(fTimeDelta) && m_fSprintEndTime > 0.1f)
    {
        m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }
    else if (m_fSprintEndTime > 0.1f || m_pPlayer->Get_Player_Stat().vGauge_Stamina.x <= 0.f)
    {
        m_pPlayer->Change_State(CPlayer::TH_RUN);
    }
    /* else if (KEY_TAP(KEY::SPACE))
     {
         m_pPlayer->Change_State(CPlayer::JUMP);
     }*/

    if (!m_isSprintEnd && KEY_NONE(KEY::SPACE))
    {
        m_isSprintEnd = true;
    }

    Control_Sound();
}

void CState_Player_TH_Sprint::End_State()
{
}

_bool CState_Player_TH_Sprint::Move(_float fTimeDelta)
{
    _bool isMoving = false;
    m_vMoveDir = _Vec4(0.f, 0.f, 0.f, 0.f);

    _Vec4 vCameraLook = m_pPlayer->Get_Camera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    _Vec4 vCameraRight = m_pPlayer->Get_Camera()->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
    vCameraLook.y = 0.f;
    vCameraRight.y = 0.f;

    vCameraLook.Normalize();
    vCameraRight.Normalize();

    if (m_pPlayer->Key_Hold(KEY::W))
    {
        m_vMoveDir += vCameraLook;
        isMoving = true;
    }
    else if (m_pPlayer->Key_Hold(KEY::S))
    {
        m_vMoveDir -= vCameraLook;
        isMoving = true;
    }

    if (m_pPlayer->Key_Hold(KEY::D))
    {
        m_vMoveDir += vCameraRight;

        isMoving = true;
    }

    if (m_pPlayer->Key_Hold(KEY::A))
    {
        m_vMoveDir -= vCameraRight;

        isMoving = true;
    }

    m_vMoveDir.Normalize();

    _Vec4 vLocalMoveDir = { m_vMoveDir.Dot(vCameraRight), 0.f, m_vMoveDir.Dot(vCameraLook), 0.f };
    vLocalMoveDir.Normalize();

    if (m_vMoveDir.Length() > 0.f)
    {
        m_pPlayer->Move_Dir(m_vMoveDir, fTimeDelta);
    }

    return isMoving;
}

void CState_Player_TH_Sprint::Control_Sound()
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

CState_Player_TH_Sprint* CState_Player_TH_Sprint::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_TH_Sprint* pInstance = new CState_Player_TH_Sprint(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_TH_Sprint"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_TH_Sprint::Free()
{
    __super::Free();
}