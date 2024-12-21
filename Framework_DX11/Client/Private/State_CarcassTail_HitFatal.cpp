#include "stdafx.h"
#include "State_CarcassTail_HitFatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassTail_HitFatal::CState_CarcassTail_HitFatal(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_HitFatal::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassTail_HitFatal::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_FATAL_START, false, 0.f);

    return S_OK;
}

void CState_CarcassTail_HitFatal::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        if (m_iAnimCnt == 0)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_FATAL_LOOP, false, 0.f);
        }
        else if (m_iAnimCnt == 1)
        {
            m_iAnimCnt = 0;
            m_pMonster->Change_State(CCarcassBigA::GROGY);
        }
    }

}

void CState_CarcassTail_HitFatal::End_State()
{
    m_fHitFatalTime = 0.f;
}

_bool CState_CarcassTail_HitFatal::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((AN_FATAL_START) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_START);
    }
    else if ((AN_FATAL_START) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_START);
    }
    else if ((AN_FATAL_LOOP) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_LOOP);
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

CState_CarcassTail_HitFatal* CState_CarcassTail_HitFatal::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_HitFatal* pInstance = new CState_CarcassTail_HitFatal(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_HitFatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_HitFatal::Free()
{
    __super::Free();
}