#include "stdafx.h"
#include "State_Player_DebuffResistance.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"

#include "Effect_Manager.h"

CState_Player_DebuffResistance::CState_Player_DebuffResistance(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_DebuffResistance::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Healing = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Item_Healing", 1.8f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_DebuffResistance::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Healing, false, 0.2f);

    m_pPlayer->Disappear_Weapon();

    m_isCreateItem = false;
    m_isActiveItem = false;
    m_isItemEffect = false;

    return S_OK;
}

void CState_Player_DebuffResistance::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame(true);

    if (!m_isCreateItem && iFrame > 10)
    {
        m_isCreateItem = true;
    }
    else if (!m_isActiveItem && (iFrame == 33 || iFrame == 34))
    {
        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Item_Resistance"),
            m_pPlayer->Calc_BoneWorldPos("Bip001-L-Hand"));

        m_pPlayer->Use_DebuffReduceItem();
        m_isActiveItem = true;
    }
    else if (!m_isItemEffect && iFrame > 37)
    {
        m_pPlayer->Active_Effect(CPlayer::EFFECT_ITEM_RESISTANCE);
        m_isItemEffect = true;
    }
    else if (End_Check())
    {
        _uint iWeponType = m_pPlayer->Get_WeaponType();

        if (iWeponType < 2)
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        else
            m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }
}

void CState_Player_DebuffResistance::End_State()
{
    m_pPlayer->Appear_Weapon();
    m_pPlayer->DeActive_Effect(CPlayer::EFFECT_ITEM_RESISTANCE);
    m_pPlayer->Stop_Sound(CPawn::PAWN_SOUND_EFFECT1);
}

_bool CState_Player_DebuffResistance::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Healing);
}

CState_Player_DebuffResistance* CState_Player_DebuffResistance::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_DebuffResistance* pInstance = new CState_Player_DebuffResistance(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_DebuffResistance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_DebuffResistance::Free()
{
    __super::Free();
}