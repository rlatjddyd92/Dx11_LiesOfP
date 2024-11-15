#include "stdafx.h"
#include "State_CarcassBigA_Grogy.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Grogy::CState_CarcassBigA_Grogy(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Grogy::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    return S_OK;
}

HRESULT CState_CarcassBigA_Grogy::Start_State(void* pArg)
{
    *m_pResetRootMove = true;

    if (*m_pTrackPos > 0) 
    {
        ++m_iAnimCnt;
        m_pMonster->Change_Animation(17 - m_iAnimCnt, false, 0);
        m_fGrogyTime = (_float)*m_pTrackPos;
    }
    else
    {
        m_pMonster->Change_Animation(17, false);
    }  //애니메이션의 시작부터 끝의 루트본의 이동값이 달라지면 안됨.

    return S_OK;
}

void CState_CarcassBigA_Grogy::Update(_float fTimeDelta)
{

    switch (m_iAnimCnt)
    {
    case 0:
        if (*m_pIsEndAnim)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(17 - m_iAnimCnt, true, 0);
        }
        break;

    case 1:
        if (m_fGrogyTime >= m_fGrogyDuration)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(17 - m_iAnimCnt, true, 0);
        }
        else
            m_fGrogyTime += fTimeDelta;
        break;

    case 2:
        if (*m_pIsEndAnim)
        {
            m_iAnimCnt = 0;
            *m_pTrackPos = 0.f;
            m_pMonster->Change_State(CCarcassBigA::IDLE);
        }
        break;

    default:
        break;
    }


    if (KEY_TAP(KEY::X))
    {
        m_pMonster->Change_State(CCarcassBigA::HITFATAL);
    }

    *m_pTrackPos = m_fGrogyTime;
}

void CState_CarcassBigA_Grogy::End_State()
{
    m_iAnimCnt = 0;//혹시 완료되지 않고 변하는 경우에 대비
    *m_pResetRootMove = false;
}

CState_CarcassBigA_Grogy* CState_CarcassBigA_Grogy::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_Grogy* pInstance = new CState_CarcassBigA_Grogy(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Grogy"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Grogy::Free()
{
    __super::Free();
}