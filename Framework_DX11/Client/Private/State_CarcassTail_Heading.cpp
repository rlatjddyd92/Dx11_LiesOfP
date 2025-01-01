#include "stdafx.h"
#include "State_CarcassTail_Heading.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassTail_Heading::CState_CarcassTail_Heading(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_Heading::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_Heading::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_HEADING, false, 0.1f, 0, true);
    m_bHeadingSound = false;

    return S_OK;
}

void CState_CarcassTail_Heading::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos < 70.f)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.2f, fTimeDelta);
    }

    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassTail_Heading::End_State()
{
}

_bool CState_CarcassTail_Heading::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_HEADING);
}

void CState_CarcassTail_Heading::Collider_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 80.f && CurTrackPos <= 95.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.3f, 3, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(3);
    }

}

void CState_CarcassTail_Heading::Sound_Check(_double CurTrackPos)
{
    if (!m_bHeadingSound)
    {
        if ((CurTrackPos >= 80.f && CurTrackPos <= 95.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Heavy_03.wav"), false);
            m_bHeadingSound = true;
        }
    }
}

CState_CarcassTail_Heading* CState_CarcassTail_Heading::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_Heading* pInstance = new CState_CarcassTail_Heading(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_Heading"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_Heading::Free()
{
    __super::Free();
}