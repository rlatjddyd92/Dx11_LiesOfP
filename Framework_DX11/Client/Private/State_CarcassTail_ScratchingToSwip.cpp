#include "stdafx.h"
#include "State_CarcassTail_ScratchingToSwip.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassTail_ScratchingToSwip::CState_CarcassTail_ScratchingToSwip(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_ScratchingToSwip::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 1.f;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassTail_ScratchingToSwip::Start_State(void* pArg)
{
    _int iCnt = rand() % 2;
    if (iCnt == 0)
    {
        m_iFirstAnim = AN_ROUTE_FIRST_L;
        m_iLastAnim = AN_ROUTE_LAST_L;
    }
    else
    {
        m_iFirstAnim = AN_ROUTE_FIRST_R;
        m_iLastAnim = AN_ROUTE_LAST_R;
    }

    m_pMonster->Change_Animation(m_iFirstAnim, false, 0.1f, 0);
    
    m_bSwingSound = false;
    m_bSwipSound = false;

    m_fIdleTime = m_fIdleDuration;
    return S_OK;
}

void CState_CarcassTail_ScratchingToSwip::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 179)
        {
            ++m_iRouteTrack;
            m_bSwingSound = false;
            m_bSwipSound = false;

            m_pMonster->Change_Animation(m_iLastAnim, false, 0.1f, 80);
        }
    }
    else
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CCarcassBigA::IDLE);
            return;
        }
    }


    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassTail_ScratchingToSwip::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_CarcassTail_ScratchingToSwip::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((m_iFirstAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iFirstAnim);
        }
        break;

    case 1:
        if ((m_iLastAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iLastAnim);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_CarcassTail_ScratchingToSwip::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 85.f && CurTrackPos <= 95.f)
        {
            if (m_iFirstAnim == AN_ROUTE_FIRST_L)
            {
                m_pMonster->Active_CurrentWeaponCollider(1.1f, 0, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
            }
            else
            {
                m_pMonster->Active_CurrentWeaponCollider(1.1f, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
            }
        }
        else
        {
            if (m_iFirstAnim == AN_ROUTE_FIRST_L)
            {
                m_pMonster->DeActive_CurretnWeaponCollider(0);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider(1);
            }
        }
    }
    else
    {
        if (CurTrackPos >= 110.f && CurTrackPos <= 135.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.1f, 2, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
            m_pMonster->Active_CurrentWeaponCollider(1.1f, 4, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(2);
        }
    }
}

void CState_CarcassTail_ScratchingToSwip::Sound_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 85.f && CurTrackPos <= 95.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_CarcassBear_SK_WS_Claw_06.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
    else
    {
        if (!m_bSwipSound)
        {
            if (CurTrackPos >= 110.f && CurTrackPos <= 135.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_PoliceBilly_06.wav"), false);
                m_bSwipSound = true;
            }
        }
    }
}

CState_CarcassTail_ScratchingToSwip* CState_CarcassTail_ScratchingToSwip::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_ScratchingToSwip* pInstance = new CState_CarcassTail_ScratchingToSwip(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_ScratchingToSwip"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_ScratchingToSwip::Free()
{
    __super::Free();
}