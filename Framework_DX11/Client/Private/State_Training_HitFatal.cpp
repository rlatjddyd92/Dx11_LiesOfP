#include "stdafx.h"
#include "State_Training_HitFatal.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster_Training01.h"

#include "GameInterface_Controller.h"

CState_Training_HitFatal::CState_Training_HitFatal(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_Training_HitFatal::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_HitFatal[0] = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Fatal_NPC_Hit_Start", 2.5f);
    m_iAnimation_HitFatal[1] = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Fatal_NPC_Hit_Ing", 2.f);
    m_iAnimation_HitFatal[2] = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Fatal_NPC_Hit_End", 2.f);

    m_iStateNum = iStateNum;

    m_pFatalAttacked = m_pMonster->Get_bFatalAttacked();

    return S_OK;
}

HRESULT CState_Training_HitFatal::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(m_iAnimation_HitFatal[0], false, 0.1f);

    return S_OK;
}

void CState_Training_HitFatal::Update(_float fTimeDelta)
{
    _uint iCurAnimIndex = m_pMonster->Get_CurrentAnimIndex();

    if (iCurAnimIndex == m_iAnimation_HitFatal[0] && m_pMonster->Get_EndAnim(m_iAnimation_HitFatal[0]))
    {
        m_pMonster->Change_Animation(m_iAnimation_HitFatal[1], true, 0.f);
    }
   
    if ((*m_pFatalAttacked) == true)
    {
        m_pMonster->Set_IsInvicible(true);
        m_pMonster->Change_Animation(m_iAnimation_HitFatal[2], false, 0.1f);
        *m_pFatalAttacked = false;
    }

    else if (iCurAnimIndex == m_iAnimation_HitFatal[2] )
    {
        if(m_pMonster->Get_EndAnim(m_iAnimation_HitFatal[2]))
            m_pMonster->Change_State(CMonster_Training01::IDLE);
    }
}

void CState_Training_HitFatal::End_State()
{
    m_pMonster->Set_IsInvicible(false);
    (*m_pFatalAttacked) = false;
}

CState_Training_HitFatal* CState_Training_HitFatal::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_Training_HitFatal* pInstance = new CState_Training_HitFatal(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Training_HitFatal"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Training_HitFatal::Free()
{
    __super::Free();
}