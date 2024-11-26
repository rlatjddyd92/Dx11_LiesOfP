#include "stdafx.h"
#include "State_SimonManusP1_JumpToSwing.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_JumpToSwing::CState_SimonManusP1_JumpToSwing(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_JumpToSwing::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_JumpToSwing::Start_State(void* pArg)
{
    //방향 정하기 - 랜덤? 뒤 왼 오 니까 나머지 3 해서 더하기
    _int iCnt = rand() % 3;
    switch (iCnt)
    {
    case 0:
        m_iCurStartAnim = AN_JUMPTO_BACK;
        break;
    
    case 1:
        m_iCurStartAnim = AN_JUMPTO_LEFT;
        break;
    
    case 2:
        m_iCurStartAnim = AN_JUMPTO_RIGHT;
        break;

    default:
        break;
    }


    m_pMonster->Change_Animation(m_iCurStartAnim, false, 0.1f, 0);

    m_bJump = false;
    return S_OK;
}

void CState_SimonManusP1_JumpToSwing::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        switch (m_iRouteTrack)
        {
        case 0:
            ++m_iRouteTrack;
            if (m_pMonster->Calc_Distance_XZ() > 5.f)
            {
                m_iCurLastAnim = AN_CANCLEATTACK;
            }
            else
                m_iCurLastAnim = AN_SWING;

            m_pMonster->Change_Animation(m_iCurLastAnim, false, 0.1f, 0);
            break;

        case 1:
            m_iRouteTrack = 0;
            m_pMonster->Change_State(CSimonManus::IDLE);
            return;
            break;

        default:
            break;
        }
    }

    Collider_Check();

}

void CState_SimonManusP1_JumpToSwing::End_State()
{
    m_bJump = false;
}

_bool CState_SimonManusP1_JumpToSwing::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((m_iCurStartAnim) == iCurAnim)  //점프 방향
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iCurStartAnim);
        }
        break;

    case 1:
        if ((m_iCurLastAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iCurLastAnim);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_SimonManusP1_JumpToSwing::Collider_Check()
{
    if (m_iCurLastAnim != AN_SWING)
    {
        return;
    }
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 2)
    {
        if (CurTrackPos >= 30.f && CurTrackPos <= 80.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

CState_SimonManusP1_JumpToSwing* CState_SimonManusP1_JumpToSwing::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_JumpToSwing* pInstance = new CState_SimonManusP1_JumpToSwing(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_JumpToSwing"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_JumpToSwing::Free()
{
    __super::Free();
}