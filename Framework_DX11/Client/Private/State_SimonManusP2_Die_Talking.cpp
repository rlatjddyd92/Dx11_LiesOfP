#include "stdafx.h"
#include "State_SimonManusP2_Die_Talking.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"
#include "GameInterface_Controller.h"

CState_SimonManusP2_Die_Talking::CState_SimonManusP2_Die_Talking(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}
HRESULT CState_SimonManusP2_Die_Talking::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_SimonManusP2_Die_Talking::Start_State(void* pArg)
{
    m_pMonster->Get_RigidBody()->Set_GloblePose(_Vec3(0.f, 0.f, 0.f));
    m_pMonster->Get_Transform()->Rotation(0.f, 50.f, 0.f);
    _uint iAnimation_Change = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Die_Idle", 0.5f);

    m_pMonster->Change_Animation(iAnimation_Change, true, 0.1f, 0);
    GET_GAMEINTERFACE->Show_Script_Npc_Talking(NPC_SCRIPT::SCR_MANUS);
    return S_OK;
}

void CState_SimonManusP2_Die_Talking::Update(_float fTimeDelta)
{
    if (GET_GAMEINTERFACE->IsEndTalk_WithNPC(NPC_SCRIPT::SCR_MANUS) && m_bStartDissolve == false)
    {
        m_bStartDissolve = true;
        m_pMonster->Start_StartDisslove();
    }
}

void CState_SimonManusP2_Die_Talking::End_State()
{
}

CState_SimonManusP2_Die_Talking* CState_SimonManusP2_Die_Talking::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Die_Talking* pInstance = new CState_SimonManusP2_Die_Talking(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Die_Talking"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CState_SimonManusP2_Die_Talking::Free()
{
    __super::Free();
}