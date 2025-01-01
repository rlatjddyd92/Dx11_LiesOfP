#include "stdafx.h"
#include "State_CarcassNormal_Bite.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "CarcassNormal.h"

CState_CarcassNormal_Bite::CState_CarcassNormal_Bite(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassNormal_Bite::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassNormal_Bite::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_BITE, false, 0.1f, 0, true);
    m_bSwingSound = false;

    return S_OK;
}

void CState_CarcassNormal_Bite::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        m_pMonster->Change_State(CCarcassNormal::IDLE);
        return;
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

    if (CurTrackPos <= 25.f ||
        CurTrackPos >= 70.f)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
    }

}

void CState_CarcassNormal_Bite::End_State()
{
}

_bool CState_CarcassNormal_Bite::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_BITE);
}

void CState_CarcassNormal_Bite::Collider_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 50.f && CurTrackPos <= 57.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

void CState_CarcassNormal_Bite::Sound_Check(_double CurTrackPos)
{
    if (!m_bSwingSound)
    {
        if ((CurTrackPos >= 50.f && CurTrackPos <= 57.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Normal_01.wav"), false);
            m_bSwingSound = true;
        }
    }
}

CState_CarcassNormal_Bite* CState_CarcassNormal_Bite::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassNormal_Bite* pInstance = new CState_CarcassNormal_Bite(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassNormal_Bite"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassNormal_Bite::Free()
{
    __super::Free();
}