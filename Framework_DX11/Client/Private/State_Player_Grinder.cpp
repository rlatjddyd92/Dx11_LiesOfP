#include "stdafx.h"
#include "State_Player_Grinder.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"

CState_Player_Grinder::CState_Player_Grinder(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Grinder::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Grinder[0] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Grinder_Start");
    m_iAnimation_Grinder[1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Grinder_Loop");
    m_iAnimation_Grinder[2] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Grinder_End");

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

HRESULT CState_Player_Grinder::Start_State(void* pArg)
{
    if (m_pFsm->Get_PrevState() != CPlayer::OH_RUN && m_pFsm->Get_PrevState() != CPlayer::TH_WALK)
        m_pPlayer->Change_Animation(m_iAnimation_Grinder[0], false, 0.5f);

    m_pPlayer->Change_Animation_Boundry(m_iAnimation_Grinder[0], false, 0.5f);

    m_pPlayer->Set_MoveSpeed(m_fMoveSpeed);

    m_isChange[0] = false;
    m_isChange[1] = false;
    m_isChange[2] = false;

    return S_OK;
}

void CState_Player_Grinder::Update(_float fTimeDelta)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _uint iCurAnim_Boundry = m_pPlayer->Get_CurrentAnimIndex_Boundry();
    _int iFrame = m_pPlayer->Get_Frame();

    if (KEY_HOLD(KEY::R))
    {
        if (iCurAnim_Boundry == m_iAnimation_Grinder[0] 
            || iCurAnim == m_iAnimation_Grinder[0])
        {
            if (m_pPlayer->Get_EndAnim(m_iAnimation_Grinder[0], true)
                || m_pPlayer->Get_EndAnim(m_iAnimation_Grinder[0]))
            {
                m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Item_Grinder_Loop_01.wav"), true);
                m_pPlayer->Active_Effect(CPlayer::EFFECT_GRIND);
                m_pPlayer->Change_Animation(m_iAnimation_Grinder[1], true, 0.f, 0, true);
                m_isChange[0] = true;
            }
            else
            {
                if (!Move(fTimeDelta) && !m_isChange[0])
                {
                    m_pPlayer->Change_Animation(m_iAnimation_Grinder[0], true, 0.1f);
                }
            }
        }
        else if (iCurAnim_Boundry == m_iAnimation_Grinder[1]
            || iCurAnim == m_iAnimation_Grinder[1])
        {
            if (!Move(fTimeDelta))
            {
                m_pPlayer->Change_Animation(m_iAnimation_Grinder[1], true, 0.1f, 0);
            }
        }
    }

    if (KEY_AWAY(KEY::R))
    {
        m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Item_Grinder_End_01.wav"));
        m_pPlayer->Change_Animation(m_iAnimation_Grinder[2], false, 0.1f);
        m_pPlayer->DeActive_Effect(CPlayer::EFFECT_GRIND);
    }
    else if (KEY_NONE(KEY::R))
    {
        if (iCurAnim_Boundry == m_iAnimation_Grinder[2])
        {
            if (m_pPlayer->Get_EndAnim(m_iAnimation_Grinder[2], true) || m_pPlayer->Get_EndAnim(m_iAnimation_Grinder[2]))
            {
                m_pPlayer->DeActive_Effect(CPlayer::EFFECT_GRIND);
                m_pPlayer->Change_State(CPlayer::OH_IDLE);
                return;
            }
            else if (!Move(fTimeDelta))
            {
                m_pPlayer->Change_Animation(m_iAnimation_Grinder[2], true, 0.1f, 0);
            }

        }
    }
}

void CState_Player_Grinder::End_State()
{
    m_pPlayer->Stop_Sound(CPawn::PAWN_SOUND_EFFECT1);
    m_pPlayer->DeActive_Effect(CPlayer::EFFECT_GRIND);
}

_bool CState_Player_Grinder::End_Check()
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };

    return bEndCheck;
}

_bool CState_Player_Grinder::Move(_float fTimeDelta)
{
    _bool isMoving = false;
    m_vMoveDir = _Vec4(0.f, 0.f, 0.f, 0.f);

    _Vec4 vCameraLook = m_pPlayer->Get_Camera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    _Vec4 vCameraRight = m_pPlayer->Get_Camera()->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
    vCameraLook.y = 0.f;
    vCameraRight.y = 0.f;

    vCameraLook.Normalize();
    vCameraRight.Normalize();

    _bool isForward = KEY_HOLD(KEY::W);
    _bool isBackward = KEY_HOLD(KEY::S);
    _bool isRight = KEY_HOLD(KEY::D);
    _bool isLeft = KEY_HOLD(KEY::A);

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

    if (m_vMoveDir.Length() > 0.f)
    {
        if (fForwardDot > 0.7f)
        {
            m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_F], true, 0.2f, 0, false);
        }
        else if (fBackwardDot > 0.7f)
        {
            m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_B], true, 0.2f, 0, false);
        }
        else
        {
            if (isLeft)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_L], true, 0.2f, 0, false);
            else if (isRight)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_R], true, 0.2f, 0, false);
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

CState_Player_Grinder* CState_Player_Grinder::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Grinder* pInstance = new CState_Player_Grinder(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Grinder"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Grinder::Free()
{
    __super::Free();
}