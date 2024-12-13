#include "stdafx.h"
#include "State_RaxasiaP2_ShieldLinked.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_ShieldLinked::CState_RaxasiaP2_ShieldLinked(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_ShieldLinked::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_ShieldLinked::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_START, false, 0.1f, 0);

    m_bSwingSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP2_ShieldLinked::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_SHIELDATTACK_TWICE, false, 0.02f, 0);
            return;
        }
        
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);

        break;

    case 1:
        if (CurTrackPos >= 255)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_START, false, 0.02f, 0);
            return;
        }

        if (CurTrackPos <= 110.f || 
            (CurTrackPos >= 155.f && CurTrackPos <= 165)||
            CurTrackPos >= 205.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    case 2:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_SHIELDATTACK, false, 0.02f, 0);
        }
        
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);

        break;

    case 3:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_JUMPMAGIC, false, 0.02f, 0);
        }

        if (CurTrackPos <= 25.f ||
            CurTrackPos >= 70.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }
        break;

    case 4:
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);

        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_ShieldLinked::End_State()
{
}

_bool CState_RaxasiaP2_ShieldLinked::End_Check()
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
        if ((AN_SHIELDATTACK_TWICE) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_SHIELDATTACK_TWICE);
        }
        break;

    case 2:
        if ((AN_START) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_START);
        }
        break;

    case 3:
        if ((AN_SHIELDATTACK) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_SHIELDATTACK);
        }
        break;

    case 4:
        if ((AN_JUMPMAGIC) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_JUMPMAGIC);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_ShieldLinked::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 130.f && CurTrackPos <= 145.f) ||
            (CurTrackPos >= 175.f && CurTrackPos <= 195.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack == 3)
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

void CState_RaxasiaP2_ShieldLinked::Effect_Check(_double CurTrackPos)
{

}

void CState_RaxasiaP2_ShieldLinked::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP2_ShieldLinked* CState_RaxasiaP2_ShieldLinked::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_ShieldLinked* pInstance = new CState_RaxasiaP2_ShieldLinked(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_ShieldLinked"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_ShieldLinked::Free()
{
    __super::Free();
}