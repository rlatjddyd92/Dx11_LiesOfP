#include "stdafx.h"
#include "Stete_CarcassBigA_SwingDownUpper.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CStete_CarcassBigA_SwingDownUpper::CStete_CarcassBigA_SwingDownUpper(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CStete_CarcassBigA_SwingDownUpper::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 35;
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CStete_CarcassBigA_SwingDownUpper::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_SWINGDOWNUPPER, false, 0.1f);

    return S_OK;
}

void CStete_CarcassBigA_SwingDownUpper::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }
}

void CStete_CarcassBigA_SwingDownUpper::End_State()
{
}

_bool CStete_CarcassBigA_SwingDownUpper::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_SWINGDOWNUPPER);
}

CStete_CarcassBigA_SwingDownUpper* CStete_CarcassBigA_SwingDownUpper::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CStete_CarcassBigA_SwingDownUpper* pInstance = new CStete_CarcassBigA_SwingDownUpper(pFsm, pMonster );

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStete_CarcassBigA_SwingDownUpper::Free()
{
    __super::Free();
}