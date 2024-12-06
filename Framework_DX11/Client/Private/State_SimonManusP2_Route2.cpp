#include "stdafx.h"
#include "State_SimonManusP2_Route2.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManusP2_Route2::CState_SimonManusP2_Route2(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Route2::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_Route2::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    m_fGoalRimAlpha = 0.1f;
    m_fCurtRimAlpha = 1.f;

    m_isJump = true;
    m_bLandSound = false;
    m_bSwingSound = false;
    m_bSwing = false;

    return S_OK;
}

void CState_SimonManusP2_Route2::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 110.f)
        {
            ++m_iRouteTrack;
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.1f, 170);
        }

        if (CurTrackPos >= 90.f && CurTrackPos <= 110.f)
        {
            m_vTargetDir = m_pMonster->Get_TargetDir();
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 2.2f, fTimeDelta);
        }
    }
    else
    {
        if (CurTrackPos >= 200.f && CurTrackPos < 230.f) //점프 이후 공중 체공 + 플레이어방향 회전
        {
            m_vTargetDir = m_pMonster->Get_TargetDir();
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 2.2f, fTimeDelta);
        }
        else if (CurTrackPos >= 230.f && CurTrackPos <= 245.f) //땅 찍기까지
        {
            if (m_bStartSpot)
            {
                _float fLength = m_vTargetDir.Length();
                m_vTargetDir = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK)) * fLength;
                m_bStartSpot = false;
            }

            _Vec3 vMove = m_vTargetDir * (((_float)CurTrackPos - 230.f) / 15.f);
            m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vFlyMoveStack) / fTimeDelta);
            m_vFlyMoveStack = vMove;
        }

        if (!m_bResetRim)
        {
            if (CurTrackPos >= 245.f)
            {
                m_fGoalRimAlpha = 1.f;
                m_bResetRim = true;
            }
        }
    }


    if (End_Check())
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
    Update_Rimlight();

}

void CState_SimonManusP2_Route2::End_State()
{
    m_pMonster->Set_RimLightColor(_Vec4{ 0.f, 0.f, 0.f, 0.f });
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP2_Route2::End_Check()
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

void CState_SimonManusP2_Route2::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0) //AN_ROUTE_FIRST, 쓰러지면서 하는 스윙
    {
        if (CurTrackPos >= 60 && CurTrackPos <= 85.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else        //하이점프 폴
    {
        if (CurTrackPos >= 140 && CurTrackPos <= 150.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_SimonManusP2_Route2::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0) //AN_ROUTE_FIRST, 쓰러지면서 하는 스윙
    {
        if ((CurTrackPos >= 60 && CurTrackPos <= 85.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
        }
    }
}

void CState_SimonManusP2_Route2::Update_Rimlight()
{
    if (m_fCurtRimAlpha != m_fGoalRimAlpha)
    {
        m_fCurtRimAlpha += (m_fGoalRimAlpha - m_fCurtRimAlpha) / 20;
        m_pMonster->Set_RimLightColor(_Vec4{ 0.9f, 0.f, 0.f, m_fCurtRimAlpha });
        if (abs(m_fGoalRimAlpha - m_fCurtRimAlpha) < 0.1f)
        {
            m_fCurtRimAlpha = m_fGoalRimAlpha;
            if (m_fGoalRimAlpha == 1.f)
            {
                m_pMonster->Set_RimLightColor(_Vec4{ 0.f, 0.f, 0.f, 1.f });
            }
        }
    }
}

void CState_SimonManusP2_Route2::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 70 && CurTrackPos <= 75.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));

                m_bSwingSound = true;
            }
        }
    }
    else
    {
        if (!m_bLandSound)
        {
            if (CurTrackPos >= 245.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_MT_Dust_M_02.wav"));
                m_bLandSound = true;
            }
        }
    }
}

CState_SimonManusP2_Route2* CState_SimonManusP2_Route2::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Route2* pInstance = new CState_SimonManusP2_Route2(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Route2"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Route2::Free()
{
    __super::Free();
}