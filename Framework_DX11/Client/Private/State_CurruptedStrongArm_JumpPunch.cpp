#include "stdafx.h"
#include "State_CurruptedStrongArm_JumpPunch.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "CurruptedStrongArm_Puppet.h"

CState_CurruptedStrongArm_JumpPunch::CState_CurruptedStrongArm_JumpPunch(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CurruptedStrongArm_JumpPunch::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CurruptedStrongArm_JumpPunch::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_PUNCHTWICE, false, 0.1f, 0, true);
    m_iRouteTrack = 0;
    
    m_bSwingSound = false;
    m_bTentaSound = false;
    m_bTentaSound_2 = false;

    return S_OK;
}

void CState_CurruptedStrongArm_JumpPunch::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwingSound = false;
            m_bTentaSound = false;
            m_bTentaSound_2 = false;
            m_pMonster->Change_Animation(AN_JUMP, false, 0.1f, 0, true);
            return;
        }
        if (CurTrackPos <= 35.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
        }
    }
    else
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CMonster::IDLE);
            return;
        }

        if ((CurTrackPos <= 30.f) ||
            (CurTrackPos >= 65.f && CurTrackPos <= 80.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
        }
    }

    

    Collider_Check(CurTrackPos);

}

void CState_CurruptedStrongArm_JumpPunch::End_State()
{
}

_bool CState_CurruptedStrongArm_JumpPunch::End_Check()
{
    _bool bEndCheck = false;

    if (m_iRouteTrack == 0)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_PUNCHTWICE);
    }
    else
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_JUMP);
    }

    return bEndCheck;
}

void CState_CurruptedStrongArm_JumpPunch::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        //¿À¸¥¼Õ
        if (CurTrackPos >= 40.f && CurTrackPos <= 52.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, CCurruptedStrongArm_Puppet::TYPE_ARM_RIGHT, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_ARM_RIGHT);
        }
        //¿À¸¥¼ö¿°
        if (CurTrackPos >= 55.f && CurTrackPos <= 75.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.1f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FR, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FR);
        }
        //¿Þ¼ö¿°
        if (CurTrackPos >= 105.f && CurTrackPos <= 125.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.1f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FL, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FL);
        }
        //¿Þ¼Õ
        if (CurTrackPos >= 130.f && CurTrackPos <= 150.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, CCurruptedStrongArm_Puppet::TYPE_ARM_LEFT, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_ARM_LEFT);
        }
    }
    else
    {
        //¾ç¼Õ
        if (CurTrackPos >= 50.f && CurTrackPos <= 60.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, CCurruptedStrongArm_Puppet::TYPE_ARM_LEFT, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
            m_pMonster->Active_CurrentWeaponCollider(1.3f, CCurruptedStrongArm_Puppet::TYPE_ARM_RIGHT, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_ARM_LEFT);
            m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_ARM_RIGHT);
        }
        //¿Þ¼ö¿°
        if (CurTrackPos >= 80.f && CurTrackPos <= 95.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.1f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FL, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FL);
        }
        //¿À¸¥¼ö¿°
        if (CurTrackPos >= 82.f && CurTrackPos <= 105.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.1f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FR, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FR);
        }
    }

}

void CState_CurruptedStrongArm_JumpPunch::Sound_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        //¿À¸¥¼Õ
        if (CurTrackPos >= 40.f && CurTrackPos <= 52.f)
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Heavy_03.wav"), false);
                m_bSwingSound = true;
            }
        }
        else if (CurTrackPos <= 62.f)
        {
            m_bSwingSound = false;
        }

        if (!m_bTentaSound)
        {
            if (CurTrackPos >= 55.f && CurTrackPos <= 75.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Sword_1H_Spear_03.wav"), false);
                m_bTentaSound = true;
            }
        }


        if (!m_bTentaSound_2)
        {
            if (CurTrackPos >= 105.f && CurTrackPos <= 125.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_SK_WS_Sword_1H_Spear_03.wav"), false);
                m_bTentaSound_2 = true;
            }
        }

        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 130.f && CurTrackPos <= 150.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Heavy_03.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
    else
    {
        //¾ç¼Õ
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 50.f && CurTrackPos <= 60.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Heavy_03.wav"), false);
                m_bSwingSound = true;
            }
        }

        //¿Þ¼ö¿°
        if (!m_bTentaSound)
        {
            if (CurTrackPos >= 80.f && CurTrackPos <= 95.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Sword_1H_Spear_03.wav"), false);
                m_bTentaSound = true;
            }
        }

        //¿À¸¥¼ö¿°
        if (!m_bTentaSound_2)
        {
            if (CurTrackPos >= 82.f && CurTrackPos <= 105.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_SK_WS_Sword_1H_Spear_03.wav"), false);
                m_bTentaSound_2 = true;
            }
        }
    }
}

CState_CurruptedStrongArm_JumpPunch* CState_CurruptedStrongArm_JumpPunch::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CurruptedStrongArm_JumpPunch* pInstance = new CState_CurruptedStrongArm_JumpPunch(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CurruptedStrongArm_JumpPunch"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CurruptedStrongArm_JumpPunch::Free()
{
    __super::Free();
}