#include "stdafx.h"
#include "State_SimonManusP1_HitFatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_HitFatal::CState_SimonManusP1_HitFatal(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_HitFatal::Initialize(_uint iStateNum, void* pArg)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;

    return S_OK;
}

HRESULT CState_SimonManusP1_HitFatal::Start_State(void* pArg)
{
    *m_pResetRootMove = false;


    m_pMonster->Change_Animation(AN_FATAL_START - (m_iAnimCnt), false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP1_HitFatal::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        if (m_iAnimCnt < 2)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_FATAL_START - (m_iAnimCnt), false, 0.0f, 0);
        }
        else
        {
            m_iAnimCnt = 0;
            m_pMonster->Change_State(CSimonManus::GROGY);
        }
    }

}

void CState_SimonManusP1_HitFatal::End_State()
{
    m_fHitFatalTime = 0.f;
    m_iAnimCnt = 0;
    *m_pResetRootMove = true;
}

_bool CState_SimonManusP1_HitFatal::End_Check()
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

CState_SimonManusP1_HitFatal* CState_SimonManusP1_HitFatal::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_HitFatal* pInstance = new CState_SimonManusP1_HitFatal(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_HitFatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_HitFatal::Free()
{
    __super::Free();
}