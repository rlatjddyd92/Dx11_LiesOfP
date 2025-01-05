#include "stdafx.h"
#include "State_Player_OpenRaxasiaDoor.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "TowerDoor.h"

CState_Player_OpenRaxasiaDoor::CState_Player_OpenRaxasiaDoor(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OpenRaxasiaDoor::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_DoorPush = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Prop_DoubleDoor_Slide", 1.75f);

    m_iStateNum = iStateNum;

    m_iFootStepFrame[0] = 460;
    m_iFootStepFrame[1] = 495;
    m_iFootStepFrame[2] = 530;
    m_iFootStepFrame[3] = 560;
    m_iFootStepFrame[4] = 605;
    m_iFootStepFrame[5] = 640;
    m_iFootStepFrame[6] = 670;

    return S_OK;
}

HRESULT CState_Player_OpenRaxasiaDoor::Start_State(void* pArg)
{
    if (m_pTowerDoor == nullptr)
        m_pTowerDoor = static_cast<CTowerDoor*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_TowerDoor"), 0));

    m_pPlayer->Change_Animation(m_iAnimation_DoorPush, false, 0.f);

    m_pPlayer->Disappear_Weapon();

    _Vec3 vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _Vec3 vTowerDoorPos = m_pTowerDoor->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _Vec3 vTowerDoorRight = m_pTowerDoor->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
    _Vec3 vTowerDoorLook = m_pTowerDoor->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    vTowerDoorRight.Normalize();
    vTowerDoorLook.Normalize();

    _Vec3 vInitPos = vTowerDoorPos + vTowerDoorLook * 0.8f;
    vInitPos.y = vTowerDoorPos.y;

    m_pPlayer->Get_RigidBody()->Set_GloblePose(vInitPos);
    m_pPlayer->Get_Transform()->Set_NewLook(-vTowerDoorLook);
    m_pPlayer->Get_Navigation()->Research_Cell(vInitPos);
    m_pTowerDoor->Set_IsOpen(true);

    for (_uint i = 0; i < 7; ++i)
    {
        m_isPlaySound[i] = false;
    }

    return S_OK;
}

void CState_Player_OpenRaxasiaDoor::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame > 350)
    {
        m_pPlayer->DeActive_Effect(CPlayer::EFFECT_CUTSCENE_ARM_OPENDOOR);
    }
    else if (iFrame > 120)
    {
        m_pPlayer->Active_Effect(CPlayer::EFFECT_CUTSCENE_ARM_OPENDOOR);
    }

    for (_uint i = 0; i < 7; ++i)
    {
        if ((iFrame == m_iFootStepFrame[i] || iFrame == m_iFootStepFrame[i] + 1) && !m_isPlaySound[i])
        {
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_FS_Stone_Walk_01.wav"));
            m_isPlaySound[i] = true;
        }
    }
}

void CState_Player_OpenRaxasiaDoor::End_State()
{
    m_pTowerDoor= nullptr;
}

_bool CState_Player_OpenRaxasiaDoor::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_DoorPush);
}

CState_Player_OpenRaxasiaDoor* CState_Player_OpenRaxasiaDoor::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OpenRaxasiaDoor* pInstance = new CState_Player_OpenRaxasiaDoor(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_OpenRaxasiaDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OpenRaxasiaDoor::Free()
{
    __super::Free();
}