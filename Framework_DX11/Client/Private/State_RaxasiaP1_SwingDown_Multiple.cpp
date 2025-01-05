#include "stdafx.h"
#include "State_RaxasiaP1_SwingDown_Multiple.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "Effect_Manager.h"

CState_RaxasiaP1_SwingDown_Multiple::CState_RaxasiaP1_SwingDown_Multiple(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_SwingDown_Multiple::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_SwingDown_Multiple::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_SWINGDOWN, false, 0.1f, 0);
    m_iCurAnimIndex = AN_SWINGDOWN;

    m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, (double)0.5);

    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);
    m_isRimLight = false;

    m_bSwingSound = false;
    m_bStampSound = false;
    m_bSpeedController = true;
    m_bSwing = false;
    m_bStamp = false;

    return S_OK;
}

void CState_RaxasiaP1_SwingDown_Multiple::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();


    if (m_iRouteTrack == 0)
    {
        if (m_bSpeedController && CurTrackPos >= 35.f)
        {
            m_bSpeedController = false;
            m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, (double)1);

        }
        if (CurTrackPos >= 80.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bStamp = false;
            m_bSwingSound = false;
            m_bStampSound = false;
            m_iCurAnimIndex = AN_SWINGDOWN_R;
            m_pMonster->Change_Animation(m_iCurAnimIndex, false, 0.1f, 0);
            return;
        }
        if (CurTrackPos <= 25.f || CurTrackPos >= 60.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 7)
    {
        if (CurTrackPos >= 70.f)
        {
            if (m_iRouteTrack == 7)
            {
                m_iCurAnimIndex = AN_SWINGDOWN;
                m_isRimLight = true;
                m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, (double)0.3f);
                m_bSpeedController = true;
            }
            else if (m_iCurAnimIndex == AN_SWINGDOWN_L)
            {
                m_iCurAnimIndex = AN_SWINGDOWN_R;
                _double SpeedRatio = -0.2f + m_iRouteTrack * 0.2f;
                if (SpeedRatio >= 2.f)
                    SpeedRatio = 2.f;
                m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, (double)1 + SpeedRatio);
            }
            else
            {
                m_iCurAnimIndex = AN_SWINGDOWN_L;
                _double SpeedRatio = -0.2f + m_iRouteTrack * 0.2f;
                if (SpeedRatio >= 2.f)
                    SpeedRatio = 2.f;
                m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, (double)1 + SpeedRatio);
            }

            ++m_iRouteTrack;
            m_bSwing = false;
            m_bStamp = false;
            m_bSwingSound = false;
            m_bStampSound = false;
            if (m_iRouteTrack == 7)
            {
                m_pMonster->Change_Animation(m_iCurAnimIndex, false, 0.5f, 15);
            }
            else
            {
                m_pMonster->Change_Animation(m_iCurAnimIndex, false, 0.1f, 0);
            }
            return;
        }

        if (CurTrackPos <= 30.f ||CurTrackPos >= 55.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }
    }
    else if (m_iRouteTrack == 8)
    {
        if (m_bSpeedController && CurTrackPos >= 35.f)
        {
            m_bSpeedController = false;
            m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, (double)1);

        }

        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        if (CurTrackPos <= 30.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
    Update_Rimlight(fTimeDelta, CurTrackPos);

}

void CState_RaxasiaP1_SwingDown_Multiple::End_State()
{
    m_pMonster->Get_Model()->Set_SpeedRatio(AN_SWINGDOWN, (double)1);
    m_pMonster->Get_Model()->Set_SpeedRatio(AN_SWINGDOWN_L, (double)1);
    m_pMonster->Get_Model()->Set_SpeedRatio(AN_SWINGDOWN_R, (double)1);

    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);
    m_pMonster->Set_RimLightColor(m_vRimLightColor);
}

_bool CState_RaxasiaP1_SwingDown_Multiple::End_Check()
{
    return m_pMonster->Get_EndAnim(m_iCurAnimIndex);;
}

void CState_RaxasiaP1_SwingDown_Multiple::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0 || m_iRouteTrack == 8)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 44.4f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 7)
    {
        if ((CurTrackPos >= 40.f && CurTrackPos <= 48.5f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP1_SwingDown_Multiple::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0 || m_iRouteTrack == 8)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 44.4f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
            m_bSwing = false;
        }
        
        if (!m_bStampSound)
        {
            if (CurTrackPos > 44.4f)
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
                m_bStampSound = true;
            }
        }
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 7)
    {
        if ((CurTrackPos >= 40.f && CurTrackPos <= 48.5f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
            m_bSwing = false;
        }

        if (!m_bStampSound)
        {
            if (CurTrackPos > 49.f)
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
                m_bStampSound = true;
            }
        }
    }
}

void CState_RaxasiaP1_SwingDown_Multiple::Update_Rimlight(_float fTimeDelta, _double CurTrackPos)
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

        m_pMonster->Set_RimLightColor(m_vRimLightColor);
    }
}

void CState_RaxasiaP1_SwingDown_Multiple::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0 || m_iRouteTrack == 8)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 44.4f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }

        if (!m_bStampSound)
        {
            if ((CurTrackPos >= 44.f))
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"));

                m_bStampSound = true;
            }
        }
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 7)
    {
        if ((CurTrackPos >= 40.f && CurTrackPos <= 48.5f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }

        if (!m_bStampSound)
        {
            if ((CurTrackPos >= 48.5f))
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"));

                m_bStampSound = true;
            }
        }
    }

}

CState_RaxasiaP1_SwingDown_Multiple* CState_RaxasiaP1_SwingDown_Multiple::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_SwingDown_Multiple* pInstance = new CState_RaxasiaP1_SwingDown_Multiple(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_SwingDown_Multiple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_SwingDown_Multiple::Free()
{
    __super::Free();
}