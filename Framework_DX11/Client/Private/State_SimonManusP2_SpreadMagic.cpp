#include "stdafx.h"
#include "State_SimonManusP2_SpreadMagic.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "AttackObject.h"

CState_SimonManusP2_SpreadMagic::CState_SimonManusP2_SpreadMagic(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_SpreadMagic::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);
    
    return S_OK;
}

HRESULT CState_SimonManusP2_SpreadMagic::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_STINGATTACK, false, 0.1f, 0);
    m_bMagicSpread = false;

    return S_OK;
}

void CState_SimonManusP2_SpreadMagic::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.5f, fTimeDelta);

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();
    Effect_Check(CurTrackPos);
}

void CState_SimonManusP2_SpreadMagic::End_State()
{

}

_bool CState_SimonManusP2_SpreadMagic::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_STINGATTACK);
}

void CState_SimonManusP2_SpreadMagic::Effect_Check(_double CurTrackPos)
{
    if (!m_bMagicSpread)
    {
        if (CurTrackPos >= 100.f)
        {
            m_bMagicSpread = true;

            CAttackObject::ATKOBJ_DESC Desc;
            _float4x4 WorldMat{};
            _Vec3 vMainDir = _Vec3{ XMVector3Normalize(m_pMonster->Get_TargetPos() - 
                m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION)) } 
            + _Vec3{ 0.f, 0.2f, 0.f };
            _Vec3 vRight = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
            vRight.Normalize();

            XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_UFBIndex(UFB_HAND_LEFT)) * (*m_pMonster->Get_Transform()->Get_WorldMatrix_Ptr())));
            Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42 - 0.2f, WorldMat._43 };
            
            Desc.vDir = vMainDir + vRight;
            Desc.vDir.Normalize();
            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_BlackBall"), &Desc);

            Desc.vDir = vMainDir + vRight * 0.6f;
            Desc.vDir.Normalize();
            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_BlackBall"), &Desc);

            Desc.vDir = vMainDir + vRight * 0.2f;
            Desc.vDir.Normalize();
            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_BlackBall"), &Desc);

            Desc.vDir = vMainDir - vRight;
            Desc.vDir.Normalize();
            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_BlackBall"), &Desc);

            Desc.vDir = vMainDir - vRight * 0.6f;
            Desc.vDir.Normalize();
            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_BlackBall"), &Desc);

            Desc.vDir = vMainDir - vRight * 0.2f;
            Desc.vDir.Normalize();
            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_BlackBall"), &Desc);

        }
    }
}

void CState_SimonManusP2_SpreadMagic::Control_Sound(_double CurTrackPos)
{
}

CState_SimonManusP2_SpreadMagic* CState_SimonManusP2_SpreadMagic::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_SpreadMagic* pInstance = new CState_SimonManusP2_SpreadMagic(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_SpreadMagic"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_SpreadMagic::Free()
{
    __super::Free();
}