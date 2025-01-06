#include "stdafx.h"
#include "State_CarcassNormal_HeadingMultiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "CarcassNormal.h"

CState_CarcassNormal_HeadingMultiple::CState_CarcassNormal_HeadingMultiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassNormal_HeadingMultiple::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassNormal_HeadingMultiple::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROTATE_HEADING, false, 0.1f, 0, true);
    m_iRouteTrack = 0;
    m_bHeadingSound = false;
    m_bSwingSound = false;

    return S_OK;
}

void CState_CarcassNormal_HeadingMultiple::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(95, true, 0.5f, 0, true);
                m_pMonster->Change_State(CCarcassNormal::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bHeadingSound = false;
            m_bSwingSound = false;
            m_pMonster->SetUp_Animation(AN_HEADING, false, 0, true);
            m_pMonster->Get_Model()->Play_Animation(0);
            m_pMonster->Get_Transform()->Look_Dir(-(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK)));
            return;
        }

        if (CurTrackPos <= 55.f ||
            (CurTrackPos >= 85.f && CurTrackPos <= 100.f) ||
            (CurTrackPos >= 130.f && CurTrackPos <= 160.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3.f, fTimeDelta);
        }
    }
    else
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CCarcassNormal::IDLE);
            return;
        }

        if (CurTrackPos <= 55.f ||
            (CurTrackPos >= 80.f && CurTrackPos <= 95.f) ||
            (CurTrackPos >= 125.f && CurTrackPos <= 155.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3.f, fTimeDelta);
        }
    }



    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassNormal_HeadingMultiple::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
    m_pMonster->DeActive_CurrentWeaponCollider(1);
    m_pMonster->DeActive_CurrentWeaponCollider(2);
}

_bool CState_CarcassNormal_HeadingMultiple::End_Check()
{
    _bool   bEndCheck = { false };
    if (m_iRouteTrack == 0)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_ROTATE_HEADING);
    }
    else
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_HEADING);
    }

    return bEndCheck;
}

void CState_CarcassNormal_HeadingMultiple::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        //머리
        if ((CurTrackPos >= 65.f && CurTrackPos <= 78.f) ||
            (CurTrackPos >= 108.f && CurTrackPos <= 123.f) ||
            (CurTrackPos >= 165.f && CurTrackPos <= 177.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.4f, 2, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(2);
        }
        //양손
        if (CurTrackPos >= 164.f && CurTrackPos <= 183.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
            m_pMonster->DeActive_CurrentWeaponCollider(1);
        }
    }
    else
    {
        //머리
        if ((CurTrackPos >= 61.f && CurTrackPos <= 74.f) ||
            (CurTrackPos >= 103.f && CurTrackPos <= 118.f) ||
            (CurTrackPos >= 160.f && CurTrackPos <= 170.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 2, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(2);
        }
        //양손
        if (CurTrackPos >= 157.f && CurTrackPos <= 175.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
            m_pMonster->DeActive_CurrentWeaponCollider(1);
        }
    }

}

void CState_CarcassNormal_HeadingMultiple::Sound_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        //머리
        if ((CurTrackPos >= 65.f && CurTrackPos <= 78.f) ||
            (CurTrackPos >= 108.f && CurTrackPos <= 123.f) ||
            (CurTrackPos >= 165.f && CurTrackPos <= 177.f))
        {
            if (!m_bHeadingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Normal_01.wav"), false);
                m_bHeadingSound = true;
            }
        }
        else
        {
            m_bHeadingSound = false;
        }

        //
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 164.f && CurTrackPos <= 183.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_SK_WS_Nail_01.wav"), false);
                m_bSwingSound = true;
            }
        }

    }
    else
    {
        //머리
        if ((CurTrackPos >= 61.f && CurTrackPos <= 74.f) ||
            (CurTrackPos >= 103.f && CurTrackPos <= 118.f) ||
            (CurTrackPos >= 160.f && CurTrackPos <= 170.f))
        {
            if (!m_bHeadingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Normal_01.wav"), false);
                m_bHeadingSound = true;
            }
        }
        else
        {
            m_bHeadingSound = false;
        }

        //양손
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 157.f && CurTrackPos <= 175.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_SK_WS_Nail_01.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
}

CState_CarcassNormal_HeadingMultiple* CState_CarcassNormal_HeadingMultiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassNormal_HeadingMultiple* pInstance = new CState_CarcassNormal_HeadingMultiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassNormal_HeadingMultiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassNormal_HeadingMultiple::Free()
{
    __super::Free();
}