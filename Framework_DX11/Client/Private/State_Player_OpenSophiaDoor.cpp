#include "stdafx.h"
#include "State_Player_OpenSophiaDoor.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "LastDoor.h"

CState_Player_OpenSophiaDoor::CState_Player_OpenSophiaDoor(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OpenSophiaDoor::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_DoorPush = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Prop_DoubleDoor_Push", 0.9f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_OpenSophiaDoor::Start_State(void* pArg)
{
    m_pLastDoor = static_cast<CLastDoor*>(pArg);

    m_pPlayer->Change_Animation(m_iAnimation_DoorPush, false, 0.8f, 316);

    m_pPlayer->Disappear_Weapon();

    _Vec3 vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _Vec3 vLastDoorPos = m_pLastDoor->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _Vec3 vLastDoorRight = m_pLastDoor->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
    _Vec3 vLastDoorLook = m_pLastDoor->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    vLastDoorRight.Normalize();
    vLastDoorLook.Normalize();

    _Vec3 vInitPos = vLastDoorPos + vLastDoorLook * 1.1f;
    vInitPos.y = vPlayerPos.y;

    m_pPlayer->Get_RigidBody()->Set_GloblePose(vInitPos);
    m_pPlayer->Get_Transform()->Set_NewLook(-vLastDoorLook);


    m_pGameInstance->Play_BGM(TEXT("MU_MS_Monastery_B_Loop.wav"), 1.f);

    return S_OK;
}

void CState_Player_OpenSophiaDoor::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame > 545)
    {
        m_pPlayer->Change_State(CPlayer::SOPHIA_WALK);
        m_pGameInstance->Play_BGM(TEXT("MU_MS_Monastery_B_Loop.wav"), g_fBGMVolume);
    }
}

void CState_Player_OpenSophiaDoor::End_State()
{
    m_pLastDoor = nullptr;
}

_bool CState_Player_OpenSophiaDoor::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_DoorPush);
}

CState_Player_OpenSophiaDoor* CState_Player_OpenSophiaDoor::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OpenSophiaDoor* pInstance = new CState_Player_OpenSophiaDoor(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_OpenSophiaDoor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OpenSophiaDoor::Free()
{
    __super::Free();
}