#include "stdafx.h"
#include "State_RaxasiaP1_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP1_Walk::CState_RaxasiaP1_Walk(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_Walk::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP1_Walk::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_WALK_FRONT, true, 0.1f, 0);;


    m_fAttackDelay = 0.f;
    return S_OK;
}

void CState_RaxasiaP1_Walk::Update(_float fTimeDelta)
{
    m_fAttackDelay += fTimeDelta;
    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
    _Vec3 vMove = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK)) * m_fWalkSpeed;

    m_pMonster->Get_RigidBody()->Set_Velocity(vMove);

    _float fDist = m_pMonster->Calc_Distance_XZ();

    if (fDist <= 7.f)
    {
        m_pMonster->Change_State(CRaxasia::IDLE);
    }
    else if (fDist <= 10.f && m_fAttackDelay >= 2.f)
    {
        _float fTime = { 1.5f };
        m_pMonster->Change_State(CRaxasia::IDLE, &fTime);
        return;
    }
}

void CState_RaxasiaP1_Walk::End_State()
{
}

CState_RaxasiaP1_Walk* CState_RaxasiaP1_Walk::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_Walk* pInstance = new CState_RaxasiaP1_Walk(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_Walk::Free()
{
    __super::Free();
}