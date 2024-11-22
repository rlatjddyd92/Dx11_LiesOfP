#include "stdafx.h"
#include "State_SimonManusP2_Stamp.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_Stamp::CState_SimonManusP2_Stamp(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Stamp::Initialize(_uint iStateNum, void* pArg)
{
    //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    m_pResetRootMove = pDesc->pIsResetRootMove;

    return S_OK;
}

HRESULT CState_SimonManusP2_Stamp::Start_State(void* pArg)
{
    *m_pResetRootMove = false;
    m_iRouteTrack = 0;
    if (m_pMonster->Calc_Distance_XZ() >= 4.f)
    {
        m_pMonster->Change_Animation(AN_STAMP_MOVE, false, 0.1f, 0);
        m_iCurStartAnim = AN_STAMP_MOVE;
    }
    else
    {
        m_pMonster->Change_Animation(AN_STAMP, false, 0.1f, 0);
        m_iCurStartAnim = AN_STAMP;
    }

    _int iCnt = rand() % 4;

    if (iCnt >= 3)
    {
        m_bStamp = true;
    }
    else
        m_bStamp = false;

    return S_OK;
}

void CState_SimonManusP2_Stamp::Update(_float fTimeDelta)
{

    if (m_bStamp)
    {
        _float fTime = 120.f;
        if (m_iCurStartAnim == AN_STAMP_MOVE)
        {
            fTime = 140.f;
        }

        if (fTime >= m_pMonster->Get_CurrentTrackPos())
        {
            m_pMonster->Change_Animation(AN_SPINBLAST, false, 0, 0);
            ++m_iRouteTrack;
            m_bStamp = false;
        }
    }


    if (End_Check())
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }
}

void CState_SimonManusP2_Stamp::End_State()
{
    m_bStamp = false;
    *m_pResetRootMove = true;
}

_bool CState_SimonManusP2_Stamp::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((m_iCurStartAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iCurStartAnim);
        }
        break;

    case 1:
        if ((AN_SPINBLAST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_SPINBLAST);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

CState_SimonManusP2_Stamp* CState_SimonManusP2_Stamp::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Stamp* pInstance = new CState_SimonManusP2_Stamp(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Stamp"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Stamp::Free()
{
    __super::Free();
}