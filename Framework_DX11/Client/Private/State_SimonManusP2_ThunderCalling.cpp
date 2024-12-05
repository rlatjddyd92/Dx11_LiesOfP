#include "stdafx.h"
#include "State_SimonManusP2_ThunderCalling.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "AttackObject.h"

CState_SimonManusP2_ThunderCalling::CState_SimonManusP2_ThunderCalling(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_ThunderCalling::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_ThunderCalling::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_THUNDERCALLING, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP2_ThunderCalling::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }
    
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
}

void CState_SimonManusP2_ThunderCalling::End_State()
{

}

_bool CState_SimonManusP2_ThunderCalling::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_THUNDERCALLING);
}

void CState_SimonManusP2_ThunderCalling::Effect_Check(_double CurTrackPos)
{
    if (!m_bAttackCheck)
    {
        if (CurTrackPos >= 180.f)
        {

            CAttackObject::ATKOBJ_DESC Desc{};
            Desc.vPos = _Vec3{ m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION) + m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK) };
            
            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderCalling"), &Desc);
            m_bAttackCheck = true;
        }
    }
}

void CState_SimonManusP2_ThunderCalling::Control_Sound(_double CurTrackPos)
{
}

CState_SimonManusP2_ThunderCalling* CState_SimonManusP2_ThunderCalling::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_ThunderCalling* pInstance = new CState_SimonManusP2_ThunderCalling(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_ThunderCalling"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_ThunderCalling::Free()
{
    __super::Free();
}