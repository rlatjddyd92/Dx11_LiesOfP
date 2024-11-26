#include "stdafx.h"
#include "State_CarcassBigA_Impact.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Impact::CState_CarcassBigA_Impact(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Impact::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassBigA_Impact::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IMPACT, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CarcassBigA_Impact::Update(_float fTimeDelta)
{
    if (End_Check())
        m_pMonster->Change_State(CCarcassBigA::IDLE);
    
    
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();
    
    if (CurTrackPos >= 55.f && CurTrackPos <= 135.f)
    {
        _Vec3 vDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

        m_pMonster->Get_RigidBody()->Set_Velocity(XMVector3Normalize(vDir) * m_fImpactSpeed);

    }
   

    Collider_Check();
}

void CState_CarcassBigA_Impact::End_State()
{

}

_bool CState_CarcassBigA_Impact::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_IMPACT);
}

void CState_CarcassBigA_Impact::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos >= 55.f && CurTrackPos <= 140.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1, 2);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(2);
    }
}

CState_CarcassBigA_Impact* CState_CarcassBigA_Impact::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_Impact* pInstance = new CState_CarcassBigA_Impact(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Impact"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Impact::Free()
{
    __super::Free();
}