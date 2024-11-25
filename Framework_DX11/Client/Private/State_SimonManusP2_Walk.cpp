#include "stdafx.h"
#include "State_SimonManusP2_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_Walk::CState_SimonManusP2_Walk(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Walk::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_SimonManusP2_Walk::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_WALK_FRONT, true, 0.1f, 0);;


    return S_OK;
}

void CState_SimonManusP2_Walk::Update(_float fTimeDelta)
{
    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
    _Vec3 vMove = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK)) * m_fWalkSpeed;

    m_pMonster->Get_RigidBody()->Set_Velocity(vMove);

    if (m_pMonster->Calc_Distance_XZ() <= 5.f)
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
    }
}

void CState_SimonManusP2_Walk::End_State()
{
}

CState_SimonManusP2_Walk* CState_SimonManusP2_Walk::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Walk* pInstance = new CState_SimonManusP2_Walk(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Walk::Free()
{
    __super::Free();
}