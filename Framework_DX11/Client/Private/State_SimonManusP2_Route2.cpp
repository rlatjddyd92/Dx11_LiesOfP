#include "stdafx.h"
#include "State_SimonManusP2_Route2.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_Route2::CState_SimonManusP2_Route2(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Route2::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_Route2::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    m_isJump = true;

    return S_OK;
}

void CState_SimonManusP2_Route2::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 110.f)
        {
            ++m_iRouteTrack;
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.1f, 170);
        }
    }
    else
    {
        if (m_isJump)
        {
            if (160 >= CurTrackPos && 130 <= CurTrackPos)
            {
                m_vTargetDir = m_pMonster->Get_TargetDir();
                m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 1.8f, fTimeDelta);

            }
            else if (160 < CurTrackPos && 240 >= CurTrackPos)
            {
                _Vec3 vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);

                _Vec3 vMove = m_vTargetDir * (((_float)CurTrackPos - 230.f) / 20.f);
                m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vFlyMoveStack) / fTimeDelta);
                m_vFlyMoveStack = vMove;
                m_isJump = false;
            }
        }
    }

    


    if (End_Check())
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    Collider_Check();

}

void CState_SimonManusP2_Route2::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP2_Route2::End_Check()
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

void CState_SimonManusP2_Route2::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();
    if (m_iRouteTrack == 0) //AN_ROUTE_FIRST, 쓰러지면서 하는 스윙
    {
        if (CurTrackPos >= 60 && CurTrackPos <= 85.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else        //하이점프 폴
    {
        if (CurTrackPos >= 140 && CurTrackPos <= 150.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

CState_SimonManusP2_Route2* CState_SimonManusP2_Route2::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Route2* pInstance = new CState_SimonManusP2_Route2(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Route2"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Route2::Free()
{
    __super::Free();
}