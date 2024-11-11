#include "stdafx.h"
#include "State_CarcassBigA_Paralize.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Paralize::CState_CarcassBigA_Paralize(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Paralize::Initialize(_uint iStateNum)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    return S_OK;
}

HRESULT CState_CarcassBigA_Paralize::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(27, true);;


    return S_OK;
}

void CState_CarcassBigA_Paralize::Update(_float fTimeDelta)
{
    m_fParalizeDuration += fTimeDelta;

    if (m_fParalizeDuration <= m_fParalizeTime)
        m_pMonster->Change_State(CCarcassBigA::IDLE);

}

void CState_CarcassBigA_Paralize::End_State()
{
}

CState_CarcassBigA_Paralize* CState_CarcassBigA_Paralize::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum)
{
    CState_CarcassBigA_Paralize* pInstance = new CState_CarcassBigA_Paralize(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Paralize"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Paralize::Free()
{
    __super::Free();
}