#include "stdafx.h"
#include "State_CarcassBigA_Paralyze.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Paralyze::CState_CarcassBigA_Paralyze(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Paralyze::Initialize(_uint iStateNum, void* pArg)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    
    return S_OK;
}

HRESULT CState_CarcassBigA_Paralyze::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(28, false);
    *m_pResetRootMove = false;//애니메이션의 시작부터 끝의 루트본의 이동값이 달라지면 안됨.
    m_iAnimCnt = 0;

    return S_OK;
}

void CState_CarcassBigA_Paralyze::Update(_float fTimeDelta)
{
    switch (m_iAnimCnt)
    {
    case 0:
        if (*m_pIsEndAnim)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(28 - m_iAnimCnt, true, true);
        }
        break;

    case 1:
        if (m_fParalizeTime >= m_fParalizeDuration)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(28 - m_iAnimCnt, false, true);
        }
        else
            m_fParalizeTime += fTimeDelta;
        break;

    case 2:
        if (*m_pIsEndAnim)
        {
            m_iAnimCnt = 0;
            m_pMonster->Change_State(CCarcassBigA::IDLE);
        }
        break;

    default:
        break;
    }

    
}

void CState_CarcassBigA_Paralyze::End_State()
{
    m_iAnimCnt = 0;//혹시 완료되지 않고 변하는 경우에 대비
    m_fParalizeTime = 0.f;
    *m_pResetRootMove = true;
}

CState_CarcassBigA_Paralyze* CState_CarcassBigA_Paralyze::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_Paralyze* pInstance = new CState_CarcassBigA_Paralyze(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Paralyze"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Paralyze::Free()
{
    __super::Free();
}