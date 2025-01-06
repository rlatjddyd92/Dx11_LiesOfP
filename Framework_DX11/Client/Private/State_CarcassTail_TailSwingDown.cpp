#include "stdafx.h"
#include "State_CarcassTail_TailSwingDown.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassTail_TailSwingDown::CState_CarcassTail_TailSwingDown(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_TailSwingDown::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_TailSwingDown::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_TAILSWINGDOWN, false, 0.1f, 0, true);

    m_bSwingSound = false;

    return S_OK;
}

void CState_CarcassTail_TailSwingDown::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(-m_pMonster->Get_TargetDir(), 1.5f, fTimeDelta);

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassTail_TailSwingDown::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(2);
    m_pMonster->DeActive_CurrentWeaponCollider(4);
}

_bool CState_CarcassTail_TailSwingDown::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_TAILSWINGDOWN);
}

void CState_CarcassTail_TailSwingDown::Collider_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 65.f && CurTrackPos <= 72.f) || 
        (CurTrackPos >= 110.f && CurTrackPos <= 115.f) ||
        (CurTrackPos >= 250.f && CurTrackPos <= 258.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.4f, 2, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        m_pMonster->Active_CurrentWeaponCollider(1.4f, 4, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);

        m_pMonster->Active_Debuff(2, 0.8f);
        m_pMonster->Active_Debuff(4, 0.8f);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(2);
        m_pMonster->DeActive_CurrentWeaponCollider(4);

        m_pMonster->DeActive_Debuff(2);
        m_pMonster->DeActive_Debuff(4);
    }
}

void CState_CarcassTail_TailSwingDown::Sound_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 65.f && CurTrackPos <= 72.f) ||
        (CurTrackPos >= 110.f && CurTrackPos <= 115.f) ||
        (CurTrackPos >= 250.f && CurTrackPos <= 258.f))
    {
        if (!m_bSwingSound)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_PoliceBilly_06.wav"), false);
            m_bSwingSound = true;
        }
    }
    else
    {
        m_bSwingSound = false;
    }
}

CState_CarcassTail_TailSwingDown* CState_CarcassTail_TailSwingDown::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_TailSwingDown* pInstance = new CState_CarcassTail_TailSwingDown(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_TailSwingDown"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_TailSwingDown::Free()
{
    __super::Free();
}