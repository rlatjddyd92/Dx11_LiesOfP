#include "stdafx.h"
#include "State_SimonManusP1_Idle.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManusP1.h"

CState_SimonManusP1_Idle::CState_SimonManusP1_Idle(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_Idle::Initialize(_uint iStateNum, void* pArg)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    return S_OK;
}

HRESULT CState_SimonManusP1_Idle::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_IDLE, true, true);
    
    return S_OK;
}

void CState_SimonManusP1_Idle::Update(_float fTimeDelta)
{
    m_fIdleTime += fTimeDelta;
    if (m_fIdleEndDuration <= m_fIdleTime)
    {
        if (m_pMonster->Calc_Distance_XZ() <= 5.f)
        {
            m_pMonster->Change_State(CSimonManusP1::ATTACK);
        }
        else if (m_pMonster->Calc_Distance_XZ() > 8.f)
        {
            m_pMonster->Change_State(CSimonManusP1::RUN);
        }
        else if (m_pMonster->Calc_Distance_XZ() > 5.f)
        {
            m_pMonster->Change_State(CSimonManusP1::WALK);
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
            m_pMonster->Change_Animation(AN_TURNLEFT, true, true);
            break;

        case 0:
            m_pMonster->Change_Animation(AN_IDLE, true, true);
            break;

        case 1:
            m_pMonster->Change_Animation(AN_TURNRIGHT, true, true);
            break;

        default:
            break;
        }

    }
    if (KEY_TAP(KEY::Z))
    {
        m_pMonster->Change_State(CSimonManusP1::GROGY);
    }

    if (KEY_TAP(KEY::C))
    {
        m_pMonster->Change_State(CSimonManusP1::PARALYZE);
    }

    if (KEY_TAP(KEY::V))
    {
        m_pMonster->Change_State(CSimonManusP1::DIE);
    }

}

void CState_SimonManusP1_Idle::End_State()
{
    m_fIdleTime = 0.f;
}

CState_SimonManusP1_Idle* CState_SimonManusP1_Idle::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_Idle* pInstance = new CState_SimonManusP1_Idle(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_Idle::Free()
{
    __super::Free();
}