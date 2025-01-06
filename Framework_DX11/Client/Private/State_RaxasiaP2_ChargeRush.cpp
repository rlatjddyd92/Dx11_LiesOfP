#include "stdafx.h"
#include "State_RaxasiaP2_ChargeRush.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"
#include "AttackObject.h"

CState_RaxasiaP2_ChargeRush::CState_RaxasiaP2_ChargeRush(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_ChargeRush::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_ChargeRush::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_CHARGE, false, 0.1f, 0);

    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);
    m_isRimLight = false;

    m_bSwingSound = false;
    m_bChargeSound = false;
    m_bRushSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP2_ChargeRush::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (m_iRouteTrack == 0)
    {
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bCharge = false;
            m_bChargeSound = false;
            m_pMonster->Change_Animation(AN_CHARGE_LOOP, false, 0.02f, 0);
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("VO_NPC_Raxasia_P2_Roar_05.wav"), false);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
    }
    else if (m_iRouteTrack>= 1 && m_iRouteTrack <= 2)
    {
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bCharge = false;
            m_bChargeSound = false;
            m_bRush = false;
            if (m_iRouteTrack == 3)
            {
                m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SHIELDCHARGE_GROUND);
                m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT2);
                m_pMonster->Change_Animation(AN_SHIELDRUSH, false, 0.2f, 0);
                return;
            }
            else
            {
                m_pMonster->Change_Animation(AN_CHARGE_LOOP, false, 0.f, 0, true, true);
                return;
            }
        }
    }
    else if (m_iRouteTrack >= 3 && m_iRouteTrack <= 4)
    {
        if (CurTrackPos >= 165.f)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bRush = false;
            m_bRushSound = false;
            m_bSpeedController = false;
            m_isRimLight = true;

            m_pMonster->Change_Animation(AN_SHIELDRUSH, false, 0.1f, 0, true, true);
            return;
        }

        if (CurTrackPos >= 7.f && CurTrackPos <= 100.f)
        {
            if (!m_bSpeedController)
            {
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_SHIELDRUSH, 2.f);
                m_bSpeedController = true;
            }
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.2f, fTimeDelta);
        }
        else
        {
            m_pMonster->Get_Model()->Set_SpeedRatio(AN_SHIELDRUSH, 1.f);
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.2f, fTimeDelta);
        }

    }
    else    //퓨리어택. 
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        if (CurTrackPos >= 7.f && CurTrackPos <= 100.f)
        {
            if (!m_bSpeedController)
            {
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_SHIELDRUSH, 2.f);
                m_bSpeedController = true;
            }
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.2f, fTimeDelta);
        }
        else
        {
            m_pMonster->Get_Model()->Set_SpeedRatio(AN_SHIELDRUSH, 1.f);
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.2f, fTimeDelta);
        }
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
    Update_Rimlight(fTimeDelta, CurTrackPos);

}

void CState_RaxasiaP2_ChargeRush::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(1);

    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2);
    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL);

    m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_VOICE);
    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);
    m_pMonster->Set_RimLightColor(m_vRimLightColor);
}

_bool CState_RaxasiaP2_ChargeRush::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_CHARGE) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_CHARGE);
        }
        break;

    case 1:
    case 2:
        if ((AN_CHARGE_LOOP) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_CHARGE_LOOP);
        }
        break;

    case 5:
        if ((AN_SHIELDRUSH) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_SHIELDRUSH);
        }
    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_ChargeRush::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack >= 3 && m_iRouteTrack <= 5)
    {
        if ((CurTrackPos >= 7.f && CurTrackPos <= 100.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
            m_pMonster->Active_Debuff(0, 1.4f);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(1);
            m_pMonster->DeActive_Debuff(0);
        }
    }
}

void CState_RaxasiaP2_ChargeRush::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bCharge)
        {
            if (CurTrackPos >= 35.f)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2, true);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SHIELDCHARGE_GROUND, true);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL, true);
                m_bCharge = true;
            }
        }
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 2)
    {
        if (CurTrackPos >= 60.f && CurTrackPos <= 70.f)
        {
            if (!m_bCharge)
            {
                CAttackObject::ATKOBJ_DESC Desc;

                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.1f, 0.f };
                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_UFBIndex(UFB_ROOT))
                    * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                _Vec3 vTargetDir = m_pMonster->Get_TargetPos() - vPos;

                Desc.vPos = vPos;
                Desc.vDir = vTargetDir;
                Desc.pOwner = m_pMonster;

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderDischarge"), &Desc);

                m_bCharge = true;
            }
        }
    }
    else if (m_iRouteTrack >= 3 && m_iRouteTrack <= 5)
    {
        if (CurTrackPos >= 7.f && CurTrackPos <= 100.f)
        {
            if (!m_bRush)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SHIELDDASH, true);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
                m_bRush = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SHIELDDASH);
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
        }
    }

}

void CState_RaxasiaP2_ChargeRush::Update_Rimlight(_float fTimeDelta, _double CurTrackPos)
{
    if (m_isRimLight)
    {
        switch (m_iRouteTrack)
        {
        case 4:
            if (CurTrackPos > 105.f)
            {
                m_vRimLightColor.x = max(m_vRimLightColor.x + 0.5f * fTimeDelta, 1.f);
                m_vRimLightColor.w = max(m_vRimLightColor.w - 0.6f * fTimeDelta, 0.1f);
            }
            break;

        case 5:
            if (CurTrackPos < 100.f)
            {
                m_vRimLightColor.x = max(m_vRimLightColor.x + 0.5f * fTimeDelta, 1.f);
                m_vRimLightColor.w = max(m_vRimLightColor.w - 0.6f * fTimeDelta, 0.1f);
            }
            else
            {
                m_vRimLightColor.x = max(m_vRimLightColor.x - 0.7f * fTimeDelta, 0.f);
                m_vRimLightColor.w = min(m_vRimLightColor.w + 0.7f * fTimeDelta, 0.5f);
            }
            break;

        default:
            break;
        }

        m_pMonster->Set_RimLightColor(m_vRimLightColor);
    }
}

void CState_RaxasiaP2_ChargeRush::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bChargeSound)
        {
            if (CurTrackPos >= 35.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Heavy_Loop_03.wav"), true);
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_VOICE, TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Electric_Loop_02.wav"), true);
                m_bChargeSound = true;
            }
        }
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 2)          //돌진 하울링에 2번 부여
    {
        if (!m_bChargeSound)
        {
            if (CurTrackPos >= 60.f && CurTrackPos <= 70.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_FX_Spark_Charge_01.wav"), false);
                m_bChargeSound = true;
            }
        }
    }
    else if (m_iRouteTrack >= 3 && m_iRouteTrack <= 5)
    {
        if (CurTrackPos >= 7.f && CurTrackPos <= 100.f)
        {
            if (!m_bRushSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("VO_NPC_Raxasia_P2_Roar_05.wav"), false);
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_FX_Blink_Spark_Foot_03.wav"), true);
                m_bRushSound = true;
            }
        }
        else
        {
            m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT1);
        }
    }
}

CState_RaxasiaP2_ChargeRush* CState_RaxasiaP2_ChargeRush::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_ChargeRush* pInstance = new CState_RaxasiaP2_ChargeRush(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_ChargeRush"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_ChargeRush::Free()
{
    __super::Free();
}