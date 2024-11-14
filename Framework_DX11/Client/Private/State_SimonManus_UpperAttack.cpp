#include "stdafx.h"
#include "State_SimonManus_UpperAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManus_UpperAttack::CState_SimonManus_UpperAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManus_UpperAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);
    
    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pColliderCtrs = pDesc->pColliderCtrs;
    return S_OK;
}

HRESULT CState_SimonManus_UpperAttack::Start_State(void* pArg)
{
    m_pMonster->Look_Player();

    m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

    m_pMonster->Change_Animation(AN_UPPERATTACK, true);

    return S_OK;
}

void CState_SimonManus_UpperAttack::Update(_float fTimeDelta)
{
    //콜라이더 조정
    if (*m_pIsEndAnim == true)//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        //m_pMonster->Change_State(IDLE);
    }
    //3회마다 다른 공격 하게 만들기?
    
}

void CState_SimonManus_UpperAttack::End_State()
{
    m_pColliderCtrs[00] = true;
    m_pColliderCtrs[00] = true;
}

CState_SimonManus_UpperAttack* CState_SimonManus_UpperAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManus_UpperAttack* pInstance = new CState_SimonManus_UpperAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManus_UpperAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManus_UpperAttack::Free()
{
    __super::Free();
}