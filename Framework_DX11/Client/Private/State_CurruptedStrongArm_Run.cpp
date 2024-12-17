#include "stdafx.h"
#include "State_CurruptedStrongArm_Run.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CurruptedStrongArm_Run::CState_CurruptedStrongArm_Run(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CurruptedStrongArm_Run::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CurruptedStrongArm_Run::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_RUN, true, 0.1f, 0);

    return S_OK;
}

void CState_CurruptedStrongArm_Run::Update(_float fTimeDelta)
{
    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.5f, fTimeDelta);
    _Vec3 vDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

    m_pMonster->Get_RigidBody()->Set_Velocity(XMVector3Normalize(vDir) * m_fRunSpeed);


    if (m_pMonster->Calc_Distance_XZ() <= 6.f)
    {
        m_pMonster->Change_State(CCarcassBigA::IDLE);
    }
}

void CState_CurruptedStrongArm_Run::End_State()
{
}

CState_CurruptedStrongArm_Run* CState_CurruptedStrongArm_Run::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CurruptedStrongArm_Run* pInstance = new CState_CurruptedStrongArm_Run(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CurruptedStrongArm_Run"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CurruptedStrongArm_Run::Free()
{
    __super::Free();
}