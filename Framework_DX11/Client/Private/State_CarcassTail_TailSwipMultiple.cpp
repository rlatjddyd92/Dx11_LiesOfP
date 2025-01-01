#include "stdafx.h"
#include "State_CarcassTail_TailSwipMultiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassTail_TailSwipMultiple::CState_CarcassTail_TailSwipMultiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_TailSwipMultiple::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_TailSwipMultiple::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_TailSwipMultiple, false, 0.1f, 0, true);

    m_bSwipSound = false;

    return S_OK;
}

void CState_CarcassTail_TailSwipMultiple::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(-m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassTail_TailSwipMultiple::End_State()
{
}

_bool CState_CarcassTail_TailSwipMultiple::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_TailSwipMultiple);
}

void CState_CarcassTail_TailSwipMultiple::Collider_Check(_double CurTrackPos)
{

    if ((CurTrackPos >= 80.f && CurTrackPos <= 110.f) ||
        (CurTrackPos >= 125.f && CurTrackPos <= 170.f) ||
        (CurTrackPos >= 185.f && CurTrackPos <= 210.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.2f, 2, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(2);
    }
}

void CState_CarcassTail_TailSwipMultiple::Sound_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 80.f && CurTrackPos <= 110.f) ||
        (CurTrackPos >= 125.f && CurTrackPos <= 170.f) ||
        (CurTrackPos >= 185.f && CurTrackPos <= 210.f))
    {
        if (!m_bSwipSound)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_PoliceBilly_06.wav"), false);
            m_bSwipSound = true;
        }
    }
    else
    {
        m_bSwipSound = false;
    }
}

CState_CarcassTail_TailSwipMultiple* CState_CarcassTail_TailSwipMultiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_TailSwipMultiple* pInstance = new CState_CarcassTail_TailSwipMultiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_TailSwipMultiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_TailSwipMultiple::Free()
{
    __super::Free();
}