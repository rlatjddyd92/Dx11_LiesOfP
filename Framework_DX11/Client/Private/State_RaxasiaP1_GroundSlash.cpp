#include "stdafx.h"
#include "State_RaxasiaP1_GroundSlash.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP1_GroundSlash::CState_RaxasiaP1_GroundSlash(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_GroundSlash::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_GroundSlash::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_GROUNDSLASH, false, 0.1f, 3);

    m_bSwingSound = false;

    m_bSwing = false;
    m_bResetList = false;
    m_bAddMark = false;
    m_bInchent = false;

    return S_OK;
}

void CState_RaxasiaP1_GroundSlash::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        ++m_iRouteTrack;
        if (m_iRouteTrack >= 3)
        {
            m_iRouteTrack = 0;
        }

        m_iRouteTrack = 2;
        switch (m_iRouteTrack)
        {
        case 0:
            m_pMonster->Change_State(CRaxasia::ATK_DISCHARGE);
            return;

        case 1:
            m_pMonster->Change_State(CRaxasia::ATK_JUMPATTACK);
            return;

        case 2:
            m_pMonster->Change_State(CRaxasia::ATK_SWINGDOWN_MULTIPLE);
            return;

        default:
            break;
        }
    }

    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 0.6f, fTimeDelta);

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP1_GroundSlash::End_State()
{
    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD);
}

_bool CState_RaxasiaP1_GroundSlash::End_Check()
{
    return  m_pMonster->Get_EndAnim(AN_GROUNDSLASH);
}

void CState_RaxasiaP1_GroundSlash::Collider_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 110.f && CurTrackPos <= 220.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }

    if (!m_bResetList)
    {
        if (CurTrackPos >= 185.f)
        {
            m_bResetList = true;
            m_pMonster->Reset_WeaponOverlapCheck();
        }
    }
}

void CState_RaxasiaP1_GroundSlash::Effect_Check(_double CurTrackPos)
{
    if (!m_bInchent)
    {
        if (CurTrackPos >= 60.f)
        {
            m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD, true);
            m_bInchent = true;
        }
    }

    if ((CurTrackPos >= 120.f && CurTrackPos <= 125.f) ||
        (CurTrackPos >= 150.f && CurTrackPos <= 155.f) ||
        (CurTrackPos >= 175.f && CurTrackPos <= 180.f) ||
        (CurTrackPos >= 195.f && CurTrackPos <= 200.f) ||
        (CurTrackPos >= 205.f && CurTrackPos <= 210.f))
    {
        if (!m_bAddMark)
        {
            //어택오브젝트로 잠시 있다 터지는 번개 생성
        }
    }
    else
    {
        m_bAddMark = false;
    }

}

void CState_RaxasiaP1_GroundSlash::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP1_GroundSlash* CState_RaxasiaP1_GroundSlash::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_GroundSlash* pInstance = new CState_RaxasiaP1_GroundSlash(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_GroundSlash"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_GroundSlash::Free()
{
    __super::Free();
}