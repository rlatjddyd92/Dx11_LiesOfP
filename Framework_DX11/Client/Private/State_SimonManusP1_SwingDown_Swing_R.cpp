#include "stdafx.h"
#include "State_SimonManusP1_SwingDown_Swing_R.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_SwingDown_Swing_R::CState_SimonManusP1_SwingDown_Swing_R(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_SwingDown_Swing_R::Initialize(_uint iStateNum, void* pArg)
{
    //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pResetRootMove = pDesc->pIsResetRootMove;

    return S_OK;
}

HRESULT CState_SimonManusP1_SwingDown_Swing_R::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    m_isSwing = true;

    return S_OK;
}

void CState_SimonManusP1_SwingDown_Swing_R::Update(_float fTimeDelta)
{
    if (m_isSwing)
    {
        if (100.f >= m_pMonster->Get_CurrentTrackPos())
        {
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0, 0);
            ++m_iRouteTrack;
            m_isSwing = false;
        }
    }


    if (End_Check())
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    Collider_Check();

}

void CState_SimonManusP1_SwingDown_Swing_R::End_State()
{
    m_iRouteTrack = 0;
    m_fIdleTime = 0.f;
    *m_pResetRootMove = true;
}

_bool CState_SimonManusP1_SwingDown_Swing_R::End_Check()
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

void CState_SimonManusP1_SwingDown_Swing_R::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 65.f && CurTrackPos <= 80.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else
    {
        if (CurTrackPos >= 45.f && CurTrackPos <= 100.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

CState_SimonManusP1_SwingDown_Swing_R* CState_SimonManusP1_SwingDown_Swing_R::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_SwingDown_Swing_R* pInstance = new CState_SimonManusP1_SwingDown_Swing_R(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_SwingDown_Swing_R"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_SwingDown_Swing_R::Free()
{
    __super::Free();
}