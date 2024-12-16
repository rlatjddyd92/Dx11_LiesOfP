#include "stdafx.h"
#include "State_RaxasiaP2_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "GameInterface_Controller.h"

CState_RaxasiaP2_Idle::CState_RaxasiaP2_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Idle::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);

    if (pArg != nullptr)
    {
        m_fIdleTime = *static_cast<_float*>(pArg);
    }
    else
    {
        m_fIdleTime = 0;
    }

    return S_OK;
}

void CState_RaxasiaP2_Idle::Update(_float fTimeDelta)
{
    m_fIdleTime += fTimeDelta;
    _float fDist = m_pMonster->Calc_Distance_XZ();
    //if (!m_bFirstMeetCheck)
    //{
    //    _Vec3 vTargetPos = m_pMonster->Get_TargetPos();
    //    _Vec3 vMonsterPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    //    if (fDist <= 25.f && abs(vTargetPos.y - vMonsterPos.y) <= 5.f)
    //    {
    //        GET_GAMEINTERFACE->Set_OnOff_OrthoUI(true, m_pMonster);
    //        m_bFirstMeetCheck = true;
    //    }
    //    else
    //    {
    //        return;
    //    }
    //}

    if (m_fIdleEndDuration <= m_fIdleTime)
    {
        if (fDist >= 30.f)
        {
            return;
        }

        if (fDist <= 15.f)
        {
            ++m_iAtkTrack;
            Calc_Act_Attack(fDist);
            return;
        }
        else if (fDist > 25.f)
        {
            m_pMonster->Change_State(CRaxasia::RUN);
            return;
        }
        else if (fDist > 15.f)
        {
            m_pMonster->Change_State(CRaxasia::WALK);
            return;
        }

        return;
    }

    _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3, fTimeDelta);
    switch (iDir)
    {
    case -1:
        m_pMonster->Change_Animation(AN_TURNLEFT, true, 0.1f, 0);
        break;

    case 0:
        m_pMonster->Change_Animation(AN_IDLE, true, 0.1f, 0);
        break;

    case 1:
        m_pMonster->Change_Animation(AN_TURNRIGHT, true, 0.1f, 0);
        break;

    default:
        break;
    }

}

void CState_RaxasiaP2_Idle::End_State()
{
}

void CState_RaxasiaP2_Idle::Calc_Act_Attack(_float fDist)
{
    if (m_iAtkTrack >= 11)
    {
        m_iAtkTrack = 0.f;
    }

    switch (m_iAtkTrack)
    {
    case 0:
        m_pMonster->Change_State(CRaxasia::ATKP2_JUMPSTAMP);
        return;

    case 1:
        m_pMonster->Change_State(CRaxasia::ATKP2_RUNNING);
        return;

    case 2:
        m_pMonster->Change_State(CRaxasia::ATKP2_STEPJUMP);
        return;

    case 3:
        m_pMonster->Change_State(CRaxasia::ATKP2_TELE);
        return;

    case 4:
        m_pMonster->Change_State(CRaxasia::ATKP2_TELEPORTATTACK);
        return;

    case 5:
        m_pMonster->Change_State(CRaxasia::ATKP2_WAVESTING);
        return;

    case 6:
        m_pMonster->Change_State(CRaxasia::ATKP2_GUARD);
        return;

    case 7:
        m_pMonster->Change_State(CRaxasia::ATKP2_CHARGERUSH);
        return;

    case 8:
        m_pMonster->Change_State(CRaxasia::ATKP2_DECLAREWAR);
        return;

    case 9:
        m_pMonster->Change_State(CRaxasia::ATKP2_SHIELDRUSH);
        return;

    case 10:
        m_pMonster->Change_State(CRaxasia::ATKP2_SHIELDLINKED);
        return;

    default:
        return;
    }
}

CState_RaxasiaP2_Idle* CState_RaxasiaP2_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Idle* pInstance = new CState_RaxasiaP2_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Idle::Free()
{
    __super::Free();
}