#include "stdafx.h"
#include "State_Training02_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster_Training01.h"

#include "GameInterface_Controller.h"

CState_Training02_Idle::CState_Training02_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_Training02_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Idle = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Idle_C", 2.f);
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Training02_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(m_iAnimation_Idle, true, 0.1f);

    return S_OK;
}

void CState_Training02_Idle::Update(_float fTimeDelta)
{

}

void CState_Training02_Idle::End_State()
{
}

CState_Training02_Idle* CState_Training02_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_Training02_Idle* pInstance = new CState_Training02_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Training02_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Training02_Idle::Free()
{
    __super::Free();
}