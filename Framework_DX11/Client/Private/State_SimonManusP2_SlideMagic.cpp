#include "stdafx.h"
#include "State_SimonManusP2_SlideMagic.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_SlideMagic::CState_SimonManusP2_SlideMagic(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_SlideMagic::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_SlideMagic::Start_State(void* pArg)
{
    _int iCnt = rand() % 2;
    if (iCnt == 0)
    {
        m_iCurAnim = AN_SLIDE_LEFT;
    }
    else
    {
        m_iCurAnim = AN_SLIDE_RIGHT;
    }

    m_pMonster->Change_Animation(m_iCurAnim, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP2_SlideMagic::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

}

void CState_SimonManusP2_SlideMagic::End_State()
{

}

_bool CState_SimonManusP2_SlideMagic::End_Check()
{
    return m_pMonster->Get_EndAnim(m_iCurAnim);
}

CState_SimonManusP2_SlideMagic* CState_SimonManusP2_SlideMagic::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_SlideMagic* pInstance = new CState_SimonManusP2_SlideMagic(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_SlideMagic"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_SlideMagic::Free()
{
    __super::Free();
}