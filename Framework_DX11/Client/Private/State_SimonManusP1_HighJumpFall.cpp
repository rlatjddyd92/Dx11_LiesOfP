#include "stdafx.h"
#include "State_SimonManusP1_HighJumpFall.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP1_HighJumpFall::CState_SimonManusP1_HighJumpFall(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_HighJumpFall::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pRootMoveCtr = pDesc->pRootMoveCtr;
    return S_OK;
}

HRESULT CState_SimonManusP1_HighJumpFall::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_HIGHJUMPFALL, false, 0.1f, 0);
    *m_pRootMoveCtr = true;
    return S_OK;
}

void CState_SimonManusP1_HighJumpFall::Update(_float fTimeDelta)
{
    m_fCurrentTime += fTimeDelta;
    
    _double CurTrackPos{};
    CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos >= 200.f && CurTrackPos < 230.f) //점프 이후 공중 체공 + 플레이어방향 회전
    {
        m_vTargetDir = m_pMonster->Get_TargetDir();
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 1.8, fTimeDelta);
    }
    else if(CurTrackPos >= 230.f && CurTrackPos <= 245.f) //땅 찍기까지
    {
        *m_pRootMoveCtr = false;
        _Vec3 vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);

        _Vec3 vMove = m_vTargetDir * (((_float)CurTrackPos - 230.f) / 20.f);
        m_pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos + vMove - m_vFlyMoveStack);

        m_vFlyMoveStack = vMove;
    }
    else
        *m_pRootMoveCtr = true;

    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    Collider_Check();

}

void CState_SimonManusP1_HighJumpFall::End_State()
{
    m_iAnimCnt = 0;//혹시 완료되지 않고 변하는 경우에 대비
    m_fParalizeTime = 0.f;
    m_vFlyMoveStack = _vector{0, 0, 0, 0};
    *m_pRootMoveCtr = true;
}

_bool CState_SimonManusP1_HighJumpFall::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_HIGHJUMPFALL);
}

void CState_SimonManusP1_HighJumpFall::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if ((CurTrackPos >= 140.f && CurTrackPos <= 150.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

CState_SimonManusP1_HighJumpFall* CState_SimonManusP1_HighJumpFall::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_HighJumpFall* pInstance = new CState_SimonManusP1_HighJumpFall(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_HighJumpFall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_HighJumpFall::Free()
{
    __super::Free();
}