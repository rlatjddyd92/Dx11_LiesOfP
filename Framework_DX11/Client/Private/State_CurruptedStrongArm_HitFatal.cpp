#include "stdafx.h"
#include "State_CurruptedStrongArm_HitFatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "CurruptedStrongArm_Puppet.h"

CState_CurruptedStrongArm_HitFatal::CState_CurruptedStrongArm_HitFatal(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CurruptedStrongArm_HitFatal::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CurruptedStrongArm_HitFatal::Start_State(void* pArg)
{
    m_iAnimTrack = 0;

    m_pMonster->Change_Animation(AN_FATAL_START, false, 0.f);

    return S_OK;
}

void CState_CurruptedStrongArm_HitFatal::Update(_float fTimeDelta)
{
    switch (m_iAnimTrack)
    {
    case 0:
        if (End_Check())
        {
            ++m_iAnimTrack;
            m_pMonster->Change_Animation(AN_FATAL_LOOP, false, 0.f);
        }
        break;

    case 1:
        if (End_Check())
        {
            m_pMonster->Change_State(CCurruptedStrongArm_Puppet::GROGY);
        }
        break;


    default:
        break;
    }

}

void CState_CurruptedStrongArm_HitFatal::End_State()
{
    m_fHitFatalTime = 0.f;
}

_bool CState_CurruptedStrongArm_HitFatal::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((AN_FATAL_START) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_START);
    }
    else if ((AN_FATAL_LOOP) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_LOOP);
    }
    else
    {

    }
    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

CState_CurruptedStrongArm_HitFatal* CState_CurruptedStrongArm_HitFatal::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CurruptedStrongArm_HitFatal* pInstance = new CState_CurruptedStrongArm_HitFatal(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CurruptedStrongArm_HitFatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CurruptedStrongArm_HitFatal::Free()
{
    __super::Free();
}