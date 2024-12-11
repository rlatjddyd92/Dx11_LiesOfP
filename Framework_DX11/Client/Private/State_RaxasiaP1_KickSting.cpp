#include "stdafx.h"
#include "State_RaxasiaP1_KickSting.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP1_KickSting::CState_RaxasiaP1_KickSting(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_KickSting::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_KickSting::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_KICKSTING, false, 0.1f, 0);

    m_bSwingSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP1_KickSting::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 170.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_JUMPATTACK, false, 0.1f, 0);
        }

        if (CurTrackPos >= 150.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    case 1:
        if (End_Check())
        {
            m_iRouteTrack = 0;
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP1_KickSting::End_State()
{
}

_bool CState_RaxasiaP1_KickSting::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_JUMPATTACK);;
}

void CState_RaxasiaP1_KickSting::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 30.f && CurTrackPos <= 35.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }

        if ((CurTrackPos >= 50.f && CurTrackPos <= 85.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 55.f && CurTrackPos <= 65.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP1_KickSting::Effect_Check(_double CurTrackPos)
{
}

void CState_RaxasiaP1_KickSting::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP1_KickSting* CState_RaxasiaP1_KickSting::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_KickSting* pInstance = new CState_RaxasiaP1_KickSting(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_KickSting"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_KickSting::Free()
{
    __super::Free();
}