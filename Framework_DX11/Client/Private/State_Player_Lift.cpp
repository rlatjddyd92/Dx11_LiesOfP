#include "stdafx.h"
#include "State_Player_Lift.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Lift_Controller.h"

CState_Player_Lift::CState_Player_Lift(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Lift::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_Lift = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Prop_Lift_Move", 2.5f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Lift::Start_State(void* pArg)
{
    m_pLiftController = static_cast<CLift_Controller*>(pArg);

    _Vec3 vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _Vec3 vLiftControllerPos = m_pLiftController->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _Vec3 vLiftControllerRight = m_pLiftController->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
    _Vec3 vLiftControllerLook = m_pLiftController->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    vLiftControllerRight.Normalize();
    vLiftControllerLook.Normalize();

    _Vec3 vInitPos = vLiftControllerPos + vLiftControllerRight * 0.9f + vLiftControllerLook * 0.25f;
    vInitPos.y = vPlayerPos.y;

    m_pPlayer->Get_RigidBody()->Set_GloblePose(vInitPos);
    m_pPlayer->Get_Transform()->Set_NewLook(-vLiftControllerRight);

    m_pPlayer->Get_RigidBody()->Set_IsOnCell(false);

    m_pPlayer->Change_Animation(m_iAnimation_Lift, false, 0.f);

    m_isPlaySound = false;
    m_isPlaySound2 = false;

    return S_OK;
}

void CState_Player_Lift::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (End_Check())
    {
        _uint iWeponType = m_pPlayer->Get_WeaponType();

        if (iWeponType < 2)
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        else
            m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }

    if (!m_isPlaySound && iFrame == 5 || iFrame == 6)
    {
        m_isPlaySound = true;
        m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Lift_Active.wav"));
    }

    if (!m_isPlaySound2 && iFrame == 120 || iFrame == 121)
    {
        m_isPlaySound2 = true;
        m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("AMB_OJ_PR_Lift_Controller_Lever_Open.wav"));
    }
}

void CState_Player_Lift::End_State()
{
    m_pPlayer->Get_RigidBody()->Set_IsOnCell(true);
}

_bool CState_Player_Lift::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Lift);
}

CState_Player_Lift* CState_Player_Lift::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Lift* pInstance = new CState_Player_Lift(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Lift"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Lift::Free()
{
    __super::Free();
}