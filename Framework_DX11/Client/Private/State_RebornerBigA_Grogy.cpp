#include "stdafx.h"
#include "State_RebornerBigA_Grogy.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_RebornerBigA_Grogy::CState_RebornerBigA_Grogy(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerBigA_Grogy::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RebornerBigA_Grogy::Start_State(void* pArg)
{
    m_iAnimCnt = 0;
    m_pMonster->Change_Animation(AN_GROGY_START, false);

    return S_OK;
}

void CState_RebornerBigA_Grogy::Update(_float fTimeDelta)
{
    switch (m_iAnimCnt)
    {
    case 0:
        if (End_Check())
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_GROGY_LOOP, true, 0.f);
        }
        break;

    case 1:
        if (m_fGrogyTime >= m_fGrogyDuration)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_GROGY_END - m_iAnimCnt, false, 0.f);
        }
        else
            m_fGrogyTime += fTimeDelta;
        break;

    case 2:
        if (End_Check())
        {
            m_iAnimCnt = 0;
            m_pMonster->Change_State(CCarcassBigA::IDLE);
        }
        break;

    default:
        break;
    }

}

void CState_RebornerBigA_Grogy::End_State()
{
    m_pMonster->Reset_GroggyPoint();
    m_iAnimCnt = 0;//혹시 완료되지 않고 변하는 경우에 대비
}

_bool CState_RebornerBigA_Grogy::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((AN_GROGY_START) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_GROGY_START);
    }
    else if ((AN_GROGY_LOOP) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_GROGY_LOOP);
    }
    else if ((AN_GROGY_END) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_GROGY_END);
    }
    else
    {

    }
    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

CState_RebornerBigA_Grogy* CState_RebornerBigA_Grogy::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerBigA_Grogy* pInstance = new CState_RebornerBigA_Grogy(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerBigA_Grogy"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerBigA_Grogy::Free()
{
    __super::Free();
}