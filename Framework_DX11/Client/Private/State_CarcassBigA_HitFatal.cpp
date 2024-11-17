#include "stdafx.h"
#include "State_CarcassBigA_HitFatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassBigA_HitFatal::CState_CarcassBigA_HitFatal(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_HitFatal::Initialize(_uint iStateNum, void* pArg)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;

    return S_OK;
}

HRESULT CState_CarcassBigA_HitFatal::Start_State(void* pArg)
{
    *m_pResetRootMove = false;

    _vector vPos = XMVectorSetY(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION), 0);
    _vector vLook = XMVectorSetY(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK), 0);
    _vector vDir = XMVectorSetY(m_pGameInstance->Get_CamPosition_Vec4(), 0);
    vDir = vDir - vPos;

    _float fRadian{};
    fRadian = acos(XMVectorGetX(XMVector3Dot(vLook, vDir)));

    if (XMConvertToDegrees(fRadian) < 90)
    {
        m_iDirCnt = DIR::DIR_FRONT;
    }
    else
    {
        m_iDirCnt = DIR::DIR_BEHIND;
    }
    m_pMonster->Change_Animation(14 - m_iDirCnt - (2 * m_iAnimCnt), false, true);

    return S_OK;
}

void CState_CarcassBigA_HitFatal::Update(_float fTimeDelta)
{
    if (*m_pIsEndAnim)
    {
        if (m_iAnimCnt < 2)
        {
            ++m_iAnimCnt;
            m_pMonster->Change_Animation(14 - m_iDirCnt - (2 * m_iAnimCnt), false, true);
        }
        else
        {
            m_iAnimCnt = 0;
            m_pMonster->Change_State(CCarcassBigA::GROGY);
        }
    }

}

void CState_CarcassBigA_HitFatal::End_State()
{
    m_fHitFatalTime = 0.f;
    *m_pResetRootMove = true;
}

_bool CState_CarcassBigA_HitFatal::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((AN_FATAL_START_F) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_START_F);
    }
    else if ((AN_FATAL_START_B) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_START_B);
    }
    else if ((AN_FATAL_LOOP_F) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_LOOP_F);
    }
    else if ((AN_FATAL_LOOP_B) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_LOOP_B);
    }
    else if ((AN_FATAL_END_F) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_END_F);
    }
    else if ((AN_FATAL_END_B) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_FATAL_END_B);
    }
    else
    {

    }
    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

CState_CarcassBigA_HitFatal* CState_CarcassBigA_HitFatal::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_HitFatal* pInstance = new CState_CarcassBigA_HitFatal(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassBigA_HitFatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_HitFatal::Free()
{
    __super::Free();
}