#include "stdafx.h"
#include "State_CarcassBigA_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Idle::CState_CarcassBigA_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Idle::Initialize(_uint iStateNum, void* pArg)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    return S_OK;
}

HRESULT CState_CarcassBigA_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(20, true, true);
    
    return S_OK;
}

void CState_CarcassBigA_Idle::Update(_float fTimeDelta)
{
    m_fIdleTime += fTimeDelta;
    if (m_fIdleEndDuration <= m_fIdleTime)
    {
        if (m_pMonster->Calc_Distance_XZ() <= 5.f)
        {
            m_pMonster->Change_State(CCarcassBigA::ATTACK);
        }
        else if (m_pMonster->Calc_Distance_XZ() > 8.f)
        {
            m_pMonster->Change_State(CCarcassBigA::RUN);
        }
        else if (m_pMonster->Calc_Distance_XZ() > 5.f)
        {
            m_pMonster->Change_State(CCarcassBigA::WALK);
        }

    }
    _vector vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _vector vCamPos = m_pGameInstance->Get_CamPosition_Vec4();//임시사용 캠 포지션

    if (!(XMVectorGetX(vPos) == XMVectorGetX(vCamPos)) 
        || !(XMVectorGetZ(vPos) == XMVectorGetZ(vCamPos)))
    {
        _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(vCamPos - vPos, 1, fTimeDelta);
        switch (iDir)
        {
        case -1:
            m_pMonster->Change_Animation(30, true, true);
            break;

        case 0:
            m_pMonster->Change_Animation(20, true, true);
            break;

        case 1:
            m_pMonster->Change_Animation(31, true, true);
            break;

        default:
            break;
        }

    }

    if (KEY_TAP(KEY::Z))
    {
        m_pMonster->Change_State(CCarcassBigA::GROGY);
    }

    if (KEY_TAP(KEY::C))
    {
        m_pMonster->Change_State(CCarcassBigA::PARALIZE);
    }

    if (KEY_TAP(KEY::V))
    {
        m_pMonster->Change_State(CCarcassBigA::DIE);
    }

}

void CState_CarcassBigA_Idle::End_State()
{
    m_fIdleTime = 0.f;
}

CState_CarcassBigA_Idle* CState_CarcassBigA_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_Idle* pInstance = new CState_CarcassBigA_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Idle::Free()
{
    __super::Free();
}