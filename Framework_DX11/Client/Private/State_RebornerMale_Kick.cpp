#include "stdafx.h"
#include "State_RebornerMale_Kick.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "CurruptedStrongArm_Puppet.h"

CState_RebornerMale_Kick::CState_RebornerMale_Kick(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerMale_Kick::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RebornerMale_Kick::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_KICK, false, 0.1f, 0, true);

    return S_OK;
}

void CState_RebornerMale_Kick::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
        return;
    }

    if (CurTrackPos <= 30.f)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);
    }

    Collider_Check(CurTrackPos);
}

void CState_RebornerMale_Kick::End_State()
{
}

_bool CState_RebornerMale_Kick::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_KICK);
}

void CState_RebornerMale_Kick::Collider_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 35.f && CurTrackPos <= 45.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.0f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

CState_RebornerMale_Kick* CState_RebornerMale_Kick::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerMale_Kick* pInstance = new CState_RebornerMale_Kick(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerMale_Kick"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerMale_Kick::Free()
{
    __super::Free();
}