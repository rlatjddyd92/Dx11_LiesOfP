#include "stdafx.h"
#include "State_CarcassBigA_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Walk::CState_CarcassBigA_Walk(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Walk::Initialize(_uint iStateNum)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    return S_OK;
}

HRESULT CState_CarcassBigA_Walk::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(33, true);;


    return S_OK;
}

void CState_CarcassBigA_Walk::Update(_float fTimeDelta)
{
    m_pMonster->Look_Player();

    //m_pMonster->Get_Transform()->Go_Straight(fTimeDelta);

    if (m_pMonster->Calc_Distance_XZ() <= 5.f)
    {
        m_pMonster->Change_State(CCarcassBigA::IDLE);
    }
}

void CState_CarcassBigA_Walk::End_State()
{
}

CState_CarcassBigA_Walk* CState_CarcassBigA_Walk::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
    CState_CarcassBigA_Walk* pInstance = new CState_CarcassBigA_Walk(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Walk::Free()
{
    __super::Free();
}