#include "stdafx.h"
#include "State_CarcassTail_Run.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassTail.h"

CState_CarcassTail_Run::CState_CarcassTail_Run(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_Run::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_Run::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_RUN, true, 0.f, 0);
    m_pMonster->Look_Player();

    return S_OK;
}

void CState_CarcassTail_Run::Update(_float fTimeDelta)
{
    _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
    _float fDist = m_pMonster->Calc_Distance_XZ();
    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);

    if (iDir == 0 && fDist <= 20.f && fDist >= 10.f)
    {
        _int iAtkNum = rand() % 2;
        switch (iAtkNum)
        {
        case 0:
            m_pMonster->Change_State(CCarcassTail::LEAP);
            return;
            break;

        case 1:
            m_pMonster->Change_State(CCarcassTail::LEAPTOATTACK);
            return;
            break;

        default:
            break;
        }
    }

    _Vec3 vDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    m_pMonster->Get_RigidBody()->Set_Velocity(XMVector3Normalize(vDir) * m_fRunSpeed);


    if (m_pMonster->Calc_Distance_XZ() <= 6.f)
    {
        m_pMonster->Change_State(CCarcassTail::IDLE);
    }
}

void CState_CarcassTail_Run::End_State()
{
}

CState_CarcassTail_Run* CState_CarcassTail_Run::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_Run* pInstance = new CState_CarcassTail_Run(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_Run"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_Run::Free()
{
    __super::Free();
}