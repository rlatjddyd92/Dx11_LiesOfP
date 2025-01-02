#include "stdafx.h"
#include "State_Player_SophiaHandEnd.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Sophia.h"
#include "Butterfly.h"

CState_Player_SophiaHandEnd::CState_Player_SophiaHandEnd(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_SophiaHandEnd::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_SophiaHandEnd = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Idle_SophiaDead_Cine", 1.f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_SophiaHandEnd::Start_State(void* pArg)
{
    m_pSophia = dynamic_cast<CSophia*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Sophia"), 0));
    m_pSophia->DeActive_HeartEffect();

    m_pPlayer->Change_Animation(m_iAnimation_SophiaHandEnd, false, 3.f);

    return S_OK;
}

void CState_Player_SophiaHandEnd::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (!m_isActiveDisappearEffect && iFrame >= 30)
    {
        m_pSophia->Active_DisapperEffect();
        m_isActiveDisappearEffect = true;
    }

    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    if (m_bMakeButterfly == false && iFrame >= 1)
    {
        m_bMakeButterfly = true;
        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), TEXT("Prototype_GameObject_Butterfly"));
    }
    if (m_bMakeButterfly2 == false && iFrame >= 20)
    {
        m_bMakeButterfly2 = true;
        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), TEXT("Prototype_GameObject_Butterfly"));
        CGameObject* pButterfly = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), 1);
        pButterfly->Get_Transform()->Rotation(0.f, 155.f, 10.f);

        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), TEXT("Prototype_GameObject_Butterfly"));
        CButterfly* pButterfly2 = static_cast<CButterfly*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), 2));
        pButterfly2->Set_Offset(0.f,-0.2f,0.f);

       m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), TEXT("Prototype_GameObject_Butterfly"));
        CButterfly* pButterfly4 = static_cast<CButterfly*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), 3));
       	pButterfly4->Set_Offset(-0.3f, 0.f, 0.85f);

    }  
    
    if (m_bMakeButterfly3 == false && iFrame >= 30)
    {
        m_bMakeButterfly3 = true;
        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), TEXT("Prototype_GameObject_Butterfly"));
    } 
    
    if (m_bMakeButterfly4 == false && iFrame >= 33)
    {
        m_bMakeButterfly4 = true;
        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), TEXT("Prototype_GameObject_Butterfly"));
        CButterfly* pButterfly3 = static_cast<CButterfly*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), 5));
        pButterfly3->Set_Offset(0.f, 0.1f, 0.f);
        pButterfly3->Get_Transform()->Rotation(0.f, 155.f, 15.f);

        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), TEXT("Prototype_GameObject_Butterfly"));
        CButterfly* pButterfly4 = static_cast<CButterfly*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Butterfly"), 6));
        pButterfly3->Get_Transform()->Rotation(0.f, 155.f, 15.f);
        pButterfly4->Set_Offset(-0.3f, 0.f, 0.85f);
    }

}

void CState_Player_SophiaHandEnd::End_State()
{
    m_pSophia = nullptr;
}

_bool CState_Player_SophiaHandEnd::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_SophiaHandEnd);
}

CState_Player_SophiaHandEnd* CState_Player_SophiaHandEnd::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_SophiaHandEnd* pInstance = new CState_Player_SophiaHandEnd(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_SophiaHandEnd"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_SophiaHandEnd::Free()
{
    __super::Free();
}