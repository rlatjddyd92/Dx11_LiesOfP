#include "stdafx.h"
#include "State_RebornerMale_RapidFire.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "RebornerBigA.h"

CState_RebornerMale_RapidFire::CState_RebornerMale_RapidFire(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerMale_RapidFire::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RebornerMale_RapidFire::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_LINKED_START, false, 0.1f, 0, true);
    m_iRouteTrack = 0;

    m_bFire = false;

    return S_OK;
}

void CState_RebornerMale_RapidFire::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (End_Check())
        {
            m_bFire = false;

            ++m_iRouteTrack;
            m_pMonster->SetUp_Animation(AN_LINKED_MIDDLE, false, 0, true);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 9)
    {
        if ((m_iRouteTrack % 3) == 0)
        {
            if (m_iRouteTrack == 9)
            {
                if (End_Check())
                {
                    ++m_iRouteTrack;
                    m_pMonster->SetUp_Animation(AN_LINKED_LAST, false, 0, true);
                    return;
                }
            }
            else
            {
                if (End_Check())
                {
                    m_bFire = false;

                    ++m_iRouteTrack;
                    m_pMonster->Change_Animation(AN_LINKED_MIDDLE, false, 0.1f, 0, true, true);
                    return;
                }
            }
        }
        else
        {
            if (CurTrackPos >= 60.f)
            {
                m_bFire = false;

                ++m_iRouteTrack;
                m_pMonster->Change_Animation(AN_LINKED_MIDDLE, false, 0.4f, 25, true, true);
                return;
            }
        }

        if (CurTrackPos <= 30.f && CurTrackPos >= 40.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);
        }
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


    Collider_Check(CurTrackPos);

}

void CState_RebornerMale_RapidFire::End_State()
{
}

_bool CState_RebornerMale_RapidFire::End_Check()
{
    _bool bEndCheck = { false };

    if (m_iRouteTrack == 0)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_START);
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 9)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_MIDDLE);
    }
    else
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_LAST);
    }

    return bEndCheck;
}

void CState_RebornerMale_RapidFire::Collider_Check(_double CurTrackPos)
{
    
}

void CState_RebornerMale_RapidFire::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack >= 1 && m_iRouteTrack <= 9)
    {
        if (!m_bFire)
        {
            if (CurTrackPos >= 30.f)
            {
                m_bFire = true;
                //藕 积己 棺 醚备拳堪 积己
            }
        }
    }
}

CState_RebornerMale_RapidFire* CState_RebornerMale_RapidFire::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerMale_RapidFire* pInstance = new CState_RebornerMale_RapidFire(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerMale_RapidFire"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerMale_RapidFire::Free()
{
    __super::Free();
}