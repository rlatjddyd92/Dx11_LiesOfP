#include "stdafx.h"
#include "State_RaxasiaP1_SwingDown_Multiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP1_SwingDown_Multiple::CState_RaxasiaP1_SwingDown_Multiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_SwingDown_Multiple::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_SwingDown_Multiple::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_SWINGDOWN, false, 0.1f, 0);
    m_iCurAnimIndex = AN_SWINGDOWN;

    m_bSwingSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP1_SwingDown_Multiple::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();


    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 80.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_SWINGDOWN_R, false, 0.1f, 0);
            m_iCurAnimIndex = AN_SWINGDOWN_R;
        }
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 7)
    {
        if (CurTrackPos >= 70.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            if (m_iRouteTrack == 7)
            {
                m_iCurAnimIndex = AN_SWINGDOWN;
            }
            else if (m_iCurAnimIndex == AN_SWINGDOWN_L)
            {
                m_iCurAnimIndex = AN_SWINGDOWN_R;
            }
            else
            {
                m_iCurAnimIndex = AN_SWINGDOWN_L;
            }
            m_pMonster->Change_Animation(m_iCurAnimIndex, false, 0.1f, 0);
        }
    }
    else if (m_iRouteTrack = 8)
    {
        //림라이트?
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
    }

    _double Ratio = (double)(1 + (m_iRouteTrack * 0.2f));
    if (Ratio >= 2.f)
    {
        Ratio = 2.f;
    }
    m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, Ratio);

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
}

void CState_RaxasiaP1_SwingDown_Multiple::End_State()
{
    m_pMonster->Get_Model()->Set_SpeedRatio(AN_SWINGDOWN, (double)1);
    m_pMonster->Get_Model()->Set_SpeedRatio(AN_SWINGDOWN_L, (double)1);
    m_pMonster->Get_Model()->Set_SpeedRatio(AN_SWINGDOWN_R, (double)1);
}

_bool CState_RaxasiaP1_SwingDown_Multiple::End_Check()
{
    return false;
}

void CState_RaxasiaP1_SwingDown_Multiple::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0 || m_iRouteTrack == 8)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 44.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 7)
    {
        if (m_iCurAnimIndex == AN_SWINGDOWN_L)
        {
            if ((CurTrackPos >= 35.f && CurTrackPos <= 44.f))
            {
                m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
        else
        {
            if ((CurTrackPos >= 40.f && CurTrackPos <= 48.f))
            {
                m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
    }
}

void CState_RaxasiaP1_SwingDown_Multiple::Effect_Check(_double CurTrackPos)
{
}

void CState_RaxasiaP1_SwingDown_Multiple::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP1_SwingDown_Multiple* CState_RaxasiaP1_SwingDown_Multiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_SwingDown_Multiple* pInstance = new CState_RaxasiaP1_SwingDown_Multiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_SwingDown_Multiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_SwingDown_Multiple::Free()
{
    __super::Free();
}