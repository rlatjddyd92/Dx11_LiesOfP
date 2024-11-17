#include "stdafx.h"
#include "State_Player_ChangeWeapon.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"

CState_Player_ChangeWeapon::CState_Player_ChangeWeapon(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_ChangeWeapon::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ChangeWeapon[0] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_EquipWeapon");
    m_iAnimation_ChangeWeapon[1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_EquipWeapon");

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_ChangeWeapon::Start_State(void* pArg)
{
    m_iChangeWeaponType = m_pPlayer->Change_WeaponType();
    if(m_iChangeWeaponType < 2)
        m_pPlayer->Change_Animation(m_iAnimation_ChangeWeapon[0], true, 0.2f);
    else
        m_pPlayer->Change_Animation(m_iAnimation_ChangeWeapon[1], true, 0.2f);

    m_isChangeWeapon = false;

    return S_OK;
}

void CState_Player_ChangeWeapon::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (!m_isChangeWeapon)
    {
        if (m_iAnimation_ChangeWeapon[0] == m_pPlayer->Get_CurrentAnimIndex())
        {
            if (iFrame == 10)
            {
                m_pPlayer->Change_Weapon();
                m_isChangeWeapon = true;
            }
        }
        else if (m_iAnimation_ChangeWeapon[1] == m_pPlayer->Get_CurrentAnimIndex())
        {
            if (iFrame == 20)
            {
                m_pPlayer->Change_Weapon();
                m_isChangeWeapon = true;
            }
        }
    }

    if (*m_pIsEndAnim)
    {
        if (m_iChangeWeaponType < 2)
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        else
            m_pPlayer->Change_State(CPlayer::TH_IDLE);
    }
}

void CState_Player_ChangeWeapon::End_State()
{
}

CState_Player_ChangeWeapon* CState_Player_ChangeWeapon::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_ChangeWeapon* pInstance = new CState_Player_ChangeWeapon(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_ChangeWeapon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_ChangeWeapon::Free()
{
    __super::Free();
}