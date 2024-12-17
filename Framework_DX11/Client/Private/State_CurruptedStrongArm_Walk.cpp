#include "stdafx.h"
#include "State_CurruptedStrongArm_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CurruptedStrongArm_Walk::CState_CurruptedStrongArm_Walk(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CurruptedStrongArm_Walk::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CurruptedStrongArm_Walk::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(33, true, 0.1f, 0);


    return S_OK;
}

void CState_CurruptedStrongArm_Walk::Update(_float fTimeDelta)
{
    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
    _Vec3 vDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

    m_pMonster->Get_RigidBody()->Set_Velocity(XMVector3Normalize(vDir) * m_fWalkSpeed);



    if (m_pMonster->Calc_Distance_XZ() <= 6.f)
    {
        m_pMonster->Change_State(CCarcassBigA::IDLE);
    }
}

void CState_CurruptedStrongArm_Walk::End_State()
{
}

CState_CurruptedStrongArm_Walk* CState_CurruptedStrongArm_Walk::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CurruptedStrongArm_Walk* pInstance = new CState_CurruptedStrongArm_Walk(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CurruptedStrongArm_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CurruptedStrongArm_Walk::Free()
{
    __super::Free();
}