#include "stdafx.h"
#include "State_Player_TH_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_TH_Walk::CState_Player_TH_Walk(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_TH_Walk::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Walk[WALK_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_B" , 2.5f);
    m_iAnimation_Walk[WALK_BL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_BL", 2.5f);
    m_iAnimation_Walk[WALK_BR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_BR", 2.5f);
    m_iAnimation_Walk[WALK_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_F", 2.5f);
    m_iAnimation_Walk[WALK_FL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_FL", 2.5f);
    m_iAnimation_Walk[WALK_FR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_FR", 2.5f);
    m_iAnimation_Walk[WALK_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_L", 2.5f);
    m_iAnimation_Walk[WALK_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_Walk_R", 2.5f);

    m_isInputSpace = false;
    m_fSpaceTime = 0.f;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_TH_Walk::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_F], true);

    m_pPlayer->Set_MoveSpeed(4.f);

    m_fSpaceTime = 0.f;
    m_isInputSpace = false;

    return S_OK;
}

void CState_Player_TH_Walk::Update(_float fTimeDelta)
{
    if (m_isInputSpace)
    {
        m_fSpaceTime += fTimeDelta;
    }

    if (false == Move(fTimeDelta))
    {
        m_pPlayer->Change_State(CPlayer::TH_IDLE);
        return;
    }

    if (m_fSpaceTime > 0.1f)
    {
        if (m_pPlayer->Key_None(KEY::SPACE))
        {
            m_pPlayer->Change_State(CPlayer::TH_DASH);
        }

        if (m_pPlayer->Key_Hold(KEY::SPACE))
        {
            m_pPlayer->Change_State(CPlayer::TH_RUN);
        }
    }
    else if (m_pPlayer->Key_Hold(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CPlayer::TH_GUARD);
    }

    if (m_pPlayer->Key_Tab(KEY::SPACE))
        m_isInputSpace = true;
}



void CState_Player_TH_Walk::End_State()
{
}

void CState_Player_TH_Walk::Select_WalkAnimation()
{
    

}

_bool CState_Player_TH_Walk::Move(_float fTimeDelta)
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

    _Vec4 vLocalMoveDir = { m_vMoveDir.Dot(vCameraRight), 0.f, m_vMoveDir.Dot(vCameraLook), 0.f};
    vLocalMoveDir.Normalize();

    //// 방향을 기반으로 애니메이션 선정
    //if (vLocalMoveDir.z > 0.5f)
    //{
    //    if (vLocalMoveDir.x > 0.5f)
    //        m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_FR], true, false);
    //    else if (vLocalMoveDir.x < 0)
    //        m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_FL], true, false);
    //    else
    //        m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_F], true, false);
    //}
    //else if (vLocalMoveDir.z < 0.5f)
    //{
    //    if (vLocalMoveDir.x > 0.5f)
    //        m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_BR], true, false);
    //    else if (vLocalMoveDir.x < 0)
    //        m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_BL], true, false);
    //    else
    //        m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_B], true, false);
    //}
    //else
    //{
    //    if (vLocalMoveDir.x > 0.5f)
    //        m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_R], true, false);
    //    else if (vLocalMoveDir.x < 0.5f)
    //        m_pPlayer->Change_Animation(m_iAnimation_Walk[WALK_L], true, false);
    //}


    if (m_vMoveDir.Length() > 0.f)
    {
        m_pPlayer->Move_Dir(m_vMoveDir, fTimeDelta);
    }

    return isMoving;
}

CState_Player_TH_Walk* CState_Player_TH_Walk::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_TH_Walk* pInstance = new CState_Player_TH_Walk(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_TH_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_TH_Walk::Free()
{
    __super::Free();
}