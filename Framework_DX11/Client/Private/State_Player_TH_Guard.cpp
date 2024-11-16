#include "stdafx.h"
#include "State_Player_TH_Guard.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_TH_Guard::CState_Player_TH_Guard(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_TH_Guard::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Walk[WALK_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_B", 2.5f);
    m_iAnimation_Walk[WALK_BL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_BL", 2.5f);
    m_iAnimation_Walk[WALK_BR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_BR", 2.5f);
    m_iAnimation_Walk[WALK_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_F", 2.5f);
    m_iAnimation_Walk[WALK_FL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_FL", 2.5f);
    m_iAnimation_Walk[WALK_FR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_FR", 2.5f);
    m_iAnimation_Walk[WALK_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_L", 2.5f);
    m_iAnimation_Walk[WALK_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_R", 2.5f);

    m_iAnimation_Guard = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Guard_Idle", 2.5f);

    m_fMoveSpeed = 1.5f;
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_TH_Guard::Start_State(void* pArg)
{
    if(m_pFsm->Get_PrevState() != CPlayer::OH_WALK)
        m_pPlayer->Change_Animation(m_iAnimation_Guard, true);

    m_pPlayer->Change_Animation_Boundry(m_iAnimation_Guard, true);

    m_pPlayer->Set_IsGuard(true);
    m_pPlayer->Set_MoveSpeed(3.f);

    return S_OK;
}

void CState_Player_TH_Guard::Update(_float fTimeDelta)
{
    if (KEY_HOLD(KEY::LSHIFT))
    {
        if (KEY_TAP(KEY::F))
        {
            m_pPlayer->Change_State(CPlayer::PARRY);
            return;
        }
        if (!Move(fTimeDelta))
        {
            m_pPlayer->Change_Animation(m_iAnimation_Guard, true);
        }
        m_pPlayer->Change_Animation_Boundry(m_iAnimation_Guard, true, 0.f);
    }

    if (KEY_NONE(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }
}

void CState_Player_TH_Guard::End_State()
{
    m_pPlayer->Set_IsGuard(false);
}

_bool CState_Player_TH_Guard::Move(_float fTimeDelta)
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

    if (m_vMoveDir.Length() > 0.f)
    {
        if (isForward)
        {
            if(isLeft)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_FL], true, 0.2f);
            else if(isRight)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_FR], true, 0.2f);
            else
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_F], true, 0.2f);
        }
        else if (isBackward)
        {
            if (isLeft)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_BL], true, 0.2f);
            else if (isRight)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_BR], true, 0.2f);
            else
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_B], true, 0.2f);
        }
        else
        {
            if (isLeft)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_L], true, 0.2f);
            else if (isRight)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_R], true, 0.2f);
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

CState_Player_TH_Guard* CState_Player_TH_Guard::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_TH_Guard* pInstance = new CState_Player_TH_Guard(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Heal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_TH_Guard::Free()
{
    __super::Free();
}