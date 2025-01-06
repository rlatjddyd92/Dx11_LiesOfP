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
    m_bSwingSound = false;
    m_bTentaSound = false;
    m_bTentaSound_2 = false;

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
    
    if (CurTrackPos <= 45.f)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CurruptedStrongArm_SwipAttack::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_ARM_RIGHT);
    m_pMonster->DeActive_CurrentWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FL);
    m_pMonster->DeActive_CurrentWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FR);
    m_pMonster->DeActive_CurrentWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_BL);
    m_pMonster->DeActive_CurrentWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_BR);
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
        m_pMonster->Active_CurrentWeaponCollider(0.6f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FL, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FL);
    }

    //¿À¸¥¼ö¿°, ¿À¸¥¼Õ
    if (CurTrackPos >= 50.f && CurTrackPos <= 68.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1.3f, CCurruptedStrongArm_Puppet::TYPE_ARM_RIGHT, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        m_pMonster->Active_CurrentWeaponCollider(0.6f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FR, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_ARM_RIGHT);
        m_pMonster->DeActive_CurrentWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FR);
    }

    //µÚ ¿À¸¥ ¼ö¿°
    if (CurTrackPos >= 62.f && CurTrackPos <= 75.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(0.6f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_BR, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_BR);
    }

    //µÚ ¿À¸¥ ¼ö¿°
    if (CurTrackPos >= 65.f && CurTrackPos <= 78.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(0.6f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_BL, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_BL);
    }
}

void CState_CurruptedStrongArm_SwipAttack::Sound_Check(_double CurTrackPos)
{
    if (!m_bSwingSound)
    {
        if (CurTrackPos >= 50.f && CurTrackPos <= 68.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Heavy_03.wav"), false);
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_SK_WS_Sword_1H_Spear_03.wav"), false);
            m_bSwingSound = true;
        }
    }

    if (!m_bTentaSound)
    {
        if (CurTrackPos >= 62.f && CurTrackPos <= 75.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_SK_WS_Sword_1H_Spear_03.wav"), false);
            m_bTentaSound = true;
        }
    }


    if (!m_bTentaSound_2)
    {
        if (CurTrackPos >= 65.f && CurTrackPos <= 78.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_SK_WS_Sword_1H_Spear_03.wav"), false);
            m_bTentaSound_2 = true;
        }
    }

}

CState_CurruptedStrongArm_SwipAttack* CState_CurruptedStrongArm_SwipAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CurruptedStrongArm_SwipAttack* pInstance = new CState_CurruptedStrongArm_SwipAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CurruptedStrongArm_SwipAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CurruptedStrongArm_SwipAttack::Free()
{
    __super::Free();
}