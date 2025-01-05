#include "stdafx.h"
#include "State_Player_OH_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"

#include "GameInterface_Controller.h"

CState_Player_OH_Idle::CState_Player_OH_Idle(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OH_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Idle = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Idle");
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_OH_Idle::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Idle, true, 0.3f);
    m_pPlayer->Change_Animation_Boundry(m_iAnimation_Idle, true, 3.3f);

    m_fRButtonTime = 0.f;

    return S_OK;
}

void CState_Player_OH_Idle::Update(_float fTimeDelta)
{
    if (m_pPlayer->Key_Hold(KEY::LSHIFT))
    {
        m_pPlayer->Change_State(CPlayer::OH_GUARD);
    }
    else if (m_pPlayer->Key_Hold(KEY::W) || m_pPlayer->Key_Hold(KEY::S) || m_pPlayer->Key_Hold(KEY::D) || m_pPlayer->Key_Hold(KEY::A))
    {
        if(m_pPlayer->Get_IsLockOn())
            m_pPlayer->Change_State(CPlayer::OH_WALK);
        else
            m_pPlayer->Change_State(CPlayer::OH_RUN);
    }
    else if (m_pPlayer->Key_Tab(KEY::SPACE))
    {
        m_pPlayer->Change_State(CPlayer::OH_DASH);
    }
    else if (m_pPlayer->Key_Tab(KEY::LBUTTON))
    {
        if (!m_pPlayer->Get_IsFatal())
        {
            if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_RAPIER)
                m_pPlayer->Change_State(CPlayer::RAPIER_LATTACK0);
            else if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_SCISSOR)
                m_pPlayer->Change_State(CPlayer::SCISSOR_LATTACK0);
        }
        else
        {
            if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_RAPIER)
                m_pPlayer->Change_State(CPlayer::RAPIER_FATAL);
            else if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_SCISSOR)
                m_pPlayer->Change_State(CPlayer::SCISSOR_FATAL);
        }
    }
    else if (m_pPlayer->Key_Hold(KEY::RBUTTON))
    {
        m_fRButtonTime += fTimeDelta;
        if (m_fRButtonTime > 0.15f)
        {
            if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_RAPIER)
                m_pPlayer->Change_State(CPlayer::RAPIER_CHARGE);
            else if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_SCISSOR)
                m_pPlayer->Change_State(CPlayer::SCISSOR_CHARGE0);
        }
    }
    else if (m_pPlayer->Key_Tab(KEY::F))
    {
        if (m_pPlayer->Check_Region_Fable01())
        {
            if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_RAPIER)
                m_pPlayer->Change_State(CPlayer::RAPIER_FABALE);
            else if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_SCISSOR)
                m_pPlayer->Change_State(CPlayer::SCISSOR_FABAL0);
        }
    }
    else if (m_pPlayer->Key_Tab(KEY::R))
    {
        SPECIAL_ITEM eNow = GET_GAMEINTERFACE->Get_Now_Select_Item();

        if (SPECIAL_ITEM::SP_PULSE_BATTERY == eNow)
        {
            m_pPlayer->Change_State(CPlayer::HEAL);
        }
        else if (SPECIAL_ITEM::SP_GRINDER == eNow)
        {
            m_pPlayer->Change_State(CPlayer::GRINDER); 
        }
        else if (SPECIAL_ITEM::SP_GRANADE == eNow 
            || SPECIAL_ITEM::SP_THERMITE == eNow
            || SPECIAL_ITEM::SP_THROW_BATTERY == eNow)
        {
            m_pPlayer->Change_State(CPlayer::THROW_ITEM);
        }
        else if (SPECIAL_ITEM::SP_RESISTANCE == eNow)
        {
            m_pPlayer->Change_State(CPlayer::DEBUFF_RESISTANCE);
        }
        else if (SPECIAL_ITEM::SP_PURIFICATION == eNow)
        {
            m_pPlayer->Change_State(CPlayer::DEBUFF_RESET);
        }
        else if (SPECIAL_ITEM::SP_TELEPOT == eNow)
        {
            //UI를 띄우자
            // 25-01-05 김성용
            // 회중시계 팝업으로 변경 
            // 팝업의 확인을 눌러야 작동함 
            // 순간이동은 Item_Manager에서 회중시계 사용을 감지하면 진행
            GET_GAMEINTERFACE->Show_ItemUsePopup(TEXT("문페이즈 회중시계"), TEXT("순간 이동을 진행합니다."), true);
        }
        else if (SPECIAL_ITEM::SP_DEAD == eNow)
        {
            //UI를 띄우자
            m_pPlayer->Change_State(CPlayer::DIE);
        }
    }
    else if (m_pPlayer->Key_Tab(KEY::TAPKEY))
    {
        m_pPlayer->Change_State(CPlayer::CHANGEWEP);
    }
    else if (m_pPlayer->Key_Hold(KEY::CTRL))
    {
        if(m_pPlayer->Get_ArmGuage_Ptr()->x >= 10.f)
            m_pPlayer->Change_State(CPlayer::ARM_START);
    }
    
    if (m_pPlayer->Key_Away(KEY::RBUTTON))
    {
        m_fRButtonTime = 0.f;
        if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_RAPIER)
            m_pPlayer->Change_State(CPlayer::RAPIER_RATTACK0);
        else if (m_pPlayer->Get_WeaponType() == CPlayer::WEP_SCISSOR)
            m_pPlayer->Change_State(CPlayer::SCISSOR_RATTACK0);
    }
}

void CState_Player_OH_Idle::End_State()
{
}

CState_Player_OH_Idle* CState_Player_OH_Idle::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OH_Idle* pInstance = new CState_Player_OH_Idle(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_TH_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OH_Idle::Free()
{
    __super::Free();
}