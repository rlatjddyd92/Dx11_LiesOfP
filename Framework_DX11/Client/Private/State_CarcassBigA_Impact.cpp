#include "stdafx.h"
#include "State_CarcassBigA_Impact.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Impact::CState_CarcassBigA_Impact(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Impact::Initialize(_uint iStateNum)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    return S_OK;
}

HRESULT CState_CarcassBigA_Impact::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(5, true);;


    return S_OK;
}

void CState_CarcassBigA_Impact::Update(_float fTimeDelta)
{
    m_fCurrentTime += fTimeDelta;
    if (m_fCurrentTime >= 20.f)
        m_pMonster->Change_State(CCarcassBigA::IDLE);

}

void CState_CarcassBigA_Impact::End_State()
{
    m_fCurrentTime = 0.f;

}

CState_CarcassBigA_Impact* CState_CarcassBigA_Impact::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
    CState_CarcassBigA_Impact* pInstance = new CState_CarcassBigA_Impact(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Impact"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Impact::Free()
{
    __super::Free();
}