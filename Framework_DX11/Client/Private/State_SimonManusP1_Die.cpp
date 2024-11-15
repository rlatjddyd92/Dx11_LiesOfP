#include "stdafx.h"
#include "State_SimonManusP1_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManusP1.h"

CState_SimonManusP1_Die::CState_SimonManusP1_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    CSimonManusP1::FSMSTATE_DESC* pDesc = static_cast<CSimonManusP1::FSMSTATE_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    return S_OK;
}

HRESULT CState_SimonManusP1_Die::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_DIE, true);

    return S_OK;
}

void CState_SimonManusP1_Die::Update(_float fTimeDelta)
{
    if (*m_pIsEndAnim == true)
    {
        //몬스터 사망
        m_pMonster->Change_State(CSimonManusP1::IDLE);   //임시

    }

}

void CState_SimonManusP1_Die::End_State()
{
}

CState_SimonManusP1_Die* CState_SimonManusP1_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_Die* pInstance = new CState_SimonManusP1_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_Die::Free()
{
    __super::Free();
}