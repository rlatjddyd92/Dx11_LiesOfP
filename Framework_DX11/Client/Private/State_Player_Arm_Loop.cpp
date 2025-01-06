#include "stdafx.h"
#include "State_Player_Arm_Loop.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "TreasureBox.h"

#include "GameInterface_Controller.h"

CState_Player_Arm_Loop::CState_Player_Arm_Loop(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Arm_Loop::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_Walk[WALK_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_B", 2.5f);
    m_iAnimation_Walk[WALK_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_F", 2.5f);
    m_iAnimation_Walk[WALK_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_L", 2.5f);
    m_iAnimation_Walk[WALK_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_R", 2.5f);

    m_iAnimation_ArmLoop = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Arm_GuardEx_Loop", 2.f);

    m_iStateNum = iStateNum;

    m_iFootStepFrame[WALK_B][0] = 28;
    m_iFootStepFrame[WALK_B][1] = 62;
    m_iFootStepFrame[WALK_F][0] = 30;
    m_iFootStepFrame[WALK_F][1] = 66;
    m_iFootStepFrame[WALK_L][0] = 25;
    m_iFootStepFrame[WALK_L][1] = 61;
    m_iFootStepFrame[WALK_R][0] = 25;
    m_iFootStepFrame[WALK_R][1] = 59;

    return S_OK;
}

HRESULT CState_Player_Arm_Loop::Start_State(void* pArg)
{
    //m_pPlayer->Change_Animation(m_iAnimation_ArmLoop, true, 0.f, 0, true);

    m_pPlayer->SetUp_Animation_Boundry(m_iAnimation_ArmLoop, true, 0);

    m_pPlayer->Set_IsArm(true);

    return S_OK;
}

void CState_Player_Arm_Loop::Update(_float fTimeDelta)
{
    if (m_pPlayer->Key_Hold(KEY::CTRL))
    {
        m_pPlayer->Decrease_Arm(2.f * fTimeDelta);

        // shift 연타하면 퍼펙트 가드 가능인 사기템
        if (m_pPlayer->Key_Tab(KEY::LSHIFT))
        {
            m_pPlayer->Set_IsGuard(true);
        }
        else if (m_pPlayer->Key_None(KEY::LSHIFT) || m_pPlayer->Key_Away(KEY::LSHIFT))
        {
            m_pPlayer->Set_IsGuard(false);
        }

        if (!Move(fTimeDelta))
        {
            m_pPlayer->Change_Animation(m_iAnimation_ArmLoop, true, 0.15f, 0, false);
        }
        else
        {
            Control_Sound();
        }

        if (m_pPlayer->Key_Tab(KEY::LBUTTON))
        {
            _uint iWeponType = m_pPlayer->Get_WeaponType();

            if (iWeponType > 0)
                m_pPlayer->Change_State(CPlayer::ARM_SWING);
            else
                m_pPlayer->Change_State(CPlayer::ARM_THRUST);
                return;
        }
        else if (m_pPlayer->Key_Tab(KEY::F))
        {
            m_pPlayer->Change_State(CPlayer::ARM_PARRY);
        }
    }
    else
    {
        m_pPlayer->Set_IsArm(false);
        _uint iWeponType = m_pPlayer->Get_WeaponType();
        m_pPlayer->Get_Model()->Set_RemoteTuning(true);

        if (iWeponType < 2)
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        else
            m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }
}

void CState_Player_Arm_Loop::End_State()
{
    m_pPlayer->Set_IsGuard(false);
}

_bool CState_Player_Arm_Loop::End_Check()
{
    return (m_pPlayer->Get_EndAnim(m_iAnimation_ArmLoop) || m_pPlayer->Get_EndAnim(m_iAnimation_ArmLoop, true));
}


_bool CState_Player_Arm_Loop::Move(_float fTimeDelta)
{
    _bool isMoving = false;
    m_vMoveDir = _Vec4(0.f, 0.f, 0.f, 0.f);

    _Vec4 vCameraLook = m_pPlayer->Get_Camera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    _Vec4 vCameraRight = m_pPlayer->Get_Camera()->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
    vCameraLook.y = vCameraRight.y = 0.f;
    vCameraLook.Normalize();
    vCameraRight.Normalize();

    _Vec4 vPlayerLook = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    _Vec4 vPlayerRight = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
    vPlayerLook.y = vPlayerRight.y = 0.f;
    vPlayerLook.Normalize();
    vPlayerRight.Normalize();

    _bool isForward = m_pPlayer->Key_Hold(KEY::W);
    _bool isBackward = m_pPlayer->Key_Hold(KEY::S);
    _bool isRight = m_pPlayer->Key_Hold(KEY::D);
    _bool isLeft = m_pPlayer->Key_Hold(KEY::A);

    if (isForward)
        m_vMoveDir += vCameraLook;

    if (isBackward)
        m_vMoveDir -= vCameraLook;

    if (isRight)
        m_vMoveDir += vCameraRight;

    if (isLeft)
        m_vMoveDir -= vCameraRight;
    m_vMoveDir.Normalize();

    _Vec4 vRight = _Vec4(1.f, 0.f, 0.f, 0.f);

    _float fForwardDirDot = vPlayerLook.Dot(m_vMoveDir);

    if (m_vMoveDir.Length() > 0.f)
    {
        // 캐릭터의 전방 벡터와 이동 방향의 Dot값을 기반으로 전후 이동 처리
        if (fForwardDirDot > 0.7f) // 전진
        {
            m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_F], true, 0.2f, 0, false);
        }
        else if (fForwardDirDot < -0.7f) // 후진
        {
            m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_B], true, 0.2f, 0, false);
        }
        else
        {
            // 카메라와 플레이어의 전방 벡터 Dot값을 기반으로 좌우 이동 처리
            _float fForwardCameraDot = vPlayerLook.Dot(vCameraLook);
            bool isCameraAlignedForward = (fForwardCameraDot > 0.f);

            if (isLeft)
            {
                m_pPlayer->Change_Animation(
                    m_iAnimation_Walk[isCameraAlignedForward ? WALK_L : WALK_R], true, 0.2f, 0, false
                );
            }
            else if (isRight)
            {
                m_pPlayer->Change_Animation(
                    m_iAnimation_Walk[isCameraAlignedForward ? WALK_R : WALK_L], true, 0.2f, 0, false
                );
            }
            else
            {
                // 카메라와 플레이어의 우측 벡터 Dot값을 기반으로 전후 좌우 혼합 이동 처리
                _float fRightCameraDot = vPlayerRight.Dot(vCameraLook);
                bool isCameraAlignedRight = (fRightCameraDot > 0.f);

                if (isForward)
                {
                    m_pPlayer->Change_Animation(
                        m_iAnimation_Walk[isCameraAlignedRight ? WALK_R : WALK_L], true, 0.2f, 0, false
                    );
                }
                else if (isBackward)
                {
                    m_pPlayer->Change_Animation(
                        m_iAnimation_Walk[isCameraAlignedRight ? WALK_L : WALK_R], true, 0.2f, 0, false
                    );
                }
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

void CState_Player_Arm_Loop::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    WALK eWalk = WALK_END;

    if (m_pPlayer->Get_CurrentAnimIndex() == m_iAnimation_Walk[WALK_F])
    {
        eWalk = WALK_F;
    }
    else if (m_pPlayer->Get_CurrentAnimIndex() == m_iAnimation_Walk[WALK_B])
    {
        eWalk = WALK_B;
    }
    else if (m_pPlayer->Get_CurrentAnimIndex() == m_iAnimation_Walk[WALK_L])
    {
        eWalk = WALK_L;
    }
    else if (m_pPlayer->Get_CurrentAnimIndex() == m_iAnimation_Walk[WALK_R])
    {
        eWalk = WALK_R;
    }
    else
    {
        return;
    }

    if (iFrame >= m_iFootStepFrame[eWalk][0] && !m_isPlaySound[0])
    {
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Walk_01.wav"));
        m_isPlaySound[0] = true;
    }
    else if (iFrame >= m_iFootStepFrame[eWalk][1] && !m_isPlaySound[1])
    {
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Walk_02.wav"));
        m_isPlaySound[1] = true;
    }
    else if (iFrame < 4)
    {
        m_isPlaySound[0] = false;
        m_isPlaySound[1] = false;
    }
}

CState_Player_Arm_Loop* CState_Player_Arm_Loop::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Arm_Loop* pInstance = new CState_Player_Arm_Loop(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Arm_Loop"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Arm_Loop::Free()
{
    __super::Free();
}
