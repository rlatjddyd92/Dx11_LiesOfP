#include "stdafx.h"
#include "State_SimonManusP1_UpperAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManusP1.h"

CState_SimonManusP1_UpperAttack::CState_SimonManusP1_UpperAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_UpperAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    CSimonManusP1::FSMSTATE_DESC* pDesc = static_cast<CSimonManusP1::FSMSTATE_DESC*>(pArg);
    
    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pColliderCtrs = pDesc->pColliderCtrs;
    return S_OK;
}

HRESULT CState_SimonManusP1_UpperAttack::Start_State(void* pArg)
{
    m_pMonster->Look_Player();

    m_pMonster->Change_Animation(AN_UPPERATTACK, true);

    return S_OK;
}

void CState_SimonManusP1_UpperAttack::Update(_float fTimeDelta)
{
    //콜라이더 조정
    if (*m_pIsEndAnim == true)//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManusP1::IDLE);
    }
    
}

void CState_SimonManusP1_UpperAttack::End_State()
{
    m_pColliderCtrs[00] = true;
    m_pColliderCtrs[00] = true;
}

CState_SimonManusP1_UpperAttack* CState_SimonManusP1_UpperAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_UpperAttack* pInstance = new CState_SimonManusP1_UpperAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_UpperAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_UpperAttack::Free()
{
    __super::Free();
}