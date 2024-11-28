#include "stdafx.h"
#include "State_CarcassTail_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassTail_Die::CState_CarcassTail_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassTail_Die::Start_State(void* pArg)
{

    _vector vPos = XMVectorSetY(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION), 0);
    _vector vLook = XMVectorSetY(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK), 0);
    _vector vDir = XMVectorSetY(m_pGameInstance->Get_CamPosition_Vec4(), 0);
    vDir = vDir - vPos;

    _float fRadian{};
    fRadian = acos(XMVectorGetX(XMVector3Dot(vLook, vDir)));

    if (XMConvertToDegrees(fRadian) < 90)
    {
        m_pMonster->Change_Animation(AN_DIE_F, false, 0.1f);
    }
    else
    {
        m_pMonster->Change_Animation(AN_DIE_B, false, 0.1f);
    }

    return S_OK;
}

void CState_CarcassTail_Die::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        //몬스터 사망
        m_pMonster->Change_State(CCarcassBigA::IDLE);   //임시

    }

}

void CState_CarcassTail_Die::End_State()
{
}

_bool CState_CarcassTail_Die::End_Check()
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

CState_CarcassTail_Die* CState_CarcassTail_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_Die* pInstance = new CState_CarcassTail_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_Die::Free()
{
    __super::Free();
}