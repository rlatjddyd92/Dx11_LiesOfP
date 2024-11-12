#include "stdafx.h"
#include "State_CarcassBigA_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Attack::CState_CarcassBigA_Attack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Attack::Initialize(_uint iStateNum, void* pArg)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    CCarcassBigA::FSMSTATE_DESC* pDesc = static_cast<CCarcassBigA::FSMSTATE_DESC*>(pArg);
    
    m_pIsEndAnim = pDesc->pIsEndAnim;

    return S_OK;
}

HRESULT CState_CarcassBigA_Attack::Start_State(void* pArg)
{
    if (m_iAttackCnt < 3)
    {
        ++m_iAttackCnt;
    }
    else
    {
        m_iAttackCnt = 0;

        m_pMonster->Change_State(CCarcassBigA::IMPACT);
        return S_OK;
    }

    m_pMonster->Look_Player();
    m_vAttackDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

    m_iAtkAnimNum = rand();
    m_iAtkAnimNum = rand() % 7;
    if (m_iAtkAnimNum >= 5)
        ++m_iAtkAnimNum;

    m_pMonster->Change_Animation(m_iAtkAnimNum, true);;
    return S_OK;
}

void CState_CarcassBigA_Attack::Update(_float fTimeDelta)
{
    //콜라이더 조정
    if (*m_pIsEndAnim == true)//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CCarcassBigA::IDLE);
    }
    //3회마다 다른 공격 하게 만들기?
    
}

void CState_CarcassBigA_Attack::End_State()
{
}

CState_CarcassBigA_Attack* CState_CarcassBigA_Attack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_Attack* pInstance = new CState_CarcassBigA_Attack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Attack::Free()
{
    __super::Free();
}