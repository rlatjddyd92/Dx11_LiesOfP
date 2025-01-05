#include "stdafx.h"
#include "State_Player_ItemGet.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "GameInterface_Controller.h"

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

    return S_OK;
}

HRESULT CState_Player_ItemGet::Start_State(void* pArg)
{
    _uint iWeponType = m_pPlayer->Get_WeaponType();

    if (iWeponType < 2)
        m_pPlayer->Change_Animation(m_iAnimation_OH_ItemGet, false, 0.f);
    else
        m_pPlayer->Change_Animation(m_iAnimation_TH_ItemGet, false, 0.f);

    m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_OJ_FX_Hidden_Item_Move_01.wav"));

    return S_OK;
}

void CState_Player_ItemGet::Update(_float fTimeDelta)
{
    if (End_Check())
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
    m_pPlayer->Get_RigidBody()->Set_IsOnCell(true);

    // 25-01-05 김성용
    // 아이템을 획득하여 인벤에 넣는 함수 
    GET_GAMEINTERFACE->AddNewItem_Inven(331, 1); // 아이템 인덱스, 수량 입력 (디폴트 1개)
    // 현재 테스트용으로 미사용 아이템인 코어(331)을 얻도록 설정 
    // 아이템의 인덱스는 아래 문서 참조 
    // Client/Bin/DataFiles/Item_Spec_Data.xlsx
    // 문서의 0번째 열의 숫자 사용하기 

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