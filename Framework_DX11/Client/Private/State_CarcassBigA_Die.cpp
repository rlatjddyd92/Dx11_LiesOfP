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
    _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
    _Vec3 vRight = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
    _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());


    _Vec3 vCrossUp = vRight.Cross(vTargetDir);

    _int iAnimIndex = {};

    if (vCrossUp.y <= 0)
    {
        m_iDirCnt = DIR::DIR_FRONT;
    }
    else
    {
        m_iDirCnt = DIR::DIR_BEHIND;
    }
    m_pMonster->Change_Animation(iAnimIndex, false, 0.1f);

    return S_OK;
}

void CState_CarcassBigA_Die::Update(_float fTimeDelta)
{
    //if (End_Check())
    //{
    //    //몬스터 사망
    //    m_pMonster->Change_State(CCarcassBigA::IDLE);   //임시
    //
    //}

}

void CState_CarcassBigA_Die::End_State()
{
}

_bool CState_CarcassBigA_Die::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((AN_DIE_START_F) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_DIE_START_F);
    }
    else if ((AN_DIE_START_B) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_DIE_START_B);
    }
    else if ((AN_DIE_LOOP_F) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_DIE_LOOP_F);
    }
    else if ((AN_DIE_LOOP_B) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_DIE_LOOP_B);
    }
    else if ((AN_DIE_END_F) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_DIE_END_F);
    }
    else if ((AN_DIE_END_B) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_DIE_END_B);
    }
    else
    {

    }
    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
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