#include "stdafx.h"
#include "State_SimonManus_Grogy.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManus_Grogy::CState_SimonManus_Grogy(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManus_Grogy::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pGrogyTrackPos;

    return S_OK;
}

HRESULT CState_SimonManus_Grogy::Start_State(void* pArg)
{
    if (*m_pTrackPos > 0) 
    {
        ++m_iAnimCnt;
        m_pMonster->Change_Animation(AN_GROGYL_START - m_iAnimCnt, true, true);
        m_fGrogyTime = (_float)*m_pTrackPos;
    }
    else
    {
        m_pMonster->Change_Animation(AN_GROGYL_START, false);
    }
    *m_pResetRootMove = false;  //애니메이션의 시작부터 끝의 루트본의 이동값이 달라지면 안됨.

    return S_OK;
}

void CState_SimonManus_Grogy::Update(_float fTimeDelta)
{

    switch (m_iAnimCnt)
    {
    case 0:
        if (*m_pIsEndAnim)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_GROGYL_START - m_iAnimCnt, true, true);
        }
        break;

    case 1:
        if (m_fGrogyTime >= m_fGrogyDuration)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(AN_GROGYL_START - m_iAnimCnt, false, true);
        }
        else
            m_fGrogyTime += fTimeDelta;
        break;

    case 2:
        if (*m_pIsEndAnim)
        {
            m_iAnimCnt = 0;
            *m_pTrackPos = 0.f;
            m_pMonster->Change_State(CSimonManus::IDLE);
        }
        break;

    default:
        break;
    }


    if (KEY_TAP(KEY::X))
    {
        m_pMonster->Change_State(CSimonManus::HITFATAL);
    }

    *m_pTrackPos = m_fGrogyTime;
}

void CState_SimonManus_Grogy::End_State()
{
    m_iAnimCnt = 0;//혹시 완료되지 않고 변하는 경우에 대비
    m_fGrogyTime = 0;
    *m_pResetRootMove = true;
}

CState_SimonManus_Grogy* CState_SimonManus_Grogy::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManus_Grogy* pInstance = new CState_SimonManus_Grogy(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManus_Grogy"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManus_Grogy::Free()
{
    __super::Free();
}