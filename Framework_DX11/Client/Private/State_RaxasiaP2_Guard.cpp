#include "stdafx.h"
#include "State_RaxasiaP2_Guard.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "GameInterface_Controller.h"

CState_RaxasiaP2_Guard::CState_RaxasiaP2_Guard(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Guard::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_Guard::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_GUARD_START, false, 0.1f, 0);
    m_iRouteTrack = 0;
    m_fIdleTime = 0;

    if (m_pMonster->Calc_Distance_XZ() >= 15.f)
    {
        m_iMovingAnimIndex = AN_GUARD_WALK_F;
    }
    else
    {
        _int iCnt = rand() % 3;

        switch (iCnt)
        {
        case 0:
            m_iMovingAnimIndex = AN_GUARD_IDLE;
            break;

        case 1:
            m_iMovingAnimIndex = AN_GUARD_WALK_L;
            break;

        case 2:
            m_iMovingAnimIndex = AN_GUARD_WALK_R;
            break;

        default:
            break;
        }

    }

    return S_OK;
}

void CState_RaxasiaP2_Guard::Update(_float fTimeDelta)
{
    m_fIdleTime += fTimeDelta;
    _float fDist = m_pMonster->Calc_Distance_XZ();

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 50.f)
        {
            ++m_iRouteTrack;
            m_pMonster->Change_Animation(m_iMovingAnimIndex, true, 0.1f, 0);
        }
        break;

    case 1:
        if (m_fIdleEndDuration <= m_fIdleTime)
        {
            ++m_iAtkTrack;
            m_pMonster->Change_State(CRaxasia::ATKP2_GUARD_PUSH);
            return;
        }

        break;

    default:
        break;
    }

    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3, fTimeDelta);
    

}

void CState_RaxasiaP2_Guard::End_State()
{
}

_bool CState_RaxasiaP2_Guard::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_GUARD_IDLE);
}

CState_RaxasiaP2_Guard* CState_RaxasiaP2_Guard::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Guard* pInstance = new CState_RaxasiaP2_Guard(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Guard"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Guard::Free()
{
    __super::Free();
}