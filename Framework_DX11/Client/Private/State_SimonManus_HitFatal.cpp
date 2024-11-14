#include "stdafx.h"
#include "State_SimonManus_HitFatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManus_HitFatal::CState_SimonManus_HitFatal(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManus_HitFatal::Initialize(_uint iStateNum, void* pArg)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;

    return S_OK;
}

HRESULT CState_SimonManus_HitFatal::Start_State(void* pArg)
{
    *m_pResetRootMove = false;


    m_pMonster->Change_Animation(AN_FATAL_START - (2 * m_iAnimCnt), false, true);

    return S_OK;
}

void CState_SimonManus_HitFatal::Update(_float fTimeDelta)
{
    if (*m_pIsEndAnim)
    {
        if (m_iAnimCnt < 2)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_FATAL_START - (2 * m_iAnimCnt), false, true);
        }
        else
        {
            m_iAnimCnt = 0;
            m_pMonster->Change_State(CSimonManus::GROGY);
        }
    }

}

void CState_SimonManus_HitFatal::End_State()
{
    m_fHitFatalTime = 0.f;
    m_iAnimCnt = 0.f;
    *m_pResetRootMove = true;
}

CState_SimonManus_HitFatal* CState_SimonManus_HitFatal::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManus_HitFatal* pInstance = new CState_SimonManus_HitFatal(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManus_HitFatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManus_HitFatal::Free()
{
    __super::Free();
}