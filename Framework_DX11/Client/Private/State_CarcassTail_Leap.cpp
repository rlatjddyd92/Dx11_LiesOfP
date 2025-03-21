#include "stdafx.h"
#include "State_CarcassTail_Leap.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassTail_Leap::CState_CarcassTail_Leap(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_Leap::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_Leap::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_LEAP, false, 0.1f, 0, true);

    m_bJumpSound = false;

    return S_OK;
}

void CState_CarcassTail_Leap::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassTail_Leap::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(3);
}

_bool CState_CarcassTail_Leap::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_LEAP);
}

void CState_CarcassTail_Leap::Collider_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 50.f && CurTrackPos <= 90.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.4f, 3, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurrentWeaponCollider(3);
    }
}

void CState_CarcassTail_Leap::Sound_Check(_double CurTrackPos)
{
    if (!m_bJumpSound)
    {
        if ((CurTrackPos >= 50.f && CurTrackPos <= 90.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Rock_Debris_L_03.wav"), false);
            m_bJumpSound = true;
        }
    }
}

CState_CarcassTail_Leap* CState_CarcassTail_Leap::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_Leap* pInstance = new CState_CarcassTail_Leap(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_Leap"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_Leap::Free()
{
    __super::Free();
}