#include "stdafx.h"
#include "State_RaxasiaP2_ShieldRush.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_ShieldRush::CState_RaxasiaP2_ShieldRush(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_ShieldRush::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_ShieldRush::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_START, false, 0.1f, 0);

    m_bSwingSound = false;

    m_bSwing = false;
    m_bCharge = false;
    m_bRush = false;
    m_bHowl = { false };

    return S_OK;
}

void CState_RaxasiaP2_ShieldRush::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_SHIELDRUSH, false, 0.02f, 0);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);

        break;

    case 1:
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);

        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_ShieldRush::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(1);

    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2);
    m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT2);
}

_bool CState_RaxasiaP2_ShieldRush::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_START) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_START);
        }
        break;

    case 1:
        if ((AN_SHIELDRUSH) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_SHIELDRUSH);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_ShieldRush::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 15.f && CurTrackPos <= 135.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
            m_pMonster->Active_Debuff(0, 1.1f);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(1);
            m_pMonster->DeActive_Debuff(0);
        }
    }
}

void CState_RaxasiaP2_ShieldRush::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bCharge)
        {
            if (CurTrackPos >= 35.f)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2, true);
                m_bCharge = true;
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if (CurTrackPos >= 7.f && CurTrackPos <= 100.f)
        {
            if (!m_bRush)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SHIELDDASH, true);
                m_bRush = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SHIELDDASH);
        }
    }
}

void CState_RaxasiaP2_ShieldRush::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bCharge)
        {
            if (CurTrackPos >= 35.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_WP_Sword_Lightning_Loop.wav"), true);
                m_bCharge = true;
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if (CurTrackPos >= 7.f && CurTrackPos <= 100.f)
        {
            if (!m_bHowl)       //ÇÔ¼º
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_VOICE, TEXT("VO_NPC_Raxasia_P2_Roar_05.wav"), false);
                m_bHowl = true;
            }
        }
    }
}

CState_RaxasiaP2_ShieldRush* CState_RaxasiaP2_ShieldRush::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_ShieldRush* pInstance = new CState_RaxasiaP2_ShieldRush(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_ShieldRush"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_ShieldRush::Free()
{
    __super::Free();
}