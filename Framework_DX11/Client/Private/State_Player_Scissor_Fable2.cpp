#include "stdafx.h"
#include "State_Player_Scissor_Fable2.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Weapon_Scissor.h"
#include "Weapon_Scissor_Handle.h"

#include "Effect_Manager.h"

CState_Player_Scissor_Fable2::CState_Player_Scissor_Fable2(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Scissor_Fable2::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ScissorCombos3 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_FableArts_Combo_ScissorSword3", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 175;
    m_iSeperateFrame = 1;
    m_iCombineFrame = 50;

    m_iColliderStartFrame_Left[0] = 25;
    m_iColliderEndFrame_Left[0] = 27;
    m_iColliderStartFrame_Left[1] = 39;
    m_iColliderEndFrame_Left[1] = 44;

    m_iColliderStartFrame_Right[0] = 21;
    m_iColliderEndFrame_Right[0] = 24;
    m_iColliderStartFrame_Right[1] = 29;
    m_iColliderEndFrame_Right[1] = 36;

    m_iColliderStartFrame = 55;
    m_iColliderEndFrame = 60;

    m_iStateNum = iStateNum;

    m_iEffectSeperateStartFrame = 19;
    m_iEffectSeperateEndFrame = 47;
    m_iEffectCombineStartFrame = 53;
    m_iEffectCombineEndFrame = 63;

    return S_OK;
}

HRESULT CState_Player_Scissor_Fable2::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_ScissorCombos3, false, 0.f);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    m_iComboIndex = 0;

    m_pPlayer->Decrease_Region();

    m_pPlayer->Set_WeaponStrength(ATK_STRONG);

    m_isActiveSeperateEffect = m_isActiveCombineEffect = false;
    m_isActiveFableEffect[0] = m_isActiveFableEffect[1] = false;

    return S_OK;
}

void CState_Player_Scissor_Fable2::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame < m_iChangeFrame)
    {
        if (m_pPlayer->Key_Tab(KEY::LBUTTON))
        {
            m_isInputLButton = true;
            m_isInputRButton = false;
        }
        else if (m_pPlayer->Key_Tab(KEY::RBUTTON))
        {
            m_isInputRButton = true;
            m_isInputLButton = false;
            m_fRButtonTime = 0.f;
        }
        else if (m_pPlayer->Key_Hold(KEY::RBUTTON))
        {
            m_fRButtonTime += fTimeDelta;
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

    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if (m_isInputLButton)
            m_pPlayer->Change_State(CPlayer::SCISSOR_LATTACK0);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE0);
            else
                m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK0);
        }
    }

    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Collider();
    Control_Effect(iFrame);
}


void CState_Player_Scissor_Fable2::End_State()
{
    m_pPlayer->DeActive_CurretnWeaponCollider();
    m_pPlayer->Combine_Scissor();
}

_bool CState_Player_Scissor_Fable2::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_ScissorCombos3);
}

void CState_Player_Scissor_Fable2::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iAnimation_ScissorCombos3 != m_pPlayer->Get_CurrentAnimIndex())
        return;

    if (m_iCombineFrame >= iFrame)
    {
        _bool isOnLeftCollider[2] = { false, false };
        _bool isOnRightCollider[2] = { false, false };

        for (_uint i = 0; i < 2; ++i)
        {

            if (m_iColliderStartFrame_Left[i] <= iFrame && iFrame <= m_iColliderEndFrame_Left[i])
            {
                isOnLeftCollider[i] = true;
                m_pPlayer->Active_CurrentWeaponCollider(3.f, 1);
            }
            else if (!isOnLeftCollider[0] && !isOnLeftCollider[1])
            {
                m_pPlayer->DeActive_CurretnWeaponCollider(1);
            }

            if (m_iColliderStartFrame_Right[i] <= iFrame && iFrame <= m_iColliderEndFrame_Right[i])
            {
                isOnRightCollider[i] = true;
                m_pPlayer->Active_CurrentWeaponCollider(3.f, 0);
            }
            else if (!isOnRightCollider[0] && !isOnRightCollider[1])
            {
                m_pPlayer->DeActive_CurretnWeaponCollider(0);
            }
        }
    }
    else
    {
        if (m_iColliderStartFrame <= iFrame && iFrame <= m_iColliderEndFrame)
        {
            m_pPlayer->Active_CurrentWeaponCollider(3.f);
        }
        else
        {
            m_pPlayer->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_Player_Scissor_Fable2::Control_Effect(_int iFrame)
{
    if (!m_isActiveSeperateEffect && m_iEffectSeperateStartFrame <= iFrame)
    {
        m_pPlayer->Active_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_LINKSLASH1, true, 0);
        m_pPlayer->Active_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_LINKSLASH1, true, 1);
        m_isActiveSeperateEffect = true;
    }
    else if (m_isActiveSeperateEffect && m_iEffectSeperateEndFrame < iFrame)
    {
        m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_LINKSLASH1, 0);
        m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor_Handle::EFFECT_LINKSLASH1, 1);
    }

    if (!m_isActiveCombineEffect && m_iEffectCombineStartFrame <= iFrame)
    {
        m_pPlayer->Active_WeaponEffect(CWeapon_Scissor::EFFECT_LINKSLASH);
        m_isActiveCombineEffect = true;
    }
    else if (m_isActiveCombineEffect && m_iEffectCombineEndFrame < iFrame)
    {
        m_pPlayer->DeActive_WeaponEffect(CWeapon_Scissor::EFFECT_LINKSLASH);
    }

    if (!m_isActiveFableEffect[0] && iFrame > 15)
    {
        const _Matrix* SocketMatrix = m_pPlayer->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");

        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Rapier_StormStab_First_Ready"), m_pPlayer->Get_Transform()->Get_WorldMatrix_Ptr(),
            SocketMatrix);

        SocketMatrix = m_pPlayer->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_L");
        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Rapier_StormStab_First_Ready"), m_pPlayer->Get_Transform()->Get_WorldMatrix_Ptr(),
            SocketMatrix);

        m_isActiveFableEffect[0] = true;
    }
    else if (!m_isActiveFableEffect[1] && iFrame > 53)
    {
        const _Matrix* SocketMatrix = m_pPlayer->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");

        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Rapier_StormStab_Second_Ready"), m_pPlayer->Get_Transform()->Get_WorldMatrix_Ptr(),
            SocketMatrix);

        m_isActiveFableEffect[1] = true;
    }
}

CState_Player_Scissor_Fable2* CState_Player_Scissor_Fable2::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Scissor_Fable2* pInstance = new CState_Player_Scissor_Fable2(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Scissor_Fable2"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Scissor_Fable2::Free()
{
    __super::Free();
}