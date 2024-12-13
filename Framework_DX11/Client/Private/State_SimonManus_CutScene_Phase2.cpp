#include "stdafx.h"
#include "State_SimonManus_CutScene_Phase2.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManus_CutScene_Phase2::CState_SimonManus_CutScene_Phase2(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManus_CutScene_Phase2::Initialize(_uint iStateNum, void* pArg)
{

    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManus_CutScene_Phase2::Start_State(void* pArg)
{
    m_iAnimation_Change = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Manus_Phase_2_P1_Change", 1.f);

    m_pMonster->Change_Animation(m_iAnimation_Change, false, 0.f, 0);

    m_isChangePhase2 = false;

    return S_OK;
}

void CState_SimonManus_CutScene_Phase2::Update(_float fTimeDelta)
{
    if (!m_isChangePhase2)
    {
        if (m_pMonster->Get_EndAnim(m_iAnimation_Change))
        {
            m_pMonster->Change_Model(0);
            m_iAnimation_Connectgod = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Manus_Phase_2_P2_Connect_God", 1.5f);
            m_iAnimation_Begod = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Manus_Phase_2_P2_be_God", 2.f);
            m_pMonster->Change_Animation(m_iAnimation_Connectgod, false, 0.1f, 0);
            m_isChangePhase2 = true;
        }
    }
    if (m_isChangePhase2)
    {
        if (m_pMonster->Get_EndAnim(m_iAnimation_Connectgod))
        {
            m_pMonster->Change_Animation(m_iAnimation_Begod, false, 0.1f, 0);
        }
        else if (m_pMonster->Get_EndAnim(m_iAnimation_Begod))
        {
            m_pMonster->End_CutScene(0);
        }
    }
}

void CState_SimonManus_CutScene_Phase2::End_State()
{
}

CState_SimonManus_CutScene_Phase2* CState_SimonManus_CutScene_Phase2::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManus_CutScene_Phase2* pInstance = new CState_SimonManus_CutScene_Phase2(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManus_CutScene_Phase2"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManus_CutScene_Phase2::Free()
{
    __super::Free();
}