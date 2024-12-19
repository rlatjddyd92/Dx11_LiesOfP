#include "stdafx.h"
#include "State_CarcassNormal_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassNormal.h"

CState_CarcassNormal_Die::CState_CarcassNormal_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassNormal_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassNormal_Die::Start_State(void* pArg)
{

    _vector vPos = XMVectorSetY(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION), 0);
    _vector vLook = XMVectorSetY(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK), 0);
    _vector vDir = XMVectorSetY(m_pGameInstance->Get_CamPosition_Vec4(), 0);
    vDir = vDir - vPos;

    _float fRadian{};
    fRadian = acos(XMVectorGetX(XMVector3Dot(vLook, vDir)));
    _int iAnimIndex = {};

    if (XMConvertToDegrees(fRadian) < 90)
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

void CState_CarcassNormal_Die::Update(_float fTimeDelta)
{
    //if (End_Check())
    //{
    //    //몬스터 사망
    //    m_pMonster->Change_State(CCarcassBigA::IDLE);   //임시
    //
    //}

}

void CState_CarcassNormal_Die::End_State()
{
}

_bool CState_CarcassNormal_Die::End_Check()
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

CState_CarcassNormal_Die* CState_CarcassNormal_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassNormal_Die* pInstance = new CState_CarcassNormal_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassNormal_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassNormal_Die::Free()
{
    __super::Free();
}