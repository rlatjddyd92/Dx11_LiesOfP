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

HRESULT CState_CarcassBigA_Impact::Initialize(_uint iStateNum, void* pArg)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;

    return S_OK;
}

HRESULT CState_CarcassBigA_Impact::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IMPACT, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CarcassBigA_Impact::Update(_float fTimeDelta)
{
    if (*m_pIsEndAnim == true)
        m_pMonster->Change_State(CCarcassBigA::IDLE);

}

void CState_CarcassBigA_Impact::End_State()
{

}

_bool CState_CarcassBigA_Impact::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_IMPACT);
}

CState_CarcassBigA_Impact* CState_CarcassBigA_Impact::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_Impact* pInstance = new CState_CarcassBigA_Impact(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
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