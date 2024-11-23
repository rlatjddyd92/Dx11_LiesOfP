#include "stdafx.h"
#include "State_SimonManusP2_LightningToWave.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_LightningToWave::CState_SimonManusP2_LightningToWave(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_LightningToWave::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_LightningToWave::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);


    return S_OK;
}

void CState_SimonManusP2_LightningToWave::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 100.f)
        {
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.2f, 0);
            ++m_iRouteTrack;
        }
    }

    if (End_Check())
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }
}

void CState_SimonManusP2_LightningToWave::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP2_LightningToWave::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_ROUTE_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_FIRST);
        }
        break;

    case 1:
        if ((AN_ROUTE_LAST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_LAST);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

CState_SimonManusP2_LightningToWave* CState_SimonManusP2_LightningToWave::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_LightningToWave* pInstance = new CState_SimonManusP2_LightningToWave(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_LightningToWave"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_LightningToWave::Free()
{
    __super::Free();
}