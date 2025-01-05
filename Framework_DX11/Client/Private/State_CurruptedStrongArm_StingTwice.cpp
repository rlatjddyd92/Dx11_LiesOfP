#include "stdafx.h"
#include "State_CurruptedStrongArm_StingTwice.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "CurruptedStrongArm_Puppet.h"

CState_CurruptedStrongArm_StingTwice::CState_CurruptedStrongArm_StingTwice(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CurruptedStrongArm_StingTwice::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CurruptedStrongArm_StingTwice::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_STINGTWICE, false, 0.1f, 0, true);

    m_bTentaSound = false;
    m_bTentaSound_2 = false;

    return S_OK;
}

void CState_CurruptedStrongArm_StingTwice::Update(_float fTimeDelta)
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

void CState_CurruptedStrongArm_StingTwice::End_State()
{
}

_bool CState_CurruptedStrongArm_StingTwice::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_STINGTWICE);
}

void CState_CurruptedStrongArm_StingTwice::Collider_Check(_double CurTrackPos)
{
    //¿À¸¥¼ö¿°
    if (CurTrackPos >= 45.f && CurTrackPos <= 52.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(0.9f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FR, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FR);
    }
    //¿Þ¼ö¿°
    if (CurTrackPos >= 52.f && CurTrackPos <= 58.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(0.9f, CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FL, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(CCurruptedStrongArm_Puppet::TYPE_TENTACLE_FL);
    }

}

void CState_CurruptedStrongArm_StingTwice::Sound_Check(_double CurTrackPos)
{
    if (!m_bTentaSound)
    {
        if (CurTrackPos >= 45.f && CurTrackPos <= 52.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_WS_Sword_1H_Spear_03.wav"), false);
            m_bTentaSound = true;
        }
    }
    
    if (!m_bTentaSound_2)
    {
        if (CurTrackPos >= 52.f && CurTrackPos <= 58.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_PC_SK_WS_Sword_1H_Spear_03.wav"), false);
            m_bTentaSound_2 = true;
        }
    }
}

CState_CurruptedStrongArm_StingTwice* CState_CurruptedStrongArm_StingTwice::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CurruptedStrongArm_StingTwice* pInstance = new CState_CurruptedStrongArm_StingTwice(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CurruptedStrongArm_StingTwice"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CurruptedStrongArm_StingTwice::Free()
{
    __super::Free();
}