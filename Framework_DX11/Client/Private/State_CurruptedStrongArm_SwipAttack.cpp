#include "stdafx.h"
#include "State_CurruptedStrongArm_SwipAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "CurruptedStrongArm_Puppet.h"

CState_CurruptedStrongArm_SwipAttack::CState_CurruptedStrongArm_SwipAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CurruptedStrongArm_SwipAttack::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CurruptedStrongArm_SwipAttack::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_SWIPATTACK, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CurruptedStrongArm_SwipAttack::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
        return;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();
    Collider_Check(CurTrackPos);

    if (CurTrackPos <= 45.f)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
    }

}

void CState_CurruptedStrongArm_SwipAttack::End_State()
{
}

_bool CState_CurruptedStrongArm_SwipAttack::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_SWIPATTACK);
}

void CState_CurruptedStrongArm_SwipAttack::Collider_Check(_double CurTrackPos)
{
    //¿Þ¼ö¿°
    if (CurTrackPos >= 50.f && CurTrackPos <= 76.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1.3f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FL, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FL);
    }

    //¿À¸¥¼ö¿°, ¿À¸¥¼Õ
    if (CurTrackPos >= 50.f && CurTrackPos <= 68.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1.5f, CCurruptedStrongArm_Puppet::TYPE_ARM_RIGHT, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        m_pMonster->Active_CurrentWeaponCollider(1.3f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FR, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_ARM_RIGHT);
        m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FR);
    }

    //µÚ ¿À¸¥ ¼ö¿°
    if (CurTrackPos >= 62.f && CurTrackPos <= 75.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1.3f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_BR, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_BR);
    }

    //µÚ ¿À¸¥ ¼ö¿°
    if (CurTrackPos >= 65.f && CurTrackPos <= 78.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1.3f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_BL, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_BL);
    }
}

CState_CurruptedStrongArm_SwipAttack* CState_CurruptedStrongArm_SwipAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CurruptedStrongArm_SwipAttack* pInstance = new CState_CurruptedStrongArm_SwipAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CurruptedStrongArm_SwipAttack::Free()
{
    __super::Free();
}