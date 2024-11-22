#include "stdafx.h"
#include "State_SimonManusP2_JumpToAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_JumpToAttack::CState_SimonManusP2_JumpToAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_JumpToAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    m_pResetRootMove = pDesc->pIsResetRootMove;

    return S_OK;
}

HRESULT CState_SimonManusP2_JumpToAttack::Start_State(void* pArg)
{
    //방향 정하기 
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

    if (m_iCurStartAnim != AN_JUMPTO_BACK)
    {
        //마법일지 점프일지 정하는 부분
        if (m_pMonster->Calc_Distance_XZ() >= 7.f)
        {
            ++m_iCurStartAnim;
        }
    }

    iCnt = rand() % 2;
    if (iCnt == 0)
    {
        m_iCurStartAnim != AN_CANCLEATTACK;
    }

    m_pMonster->Change_Animation(m_iCurStartAnim, false, 0.1f, 0);

    m_bJump = false;
    return S_OK;
}

void CState_SimonManusP2_JumpToAttack::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        switch (m_iRouteTrack)
        {
        case 0:
            ++m_iRouteTrack;
            if (m_iCurLastAnim != AN_CANCLEATTACK)
            {
                if (m_pMonster->Calc_Distance_XZ() >= 8.f)
                {
                    m_iCurLastAnim = AN_WAVE;
                }
                else
                    m_iCurLastAnim = AN_SWING;
            }

            m_pMonster->Change_Animation(m_iCurLastAnim, false, 0, 0);
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

}

void CState_SimonManusP2_JumpToAttack::End_State()
{
    m_bJump = false;
}

_bool CState_SimonManusP2_JumpToAttack::End_Check()
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

CState_SimonManusP2_JumpToAttack* CState_SimonManusP2_JumpToAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_JumpToAttack* pInstance = new CState_SimonManusP2_JumpToAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_JumpToAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_JumpToAttack::Free()
{
    __super::Free();
}