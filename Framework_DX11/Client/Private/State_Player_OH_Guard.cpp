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
    m_iAnimation_Walk[WALK_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Guard_Walk_B", 2.5f);
    m_iAnimation_Walk[WALK_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Guard_Walk_F", 2.5f);
    m_iAnimation_Walk[WALK_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Guard_Walk_L", 2.5f);
    m_iAnimation_Walk[WALK_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Guard_Walk_R", 2.5f);

    m_iAnimation_Guard = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Guard_Idle", 3.5f);

    m_fMoveSpeed = 1.5f;
    m_iStateNum = iStateNum;

    m_iFootStepFrame[WALK_B][0] = 28;
    m_iFootStepFrame[WALK_B][1] = 62;
    m_iFootStepFrame[WALK_F][0] = 30;
    m_iFootStepFrame[WALK_F][1] = 66;
    m_iFootStepFrame[WALK_L][0] = 30;
    m_iFootStepFrame[WALK_L][1] = 66;
    m_iFootStepFrame[WALK_R][0] = 34;
    m_iFootStepFrame[WALK_R][1] = 65;

    return S_OK;
}

HRESULT CState_Player_OH_Guard::Start_State(void* pArg)
{
    if (m_pFsm->Get_PrevState() != CPlayer::OH_WALK)
        m_pPlayer->Change_Animation(m_iAnimation_Guard, true, 0.05f, 0, true);

    m_pPlayer->Change_Animation_Boundry(m_iAnimation_Guard, true);

    if (m_pFsm->Get_PrevState() == CPlayer::OH_GUARDHIT
        || m_pFsm->Get_PrevState() == CPlayer::TH_GUARDHIT)
    {
        m_pPlayer->Set_IsGuard(true, false);
    }
    else
    {
        m_pPlayer->Set_IsGuard(true);
    }

    m_pPlayer->Set_MoveSpeed(3.f);

    m_pPlayer->Change_CameraMode(CPlayerCamera::MODE_STATIC);

    return S_OK;
}

void CState_Player_OH_Guard::Update(_float fTimeDelta)
{
    if (KEY_HOLD(KEY::LSHIFT))
    {
        if (KEY_TAP(KEY::F))
        {
            if (m_pPlayer->Check_Region_Fable02())
            {
                if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_RAPIER)
                    m_pPlayer->Change_State(CPlayer::PARRY);
                else if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_SCISSOR)
                    m_pPlayer->Change_State(CPlayer::SCISSOR_BUFF);
                return;
            }
        }
        if (!Move(fTimeDelta))
        {
            m_pPlayer->Change_Animation(m_iAnimation_Guard, true, 0.15f);
        }
        m_pPlayer->Change_Animation_Boundry(m_iAnimation_Guard, true, 0.05f);
    }

    if (KEY_NONE(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Sound();
}
    


void CState_Player_OH_Guard::End_State()
{
    m_pPlayer->Set_IsGuard(false);
}

_bool CState_Player_OH_Guard::Move(_float fTimeDelta)
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

    _bool isForward = KEY_HOLD(KEY::W);
    _bool isBackward = KEY_HOLD(KEY::S);
    _bool isRight = KEY_HOLD(KEY::D);
    _bool isLeft = KEY_HOLD(KEY::A);

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
            m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_F], true, 0.2f);
        }
        else if (fForwardDirDot < -0.7f) // 후진
        {
            m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_B], true, 0.2f);
        }
        else
        {
            // 카메라와 플레이어의 전방 벡터 Dot값을 기반으로 좌우 이동 처리
            _float fForwardCameraDot = vPlayerLook.Dot(vCameraLook);
            bool isCameraAlignedForward = (fForwardCameraDot > 0.f);

            if (isLeft)
            {
                m_pPlayer->Change_Animation(
                    m_iAnimation_Walk[isCameraAlignedForward ? WALK_L : WALK_R], true, 0.2f
                );
            }
            else if (isRight)
            {
                m_pPlayer->Change_Animation(
                    m_iAnimation_Walk[isCameraAlignedForward ? WALK_R : WALK_L], true, 0.2f
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
                        m_iAnimation_Walk[isCameraAlignedRight ? WALK_R : WALK_L], true, 0.2f
                    );
                }
                else if (isBackward)
                {
                    m_pPlayer->Change_Animation(
                        m_iAnimation_Walk[isCameraAlignedRight ? WALK_L : WALK_R], true, 0.2f
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

void CState_Player_OH_Guard::Control_Sound()
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

    if ((iFrame == m_iFootStepFrame[eWalk][0] || iFrame == m_iFootStepFrame[eWalk][0] + 1) && !m_isPlaySound[0])
    {
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Walk_01.wav"));
        m_isPlaySound[0] = true;
    }
    else if ((iFrame == m_iFootStepFrame[eWalk][1] || iFrame == m_iFootStepFrame[eWalk][1] + 1) && !m_isPlaySound[1])
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

CState_Player_OH_Guard* CState_Player_OH_Guard::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OH_Guard* pInstance = new CState_Player_OH_Guard(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Heal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OH_Guard::Free()
{
    __super::Free();
}