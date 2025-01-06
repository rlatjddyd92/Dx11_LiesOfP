#include "stdafx.h"
#include "State_Player_Scissor_Fable0.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Weapon_Scissor.h"
#include "Weapon_Scissor_Handle.h"

#include "Effect_Manager.h"

CState_Player_Scissor_Fable0::CState_Player_Scissor_Fable0(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Scissor_Fable0::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ScissorCombos1 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_FableArts_Combo_ScissorSword1", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 145;
    m_iComboFrame = 30;
    m_iSeperateFrame = 1;
    m_iCombineFrame = 87;

    m_iColliderStartFrame = 18;
    m_iColliderEndFrame = 22;

    m_iStateNum = iStateNum;

    m_iEffectStartFrame = 10;
    m_iEffectEndFrame = 27;

    m_iSoundFrame = 17;

    return S_OK;
}

HRESULT CState_Player_Scissor_Fable0::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorCombos1, false, 0.1f);

    m_isInputLButton = false;
    m_isInputFButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_isSoundPlay = false;

    m_pPlayer->Decrease_Region();

    m_pPlayer->Set_WeaponStrength(ATK_STRONG);
    m_pPlayer->Get_CurrentWeapon()->Set_DamageAmount(25.f);

    m_isActiveEffect = m_isActiveFableEffect =  false;

    return S_OK;
}

void CState_Player_Scissor_Fable0::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();
    _uint iCurrentAnim = m_pPlayer->Get_Model()->Get_CurrentAnimationIndex();

    if (iFrame < m_iChangeFrame)
    {
        if (m_pPlayer->Key_Tab(KEY::LBUTTON) && m_pPlayer->Get_Player_Stat().vGauge_Stamina.x > 30.f)
        {
            m_isInputLButton = true;
            m_isInputRButton = false;
            m_isInputFButton = false;
        }
        else if (m_pPlayer->Key_Tab(KEY::RBUTTON) && m_pPlayer->Get_Player_Stat().vGauge_Stamina.x > 30.f)
        {
            m_isInputRButton = true;
            m_isInputLButton = false;
            m_isInputFButton = false;
            m_fRButtonTime = 0.f;
        }
        else if (m_pPlayer->Key_Hold(KEY::RBUTTON) && m_pPlayer->Get_Player_Stat().vGauge_Stamina.x > 30.f)
        {
            m_fRButtonTime += fTimeDelta;
        }
        else if (m_pPlayer->Key_Tab(KEY::F))
        {
            m_isInputFButton = true;
            m_isInputLButton = false;
            m_isInputRButton = false;
            m_fRButtonTime = 0.f;
        }
    }

    if (m_iSeperateFrame == iFrame || m_iSeperateFrame + 1 == iFrame)
    {
        m_pPlayer->Seperate_Scissor();
    }
    else if (m_iCombineFrame == iFrame || m_iCombineFrame + 1 == iFrame)
    {
        m_pPlayer->Combine_Scissor();
    }

    if (!m_isInputFButton && m_pPlayer->Check_Region_Fable01())
    {
        if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
        {
            if (m_pPlayer->Key_Hold(KEY::LSHIFT))
            {
                m_pPlayer->Change_State(CPlayer::OH_GUARD);
            }
            else if (m_isInputLButton)
                m_pPlayer->Change_State(CPlayer::SCISSOR_LATTACK0);
            else if (m_isInputRButton)
            {
                if (m_fRButtonTime > 0.15f)
                    m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE0);
                else
                    m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK0);
            }
        }
    }
    else if (m_isInputFButton)
    {
        if (m_iComboFrame < iFrame && iFrame < m_iComboFrame + 10)
        {
            m_pPlayer->Change_State(CPlayer::SCISSOR_FABAL1);
        }
    }

    if (iFrame >= m_iSoundFrame && !m_isSoundPlay)
    {
        m_isSoundPlay = true;
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Glaive_P_B_M_03.wav"));
    }

    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Collider();
    Control_Effect(iFrame);
}

void CState_Player_Scissor_Fable0::End_State()
{
    m_pPlayer->DeActive_CurretnWeaponCollider();
}

_bool CState_Player_Scissor_Fable0::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ScissorCombos1);
}

void CState_Player_Scissor_Fable0::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iColliderStartFrame <= iFrame && iFrame <= m_iColliderEndFrame)
    {
        m_pPlayer->Active_CurrentWeaponCollider(1.f, 0);
        m_pPlayer->Active_CurrentWeaponCollider(1.f, 1);
    }
    else
    {
        m_pPlayer->DeActive_CurretnWeaponCollider(0);
        m_pPlayer->DeActive_CurretnWeaponCollider(1);
    }
}

void CState_Player_Scissor_Fable0::Control_Effect(_int iFrame)
{
    if (!m_isActiveEffect && m_iEffectStartFrame <= iFrame)
    {
        m_pPlayer->Active_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_LINKSLASH1, true, 0);
        m_pPlayer->Active_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_LINKSLASH1, true, 1);
        m_isActiveEffect = true;
    }
    else if (m_isActiveEffect && m_iEffectEndFrame < iFrame)
    {
        m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_LINKSLASH1, 0);
        m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_LINKSLASH1, 1);
    }

    if (!m_isActiveFableEffect && iFrame > 13)
    {
        const _Matrix* SocketMatrix = m_pPlayer->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");

        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Rapier_StormStab_First_Ready"), m_pPlayer->Get_Transform()->Get_WorldMatrix_Ptr(),
            SocketMatrix);

        SocketMatrix = m_pPlayer->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_L");
        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Rapier_StormStab_First_Ready"), m_pPlayer->Get_Transform()->Get_WorldMatrix_Ptr(),
            SocketMatrix);

        m_isActiveFableEffect = true;
    }
}

CState_Player_Scissor_Fable0* CState_Player_Scissor_Fable0::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Scissor_Fable0* pInstance = new CState_Player_Scissor_Fable0(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Scissor_Fable0"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Scissor_Fable0::Free()
{
    __super::Free();
}