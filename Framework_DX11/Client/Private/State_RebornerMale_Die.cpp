#include "stdafx.h"
#include "State_RebornerMale_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_RebornerMale_Die::CState_RebornerMale_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerMale_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RebornerMale_Die::Start_State(void* pArg)
{
    _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
    _Vec3 vRight = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
    _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());


    _Vec3 vCrossUp = vRight.Cross(vTargetDir);

    _int iAnimIndex = {};

    if (vCrossUp.y <= 0)
    {
        iAnimIndex = AN_DIE_B;
    }
    else
    {
        iAnimIndex = AN_DIE_F;
    }
    m_pMonster->Change_Animation(iAnimIndex, false, 0.1f);

    return S_OK;
}

void CState_RebornerMale_Die::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Get_RigidBody()->Remove_Actor();
    }


}

void CState_RebornerMale_Die::End_State()
{
}

_bool CState_RebornerMale_Die::End_Check()
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

CState_RebornerMale_Die* CState_RebornerMale_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerMale_Die* pInstance = new CState_RebornerMale_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerMale_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerMale_Die::Free()
{
    __super::Free();
}