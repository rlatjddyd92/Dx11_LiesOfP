#include "stdafx.h"
#include "State_CarcassTail_ScratchingMultiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassTail_ScratchingMultiple::CState_CarcassTail_ScratchingMultiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_ScratchingMultiple::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 1.f;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassTail_ScratchingMultiple::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    m_fIdleTime = m_fIdleDuration;
    m_bSwingSound = false;

    return S_OK;
}

void CState_CarcassTail_ScratchingMultiple::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (!m_isDelayed)
    {
        if (End_Check())
        {
            ++m_iRouteTrack;

            if (m_iRouteTrack >= 2)
            {
                m_pMonster->Change_State(CCarcassBigA::IDLE);
                return;
            }
            m_fIdleTime = 0.f;
            m_isDelayed = true;
        }

        if (m_iRouteTrack == 1)
        {
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.1f, 0, true);
            m_bSwingSound = false;

            return;
        }

    }
    else
    {
        m_fIdleTime += fTimeDelta;

        if (m_fIdleTime >= m_fIdleDuration)
        {
            m_isDelayed = false;
        }
        _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3, fTimeDelta);
        switch (iDir)
        {
        case -1:
            m_pMonster->Change_Animation(49, true, 0.1f);
            break;

        case 0:
            m_pMonster->Change_Animation(44, true, 0.1f);
            break;

        case 1:
            m_pMonster->Change_Animation(50, true, 0.1f);
            break;

        default:
            break;
        }
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassTail_ScratchingMultiple::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_CarcassTail_ScratchingMultiple::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_ROUTE_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_FIRST);
        }
        break;

    case 1:
        if ((AN_ROUTE_LAST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_LAST);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_CarcassTail_ScratchingMultiple::Collider_Check(_double CurTrackPos)
{
    if (CurTrackPos >= 85.f && CurTrackPos <= 115.f)
    {
        if (m_iRouteTrack == 0)
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
        if (m_iRouteTrack == 0)
        {
            m_pMonster->DeActive_CurretnWeaponCollider(0);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
}

void CState_CarcassTail_ScratchingMultiple::Sound_Check(_double CurTrackPos)
{
    if (!m_bSwingSound)
    {
        if (CurTrackPos >= 85.f && CurTrackPos <= 115.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Nail_03.wav"), false);
            m_bSwingSound = true;
        }
    }
}

CState_CarcassTail_ScratchingMultiple* CState_CarcassTail_ScratchingMultiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_ScratchingMultiple* pInstance = new CState_CarcassTail_ScratchingMultiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_ScratchingMultiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_ScratchingMultiple::Free()
{
    __super::Free();
}