#include "stdafx.h"
#include "State_SimonManusP2_HitFatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_HitFatal::CState_SimonManusP2_HitFatal(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_HitFatal::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_HitFatal::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_FATAL_START - (m_iAnimCnt), false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP2_HitFatal::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        if (m_iAnimCnt < 2)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_FATAL_START - (m_iAnimCnt), false, 0.1f, 0);
        }
        else
        {
            m_iAnimCnt = 0;
            m_pMonster->Change_State(CSimonManus::IDLE);
        }
    }

}

void CState_SimonManusP2_HitFatal::End_State()
{
    m_pMonster->Reset_GroggyPoint();
    m_pMonster->Get_Status()->bFatalAttack = false;
    m_fHitFatalTime = 0.f;
    m_iAnimCnt = 0;
}

_bool CState_SimonManusP2_HitFatal::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };

    switch (m_iAnimCnt)
    {
    case 0:
        if ((AN_FATAL_START) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_START);
        }
        break;

    case 1:
        if ((AN_FATAL_LOOP) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_LOOP);
        }
        break;

    case 2:
        if ((AN_FATAL_END) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_END);
        }
        break;

    default:
        break;
    }

    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

CState_SimonManusP2_HitFatal* CState_SimonManusP2_HitFatal::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_HitFatal* pInstance = new CState_SimonManusP2_HitFatal(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_HitFatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_HitFatal::Free()
{
    __super::Free();
}