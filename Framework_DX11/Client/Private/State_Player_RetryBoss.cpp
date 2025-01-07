#include "stdafx.h"
#include "State_Player_RetryBoss.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "CMoveBlockObj.h"
#include "Monster.h"

CState_Player_RetryBoss::CState_Player_RetryBoss(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_RetryBoss::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_Film = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Prop_ErgoFlim", 2.f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_RetryBoss::Start_State(void* pArg)
{
    m_pBlockObj = static_cast<CMoveBlockObj*>(pArg);

    _Vec3 vBlockPos = m_pBlockObj->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _Vec3 vBlockLook = m_pBlockObj->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    vBlockLook.Normalize();

    _Vec3 vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _Vec3 vInitPos = vBlockPos + vBlockLook * 1.3f;
    vInitPos.y = vPlayerPos.y;

    m_pPlayer->Get_RigidBody()->Set_GloblePose(vInitPos);

    m_pPlayer->Change_Animation(m_iAnimation_Film, false, 0.f);

    m_isPlaySound = false;
    m_isPlaySound2 = false;

    if (m_pBlockObj->Get_Type() == 0)
    {
        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Raxasia"), TEXT("Prototype_GameObject_Raxasia"))))
            return E_FAIL;
    }
    else if (m_pBlockObj->Get_Type() == 2)
    {
        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_SimonManus"), TEXT("Prototype_GameObject_SimonManus"))))
        	return E_FAIL;
    }

    m_pPlayer->Disappear_Weapon();

    return S_OK;
}

void CState_Player_RetryBoss::Update(_float fTimeDelta)
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

    if (!m_isPlaySound && (iFrame == 0 || iFrame == 1))
    {
        m_pBlockObj->Open_Door();

        m_isPlaySound = true;
        m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_OJ_FX_Hidden_Item_Move_01.wav"));
    }
}

void CState_Player_RetryBoss::End_State()
{
    if (m_pBlockObj->Get_Type() == 0)
    {
        static_cast<CMonster*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Raxasia"), 0))->SetUp_Act();
    }
    else if (m_pBlockObj->Get_Type() == 2)
    {
        static_cast<CMonster*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_SimonManus"), 0))->SetUp_Act();
    }

    m_pBlockObj->Close_Door();
    m_pBlockObj = nullptr;

    m_pPlayer->Appear_Weapon();
    m_pPlayer->Get_RigidBody()->Set_IsOnCell(true);
}

_bool CState_Player_RetryBoss::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Film);
}

CState_Player_RetryBoss* CState_Player_RetryBoss::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_RetryBoss* pInstance = new CState_Player_RetryBoss(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_RetryBoss"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_RetryBoss::Free()
{
    __super::Free();
}