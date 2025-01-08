#include "stdafx.h"
#include "State_RaxasiaP2_Grogy.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_Grogy::CState_RaxasiaP2_Grogy(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Grogy::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_Grogy::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_GROGY_START, false, 0.1f, 0);

    return S_OK;
}

void CState_RaxasiaP2_Grogy::Update(_float fTimeDelta)
{

    switch (m_iAnimCnt)
    {
    case 0:
        if (End_Check())
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_GROGY_START - m_iAnimCnt, true, 0.0f, 0);
        }
        break;

    case 1:
        if (m_fGrogyTime >= m_fGrogyDuration)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_GROGY_START - m_iAnimCnt, false, 0.0f, 0);
        }
        else
            m_fGrogyTime += fTimeDelta;
        break;

    case 2:
        if (End_Check())
        {
            m_iAnimCnt = 0;
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
        break;

    default:
        break;
    }


    if (KEY_TAP(KEY::X))
    {
        m_pMonster->Change_State(CRaxasia::HITFATAL);
    }

}

void CState_RaxasiaP2_Grogy::End_State()
{
    m_pMonster->Reset_GroggyPoint();
    m_iAnimCnt = 0;//혹시 완료되지 않고 변하는 경우에 대비
    m_fGrogyTime = 0;
}

_bool CState_RaxasiaP2_Grogy::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };

    switch (m_iAnimCnt)
    {
    case 0:
        if ((AN_GROGY_START) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_GROGY_START);
        }
        break;

    case 1:
        if ((AN_GROGY_LOOP) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_GROGY_LOOP);
        }
        break;

    case 2:
        if ((AN_GROGY_END) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_GROGY_END);
        }
        break;

    default:
        break;
    }

    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

CState_RaxasiaP2_Grogy* CState_RaxasiaP2_Grogy::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Grogy* pInstance = new CState_RaxasiaP2_Grogy(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Grogy"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Grogy::Free()
{
    __super::Free();
}