#include "stdafx.h"
#include "State_CarcassBigA_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Die::CState_CarcassBigA_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Die::Initialize(_uint iStateNum)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    return S_OK;
}

HRESULT CState_CarcassBigA_Die::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(10, true);;


    return S_OK;
}

void CState_CarcassBigA_Die::Update(_float fTimeDelta)
{
    if (KEY_AWAY(KEY::S))
        m_pMonster->Change_State(CCarcassBigA::IDLE);

}

void CState_CarcassBigA_Die::End_State()
{
}

CState_CarcassBigA_Die* CState_CarcassBigA_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
    CState_CarcassBigA_Die* pInstance = new CState_CarcassBigA_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Die::Free()
{
    __super::Free();
}