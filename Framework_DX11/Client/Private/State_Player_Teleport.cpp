#include "stdafx.h"
#include "State_Player_Teleport.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "SteppingStone.h"
#include "Stargazer.h"
#include "Effect_Manager.h"
#include "GameInterface_Controller.h"

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
    TELEPORT_DESC* pDesc = static_cast<TELEPORT_DESC*>(pArg);
    if (nullptr == pDesc)
    {
        m_pSteppingStone = nullptr;
        m_isDieTeleport = false;
        m_iStargazerCellNum = -1;
    }
    else
    {
        m_pSteppingStone = pDesc->pSteppingStone;
        if (nullptr != m_pSteppingStone)
            Safe_AddRef(m_pSteppingStone);

        m_isDieTeleport = pDesc->isDie;
        m_iStargazerCellNum = pDesc->iCellNum;
    }

    if (m_pSteppingStone)
    {
        m_pPlayer->Change_Animation(m_iAnimation_TeleportStart, false, 0.f);

        m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);

        m_isEnd_Teleport = false;
        m_isAppearStartEffect = false;

        m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Teleport_Start_VolumeUP.wav"));

        m_fDissloveRatio = 0.f;

    }
    else if (m_isDieTeleport)
    {
        m_pStarGazer = m_pPlayer->Find_Stargazer();
        if (nullptr != m_pStarGazer)
            Safe_AddRef(m_pStarGazer);

        m_fDissloveRatio = 1.f;
        m_vRimLightColor.z = 1.f;
        m_vRimLightColor.w = 0.1f;

        m_pPlayer->SetUp_Die();
    }
    else
    {
        m_pStarGazer = m_pPlayer->Find_Stargazer(m_iStargazerCellNum);
        if (nullptr != m_pStarGazer)
            Safe_AddRef(m_pStarGazer);

        m_pPlayer->Change_Animation(m_iAnimation_TeleportStart, false, 0.f); 
        m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);

        m_isEnd_Teleport = false;
        m_isAppearStartEffect = false;

        m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Teleport_Start_VolumeUP.wav"));

        m_fDissloveRatio = 0.f;
    }

    m_isFadeIn = false;
    m_isFadeOut = false;

    m_pPlayer->Disappear_Weapon();

    return S_OK;
}

void CState_Player_Teleport::Update(_float fTimeDelta)
{
    if (m_pSteppingStone)
    {
        Update_SteppingStone(fTimeDelta);
    }
    else if (!m_isDieTeleport)
    {
        Update_Stargazer(fTimeDelta);
    }
    else
    {
        Update_Die(fTimeDelta);
    }

    m_pPlayer->Set_DissloveRatio(m_fDissloveRatio);
    m_pPlayer->Set_RimLightColor(m_vRimLightColor);
}

void CState_Player_Teleport::End_State()
{
    m_fDissloveRatio = 0.f;
    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.f);
    m_pPlayer->Set_DissloveRatio(m_fDissloveRatio);
    m_pPlayer->Set_RimLightColor(m_vRimLightColor);

    if (m_pStarGazer)
    {
        Safe_Release(m_pStarGazer);
    }

    if (m_pSteppingStone)
    {
        Safe_Release(m_pSteppingStone);
    }

    m_pPlayer->Reset_Die();
}

_bool CState_Player_Teleport::End_Check()
{
    m_pPlayer->Appear_Weapon();
    return m_pPlayer->Get_EndAnim(m_iAnimation_TeleportEnd);
}

void CState_Player_Teleport::Update_SteppingStone(_float fTimeDelta)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _int iFrame = m_pPlayer->Get_Frame();

    if (iCurAnim == m_iAnimation_TeleportStart)
    {
        if (!m_isFadeOut && iFrame >= 110)
        {
            GET_GAMEINTERFACE->Fade_Out(TEXT(""), TEXT(""));
            GET_GAMEINTERFACE->UIPart_Off();
            GET_GAMEINTERFACE->Reservate_Region_Info(m_iStargazerCellNum);
            m_isFadeOut = true;
        }
        else if (iFrame >= 150)
        {
            m_fDissloveRatio = 1.f;
            m_pSteppingStone->Change_Player_Pos();
            m_isEnd_Teleport = true;
        }
        else if (m_isEnd_Teleport)
        {
            m_isEnd_Teleport = false;
            m_pPlayer->Change_Animation(m_iAnimation_TeleportEnd, false, 0.3f);

            GET_GAMEINTERFACE->Fade_In(0.7f);
            GET_GAMEINTERFACE->UIPart_On();
            m_isFadeOut = true;

            _Vec3 vPos = (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
            //vPos.y += 1.f;
            CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Teleport_Arrive"), vPos);
            m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_MT_Teleport_End_VolumeUP.wav"));
        }
        else if (iFrame > 80)
        {
            m_fDissloveRatio += fTimeDelta;
            m_vRimLightColor.z = max(m_vRimLightColor.z + fTimeDelta, 1.f);
            m_vRimLightColor.w = max(m_vRimLightColor.w - 0.6f * fTimeDelta, 0.1f);

            if (!m_isAppearStartEffect)
            {
                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Teleport_Depart"), (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION));
                m_pPlayer->On_DissolveEffect(CPlayer::DISSOLVE_DEAD, true);
                m_isAppearStartEffect = true;
            }
        }
    }
    else if (iCurAnim == m_iAnimation_TeleportEnd)
    {
        if (iFrame > 10)
        {
            m_fDissloveRatio -= 1.f * fTimeDelta;
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
}

void CState_Player_Teleport::Update_Stargazer(_float fTimeDelta)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _int iFrame = m_pPlayer->Get_Frame();

    if (iCurAnim == m_iAnimation_TeleportStart)
    {
        if (!m_isFadeOut && iFrame >= 110)
        {
            GET_GAMEINTERFACE->Fade_Out(TEXT(""), TEXT(""));
            GET_GAMEINTERFACE->UIPart_Off();
            GET_GAMEINTERFACE->Reservate_Region_Info(m_iStargazerCellNum);

            m_isFadeOut = true;
        }
        if (iFrame >= 150)
        {
            Move_To_Stargazer();

            m_fDissloveRatio = 1.f;
            m_isEnd_Teleport = true;
        }
        else if (m_isEnd_Teleport)
        {
            m_pPlayer->Set_IsRespawnMonster(true);

            m_isEnd_Teleport = false;
            m_pPlayer->Change_Animation(m_iAnimation_TeleportEnd, false, 0.3f);

            GET_GAMEINTERFACE->Fade_In(0.7f);
            GET_GAMEINTERFACE->UIPart_On();
            

            m_isFadeOut = true;

            _Vec3 vPos = (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
            //vPos.y += 1.f;
            CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Teleport_Arrive"), vPos);
            m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_MT_Teleport_End_VolumeUP.wav"));
        }
        else if (iFrame > 80)
        {
            m_fDissloveRatio += fTimeDelta;
            m_vRimLightColor.z = max(m_vRimLightColor.z + fTimeDelta, 1.f);
            m_vRimLightColor.w = max(m_vRimLightColor.w - 0.6f * fTimeDelta, 0.1f);

            if (!m_isAppearStartEffect)
            {
                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Teleport_Depart"), (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION));
                m_pPlayer->On_DissolveEffect(CPlayer::DISSOLVE_DEAD, true);
                m_isAppearStartEffect = true;
            }
        }
    }
    else if (iCurAnim == m_iAnimation_TeleportEnd)
    {
        if (iFrame > 10)
        {
            m_fDissloveRatio -= 1.f * fTimeDelta;
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
}

void CState_Player_Teleport::Update_Die(_float fTimeDelta)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _int iFrame = m_pPlayer->Get_Frame();


    // 25-01-04 김성용
    // 플레이어 사망 UI 추가 
    if (GET_GAMEINTERFACE->IsPlayerDead_UI_NowEnd() == true)
    {
        m_pPlayer->Change_Animation(m_iAnimation_TeleportEnd, false, 0.2f);
        Move_To_Stargazer();
        GET_GAMEINTERFACE->SetPlayerDead_UI_NowEnd_False();
    }

    if (iCurAnim == m_iAnimation_TeleportEnd)
    {
        if (iFrame > 3)
        {
            if (!m_isFadeIn)
            {
                m_pPlayer->Set_IsRespawnMonster(true);

                //GET_GAMEINTERFACE->Fade_In(0.9f);

                _Vec3 vPos = (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
                //vPos.y += 1.f;
                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Teleport_Arrive"), vPos);
                m_pPlayer->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_MT_Teleport_End_VolumeUP.wav"));
                m_isFadeIn = true;
            }
        }
        if (iFrame > 10)
        {
            m_fDissloveRatio -= 1.f * fTimeDelta;
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
}

void CState_Player_Teleport::Move_To_Stargazer()
{
    _Matrix StargazerWorldMatrix = m_pStarGazer->Get_Transform()->Get_WorldMatrix();
    _Vec4 vStargazerLook = StargazerWorldMatrix.Forward();
    vStargazerLook.Normalize();

    _Vec4 vTargetPos = StargazerWorldMatrix.Translation() - vStargazerLook;

    m_pPlayer->Get_RigidBody()->Set_GloblePose((_Vec3)vTargetPos);
    m_pPlayer->Get_Navigation()->Research_Cell((_Vec3)vTargetPos);
    m_pPlayer->Get_Transform()->LookAt_NoHeight(vTargetPos + vStargazerLook);
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