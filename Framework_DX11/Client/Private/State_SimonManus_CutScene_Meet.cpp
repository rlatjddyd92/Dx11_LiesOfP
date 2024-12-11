#include "stdafx.h"
#include "State_SimonManus_CutScene_Meet.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"
#include "CutScene.h"
CState_SimonManus_CutScene_Meet::CState_SimonManus_CutScene_Meet(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManus_CutScene_Meet::Initialize(_uint iStateNum, void* pArg)
{
    
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManus_CutScene_Meet::Start_State(void* pArg)
{
    // 모델이 달라서 여기서 해주기
    m_iAnimation_Turn = m_pMonster->Get_Model()->Find_AnimationIndex("AS_MOB_Manus_Turn_Cine", 1.f);
    m_iAnimation_Talk = m_pMonster->Get_Model()->Find_AnimationIndex("AS_MOB_Manus_Talk_Cine", 1.5f);
    m_iAnimation_Hand = m_pMonster->Get_Model()->Find_AnimationIndex("AS_MOB_Manus_Hand_Cine", 1.f);
    m_iAnimation_End = m_pMonster->Get_Model()->Find_AnimationIndex("AS_MOB_Manus_End_Cine", 1.f);

    m_pMonster->Change_Animation(m_iAnimation_Turn, false, 0.f, 0);

    m_isStartCutScene = false;
    m_fDelay = 0.f;


    return S_OK;
}

void CState_SimonManus_CutScene_Meet::Update(_float fTimeDelta)
{
    m_fDelay += fTimeDelta;
    if (m_fDelay >= 16.f && !m_isStartCutScene)
    {
        m_pMonster->Play_Animation();
        m_isStartCutScene = true;
    }
    else if(m_fDelay < 16.f)
    {
        m_pMonster->Stop_Animation();
    }

    if(m_isStartCutScene)

    if (m_pMonster->Get_EndAnim(m_iAnimation_Turn))
    {
        m_pMonster->Change_Animation(m_iAnimation_Talk, false, 0.1f, 0.5f);
    }
    else if (m_pMonster->Get_EndAnim(m_iAnimation_Talk))
    {
        m_pMonster->Change_Animation(m_iAnimation_Hand, false, 0.2f, 65);
    }
    else if (m_pMonster->Get_EndAnim(m_iAnimation_Hand))
    {
        m_pMonster->Change_Animation(m_iAnimation_End, false, 0.2f, 0);
    }
    else if (m_pMonster->Get_EndAnim(m_iAnimation_End))
    {
        m_pMonster->End_CutScene(0);
    }

    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    if (iCurAnim == m_iAnimation_Hand)
    {
        _int iFrame = m_pMonster->Get_Frame();
        if (iFrame > 230)
        {
            m_pMonster->Active_Weapon();
        }
    }
}

void CState_SimonManus_CutScene_Meet::End_State()
{
}

CState_SimonManus_CutScene_Meet* CState_SimonManus_CutScene_Meet::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManus_CutScene_Meet* pInstance = new CState_SimonManus_CutScene_Meet(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManus_CutScene_Meet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManus_CutScene_Meet::Free()
{
    __super::Free();
}