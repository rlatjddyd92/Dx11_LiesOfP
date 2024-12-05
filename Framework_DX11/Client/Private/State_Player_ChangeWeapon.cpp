#include "stdafx.h"
#include "State_Player_ChangeWeapon.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"

// 24-12-05 김성용
// 무기 락 적용 
#include "GameInterface_Controller.h"

CState_Player_ChangeWeapon::CState_Player_ChangeWeapon(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_ChangeWeapon::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_ChangeWeapon[0] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_EquipWeapon", 2.f);
    m_iAnimation_ChangeWeapon[1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_T_EquipWeapon", 2.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);
    
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_ChangeWeapon::Start_State(void* pArg)
{
    m_iChangeWeaponType = m_pPlayer->Change_WeaponType();
    if(m_iChangeWeaponType < 2)
        m_pPlayer->Change_Animation(m_iAnimation_ChangeWeapon[0], false, 0.2f);
    else
        m_pPlayer->Change_Animation(m_iAnimation_ChangeWeapon[1], false, 0.2f);

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

    if (End_Check())
    {
        if (m_iChangeWeaponType < 2)
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        else
            m_pPlayer->Change_State(CPlayer::TH_IDLE);

        // 24-12-05 김성용
        // 무기 락 적용 
        GET_GAMEINTERFACE->SetWeaponLock(false);
    }
}

void CState_Player_ChangeWeapon::End_State()
{
}

_bool CState_Player_ChangeWeapon::End_Check()
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((m_iAnimation_ChangeWeapon[0]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_ChangeWeapon[0]);
    }
    else if ((m_iAnimation_ChangeWeapon[1]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_ChangeWeapon[1]);
    }
    else
    {

    }
        //애니메이션 번호와 일치하지 않는?다
    
    return bEndCheck;
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