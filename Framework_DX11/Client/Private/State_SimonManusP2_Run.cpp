#include "stdafx.h"
#include "State_SimonManusP2_Run.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_Run::CState_SimonManusP2_Run(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Run::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_SimonManusP2_Run::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_RUN, true, 0.1f, 0);
    m_pMonster->Look_Player();

    return S_OK;
}

void CState_SimonManusP2_Run::Update(_float fTimeDelta)
{
    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
    _Vec3 vMove = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK)) * m_fRunSpeed;

    m_pMonster->Get_RigidBody()->Set_Velocity(vMove);

    if (m_pMonster->Calc_Distance_XZ() <= 5.f)
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
    }
}

void CState_SimonManusP2_Run::End_State()
{
}

CState_SimonManusP2_Run* CState_SimonManusP2_Run::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Run* pInstance = new CState_SimonManusP2_Run(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Run"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Run::Free()
{
    __super::Free();
}