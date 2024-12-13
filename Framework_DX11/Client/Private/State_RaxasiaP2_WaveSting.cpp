#include "stdafx.h"
#include "State_RaxasiaP2_WaveSting.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_WaveSting::CState_RaxasiaP2_WaveSting(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_WaveSting::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_WaveSting::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_CHARGE, false, 0.1f, 50);

    m_bSwingSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP2_WaveSting::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_WAVE, false, 0.02f, 50);
            return;
        }

        if (CurTrackPos >= 95.f && CurTrackPos <= 105)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    case 1:
        if (CurTrackPos >= 150.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_STING, false, 0.02f, 115);
            return;
        }

        break;

    case 2:
        if (End_Check())
        {
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

void CState_RaxasiaP2_WaveSting::End_State()
{
}

_bool CState_RaxasiaP2_WaveSting::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_CHARGE) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_CHARGE);
        }
        break;

    case 2:
        if ((AN_STING) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_STING);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_WaveSting::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 60.f && CurTrackPos <= 90.f)||
            (CurTrackPos >= 110.f && CurTrackPos <= 135.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 160.f && CurTrackPos <= 170.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.6f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_STRONG);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP2_WaveSting::Effect_Check(_double CurTrackPos)
{

}

void CState_RaxasiaP2_WaveSting::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP2_WaveSting* CState_RaxasiaP2_WaveSting::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_WaveSting* pInstance = new CState_RaxasiaP2_WaveSting(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_WaveSting"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_WaveSting::Free()
{
    __super::Free();
}