#include "stdafx.h"
#include "State_SimonManusP1_Run.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManusP1.h"

CState_SimonManusP1_Run::CState_SimonManusP1_Run(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_Run::Initialize(_uint iStateNum, void* pArg)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    return S_OK;
}

HRESULT CState_SimonManusP1_Run::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_RUN, true, 0.1f, 0);
    m_pMonster->Look_Player();

    return S_OK;
}

void CState_SimonManusP1_Run::Update(_float fTimeDelta)
{
    _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
    m_pMonster->Get_Transform()->Go_Straight(fTimeDelta * m_fRunSpeed);


    if (m_pMonster->Calc_Distance_XZ() <= 5.f)
    {
        m_pMonster->Change_State(CSimonManusP1::IDLE);
    }
}

void CState_SimonManusP1_Run::End_State()
{
}

CState_SimonManusP1_Run* CState_SimonManusP1_Run::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_Run* pInstance = new CState_SimonManusP1_Run(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_Run"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_Run::Free()
{
    __super::Free();
}