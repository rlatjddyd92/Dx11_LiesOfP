#include "stdafx.h"
#include "State_SimonManusP2_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "Effect_Manager.h"

CState_SimonManusP2_Die::CState_SimonManusP2_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_Die::Start_State(void* pArg)
{
    m_pMonster->Get_RigidBody()->Set_GloblePose(_Vec3(0.f, 0.f, 0.f));
    m_pMonster->Get_Transform()->Rotation(0.f, 50.f, 0.f);
    m_pMonster->Change_Animation(AN_DIE, false, 0.1f, 150);

    m_pMonster->Set_EmissiveMask(0.5f);

    return S_OK;
}

void CState_SimonManusP2_Die::Update(_float fTimeDelta)
{
    _int iFrame = m_pMonster->Get_Frame();

    if (!m_isEnd_DownEmissive)
    {
        Contorl_Emissive(fTimeDelta);
    }
    else
    {

    }

    if (End_Check())
    {
        m_pMonster->Get_RigidBody()->Set_Kinematic(false);
    }
}

void CState_SimonManusP2_Die::End_State()
{
}

_bool CState_SimonManusP2_Die::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_DIE);
}

void CState_SimonManusP2_Die::Contorl_Emissive(_float fTimeDelta)
{
    _float fEmissive = m_pMonster->Get_EmissiveMask();

    fEmissive -= 0.07f * fTimeDelta;

    if (fEmissive <= 0.f)
    {
        fEmissive = 0.f;
        m_isEnd_DownEmissive = true;
    }
   
    if (fEmissive < 0.1f)
    {
        if (!m_isDisappearDome)
        {
            CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Map_SimonManus_2P_Death"));
            m_isDisappearDome = true;
        }
        else
        {
            static_cast<CSimonManus*>(m_pMonster)->On_Aura(false, false);
            m_pMonster->DeActive_AllEffect();
            m_pMonster->Set_Dead_Effect(CSimonManus::CUTSCENE_P2_MAP);
        }
    }

    m_pMonster->Set_EmissiveMask(fEmissive);
}

CState_SimonManusP2_Die* CState_SimonManusP2_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Die* pInstance = new CState_SimonManusP2_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Die::Free()
{
    __super::Free();
}