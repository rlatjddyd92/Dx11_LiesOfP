#include "stdafx.h"
#include "State_Training_Grogy.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster_Training01.h"

#include "GameInterface_Controller.h"

CState_Training_Grogy::CState_Training_Grogy(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_Training_Grogy::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Grogy[0] = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Groggy_Start", 2.5f);
    m_iAnimation_Grogy[1] = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Groggy_Loop", 2.f);
    m_iAnimation_Grogy[2] = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Groggy_End", 2.f);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Training_Grogy::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(m_iAnimation_Grogy[0], false, 0.1f);

    m_fLoopDelayTime = 0.f;

    return S_OK;
}

void CState_Training_Grogy::Update(_float fTimeDelta)
{
    _uint iCurAnimIndex = m_pMonster->Get_CurrentAnimIndex();

    if (iCurAnimIndex == m_iAnimation_Grogy[0] && m_pMonster->Get_EndAnim(m_iAnimation_Grogy[0]))
    {
        m_pMonster->Change_Animation(m_iAnimation_Grogy[1], true, 0.f);
    }
    else if (iCurAnimIndex == m_iAnimation_Grogy[1])
    {
        m_fLoopDelayTime += fTimeDelta;
        if (m_fLoopDelayTime >= 2.5f)
        {
            m_fLoopDelayTime = 0.f;
            m_pMonster->Change_Animation(m_iAnimation_Grogy[2], false, 0.05f);
        }
    }
    else if (iCurAnimIndex == m_iAnimation_Grogy[2] && m_pMonster->Get_EndAnim(m_iAnimation_Grogy[2]))
    {
        m_pMonster->Change_State(CMonster_Training01::IDLE);
    }
}

void CState_Training_Grogy::End_State()
{
    m_pMonster->Reset_GroggyPoint();
}

CState_Training_Grogy* CState_Training_Grogy::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_Training_Grogy* pInstance = new CState_Training_Grogy(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Training_Grogy"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Training_Grogy::Free()
{
    __super::Free();
}