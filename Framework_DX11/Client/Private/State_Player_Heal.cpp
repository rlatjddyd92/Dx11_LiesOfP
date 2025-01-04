#include "stdafx.h"
#include "State_Player_Heal.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

// 25-01-04 김성용
   // 포션, 투척물 사용 확정
#include "GameInterface_Controller.h"

CState_Player_Heal::CState_Player_Heal(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Heal::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_Walk[0][WALK_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_B", 2.5f);
    m_iAnimation_Walk[0][WALK_BL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_BL", 2.5f);
    m_iAnimation_Walk[0][WALK_BR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_BR", 2.5f);
    m_iAnimation_Walk[0][WALK_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_F", 2.5f);
    m_iAnimation_Walk[0][WALK_FL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_FL", 2.5f);
    m_iAnimation_Walk[0][WALK_FR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_FR", 2.5f);
    m_iAnimation_Walk[0][WALK_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_L", 2.5f);
    m_iAnimation_Walk[0][WALK_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_R", 2.5f);

    m_iAnimation_Walk[1][WALK_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_B", 2.5f);
    m_iAnimation_Walk[1][WALK_BL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_BL", 2.5f);
    m_iAnimation_Walk[1][WALK_BR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_BR", 2.5f);
    m_iAnimation_Walk[1][WALK_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_F", 2.5f);
    m_iAnimation_Walk[1][WALK_FL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_FL", 2.5f);
    m_iAnimation_Walk[1][WALK_FR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_FR", 2.5f);
    m_iAnimation_Walk[1][WALK_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_L", 2.5f);
    m_iAnimation_Walk[1][WALK_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_R", 2.5f);

    m_iAnimation_Heal = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Item_Ergo", 2.5f);

    m_fMoveSpeed = 1.5f;
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Heal::Start_State(void* pArg)
{
    if (m_pFsm->Get_PrevState() != CPlayer::OH_RUN && m_pFsm->Get_PrevState() != CPlayer::TH_WALK)
        m_pPlayer->Change_Animation(m_iAnimation_Heal, false, 0.3f, 0, false);

    m_pPlayer->Change_Animation_Boundry(m_iAnimation_Heal, false, 0.3f, 0);

    m_pPlayer->Set_MoveSpeed(m_fMoveSpeed);

    m_isPlaySound = false;
    m_isRecoveryHP = false;

    m_fRecoveryAmount = 0.f;

    // 25-01-04 김성용
    // 포션, 투척물 락 풀기
    GET_GAMEINTERFACE->Set_PotionThrow_Lock(false);

    return S_OK;
}

void CState_Player_Heal::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame(true);

    if (m_isRecoveryHP)
    {
        if (m_fRecoveryAmount >= 200.f)
            m_isRecoveryHP = false;
        m_pPlayer->Recovery_HP(1.f);
    }

    if (!Move(fTimeDelta))
    {
        m_pPlayer->Change_Animation(m_iAnimation_Heal, false, 0.1f, 0, false);
    }
    //m_pPlayer->Change_Animation_Boundry(m_iAnimation_Heal, false, 0.1f);
    
    if ((iFrame == 50 || iFrame == 51) && !m_isPlaySound)
    {
        m_isRecoveryHP = true;
        m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FX_Item_Ergo_S.wav"));
        m_pPlayer->Active_Effect(CPlayer::EFFECT_HEAL, false);
        m_isPlaySound = true;
    }

    if (End_Check())
    {
        _uint iWeponType = m_pPlayer->Get_WeaponType();

        if(iWeponType < 2)
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        else
            m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }
}

void CState_Player_Heal::End_State()
{
}

_bool CState_Player_Heal::Move(_float fTimeDelta)
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

    _uint iWalkType = m_pPlayer->Get_WeaponType();
    if (iWalkType <= 1)
        iWalkType = 0;
    else
        iWalkType = 1;

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
            m_pPlayer->Change_Animation(m_iAnimation_Walk[iWalkType][WALK_F], true, 0.2f, 0, false);
        }
        else if (fForwardDirDot < -0.7f) // 후진
        {
            m_pPlayer->Change_Animation(m_iAnimation_Walk[iWalkType][WALK_B], true, 0.2f, 0, false);
        }
        else
        {
            // 카메라와 플레이어의 전방 벡터 Dot값을 기반으로 좌우 이동 처리
            _float fForwardCameraDot = vPlayerLook.Dot(vCameraLook);
            bool isCameraAlignedForward = (fForwardCameraDot > 0.f);

            if (isLeft)
            {
                m_pPlayer->Change_Animation(
                    m_iAnimation_Walk[iWalkType][isCameraAlignedForward ? WALK_L : WALK_R], true, 0.2f, 0, false);
            }
            else if (isRight)
            {
                m_pPlayer->Change_Animation(
                    m_iAnimation_Walk[iWalkType][isCameraAlignedForward ? WALK_R : WALK_L], true, 0.2f, 0, false);
            }
            else
            {
                // 카메라와 플레이어의 우측 벡터 Dot값을 기반으로 전후 좌우 혼합 이동 처리
                _float fRightCameraDot = vPlayerRight.Dot(vCameraLook);
                bool isCameraAlignedRight = (fRightCameraDot > 0.f);

                if (isForward)
                {
                    m_pPlayer->Change_Animation(
                        m_iAnimation_Walk[iWalkType][isCameraAlignedRight ? WALK_R : WALK_L], true, 0.2f, 0, false);
                }
                else if (isBackward)
                {
                    m_pPlayer->Change_Animation(
                        m_iAnimation_Walk[iWalkType][isCameraAlignedRight ? WALK_L : WALK_R], true, 0.2f, 0, false);
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

_bool CState_Player_Heal::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Heal, true);
}

CState_Player_Heal* CState_Player_Heal::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Heal* pInstance = new CState_Player_Heal(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Heal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Heal::Free()
{
    __super::Free();
}