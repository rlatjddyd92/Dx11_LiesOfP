#include "stdafx.h"
#include "State_Player_Arm_Start.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "TreasureBox.h"

CState_Player_Arm_Start::CState_Player_Arm_Start(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Arm_Start::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_Walk[WALK_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_B", 2.5f);
    m_iAnimation_Walk[WALK_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_F", 2.5f);
    m_iAnimation_Walk[WALK_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_L", 2.5f);
    m_iAnimation_Walk[WALK_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_R", 2.5f);

    m_iAnimation_ArmStart = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Arm_GuardEx_Start", 4.f);

    m_iStateNum = iStateNum;

    m_iEffectFrame = 55;

    return S_OK;
}

HRESULT CState_Player_Arm_Start::Start_State(void* pArg)
{
    if (m_pFsm->Get_PrevState() != CPlayer::OH_WALK)
        m_pPlayer->Change_Animation(m_iAnimation_ArmStart, false, 0.1f, 0, true);

    m_pPlayer->Change_Animation_Boundry(m_iAnimation_ArmStart, false);

    m_pPlayer->Set_MoveSpeed(3.f);

    m_pPlayer->Change_CameraMode(CPlayerCamera::MODE_STATIC);

    m_pPlayer->Get_Model()->Set_RemoteTuning(false);

    m_isActiveEffect = false;

    m_isChangeLoop = false;

    return S_OK;
}

void CState_Player_Arm_Start::Update(_float fTimeDelta)
{
    _int iFrmae = m_pPlayer->Get_Frame();

    if (m_pPlayer->Key_Hold(KEY::CTRL))
    {
        if (End_Check())
        {
            m_isChangeLoop = true;
            m_pPlayer->Change_State(CPlayer::ARM_LOOP);
            return;
        }
        else if (!Move(fTimeDelta))
        {
            m_pPlayer->Change_Animation(m_iAnimation_ArmStart, false, 0.15f);
        }
        m_pPlayer->Change_Animation_Boundry(m_iAnimation_ArmStart, false, 0.05f);
    }
    else
    {
        _uint iWeponType = m_pPlayer->Get_WeaponType();

        if (iWeponType < 2)
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        else
            m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }

    Control_Effect(iFrmae);
}

void CState_Player_Arm_Start::End_State()
{
    if(!m_isChangeLoop)
        m_pPlayer->Get_Model()->Set_RemoteTuning(true);
}

_bool CState_Player_Arm_Start::End_Check()
{
    return (m_pPlayer->Get_EndAnim(m_iAnimation_ArmStart) || m_pPlayer->Get_EndAnim(m_iAnimation_ArmStart, true));
}


_bool CState_Player_Arm_Start::Move(_float fTimeDelta)
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

void CState_Player_Arm_Start::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    for (_uint i = 0; i < 14; ++i)
    {
        if ((iFrame == m_iFootStepFrame[i] || iFrame == m_iFootStepFrame[i] + 1) && !m_isPlaySound[i])
        {
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Walk_01.wav"));
            m_isPlaySound[i] = true;
        }
    }
}

void CState_Player_Arm_Start::Control_Effect(_int iFrame)
{
    if (!m_isActiveEffect && m_iEffectFrame < (_uint)iFrame)
    {
        m_pPlayer->Active_Effect(CPlayer::EFFECT_ARM_SHIELDBLOCK, false);
        m_isActiveEffect = true;
    }
}

CState_Player_Arm_Start* CState_Player_Arm_Start::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Arm_Start* pInstance = new CState_Player_Arm_Start(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Arm_Start"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Arm_Start::Free()
{
    __super::Free();
}
