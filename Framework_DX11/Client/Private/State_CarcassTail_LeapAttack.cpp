#include "stdafx.h"
#include "State_CarcassTail_LeapAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassTail_LeapAttack::CState_CarcassTail_LeapAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_LeapAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 1.f;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassTail_LeapAttack::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    return S_OK;
}

void CState_CarcassTail_LeapAttack::Update(_float fTimeDelta)
{
    if (m_iRouteTrack == 0)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.5f, fTimeDelta);

    }
    else if (m_iRouteTrack == 1)
    {
        if (XMVectorGetX(XMVector3Length(m_vStartingPos - m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION))) >= m_fDistPoint - 5.f)
        {
            ++m_iRouteTrack;
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.f, 0);
        }
        else
        {
            m_pMonster->Get_RigidBody()->Set_Velocity(m_vMoveSpeed * 40);
        }
    }


    if (End_Check())
    {
        if (m_iRouteTrack == 0)
        {
            m_pMonster->Change_Animation(AN_ROUTE_MIDDLE, false, 0.1f, 0);
            m_pMonster->Active_CurrentWeaponCollider(1, 3);

            m_vStartingPos = (_Vec3)m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
            m_fDistPoint = XMVectorGetX(XMVector3Length(m_pMonster->Get_TargetPos() - m_vStartingPos));
            m_vMoveSpeed = XMVector3Normalize(m_pMonster->Get_TargetDir());

        }
        else if (m_iRouteTrack == 2)
        {
            m_pMonster->Change_State(CCarcassBigA::IDLE);
            return;
        }
        ++m_iRouteTrack;
    }


    Collider_Check();

}

void CState_CarcassTail_LeapAttack::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_CarcassTail_LeapAttack::End_Check()
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
        if ((AN_ROUTE_MIDDLE) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_MIDDLE);
        }
        break;

    case 2:
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

void CState_CarcassTail_LeapAttack::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 230.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.4, 3, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(3);
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if ((CurTrackPos <= 40.f)||
            (CurTrackPos <= 60.f && CurTrackPos <= 80.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.4, 3, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(3);
        }
    }
}

CState_CarcassTail_LeapAttack* CState_CarcassTail_LeapAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_LeapAttack* pInstance = new CState_CarcassTail_LeapAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_LeapAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_LeapAttack::Free()
{
    __super::Free();
}