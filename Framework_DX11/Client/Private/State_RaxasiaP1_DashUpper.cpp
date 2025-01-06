#include "stdafx.h"
#include "State_RaxasiaP1_DashUpper.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "Effect_Manager.h"

CState_RaxasiaP1_DashUpper::CState_RaxasiaP1_DashUpper(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_DashUpper::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_DashUpper::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_DASHUPPER_DASH, false, 0.1f, 0);

    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);
    m_isRimLight = false;

    m_bSwingSound = false;
    m_bStampSound = false;

    m_bSwing = false;
    m_bStamp = false;
    m_bDash = false;
    m_bDrag = false;

    return S_OK;
}

void CState_RaxasiaP1_DashUpper::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            m_bSwingSound = false;
            m_bSwing = false;
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_DASH);
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        if (CurTrackPos >= 30.f && CurTrackPos <= 255.f)
        {
            _float fDist = m_pMonster->Calc_Distance_XZ();

            if (fDist <= 3.f)
            {
                _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
                _Vec3 vLook = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK));
                _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());

                _Vec3 vCrossUp = vLook.Cross(vTargetDir);

                if (fabs(vUp.Dot(vCrossUp)) <= 0.3f)
                {
                    ++m_iRouteTrack;
                    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_DASH);
                    m_pMonster->Change_Animation(AN_DASHUPPER_ATTACK, false, 0.1f, 0);
                    return;
                }
            }
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 0.5f, fTimeDelta);

        break;

    case 1:
        if (CurTrackPos >= 130.f)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwingSound = false;
            m_bStampSound = false;
            m_bSwing = false;
            m_bStamp = false;
            m_pMonster->Change_Animation(AN_TRIPLELINK_FIRST, false, 0.1f, 0);
            return;
        }

        if (CurTrackPos >= 100.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }
        break;

    case 2:
        if (CurTrackPos >= 70.f)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwingSound = false;
            m_bStampSound = false;
            m_bSwing = false;
            m_bStamp = false;
            m_pMonster->Change_Animation(AN_TRIPLELINK_SECOND, false, 0.1f, 0);
            return;
        }

        if (CurTrackPos <= 30.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }
        if (CurTrackPos >= 60.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }
        break;

    case 3:
        if (CurTrackPos >= 70.f)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwingSound = false;
            m_bStampSound = false;
            m_bSwing = false;
            m_bStamp = false;
            m_bSpeedController = true;//림라이트 온
            m_isRimLight = true;
            m_pMonster->On_PowerAttack(true);

            m_pMonster->Change_Animation(AN_TRIPLELINK_FIRST, false, 0.1f, 0);
            m_pMonster->Get_Model()->Set_SpeedRatio(AN_TRIPLELINK_FIRST, (double)0.5f);
            return;
        }
        if (CurTrackPos <= 25.f ||
            CurTrackPos >= 60.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }
        break;

    case 4:
        if (m_bSpeedController)
        {
            if (CurTrackPos >= 40.f)
            {
                m_bSpeedController = false;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_TRIPLELINK_FIRST, (double)1);
            }
            else
            {
                m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
            }
        }
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
    Update_Rimlight(fTimeDelta, CurTrackPos);


}

void CState_RaxasiaP1_DashUpper::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);

    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);
    m_pMonster->On_PowerAttack(false);
    m_pMonster->Set_RimLightColor(m_vRimLightColor);
}

_bool CState_RaxasiaP1_DashUpper::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_DASHUPPER_DASH) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_DASHUPPER_DASH);
        }
        break;

    case 4:
        if ((AN_TRIPLELINK_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_TRIPLELINK_FIRST);
        }
        break;

    default:
        break;

    }
    return  bEndCheck;
}

void CState_RaxasiaP1_DashUpper::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 30.f && CurTrackPos <= 50.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 40.f && CurTrackPos <= 48.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.4f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
    else if (m_iRouteTrack == 3)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 44.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.4f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
    else if (m_iRouteTrack == 4)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 48.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.6f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_STRONG);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
}

void CState_RaxasiaP1_DashUpper::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bDash)
        {
            if ((CurTrackPos >= 140.f && CurTrackPos <= 250.f))
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_DASH, true);
                m_bDash = true;
            }
        }
        else if (CurTrackPos >= 250.f)
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_DASH);
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 30.f && CurTrackPos <= 50.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 40.f && CurTrackPos <= 48.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }

        if (!m_bStamp)
        {
            if ((CurTrackPos >= 48.f))
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, -1.75f };
                XMStoreFloat4x4(&WorldMat,
                    (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_Model()->Get_UFBIndices(UFB_WEAPON))
                        * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
                vPos.y = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_Stamp"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir()});
                m_bStamp = true;
            }
        }
    }
    else if (m_iRouteTrack == 3)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 44.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }

        if (!m_bStamp)
        {
            if ((CurTrackPos >= 44.f))
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, -1.75f };
                XMStoreFloat4x4(&WorldMat,
                    (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_Model()->Get_UFBIndices(UFB_WEAPON))
                        * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
                vPos.y = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_Stamp"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });
                m_bStamp = true;
            }
        }
    }
    else if (m_iRouteTrack == 4)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 48.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }

        if (!m_bStamp)
        {
            if ((CurTrackPos >= 48.f))
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, -1.75f };
                XMStoreFloat4x4(&WorldMat,
                    (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_Model()->Get_UFBIndices(UFB_WEAPON))
                        * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
                vPos.y = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_Stamp"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });
                m_bStamp = true;
            }
        }
    }
}

void CState_RaxasiaP1_DashUpper::Update_Rimlight(_float fTimeDelta, _double CurTrackPos)
{
    if (m_isRimLight)
    {
        if (CurTrackPos < 50.f)
        {
            m_vRimLightColor.x = max(m_vRimLightColor.x + 0.5f * fTimeDelta, 1.f);
            m_vRimLightColor.w = max(m_vRimLightColor.w - 0.6f * fTimeDelta, 0.1f);
        }
        else
        {
            m_vRimLightColor.x = max(m_vRimLightColor.x - 0.7f * fTimeDelta, 0.f);
            m_vRimLightColor.w = min(m_vRimLightColor.w + 0.7f * fTimeDelta, 0.5f);
        }

        if (m_vRimLightColor.x == 0.f && m_vRimLightColor.w == 0.5f)
            m_pMonster->On_PowerAttack(false);

        m_pMonster->Set_RimLightColor(m_vRimLightColor);
    }
}

void CState_RaxasiaP1_DashUpper::Control_Sound(_double CurTrackPos)
{

    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 30.f && CurTrackPos <= 50.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 40.f && CurTrackPos <= 48.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }

        if (!m_bStampSound)
        {
            if ((CurTrackPos >= 48.f))
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"));

                m_bStampSound = true;
            }
        }
    }
    else if (m_iRouteTrack == 3)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 44.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }

        if (!m_bStampSound)
        {
            if ((CurTrackPos >= 48.f))
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"));

                m_bStampSound = true;
            }
        }
    }
    else if (m_iRouteTrack == 4)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 48.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }

        if (!m_bStampSound)
        {
            if ((CurTrackPos >= 48.f))
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"));

                m_bStampSound = true;
            }
        }
    }
}

CState_RaxasiaP1_DashUpper* CState_RaxasiaP1_DashUpper::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_DashUpper* pInstance = new CState_RaxasiaP1_DashUpper(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_DashUpper"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_DashUpper::Free()
{
    __super::Free();
}