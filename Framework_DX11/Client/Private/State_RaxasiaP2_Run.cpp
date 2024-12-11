#include "stdafx.h"
#include "State_RaxasiaP2_Run.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_Run::CState_RaxasiaP2_Run(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Run::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_Run::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_RUN, true, 0.1f, 0);

    m_fAttackDelay = 0.f;
    return S_OK;
}

void CState_RaxasiaP2_Run::Update(_float fTimeDelta)
{
    m_fAttackDelay += fTimeDelta;
    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3, fTimeDelta);
    _Vec3 vMove = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK)) * m_fRunSpeed;

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

void CState_RaxasiaP2_Run::End_State()
{
}

CState_RaxasiaP2_Run* CState_RaxasiaP2_Run::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Run* pInstance = new CState_RaxasiaP2_Run(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Run"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Run::Free()
{
    __super::Free();
}