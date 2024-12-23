#include "stdafx.h"
#include "State_RebornerBigA_KnockBack.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_RebornerBigA_KnockBack::CState_RebornerBigA_KnockBack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerBigA_KnockBack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RebornerBigA_KnockBack::Start_State(void* pArg)
{
    _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
    _Vec3 vRight = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
    _Vec3 vLook = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK));
    _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());


    _Vec3 vCrossUp = vRight.Cross(vTargetDir);

    if (vCrossUp.y <= 0)
    {
        _Vec3 vCrossUpForLR = vLook.Cross(vTargetDir);
        if (vCrossUpForLR.y <= 0)   //¿ÞÂÊ
            m_iCurAnim = AN_KNOCKBACK_R;
        else                        //¿À¸¥ÂÊ
            m_iCurAnim = AN_KNOCKBACK_L;
        
    }
    else
    {
        m_iCurAnim = AN_KNOCKBACK_B;
    }

    m_pMonster->Change_Animation(m_iCurAnim, false, 0.1f);

    return S_OK;
}

void CState_RebornerBigA_KnockBack::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Change_State(CCarcassBigA::IDLE);
    }

}

void CState_RebornerBigA_KnockBack::End_State()
{
}

_bool CState_RebornerBigA_KnockBack::End_Check()
{
    return m_pMonster->Get_EndAnim(m_iCurAnim);
}

CState_RebornerBigA_KnockBack* CState_RebornerBigA_KnockBack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerBigA_KnockBack* pInstance = new CState_RebornerBigA_KnockBack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerBigA_KnockBack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerBigA_KnockBack::Free()
{
    __super::Free();
}