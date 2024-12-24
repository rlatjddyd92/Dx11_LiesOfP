#include "stdafx.h"
#include "State_RebornerMale_Reload.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "RebornerBigA.h"

CState_RebornerMale_Reload::CState_RebornerMale_Reload(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerMale_Reload::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RebornerMale_Reload::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_LINKED_START, false, 0.1f, 0, true);
    m_iRouteTrack = 0;

    return S_OK;
}

void CState_RebornerMale_Reload::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_pMonster->SetUp_Animation(AN_LINKED_MIDDLE, false, 0, true);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);
    }
    else if (m_iRouteTrack == 1)
    {
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_pMonster->SetUp_Animation(AN_LINKED_LAST, false, 0, true);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);
    }
    else
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CMonster::IDLE);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);
    }


    //Collider_Check(CurTrackPos);

}

void CState_RebornerMale_Reload::End_State()
{
}

_bool CState_RebornerMale_Reload::End_Check()
{
    _bool bEndCheck = { false };

    switch (m_iRouteTrack)
    {
    case 0:
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_START);
        break;

    case 1:
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_MIDDLE);
        break;

    case 2:
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_LAST);
        break;
        
    default:
        break;
    }


    return bEndCheck;
}

void CState_RebornerMale_Reload::Collider_Check(_double CurTrackPos)
{

}

CState_RebornerMale_Reload* CState_RebornerMale_Reload::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerMale_Reload* pInstance = new CState_RebornerMale_Reload(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerMale_Reload"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerMale_Reload::Free()
{
    __super::Free();
}