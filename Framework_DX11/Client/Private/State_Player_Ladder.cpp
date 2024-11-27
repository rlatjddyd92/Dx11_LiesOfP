#include "stdafx.h"
#include "State_Player_Ladder.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"

CState_Player_Ladder::CState_Player_Ladder(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Ladder::Initialize(_uint iStateNum, void* pArg)
{
    // 올라가유
    m_iAnimation_Ladder[LADDER_UP][0] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Start_Up", 2.f);
    m_iAnimation_Ladder[LADDER_UP][1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Up_Left", 2.f);
    m_iAnimation_Ladder[LADDER_UP][2] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Up_Right", 2.f);
    m_iAnimation_Ladder[LADDER_UP][3] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_End_Up_Left", 2.f);
    m_iAnimation_Ladder[LADDER_UP][4] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_End_Up_Right", 2.f);

    // 내려가유
    m_iAnimation_Ladder[LADDER_DOWN][0] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Start_Down", 2.f);
    m_iAnimation_Ladder[LADDER_DOWN][1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Down_Left", 2.f);
    m_iAnimation_Ladder[LADDER_DOWN][2] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Down_Right", 2.f);
    m_iAnimation_Ladder[LADDER_DOWN][3] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_End_Down_Left", 2.f);
    m_iAnimation_Ladder[LADDER_DOWN][4] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_End_Down_Right", 2.f);

    // 기다려유
    m_iAnimation_LadderIdle[0] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Idle_Left", 2.f);
    m_iAnimation_LadderIdle[1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Idle_Right", 2.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    m_iSoundFrame = 30;

    return S_OK;
}

HRESULT CState_Player_Ladder::Start_State(void* pArg)
{
    _Vec3* vLadderPos = static_cast<_Vec3*>(pArg);
    Choice_UpDown(*vLadderPos);

    
    m_pPlayer->Change_Animation(m_iAnimation_Ladder[m_eUpDownType][0], false, 0.f);

    m_pPlayer->Get_RigidBody()->Set_IsOnCell(false);

    return S_OK;
}

void CState_Player_Ladder::Update(_float fTimeDelta)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();

    if (KEY_TAP(KEY::W))
    {
        m_isInputW = true;
        m_isInputS = false;
    }
    else if (KEY_TAP(KEY::S))
    {
        m_isInputS = true;
        m_isInputW = false;
    }



    if (m_pPlayer->Get_EndAnim(iCurAnim))
    {
        ++m_iHandIndex;

        if (m_isInputW)
        {
            m_iHandIndex = ((m_iHandIndex + 1) % 2) + 1;
            m_eUpDownType = LADDER_UP;
            m_pPlayer->Change_Animation(m_iAnimation_Ladder[m_eUpDownType][m_iHandIndex], false, 0.f);
        }
        else if (m_isInputS)
        {
            m_iHandIndex = ((m_iHandIndex + 1) % 2) + 1;
            m_eUpDownType = LADDER_DOWN;
            m_pPlayer->Change_Animation(m_iAnimation_Ladder[m_eUpDownType][m_iHandIndex], false, 0.f);
        }
        else
        {
            //m_iHandIndex = ((m_iHandIndex + 1) % 2);
            m_pPlayer->Change_Animation(m_iAnimation_LadderIdle[m_iHandIndex], true, 0.f);
        }

        m_isInputW = false;
        m_isInputS = false;
    }
    else if (iCurAnim == m_iAnimation_LadderIdle[0]
        || iCurAnim == m_iAnimation_LadderIdle[1])
    {
        if (m_isInputW)
        {
            m_iHandIndex = ((m_iHandIndex+1) % 2) + 1;
            m_eUpDownType = LADDER_UP;
            m_pPlayer->Change_Animation(m_iAnimation_Ladder[m_eUpDownType][m_iHandIndex], false, 0.f);
        }
        else if (m_isInputS)
        {
            m_iHandIndex = ((m_iHandIndex + 1) % 2) + 1;
            m_eUpDownType = LADDER_DOWN;
            m_pPlayer->Change_Animation(m_iAnimation_Ladder[m_eUpDownType][m_iHandIndex], false, 0.f);
        }

        m_isInputW = false;
        m_isInputS = false;
    }

}

void CState_Player_Ladder::End_State()
{
    m_pPlayer->Get_RigidBody()->Set_IsOnCell(true);
}

void CState_Player_Ladder::Choice_UpDown(_Vec3 vLadderPos)
{
    _Vec3 vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

    _float fHeightDiff = vLadderPos.y - vPlayerPos.y;
    if (fHeightDiff > 0.f)
    {
        m_eUpDownType = LADDER_UP;
        m_iHandIndex = 0;
    }
    else
    {
        m_eUpDownType = LADDER_DOWN;
        m_iHandIndex = 1;
    }
}

_bool CState_Player_Ladder::End_Check()
{
    return true;
}

void CState_Player_Ladder::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if ((iFrame == m_iSoundFrame || iFrame == m_iSoundFrame + 1) && !m_isPlaySound)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_FX_Rapier_1H_H_FableArts_Parry_01.wav"));
        m_isPlaySound = true;
    }
    else
    {
        m_isPlaySound = false;
    }
}

CState_Player_Ladder* CState_Player_Ladder::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Ladder* pInstance = new CState_Player_Ladder(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Ladder"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Ladder::Free()
{
    __super::Free();
}