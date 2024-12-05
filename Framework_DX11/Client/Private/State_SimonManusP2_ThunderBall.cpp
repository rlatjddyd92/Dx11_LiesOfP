#include "stdafx.h"
#include "State_SimonManusP2_ThunderBall.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "AttackObject.h"

CState_SimonManusP2_ThunderBall::CState_SimonManusP2_ThunderBall(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_ThunderBall::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_ThunderBall::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_THUNDERBALL, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP2_ThunderBall::Update(_float fTimeDelta)
{//Prototype_GameObject_LightningBall
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
}

void CState_SimonManusP2_ThunderBall::End_State()
{

}

_bool CState_SimonManusP2_ThunderBall::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_THUNDERBALL);
}

void CState_SimonManusP2_ThunderBall::Effect_Check(_double CurTrackPos)
{
    if (!m_bCreateBall)
    {
        if (CurTrackPos >= 100.f)
        {
            CAttackObject::ATKOBJ_DESC Desc{};
            _float4x4 WorldMat{};
            XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_UFBIndex(UFB_HAND_LEFT)) * (*m_pMonster->Get_Transform()->Get_WorldMatrix_Ptr())));
            Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42 - 0.2f, WorldMat._43 };

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_LightningBall"), &Desc);
            m_bCreateBall = true;
        }
    }
}

void CState_SimonManusP2_ThunderBall::Control_Sound(_double CurTrackPos)
{
}

CState_SimonManusP2_ThunderBall* CState_SimonManusP2_ThunderBall::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_ThunderBall* pInstance = new CState_SimonManusP2_ThunderBall(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_ThunderBall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_ThunderBall::Free()
{
    __super::Free();
}