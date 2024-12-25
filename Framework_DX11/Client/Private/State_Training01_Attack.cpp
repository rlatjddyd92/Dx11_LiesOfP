#include "stdafx.h"
#include "State_Training01_Attack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster_Training01.h"

#include "GameInterface_Controller.h"

CState_Training01_Attack::CState_Training01_Attack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_Training01_Attack::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Attack = m_pMonster->Get_Model()->Find_AnimationIndex("AT_Training_Skill_01", 2.8f);
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Training01_Attack::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(m_iAnimation_Attack, true, 0.1f);

    return S_OK;
}

void CState_Training01_Attack::Update(_float fTimeDelta)
{
    if (m_pMonster->Get_EndAnim(m_iAnimation_Attack))
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

}

void CState_Training01_Attack::End_State()
{
}

CState_Training01_Attack* CState_Training01_Attack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_Training01_Attack* pInstance = new CState_Training01_Attack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Training01_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Training01_Attack::Free()
{
    __super::Free();
}