#include "stdafx.h"
#include "State_RaxasiaP1_LinkedAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP1_LinkedAttack::CState_RaxasiaP1_LinkedAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_LinkedAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_LinkedAttack::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_LINKEDATTACK_FIRST, false, 0.1f, 0);

    m_bSwingSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP1_LinkedAttack::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 150.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_LINKEDATTACK_SECOND, false, 0.1f, 0);
        }

        if (CurTrackPos <= 50.f || 
            CurTrackPos >= 130.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    case 1:
        if (CurTrackPos >= 130.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_LINKEDATTACK_LAST, false, 0.1f, 0);
        }

        if (CurTrackPos <= 30.f ||
            (CurTrackPos >= 80.f && CurTrackPos <= 90.f) ||
            CurTrackPos >= 115.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    case 2:
        if (End_Check())
        {
            m_iRouteTrack = 0;
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
        break;

        if (CurTrackPos <= 60.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }
    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP1_LinkedAttack::End_State()
{
}

_bool CState_RaxasiaP1_LinkedAttack::End_Check()
{
    return  m_pMonster->Get_EndAnim(AN_LINKEDATTACK_LAST);
}

void CState_RaxasiaP1_LinkedAttack::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 70.f && CurTrackPos <= 110.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 40.f && CurTrackPos <= 65.f) ||
            (CurTrackPos >= 105.f && CurTrackPos <= 130.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 70.f && CurTrackPos <= 100.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP1_LinkedAttack::Effect_Check(_double CurTrackPos)
{
    //if (m_iRouteTrack == 0)
    //{
    //    if ((CurTrackPos >= 110.f && CurTrackPos <= 220.f))
    //    {
    //        if (!m_bSwing)
    //        {
    //            m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
    //            m_bSwing = true;
    //        }
    //    }
    //    else
    //    {
    //        m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
    //    }
    //}
    //else if (m_iRouteTrack == 1)
    //{
    //    if ((CurTrackPos >= 40.f && CurTrackPos <= 65.f) ||
    //        (CurTrackPos >= 105.f && CurTrackPos <= 130.f))
    //    {
    //        if (!m_bSwing)
    //        {
    //            m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
    //            m_bSwing = true;
    //        }
    //    }
    //    else
    //    {
    //        m_bSwing = false;
    //        m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
    //    }
    //}
    //else if (m_iRouteTrack == 2)
    //{
    //    if ((CurTrackPos >= 70.f && CurTrackPos <= 100.f))
    //    {
    //        if (!m_bSwing)
    //        {
    //            m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
    //            m_bSwing = true;
    //        }
    //    }
    //    else
    //    {
    //        m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
    //    }
    //}
}

void CState_RaxasiaP1_LinkedAttack::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP1_LinkedAttack* CState_RaxasiaP1_LinkedAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_LinkedAttack* pInstance = new CState_RaxasiaP1_LinkedAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_LinkedAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_LinkedAttack::Free()
{
    __super::Free();
}