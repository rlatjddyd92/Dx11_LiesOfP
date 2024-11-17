#include "stdafx.h"
#include "State_Player_Heal.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Heal::CState_Player_Heal(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Heal::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
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
    //if (m_pFsm->Get_PrevState() != CPlayer::OH_RUN && m_pFsm->Get_PrevState() != CPlayer::TH_WALK)
    //    m_pPlayer->Change_Animation(m_iAnimation_Heal, false);

    //m_pPlayer->Change_Animation_Boundry(m_iAnimation_Heal, false);

    return S_OK;
}

void CState_Player_Heal::Update(_float fTimeDelta)
{
    
    if (!Move(fTimeDelta))
    {
        m_pPlayer->Change_Animation(m_iAnimation_Heal, false);
    }
    m_pPlayer->Change_Animation_Boundry(m_iAnimation_Heal, false, 0.1f);
    

    if (*m_pIsEndAnim)
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
    m_pPlayer->Set_IsGuard(false);
}

_bool CState_Player_Heal::Move(_float fTimeDelta)
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

    _uint iWalkType = m_pPlayer->Get_WeaponType();
    if (iWalkType <= 1)
        iWalkType = 0;
    else
        iWalkType = 1;

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
            if (isLeft)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[iWalkType][WALK_FL], true, 0.2f, 0, false);
            else if (isRight)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[iWalkType][WALK_FR], true, 0.2f, 0, false);
            else
                m_pPlayer->Change_Animation(m_iAnimation_Walk[iWalkType][WALK_F], true, 0.2f, 0, false);
        }
        else if (isBackward)
        {
            if (isLeft)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[iWalkType][WALK_BL], true, 0.2f, 0, false);
            else if (isRight)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[iWalkType][WALK_BR], true, 0.2f, 0, false);
            else
                m_pPlayer->Change_Animation(m_iAnimation_Walk[iWalkType][WALK_B], true, 0.2f, 0, false);
        }
        else
        {
            if (isLeft)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[iWalkType][WALK_L], true, 0.2f, 0.f, false);
            else if (isRight)
                m_pPlayer->Change_Animation(m_iAnimation_Walk[iWalkType][WALK_R], true, 0.2f, 0.f, false);
        }


        if (m_vMoveDir.Length() > 0.f)
        {
            // 가드 상태에서는 회전 안 함
            m_pPlayer->Move_Dir(m_vMoveDir , fTimeDelta, false);
        }
        isMoving = true;
    }


    return isMoving;
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