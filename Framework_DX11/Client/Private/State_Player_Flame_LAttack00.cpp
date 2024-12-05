#include "stdafx.h"
#include "State_Player_Flame_LAttack00.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"

CState_Player_Flame_LAttack00::CState_Player_Flame_LAttack00(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Flame_LAttack00::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_FlameNA3 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_FlameSword_NA3", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 55;
    m_iStateNum = iStateNum;

    m_iColliderStartFrame = 33;
    m_iColliderEndFrame = 38;

    return S_OK;
}

HRESULT CState_Player_Flame_LAttack00::Start_State(void* pArg)
{
    if (m_pFsm->Get_PrevState() == CPlayer::TH_IDLE)
        m_pPlayer->Change_Animation(m_iAnimation_FlameNA3, false, 0.f);
    else
        m_pPlayer->Change_Animation(m_iAnimation_FlameNA3, false, 0.5f, 7);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_iColliderStartFrame = 33;
    m_iColliderEndFrame = 38;

    m_isPlaySound = false;

    return S_OK;
}

void CState_Player_Flame_LAttack00::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame < m_iChangeFrame)
    {
        if (KEY_TAP(KEY::LBUTTON))
        {
            m_isInputLButton = true;
            m_isInputRButton = false;
        }
        else if (KEY_TAP(KEY::RBUTTON))
        {
            m_isInputRButton = true;
            m_isInputLButton = false;
            m_fRButtonTime = 0.f;
        }
        else if (KEY_HOLD(KEY::RBUTTON))
        {
            m_fRButtonTime += fTimeDelta;
        }
    }

    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if (m_isInputLButton)
            m_pPlayer->Change_State(CPlayer::FLAME_LATTACK1);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::FLAME_CHARGE1);
            else
                m_pPlayer->Change_State(CPlayer::FLAME_RATTACK1);
        }
    }
    else if (iFrame >= 100)
    {
        if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D) || KEY_HOLD(KEY::A))
        {
            m_pPlayer->Change_State(CPlayer::OH_RUN);
        }
    }
    else if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }

    Control_Collider();
    Control_Sound();
}

void CState_Player_Flame_LAttack00::End_State()
{
    m_pPlayer->DeActive_CurretnWeaponCollider();

}

_bool CState_Player_Flame_LAttack00::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_FlameNA3);
}

void CState_Player_Flame_LAttack00::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iColliderStartFrame <= iFrame && iFrame <= m_iColliderEndFrame)
        m_pPlayer->Active_CurrentWeaponCollider();
    else
        m_pPlayer->DeActive_CurretnWeaponCollider();
}

void CState_Player_Flame_LAttack00::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if ((iFrame == m_iColliderStartFrame || iFrame == m_iColliderStartFrame + 1) && !m_isPlaySound)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Glaive_P_B_M_01.wav"));
        m_isPlaySound = true;
    }
}

CState_Player_Flame_LAttack00* CState_Player_Flame_LAttack00::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Flame_LAttack00* pInstance = new CState_Player_Flame_LAttack00(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Flame_LAttack00::Free()
{
    __super::Free();
}