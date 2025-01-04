#include "stdafx.h"
#include "State_CarcassTail_HeadingMultiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassTail_HeadingMultiple::CState_CarcassTail_HeadingMultiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_HeadingMultiple::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_HeadingMultiple::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_HEADING, false, 0.1f, 0, true);

    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);

    m_isRimLight = false;
    m_bHeadingSound = false;

    m_iRouteTrack = 0;
    return S_OK;
}

void CState_CarcassTail_HeadingMultiple::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos < 65.f)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
    }


    if (m_iRouteTrack < 2)
    {
        if (CurTrackPos >= 140)
        {
            ++m_iRouteTrack;
            m_bHeadingSound = false;
            if (m_iRouteTrack == 2)
            {
                m_isRimLight = true;
            }
            m_pMonster->Change_Animation(AN_HEADING, false, 0.1f, 0, true, true);
            return;
        }
    }
    else
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CMonster::IDLE);
        }
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);
    Update_Rimlight(fTimeDelta, CurTrackPos);

}

void CState_CarcassTail_HeadingMultiple::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_CarcassTail_HeadingMultiple::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_HEADING);
}

void CState_CarcassTail_HeadingMultiple::Collider_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 80.f && CurTrackPos <= 95.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.4f, 3, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(3);
    }

}

void CState_CarcassTail_HeadingMultiple::Sound_Check(_double CurTrackPos)
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

void CState_CarcassTail_HeadingMultiple::Update_Rimlight(_float fTimeDelta, _double CurTrackPos)
{
    if (m_isRimLight)
    {
        if (CurTrackPos < 95.f)
        {
            m_vRimLightColor.x = max(m_vRimLightColor.x + 0.5f * fTimeDelta, 1.f);
            m_vRimLightColor.w = max(m_vRimLightColor.w - 0.6f * fTimeDelta, 0.1f);
        }
        else
        {
            m_vRimLightColor.x = max(m_vRimLightColor.x - 0.7f * fTimeDelta, 0.f);
            m_vRimLightColor.w = min(m_vRimLightColor.w + 0.7f * fTimeDelta, 0.5f);
        }

        m_pMonster->Set_RimLightColor(m_vRimLightColor);
    }
}

CState_CarcassTail_HeadingMultiple* CState_CarcassTail_HeadingMultiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_HeadingMultiple* pInstance = new CState_CarcassTail_HeadingMultiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_HeadingMultiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_HeadingMultiple::Free()
{
    __super::Free();
}