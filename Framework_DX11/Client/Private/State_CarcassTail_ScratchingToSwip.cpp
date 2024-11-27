#include "stdafx.h"
#include "State_CarcassTail_ScratchingToSwip.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassTail_ScratchingToSwip::CState_CarcassTail_ScratchingToSwip(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_ScratchingToSwip::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 1.f;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassTail_ScratchingToSwip::Start_State(void* pArg)
{
    m_iFirstAnim = 1;   //지정하고 시작해서 따라가도록
    m_iLastAnim = 0;

    m_pMonster->Change_Animation(m_iFirstAnim, false, 0.1f, 0);

    m_fIdleTime = m_fIdleDuration;
    return S_OK;
}

void CState_CarcassTail_ScratchingToSwip::Update(_float fTimeDelta)
{
    if (!m_isDelayed)
    {
        if (m_iRouteTrack == 1)
        {
            m_pMonster->Change_Animation(m_iLastAnim, false, 0.1f, 0, true);
        }

        if (End_Check())
        {
            ++m_iRouteTrack;

            if (m_iRouteTrack >= 2)
            {
                m_pMonster->Change_State(CCarcassBigA::IDLE);
                return;
            }
            m_fIdleTime = 0.f;
            m_isDelayed = true;
        }
    }
    else
    {
        m_fIdleTime += fTimeDelta;

        if (m_fIdleTime >= m_fIdleDuration)
        {
            m_isDelayed = false;
        }
        _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3, fTimeDelta);
        switch (iDir)
        {
        case -1:
            m_pMonster->Change_Animation(30, true, 0.1f);
            break;

        case 0:
            m_pMonster->Change_Animation(20, true, 0.1f);
            break;

        case 1:
            m_pMonster->Change_Animation(31, true, 0.1f);
            break;

        default:
            break;
        }
    }

    Collider_Check();

}

void CState_CarcassTail_ScratchingToSwip::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_CarcassTail_ScratchingToSwip::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((m_iFirstAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iFirstAnim);
        }
        break;

    case 1:
        if ((m_iLastAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iLastAnim);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_CarcassTail_ScratchingToSwip::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 50.f && CurTrackPos <= 70.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1, 1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
    else
    {
        if (CurTrackPos >= 90.f && CurTrackPos <= 105.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1, 1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
}

CState_CarcassTail_ScratchingToSwip* CState_CarcassTail_ScratchingToSwip::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_ScratchingToSwip* pInstance = new CState_CarcassTail_ScratchingToSwip(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_ScratchingToSwip"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_ScratchingToSwip::Free()
{
    __super::Free();
}