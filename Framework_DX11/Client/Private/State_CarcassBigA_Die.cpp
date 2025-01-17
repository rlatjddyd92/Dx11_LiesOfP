#include "stdafx.h"
#include "State_CarcassBigA_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_Die::CState_CarcassBigA_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassBigA_Die::Start_State(void* pArg)
{
    m_iRouteTrack = 0;

    _int iAnimIndex = {};

    _Vec3 vRight = XMVectorSetY(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT), 0);
    _Vec3 vDir = m_pMonster->Get_TargetDir();
    vDir.Normalize();
    vRight.Normalize();

    _Vec3 fDirCheck{};
    fDirCheck = vRight.Cross(vDir);

    if (fDirCheck.y < 0)
    {
        m_iDirCnt = DIR::DIR_FRONT;
    }
    else
    {
        m_iDirCnt = DIR::DIR_BEHIND;
    }
    m_pMonster->Change_Animation(AN_DIE_B + m_iDirCnt, false, 0.1f);

    return S_OK;
}

void CState_CarcassBigA_Die::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Get_RigidBody()->Remove_Actor();
    }

}

void CState_CarcassBigA_Die::End_State()
{
}

_bool CState_CarcassBigA_Die::End_Check()
{
    return  m_pMonster->Get_EndAnim(AN_DIE_B + m_iDirCnt);;
}

CState_CarcassBigA_Die* CState_CarcassBigA_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_Die* pInstance = new CState_CarcassBigA_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_Die::Free()
{
    __super::Free();
}