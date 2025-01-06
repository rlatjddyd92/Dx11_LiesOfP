#include "stdafx.h"
#include "State_CarcassNormal_ClawMultiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "CarcassNormal.h"

CState_CarcassNormal_ClawMultiple::CState_CarcassNormal_ClawMultiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassNormal_ClawMultiple::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassNormal_ClawMultiple::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_CLAWMULTIPLE, false, 0.1f, 0, true);

    m_bSwingSound = false;
    return S_OK;
}

void CState_CarcassNormal_ClawMultiple::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        m_pMonster->Change_State(CCarcassNormal::IDLE);
        return;
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

    if (CurTrackPos <= 75.f ||
        (CurTrackPos >= 100.f && CurTrackPos <= 110.f) ||
        (CurTrackPos >= 125.f && CurTrackPos <= 130.f) ||
        (CurTrackPos >= 145.f && CurTrackPos <= 155.f) ||
        (CurTrackPos >= 170.f && CurTrackPos <= 175.f))
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3.f, fTimeDelta);
    }

}

void CState_CarcassNormal_ClawMultiple::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
    m_pMonster->DeActive_CurrentWeaponCollider(1);
}

_bool CState_CarcassNormal_ClawMultiple::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_CLAWMULTIPLE);
}

void CState_CarcassNormal_ClawMultiple::Collider_Check(_double CurTrackPos)
{
    //¿Þ¼Õ
    if ((CurTrackPos >= 110.f && CurTrackPos <= 125.f) ||
        (CurTrackPos >= 155.f && CurTrackPos <= 170.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(0.3f, 0, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);

        m_pMonster->Active_Debuff(0, 0.4f);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(0);

        m_pMonster->DeActive_Debuff(0);
    }

    //¿À¸¥¼Õ
    if ((CurTrackPos >= 85.f && CurTrackPos <= 98.f) ||
        (CurTrackPos >= 130.f && CurTrackPos <= 145.f) ||
        (CurTrackPos >= 175.f && CurTrackPos <= 190.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(0.35f, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);

        m_pMonster->Active_Debuff(1, 0.4f);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(1);

        m_pMonster->DeActive_Debuff(1);
    }
}

void CState_CarcassNormal_ClawMultiple::Sound_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 85.f && CurTrackPos <= 98.f) || 
        (CurTrackPos >= 110.f && CurTrackPos <= 125.f) ||
        (CurTrackPos >= 130.f && CurTrackPos <= 145.f) ||
        (CurTrackPos >= 155.f && CurTrackPos <= 170.f) ||
        (CurTrackPos >= 175.f && CurTrackPos <= 190.f))
    {
        if (!m_bSwingSound)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Nail_01.wav"), false);
            m_bSwingSound = true;
        }
    }
    else
    {
        m_bSwingSound = false;
    }
}

CState_CarcassNormal_ClawMultiple* CState_CarcassNormal_ClawMultiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassNormal_ClawMultiple* pInstance = new CState_CarcassNormal_ClawMultiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassNormal_ClawMultiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassNormal_ClawMultiple::Free()
{
    __super::Free();
}