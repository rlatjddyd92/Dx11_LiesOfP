#include "stdafx.h"
#include "State_Player_ItemGet.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "GameInterface_Controller.h"

#include "Item_Dropped.h"

CState_Player_ItemGet::CState_Player_ItemGet(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_ItemGet::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_OH_ItemGet = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Prop_O_ItemGet", 2.f);
    m_iAnimation_TH_ItemGet = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Prop_T_ItemGet", 2.f);

    m_iStateNum = iStateNum;

    m_iItemGetFrame = 50;

    return S_OK;
}

HRESULT CState_Player_ItemGet::Start_State(void* pArg)
{
    m_pDroppedItem = static_cast<CItem_Dropped*>(pArg);
    if (!m_pDroppedItem)
        return E_FAIL;

    _uint iWeponType = m_pPlayer->Get_WeaponType();

    if (iWeponType < 2)
        m_pPlayer->Change_Animation(m_iAnimation_OH_ItemGet, false, 0.f);
    else
        m_pPlayer->Change_Animation(m_iAnimation_TH_ItemGet, false, 0.f);

    m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_OJ_FX_Hidden_Item_Move_01.wav"));

    m_isItemGet = false;

    return S_OK;
}

void CState_Player_ItemGet::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (!m_isItemGet && iFrame >= m_iItemGetFrame)
    {    
        // 아이템을 획득하여 인벤에 넣는 함수 
        GET_GAMEINTERFACE->AddNewItem_Inven(m_pDroppedItem->Get_ItemIndex(), 1);
        // 아이템의 인덱스는 아래 문서 참조 
        // Client/Bin/DataFiles/Item_Spec_Data.xlsx

        m_pDroppedItem->Set_Dead(true);
        m_isItemGet = true;
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

void CState_Player_ItemGet::End_State()
{
    m_pDroppedItem = nullptr;
}

_bool CState_Player_ItemGet::End_Check()
{
    _uint iWeponType = m_pPlayer->Get_WeaponType();

    if (iWeponType < 2) 
        return m_pPlayer->Get_EndAnim(m_iAnimation_OH_ItemGet);
    else
        return m_pPlayer->Get_EndAnim(m_iAnimation_TH_ItemGet);
   
}

CState_Player_ItemGet* CState_Player_ItemGet::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_ItemGet* pInstance = new CState_Player_ItemGet(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_ItemGet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_ItemGet::Free()
{
    __super::Free();
}