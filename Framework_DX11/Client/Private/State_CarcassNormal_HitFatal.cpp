#include "stdafx.h"
#include "State_CarcassNormal_HitFatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassNormal.h"

CState_CarcassNormal_HitFatal::CState_CarcassNormal_HitFatal(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassNormal_HitFatal::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassNormal_HitFatal::Start_State(void* pArg)
{
    m_iAnimTrack = 0;


    _Vec3 vRight = XMVectorSetY(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT), 0);
    _Vec3 vDir = m_pMonster->Get_TargetDir();
    vDir.Normalize();
    vRight.Normalize();

    _Vec3 fDirCheck{};
    fDirCheck = vRight.Cross(vDir);

    if (fDirCheck.y < 0)
    {
        m_iDirCnt = DIR::DIR_BEHIND ;
    }
    else
    {
        m_iDirCnt = DIR::DIR_FRONT * 2;
    }

    m_pMonster->Change_Animation(AN_FATAL_START, false, 0.f);

    return S_OK;
}

void CState_CarcassNormal_HitFatal::Update(_float fTimeDelta)
{
    switch (m_iAnimTrack)
    {
    case 0:     //페이탈 시작
        if (End_Check())
        {
            ++m_iAnimTrack;
            m_pMonster->Change_Animation(AN_FATAL_LOOP, false, 0.f);
            return;
        }
        break;

    case 1:     //페이탈 루프
        if (End_Check())
        {
            ++m_iAnimTrack;
            m_pMonster->Change_Animation(AN_DOWN_B + m_iDirCnt, false, 0.f);
            return;
        }
        break;

    case 2:     //넘어짐
        if (End_Check())
        {
            m_pMonster->Change_State(CCarcassNormal::IDLE);
            return;
        }
        break;

    default:
        break;
    }

}

void CState_CarcassNormal_HitFatal::End_State()
{
    m_fHitFatalTime = 0.f;
}

_bool CState_CarcassNormal_HitFatal::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((AN_FATAL_START) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_START);
    }
    else if ((AN_FATAL_LOOP) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_LOOP);
    }
    if ((AN_DOWN_B + m_iDirCnt) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim((AN_DOWN_B + m_iDirCnt));
    }
    else
    {

    }
    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

CState_CarcassNormal_HitFatal* CState_CarcassNormal_HitFatal::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassNormal_HitFatal* pInstance = new CState_CarcassNormal_HitFatal(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassNormal_HitFatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassNormal_HitFatal::Free()
{
    __super::Free();
}