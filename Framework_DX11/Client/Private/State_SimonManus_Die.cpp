#include "stdafx.h"
#include "State_SimonManus_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManus_Die::CState_SimonManus_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManus_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    return S_OK;
}

HRESULT CState_SimonManus_Die::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_DIE, true);

    return S_OK;
}

void CState_SimonManus_Die::Update(_float fTimeDelta)
{
    if (*m_pIsEndAnim == true)
    {
        //몬스터 사망
        m_pMonster->Change_State(CSimonManus::IDLE);   //임시

    }

}

void CState_SimonManus_Die::End_State()
{
}

CState_SimonManus_Die* CState_SimonManus_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManus_Die* pInstance = new CState_SimonManus_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManus_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManus_Die::Free()
{
    __super::Free();
}