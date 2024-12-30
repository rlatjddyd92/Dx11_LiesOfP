#include "stdafx.h"
#include "State_RaxasiaP2_Running_Fury.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "Effect_Manager.h"

CState_RaxasiaP2_Running_Fury::CState_RaxasiaP2_Running_Fury(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Running_Fury::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_Running_Fury::Start_State(void* pArg)
{
    m_iRouteTrack = 0;

    m_pMonster->Change_Animation(AN_RUNNING, false, 0.1f, 0, true, true);

    m_bRunningWise = *static_cast<_bool*>(pArg);

    m_fDistance = m_pMonster->Calc_Distance_XZ();

    m_bResetRim = false;
    m_bControlRim = false;

    m_fGoalRimAlpha = 0.1f;
    m_fCurtRimAlpha = 1.f;

    m_bLockOn = false;
    m_bSwingSound = false;
    m_bStartForSound = false;
    m_bChargeForSound = false;

    m_bSpeedController = false;
    m_bSwing = false;
    m_bShieldAttack = false;
    m_bStart = false;

    return S_OK;
}

void CState_RaxasiaP2_Running_Fury::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
    {
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bLockOn = false;
            m_bControlRim = true;
            m_pMonster->Change_Animation(AN_STING, false, 0.2f, 50);
            return;
        }

        if (CurTrackPos <= 120.f)
        {
            _Vec3 vTargetPos = m_pMonster->Get_TargetPos();

            _Vec3 vDir = m_pMonster->Get_TargetDir();
            _Vec3 vUp{ 0, 1, 0 };

            _Vec3 vRight = vUp.Cross(vDir);
            vRight.Normalize();
            _Vec3 vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);

            _float vVeloSpeed = m_pMonster->Get_RigidBody()->Get_Velocity().Length();

            if (m_bRunningWise)
            {
                vPos += (vRight * vVeloSpeed * fTimeDelta);
                m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(_Vec4{ vRight }, 10.f, fTimeDelta);
            }
            else
            {
                vPos -= (vRight * vVeloSpeed * fTimeDelta);
                m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(_Vec4{ -vRight }, 10.f, fTimeDelta);
            }

            vDir = vTargetPos - vPos;
            vDir.Normalize();

            m_fDistance = m_pMonster->Calc_Distance_XZ();

            vDir *= m_fDistance;
            _Vec3 vTemp{};
            if (CurTrackPos >= 30.f)
            {
                vTemp = vDir;
                vTemp.Normalize();
                if (m_fDistance < 4.f)
                {
                    vTemp *= -5.f;
                }
                else
                {
                    vTemp *= 5.f;
                }
            }

            m_pMonster->Get_RigidBody()->Set_GloblePose(vTargetPos - vDir + (vTemp * fTimeDelta));
        }
        else if (CurTrackPos <= 160)
        {
            if (!m_bSpeedController)
            {
                if (CurTrackPos >= 135 && CurTrackPos <= 145.f)
                {
                    m_bSpeedController = true;
                    m_pMonster->Get_Model()->Set_SpeedRatio(AN_RUNNING, (_double)0.5);
                }
            }
            else
            {
                if (CurTrackPos >= 150)
                {
                    m_bSpeedController = false;
                    m_pMonster->Get_Model()->Set_SpeedRatio(AN_RUNNING, (_double)1);
                }
            }
        }
        if (CurTrackPos <= 150.f ||
            CurTrackPos >= 160.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }

        if (CurTrackPos >= 150 && CurTrackPos <= 155)
        {
            if (!m_bLockOn)
            {
                m_vLockVec = (_Vec3)m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);
                m_vLockVec.Normalize();
                m_bLockOn = true;
            }
            m_pMonster->Get_RigidBody()->Set_Velocity(m_vLockVec * 60);
        }

        break;
    }

    case 1:
    {
        //퓨리
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
        if (CurTrackPos <= 150.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }
        if (CurTrackPos >= 165 && CurTrackPos <= 172.f)
        {
            if (!m_bLockOn)
            {
                m_vLockVec = (_Vec3)m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);
                m_vLockVec.Normalize();
                m_bLockOn = true;
            }
            m_pMonster->Get_RigidBody()->Set_Velocity(m_vLockVec * 60);
        }

        if (!m_bResetRim)
        {
            if (CurTrackPos >= 175.f)
            {
                m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD);
                m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT2);
                m_fGoalRimAlpha = 1.f;
                m_bResetRim = true;
            }
        }

        break;
    }

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

    if (m_bControlRim)
    {
        Update_Rimlight();
    }

}

void CState_RaxasiaP2_Running_Fury::End_State()
{
}

_bool CState_RaxasiaP2_Running_Fury::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_RUNNING) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_RUNNING);
        }
        break;

    case 1:
        if ((AN_STING) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_STING);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_Running_Fury::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 145.f && CurTrackPos <= 160.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }

        if ((CurTrackPos >= 165.f && CurTrackPos <= 185.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 160.f && CurTrackPos <= 170.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.6f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_STRONG);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP2_Running_Fury::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if(!m_bStart)
        {
            if (CurTrackPos <= 10.f)
            {
                m_bStart = true;
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL, true);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
            }
        }
        else
        {
            if (CurTrackPos >= 145.f)
            {
                m_bStart = false;
                m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL);
                m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
            }
        }

        if (!m_bSwing)
        {
            if ((CurTrackPos >= 145.f && CurTrackPos <= 160.f))
            {
                m_bSwing = true;
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }

        if ((CurTrackPos >= 165.f && CurTrackPos <= 185.f))
        {
            if (!m_bShieldAttack)
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, 0.f };
                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_UFBIndex(UFB_HAND_RIGHT)) * m_pMonster->Get_Transform()->Get_WorldMatrix()));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_ThunderInchent"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });

                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD);
                m_bShieldAttack = true;
            }
        }
    }
}

void CState_RaxasiaP2_Running_Fury::Update_Rimlight()
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

void CState_RaxasiaP2_Running_Fury::Control_Sound(_double CurTrackPos)
{

    if (m_iRouteTrack == 0)
    {
        if (!m_bStartForSound)
        {
            if (CurTrackPos <= 10.f)
            {
                m_bStartForSound = true;
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_WP_Sword_Lightning_Loop.wav"), true);
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_FX_Blink_Spark_Foot_03.wav"), true);
            }
        }
        else
        {
            if (CurTrackPos >= 145.f)
            {
                m_bStartForSound = false;
                m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT1);
                m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT2);
            }
        }

        if (!m_bSwingSound)
        {
            if ((CurTrackPos >= 145.f && CurTrackPos <= 160.f))
            {
                m_bSwingSound = true;
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
            }
        }

        if ((CurTrackPos >= 165.f && CurTrackPos <= 185.f))
        {
            if (!m_bChargeForSound)
            {
                //번개 사운드 1
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_FX_Spark_Fall_02.wav"), false);
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_WP_Sword_Lightning_Loop.wav"), false);
                m_bChargeForSound = true;
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 160.f && CurTrackPos <= 170.f)
            {
                m_bSwingSound = true;
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
            }
        }
    }
}

CState_RaxasiaP2_Running_Fury* CState_RaxasiaP2_Running_Fury::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Running_Fury* pInstance = new CState_RaxasiaP2_Running_Fury(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Running_Fury"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Running_Fury::Free()
{
    __super::Free();
}