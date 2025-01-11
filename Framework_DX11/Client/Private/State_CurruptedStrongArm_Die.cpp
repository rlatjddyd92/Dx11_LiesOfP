#include "stdafx.h"
#include "State_CurruptedStrongArm_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CurruptedStrongArm_Die::CState_CurruptedStrongArm_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CurruptedStrongArm_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CurruptedStrongArm_Die::Start_State(void* pArg)
{
    _int iAnimIndex = {};

    _Vec3 vRight = XMVectorSetY(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT), 0);
    _Vec3 vDir = m_pMonster->Get_TargetDir();
    vDir.Normalize();
    vRight.Normalize();

    _Vec3 fDirCheck{};
    fDirCheck = vRight.Cross(vDir);


    if (fDirCheck.y < 0)
    {
        iAnimIndex = AN_DIE_F;
    }
    else
    {
        iAnimIndex = AN_DIE_B;
    }
    m_pMonster->Change_Animation(iAnimIndex, false, 0.1f);

    return S_OK;
}

void CState_CurruptedStrongArm_Die::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Get_RigidBody()->Remove_Actor();
    }

}

void CState_CurruptedStrongArm_Die::End_State()
{
}

_bool CState_CurruptedStrongArm_Die::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((AN_DIE_F) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_DIE_F);
    }
    else if ((AN_DIE_B) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_DIE_B);
    }
    else
    {

    }
    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

CState_CurruptedStrongArm_Die* CState_CurruptedStrongArm_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CurruptedStrongArm_Die* pInstance = new CState_CurruptedStrongArm_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CurruptedStrongArm_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CurruptedStrongArm_Die::Free()
{
    __super::Free();
}