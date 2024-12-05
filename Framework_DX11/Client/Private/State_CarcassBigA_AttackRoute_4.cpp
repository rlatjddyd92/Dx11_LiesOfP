#include "stdafx.h"
#include "State_CarcassBigA_AttackRoute_4.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_AttackRoute_4::CState_CarcassBigA_AttackRoute_4(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_AttackRoute_4::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
   // FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassBigA_AttackRoute_4::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f);

    return S_OK;
}

void CState_CarcassBigA_AttackRoute_4::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        ++m_iRouteTrack;
        if (m_iRouteTrack == 1)
        {
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.1f);
        }
        else if (m_iRouteTrack == 2)
        {
            m_pMonster->Change_State(CCarcassBigA::IDLE);
            return;
        }
    }
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0 && CurTrackPos >= 55.f && CurTrackPos <= 140.f)
    {
        _Vec3 vDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

        m_pMonster->Get_RigidBody()->Set_Velocity(XMVector3Normalize(vDir) * m_fImpactSpeed);
    }
    else
    {

        if (CurTrackPos <= 30.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.5, fTimeDelta);
        }
    }


    Collider_Check();

}

void CState_CarcassBigA_AttackRoute_4::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_CarcassBigA_AttackRoute_4::End_Check()
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

void CState_CarcassBigA_AttackRoute_4::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 55.f && CurTrackPos <= 140.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.4, 2, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(2);
        }
    }
    else
    {
        if (CurTrackPos >= 90.f && CurTrackPos <= 105.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }

}

CState_CarcassBigA_AttackRoute_4* CState_CarcassBigA_AttackRoute_4::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_AttackRoute_4* pInstance = new CState_CarcassBigA_AttackRoute_4(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_AttackRoute_4"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_AttackRoute_4::Free()
{
    __super::Free();
}