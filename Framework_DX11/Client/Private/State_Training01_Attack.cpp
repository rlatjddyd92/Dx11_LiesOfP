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

    m_iColliderStartFrame = 124;
    m_iColliderEndFrame = 130;

    return S_OK;
}

HRESULT CState_Training01_Attack::Start_State(void* pArg)
{
    _int iAttackStrength = dynamic_cast<CMonster_Training01*>(m_pMonster)->Get_AttackStrength();
    m_isRimLight = m_isEndRImLight = false;

    if (iAttackStrength == ATK_STRONG)
    {
        m_isRimLight = true;
        m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);
        m_pMonster->Get_Model()->Set_SpeedRatio(m_iAnimation_Attack, 2.4);
    }
    else
    {
        m_pMonster->Get_Model()->Set_SpeedRatio(m_iAnimation_Attack, 2.8);
    }

    m_pMonster->Change_Animation(m_iAnimation_Attack, false, 0.1f);

    return S_OK;
}

void CState_Training01_Attack::Update(_float fTimeDelta)
{
    _int iFrame = m_pMonster->Get_Frame();


    if (m_isRimLight)
    {
        if (iFrame < m_iColliderStartFrame && !m_isEndRImLight)
        {
            m_vRimLightColor.x = max(m_vRimLightColor.x + 0.5f * fTimeDelta, 1.f);
            m_vRimLightColor.w = max(m_vRimLightColor.w - 0.6f * fTimeDelta, 0.2f);
        }
        else
        {
            m_isEndRImLight = true;

            m_vRimLightColor.x = max(m_vRimLightColor.x - 0.7f * fTimeDelta, 0.f);
            m_vRimLightColor.w = min(m_vRimLightColor.w + 0.7f * fTimeDelta, 0.5f);
        }

        m_pMonster->Set_RimLightColor(m_vRimLightColor);
    }

    if (m_pMonster->Get_EndAnim(m_iAnimation_Attack))
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    Control_Collider();
}

void CState_Training01_Attack::End_State()
{
    dynamic_cast<CMonster_Training01*>(m_pMonster)->Change_AttackType();

    m_pMonster->DeActive_CurrentWeaponCollider(0);

   // m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.f);
   // m_pMonster->Set_RimLightColor(m_vRimLightColor);
}

void CState_Training01_Attack::Control_Collider()
{
    _int iFrame = m_pMonster->Get_Frame();

    if (m_iColliderStartFrame <= iFrame && iFrame <= m_iColliderEndFrame)
    {
        m_pMonster->Active_CurrentWeaponCollider(1.f);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(0);
    }
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