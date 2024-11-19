#include "stdafx.h"
#include "State_SimonManusP1_Stamp.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManusP1.h"

CState_SimonManusP1_Stamp::CState_SimonManusP1_Stamp(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_Stamp::Initialize(_uint iStateNum, void* pArg)
{
    //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;
    CSimonManusP1::FSMSTATE_DESC* pDesc = static_cast<CSimonManusP1::FSMSTATE_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;

    return S_OK;
}

HRESULT CState_SimonManusP1_Stamp::Start_State(void* pArg)
{
    *m_pResetRootMove = false;


    //방향 정하기 - 랜덤? 뒤 왼 오 니까 나머지 3 해서 더하기
    m_pMonster->Change_Animation(AN_STAMP, false, 0.1f, 0);

    m_bStamp = false;
    return S_OK;
}

void CState_SimonManusP1_Stamp::Update(_float fTimeDelta)
{
    if (*m_pIsEndAnim)
    {
        if (!m_bStamp)
        {
            m_bStamp = true;
            m_pMonster->Change_Animation(AN_SPINBLAST, false, true);
        }
        else
        {
            m_pMonster->Change_State(CSimonManusP1::IDLE);
        }
    }
}

void CState_SimonManusP1_Stamp::End_State()
{
    m_bStamp = false;
    *m_pResetRootMove = true;
}

_bool CState_SimonManusP1_Stamp::End_Check()
{
    return _bool();
}

CState_SimonManusP1_Stamp* CState_SimonManusP1_Stamp::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_Stamp* pInstance = new CState_SimonManusP1_Stamp(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_Stamp"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_Stamp::Free()
{
    __super::Free();
}