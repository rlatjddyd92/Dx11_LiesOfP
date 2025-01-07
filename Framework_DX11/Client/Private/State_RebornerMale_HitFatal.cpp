#include "stdafx.h"
#include "State_RebornerMale_HitFatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "RebornerBigA.h"

CState_RebornerMale_HitFatal::CState_RebornerMale_HitFatal(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerMale_HitFatal::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);
    m_pFatalAttacked = m_pMonster->Get_bFatalAttacked();
    m_pBlockDead = m_pMonster->Get_BlockDead();
    return S_OK;
}

HRESULT CState_RebornerMale_HitFatal::Start_State(void* pArg)
{
    m_iAnimTrack = 0;
    *m_pBlockDead = true;

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

    m_pMonster->Change_Animation(AN_FATAL_START, false, 0.f);

    return S_OK;
}

void CState_RebornerMale_HitFatal::Update(_float fTimeDelta)
{
    switch (m_iAnimTrack)
    {
    case 0:     //페이탈 시작
        if (End_Check())
        {
            ++m_iAnimTrack;
            m_pMonster->Change_Animation(AN_FATAL_LOOP, true, 0.f);
            return;
        }
        break;

    case 1:     //페이탈 루프
        if ((*m_pFatalAttacked) == true)
        {
            *m_pBlockDead = false;
            ++m_iAnimTrack;
            m_pMonster->Change_Animation(AN_DOWN_B + m_iDirCnt, false, 0.1f);

            *m_pFatalAttacked = false;

            return;
        }
        break;

    case 2:     //넘어짐
        if (End_Check())
        {
            ++m_iAnimTrack;
            m_pMonster->Change_Animation(AN_UP_B + m_iDirCnt, false, 0.f);
            return;
        }
        break;

    case 3:     //일어섬
        if (End_Check())
        {
            m_pMonster->Change_State(CRebornerBigA::IDLE);
            return;
        }
        break;


    default:
        break;
    }

}

void CState_RebornerMale_HitFatal::End_State()
{
    m_fHitFatalTime = 0.f;
}

_bool CState_RebornerMale_HitFatal::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((AN_FATAL_START) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_START);
    }
    else if ((AN_DOWN_B + m_iDirCnt) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim((AN_DOWN_B + m_iDirCnt));
    }
    else if ((AN_UP_B + m_iDirCnt) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim((AN_UP_B + m_iDirCnt));
    }
    else
    {

    }
    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

CState_RebornerMale_HitFatal* CState_RebornerMale_HitFatal::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerMale_HitFatal* pInstance = new CState_RebornerMale_HitFatal(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerMale_HitFatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerMale_HitFatal::Free()
{
    __super::Free();
}