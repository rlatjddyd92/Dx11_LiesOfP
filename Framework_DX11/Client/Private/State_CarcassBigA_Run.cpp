#include "stdafx.h"
#include "State_CarcassBigA_Run.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Run::CState_CarcassBigA_Run(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Run::Initialize(_uint iStateNum, void* pArg)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    return S_OK;
}

HRESULT CState_CarcassBigA_Run::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(29, true, 0.f, 0.f);;
    m_pMonster->Look_Player();

    return S_OK;
}

void CState_CarcassBigA_Run::Update(_float fTimeDelta)
{
    m_pMonster->Look_Player();
    m_pMonster->Get_Transform()->Go_Straight(fTimeDelta * m_fRunSpeed);


    if (m_pMonster->Calc_Distance_XZ() <= 5.f)
    {
        m_pMonster->Change_State(CCarcassBigA::IDLE);
    }
}

void CState_CarcassBigA_Run::End_State()
{
}

CState_CarcassBigA_Run* CState_CarcassBigA_Run::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_Run* pInstance = new CState_CarcassBigA_Run(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Run"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Run::Free()
{
    __super::Free();
}