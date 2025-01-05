#include "stdafx.h"
#include "State_Player_OH_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_OH_Walk::CState_Player_OH_Walk(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OH_Walk::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Walk[WALK_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_B" , 2.5f);
    m_iAnimation_Walk[WALK_BL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_BL", 2.5f);
    m_iAnimation_Walk[WALK_BR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_BR", 2.5f);
    m_iAnimation_Walk[WALK_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_F", 2.5f);
    m_iAnimation_Walk[WALK_FL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_FL", 2.5f);
    m_iAnimation_Walk[WALK_FR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_FR", 2.5f);
    m_iAnimation_Walk[WALK_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_L", 2.5f);
    m_iAnimation_Walk[WALK_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_R", 2.5f);

    m_isInputSpace = false;
    m_fSpaceTime = 0.f;

    m_iStateNum = iStateNum;

    m_iFootStepFrame[WALK_B][0] = 28;
    m_iFootStepFrame[WALK_B][1] = 64;
    m_iFootStepFrame[WALK_BL][0] = 24;
    m_iFootStepFrame[WALK_BL][1] = 56;
    m_iFootStepFrame[WALK_BR][0] = 26;
    m_iFootStepFrame[WALK_BR][1] = 61;
    m_iFootStepFrame[WALK_F][0] = 3;
    m_iFootStepFrame[WALK_F][1] = 37;
    m_iFootStepFrame[WALK_FL][0] = 2; // 70 씩 더하기
    m_iFootStepFrame[WALK_FL][1] = 34; // 보류
    m_iFootStepFrame[WALK_FR][0] = 6;
    m_iFootStepFrame[WALK_FR][1] = 37;
    m_iFootStepFrame[WALK_L][0] = 9;
    m_iFootStepFrame[WALK_L][1] = 48;
    m_iFootStepFrame[WALK_R][0] = 12;
    m_iFootStepFrame[WALK_R][1] = 51;

    return S_OK;
}

HRESULT CState_Player_OH_Walk::Start_State(void* pArg)
{
    //m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_F], true);

    m_pPlayer->Set_MoveSpeed(2.f);

    m_fSpaceTime = 0.f;
    m_isInputSpace = false;

    return S_OK;
}

void CState_Player_OH_Walk::Update(_float fTimeDelta)
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
        if (m_pPlayer->Key_None(KEY::SPACE))
        {
            m_pPlayer->Change_State(CPlayer::OH_DASH);
        }

        if (m_pPlayer->Key_Hold(KEY::SPACE) && m_pPlayer->Get_Player_Stat().vGauge_Stamina.x > 10.f)
        {
            m_pPlayer->Change_State(CPlayer::OH_SPRINT);
        }
    }
    else if (m_pPlayer->Key_Hold(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CPlayer::OH_GUARD);
    }

    if (m_pPlayer->Key_Tab(KEY::SPACE))
        m_isInputSpace = true;

    Control_Sound();
}



void CState_Player_OH_Walk::End_State()
{
}

_bool CState_Player_OH_Walk::Move(_float fTimeDelta)
{
    // 움직임은 카메라 방향 그대로 
   // 애니메이션은 플레이어 기준
    _bool isMoving = false;
    m_vMoveDir = _Vec4(0.f, 0.f, 0.f, 0.f);

    _Vec4 vCameraLook = m_pPlayer->Get_Camera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    _Vec4 vCameraRight = m_pPlayer->Get_Camera()->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
    vCameraLook.y = 0.f;
    vCameraRight.y = 0.f;

    vCameraLook.Normalize();
    vCameraRight.Normalize();

    _bool isForward = m_pPlayer->Key_Hold(KEY::W);
    _bool isBackward = m_pPlayer->Key_Hold(KEY::S);
    _bool isRight = m_pPlayer->Key_Hold(KEY::D);
    _bool isLeft = m_pPlayer->Key_Hold(KEY::A);

    if (isForward)
    {
        m_vMoveDir += vCameraLook;
    }

    if (isBackward)
    {
        m_vMoveDir -= vCameraLook;
    }

    if (isRight)
    {
        m_vMoveDir += vCameraRight;
    }

    if (isLeft)
    {
        m_vMoveDir -= vCameraRight;
    }
    _Vec4 vRight = _Vec4(1.f, 0.f, 0.f, 0.f);

    _float fForwardDot = m_vMoveDir.Dot(vCameraLook);
    _float fBackwardDot = m_vMoveDir.Dot(-vCameraLook);

    m_vMoveDir.Normalize();

    if (m_vMoveDir.Length() > 0.f)
    {
        if (fForwardDot > 0.7f)
        {
            if (isLeft)
            {
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_FL], true, 0.05f);
                m_eWalkState = WALK_FL;
            }
            else if (isRight)
            {
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_FR], true, 0.05f);
                m_eWalkState = WALK_FR;
            }
            else
            {
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_F], true, 0.05f);
                m_eWalkState = WALK_F;
            }
        }
        else if (fBackwardDot > 0.7f)
        {
            if (isLeft)
            {
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_BL], true, 0.05f);
                m_eWalkState = WALK_BL;
            }
            else if (isRight)
            {
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_BR], true, 0.05f);
                m_eWalkState = WALK_BR;
            }
            else
            {
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_B], true, 0.05f);
                m_eWalkState = WALK_B;
            }
        }
        else
        {
            if (isLeft)
            {
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_L], true, 0.05f);
                m_eWalkState = WALK_L;
            }
            else if (isRight)
            {
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_R], true, 0.05f);
                m_eWalkState = WALK_R;
            }
        }

        if (m_vMoveDir.Length() > 0.f)
        {
            // 가드 상태에서는 회전 안 함
            m_pPlayer->Move_Dir(m_vMoveDir, fTimeDelta, false);
        }
        isMoving = true;
    }

    return isMoving;
}

void CState_Player_OH_Walk::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_eWalkState == WALK_FL || m_eWalkState == WALK_BR)
    {
        if (iFrame % 70 == m_iFootStepFrame[m_eWalkState][0] && !m_isPlaySound)
        {
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Walk_01.wav"));
            m_isPlaySound = true;
        }
        else if (iFrame % 70 == m_iFootStepFrame[m_eWalkState][1] && !m_isPlaySound)
        {
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Walk_02.wav"));
            m_isPlaySound = true;
        }
        else
        {
            m_isPlaySound = false;
        }
    }
    else if (iFrame == m_iFootStepFrame[m_eWalkState][0] && !m_isPlaySound)
    {
        if (m_eWalkState == WALK_FL || m_eWalkState == WALK_BR)
            return;
        
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Walk_01.wav"));
        m_isPlaySound = true;
    }
    else if (iFrame == m_iFootStepFrame[m_eWalkState][1] && !m_isPlaySound)
    {
        if (m_eWalkState == WALK_FL || m_eWalkState == WALK_BR)
            return;

        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Walk_02.wav"));
        m_isPlaySound = true;
    }
    else
    {
        m_isPlaySound = false;
    }
}

CState_Player_OH_Walk* CState_Player_OH_Walk::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OH_Walk* pInstance = new CState_Player_OH_Walk(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_OH_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OH_Walk::Free()
{
    __super::Free();
}