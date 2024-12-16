#include "stdafx.h"
#include "State_RaxasiaP2_Declare_War.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_Declare_War::CState_RaxasiaP2_Declare_War(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Declare_War::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_Declare_War::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_HOWLLING, false, 0.1f, 50);

    m_bSwingSound = false;

    m_bSwing = false;
    m_bStart = false;

    return S_OK;
}

void CState_RaxasiaP2_Declare_War::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_DECLARE_WAR, false, 0.1f, 0);
            return;
        }
        
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        
        break;

    case 1:
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

void CState_RaxasiaP2_Declare_War::End_State()
{
}

_bool CState_RaxasiaP2_Declare_War::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_HOWLLING) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_HOWLLING);
        }
        break;

    case 2:
        if ((AN_DECLARE_WAR) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_DECLARE_WAR);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_Declare_War::Collider_Check(_double CurTrackPos)
{
}

void CState_RaxasiaP2_Declare_War::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bStart)
        {
            if (CurTrackPos >= 50.f)
            {
                m_bStart = true;
                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2, true);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_HOWLING, true);
            }
        }
        else
        {
            if (CurTrackPos >= 300.f)
            {
                m_pMonster->DeActive_Effect(CRaxasia::EFFECT_HOWLING);
            }
        }
    }
    else
    {

    }

}

void CState_RaxasiaP2_Declare_War::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP2_Declare_War* CState_RaxasiaP2_Declare_War::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Declare_War* pInstance = new CState_RaxasiaP2_Declare_War(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Declare_War"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Declare_War::Free()
{
    __super::Free();
}