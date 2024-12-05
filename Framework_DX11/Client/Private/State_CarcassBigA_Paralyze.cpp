#include "stdafx.h"
#include "GameInstance.h"
#include "State_CarcassBigA_Paralyze.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Paralyze::CState_CarcassBigA_Paralyze(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Paralyze::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassBigA_Paralyze::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_PARALYZE_START, false, 0.2f, 0);
    m_iAnimCnt = 0;

    return S_OK;
}

void CState_CarcassBigA_Paralyze::Update(_float fTimeDelta)
{
    _bool bCheck = End_Check();

    switch (m_iAnimCnt)
    {
    case 0:
        if (bCheck)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_PARALYZE_START - m_iAnimCnt, true, 0.f, 0);
        }
        break;

    case 1:
        if (m_fParalizeTime >= m_fParalizeDuration)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_PARALYZE_START - m_iAnimCnt, false, 0.f, 0);
        }
        else
            m_fParalizeTime += fTimeDelta;
        break;

    case 2:
        if (bCheck)
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
}

_bool CState_CarcassBigA_Paralyze::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((AN_PARALYZE_START) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_PARALYZE_START);
    }
    else if ((AN_PARALYZE_LOOP) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_PARALYZE_LOOP);
    }
    else if ((AN_PARALYZE_END) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_PARALYZE_END);
    }
    else
    {

    }
        //애니메이션 번호와 일치하지 않는?다
    
    return bEndCheck;

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