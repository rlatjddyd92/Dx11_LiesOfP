#include "stdafx.h"
#include "State_RaxasiaP2_ChargeRush.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_ChargeRush::CState_RaxasiaP2_ChargeRush(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_ChargeRush::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_ChargeRush::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_START, false, 0.1f, 0);

    m_bSwingSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP2_ChargeRush::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_CHARGE, false, 0.02f, 0);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
    }
    else if (m_iRouteTrack>= 1 && m_iRouteTrack <= 2)
    {
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            if (m_iRouteTrack == 3)
            {
                m_pMonster->Change_Animation(AN_SHIELDRUSH, false, 0.2f, 0);
                return;
            }
            else
            {
                m_pMonster->Change_Animation(AN_CHARGE, false, 0.f, 0, true, true);
                return;
            }
        }
    }
    else if (m_iRouteTrack >= 3 && m_iRouteTrack <= 4)
    {
        if (CurTrackPos >= 165.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;

            m_pMonster->Change_Animation(AN_SHIELDRUSH, false, 0.1f, 0, true, true);
            return;
        }
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
    }
    else    //퓨리어택. 
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_ChargeRush::End_State()
{
}

_bool CState_RaxasiaP2_ChargeRush::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_START) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_START);
        }
        break;

    case 1:
        if ((AN_SHIELDRUSH) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_SHIELDRUSH);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_ChargeRush::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 60.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP2_ChargeRush::Effect_Check(_double CurTrackPos)
{

}

void CState_RaxasiaP2_ChargeRush::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP2_ChargeRush* CState_RaxasiaP2_ChargeRush::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_ChargeRush* pInstance = new CState_RaxasiaP2_ChargeRush(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_ChargeRush"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_ChargeRush::Free()
{
    __super::Free();
}