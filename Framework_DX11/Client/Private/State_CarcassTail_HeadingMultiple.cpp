#include "stdafx.h"
#include "State_CarcassTail_HeadingMultiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassTail_HeadingMultiple::CState_CarcassTail_HeadingMultiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_HeadingMultiple::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_HeadingMultiple::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_HEADING, false, 0.1f, 0, true);

    m_iRouteTrack = 0;
    return S_OK;
}

void CState_CarcassTail_HeadingMultiple::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos < 65.f)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
    }


    if (m_iRouteTrack < 2)
    {
        if (CurTrackPos >= 140)
        {
            ++m_iRouteTrack;
            m_pMonster->Change_Animation(AN_HEADING, false, 0.1f, 0, true, true);
        }
    }
    else
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CMonster::IDLE);
        }
    }

    Collider_Check();

}

void CState_CarcassTail_HeadingMultiple::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_CarcassTail_HeadingMultiple::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_HEADING);
}

void CState_CarcassTail_HeadingMultiple::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if ((CurTrackPos >= 80.f && CurTrackPos <= 95.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.4, 3, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(3);
    }

}

CState_CarcassTail_HeadingMultiple* CState_CarcassTail_HeadingMultiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_HeadingMultiple* pInstance = new CState_CarcassTail_HeadingMultiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_HeadingMultiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_HeadingMultiple::Free()
{
    __super::Free();
}