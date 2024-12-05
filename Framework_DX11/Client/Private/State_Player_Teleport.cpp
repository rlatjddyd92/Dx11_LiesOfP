#include "stdafx.h"
#include "State_Player_Teleport.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "SteppingStone.h"
#include "Effect_Manager.h"

CState_Player_Teleport::CState_Player_Teleport(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Teleport::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_TeleportStart = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Teleport_Start", 1.5f);
    m_iAnimation_TeleportEnd = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Teleport_End", 1.5f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Teleport::Start_State(void* pArg)
{
    m_pSteppingStone = static_cast<CSteppingStone*>(pArg);

    m_pPlayer->Change_Animation(m_iAnimation_TeleportStart, false, 0.f);

    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);

    m_isEnd_Teleport = false;
    m_isAppearStartEffect = false;

    m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Teleport_Start.wav"));

    return S_OK;
}

void CState_Player_Teleport::Update(_float fTimeDelta)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _int iFrame = m_pPlayer->Get_Frame();

    if (iCurAnim == m_iAnimation_TeleportStart)
    {
        if (iFrame >= 150)
        {
            m_pSteppingStone->Change_Player_Pos();
            m_isEnd_Teleport = true;
        }
        else if (m_isEnd_Teleport)
        {
            m_isEnd_Teleport = false;
            m_pPlayer->Change_Animation(m_iAnimation_TeleportEnd, false, 0.3f);

            CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Teleport_Arrive"), (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION));
            m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_MT_Teleport_End.wav"));
        }
        else if (iFrame > 80)
        {
            m_vRimLightColor.z = max(m_vRimLightColor.z + fTimeDelta, 1.f);
            m_vRimLightColor.w = max(m_vRimLightColor.w - 0.6f * fTimeDelta, 0.1f);

            if (!m_isAppearStartEffect)
            {
                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Teleport_Depart"), (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION));
                m_isAppearStartEffect = true;
            }
        }
    }
    else if (iCurAnim == m_iAnimation_TeleportEnd)
    {
        if (iFrame > 10)
        {
            m_vRimLightColor.z = max(m_vRimLightColor.z - 1.5f * fTimeDelta, 0.f);
            m_vRimLightColor.w = min(m_vRimLightColor.w + 1.5f * fTimeDelta, 0.5f);
        }
        if (End_Check())
        {
            _uint iWeponType = m_pPlayer->Get_WeaponType();

            if (iWeponType < 2)
                m_pPlayer->Change_State(CPlayer::OH_IDLE);
            else
                m_pPlayer->Change_State(CPlayer::TH_IDLE);
        }
    }

    m_pPlayer->Set_RimLightColor(m_vRimLightColor);
}

void CState_Player_Teleport::End_State()
{
    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.f);
    m_pPlayer->Set_RimLightColor(m_vRimLightColor);

    m_pSteppingStone = nullptr;
}

_bool CState_Player_Teleport::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_TeleportEnd);
}

CState_Player_Teleport* CState_Player_Teleport::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Teleport* pInstance = new CState_Player_Teleport(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Teleport"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Teleport::Free()
{
    __super::Free();
}