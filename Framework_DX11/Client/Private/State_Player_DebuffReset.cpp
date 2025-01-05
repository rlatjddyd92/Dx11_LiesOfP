#include "stdafx.h"
#include "State_Player_DebuffReset.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"

#include "Effect_Manager.h"

CState_Player_DebuffReset::CState_Player_DebuffReset(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_DebuffReset::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Buff = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Item_LampBuff", 1.4f);

    m_iAnimation_Walk[WALK_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_B", 2.5f);
    m_iAnimation_Walk[WALK_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_F", 2.5f);
    m_iAnimation_Walk[WALK_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_L", 2.5f);
    m_iAnimation_Walk[WALK_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Walk_R", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_fMoveSpeed = 1.5f;
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_DebuffReset::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Buff, false, 0.2f);

    m_pPlayer->Disappear_Weapon();

    m_pPlayer->Set_MoveSpeed(m_fMoveSpeed);

    m_isCreateItem = false;
    m_isActiveItem = false;

    return S_OK;
}

void CState_Player_DebuffReset::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame(true);

    if (!m_isCreateItem && iFrame > 20)
    {
        m_pPlayer->Active_Effect(CPlayer::EFFECT_ITEM_PURIFICATION);
        m_isCreateItem = true;
    }
    else if (!m_isActiveItem && (iFrame == 65 || iFrame == 66))
    {
        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Item_Purification_Active"), 
            (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION));

        m_pPlayer->Use_DebuffResetItem();

        m_isActiveItem = true;
    }
    else if (End_Check())
    {
        _uint iWeponType = m_pPlayer->Get_WeaponType();

        if (iWeponType < 2)
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        else
            m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }
}

void CState_Player_DebuffReset::End_State()
{
    m_pPlayer->Appear_Weapon();
    m_pPlayer->Stop_Sound(CPawn::PAWN_SOUND_EFFECT1);
    m_pPlayer->DeActive_Effect(CPlayer::EFFECT_ITEM_PURIFICATION);
}

_bool CState_Player_DebuffReset::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Buff);
}

_bool CState_Player_DebuffReset::Move(_float fTimeDelta)
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
                    m_iAnimation_Walk[isCameraAlignedForward ? WALK_L : WALK_R], true, 0.2f, 0, false);
            }
            else if (isRight)
            {
                m_pPlayer->Change_Animation(
                    m_iAnimation_Walk[isCameraAlignedForward ? WALK_R : WALK_L], true, 0.2f, 0, false);
            }
            else
            {
                // 카메라와 플레이어의 우측 벡터 Dot값을 기반으로 전후 좌우 혼합 이동 처리
                _float fRightCameraDot = vPlayerRight.Dot(vCameraLook);
                bool isCameraAlignedRight = (fRightCameraDot > 0.f);

                if (isForward)
                {
                    m_pPlayer->Change_Animation(
                        m_iAnimation_Walk[isCameraAlignedRight ? WALK_R : WALK_L], true, 0.2f, 0, false);
                }
                else if (isBackward)
                {
                    m_pPlayer->Change_Animation(
                        m_iAnimation_Walk[isCameraAlignedRight ? WALK_L : WALK_R], true, 0.2f, 0, false);
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

CState_Player_DebuffReset* CState_Player_DebuffReset::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_DebuffReset* pInstance = new CState_Player_DebuffReset(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_DebuffReset"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_DebuffReset::Free()
{
    __super::Free();
}