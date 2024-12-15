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

    m_bSwingSound = false;
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
            m_pMonster->Change_Animation(AN_SWINGDOWN_R, false, 0.1f, 0);
            m_iCurAnimIndex = AN_SWINGDOWN_R;
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
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bStamp = false;
            if (m_iRouteTrack == 7)
            {
                m_iCurAnimIndex = AN_SWINGDOWN;
                m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, (double)0.5f);
                m_bSpeedController = true;
            }
            else if (m_iCurAnimIndex == AN_SWINGDOWN_L)
            {
                m_iCurAnimIndex = AN_SWINGDOWN_R;
                _double SpeedRatio = m_iRouteTrack * 0.2f;
                if (SpeedRatio >= 2.f)
                    SpeedRatio = 2.f;
                m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, (double)1 + SpeedRatio);
            }
            else
            {
                m_iCurAnimIndex = AN_SWINGDOWN_L;
                _double SpeedRatio = m_iRouteTrack * 0.2f;
                if (SpeedRatio >= 2.f)
                    SpeedRatio = 2.f;
                m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, (double)1 + SpeedRatio);
            }
            m_pMonster->Change_Animation(m_iCurAnimIndex, false, 0.1f, 0);
        }

        if (CurTrackPos <= 30.f ||CurTrackPos >= 55.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }
    }
    else if (m_iRouteTrack = 8)
    {
        if (m_bSpeedController && CurTrackPos >= 35.f)
        {
            m_bSpeedController = false;
            m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, (double)1);

        }
        //림라이트?
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

    _double Ratio = (double)(1 + (m_iRouteTrack * 0.2f));
    if (Ratio >= 2.f)
    {
        Ratio = 2.f;
    }
    m_pMonster->Get_Model()->Set_SpeedRatio(m_iCurAnimIndex, Ratio);

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
}

void CState_RaxasiaP1_SwingDown_Multiple::End_State()
{
    m_pMonster->Get_Model()->Set_SpeedRatio(AN_SWINGDOWN, (double)1);
    m_pMonster->Get_Model()->Set_SpeedRatio(AN_SWINGDOWN_L, (double)1);
    m_pMonster->Get_Model()->Set_SpeedRatio(AN_SWINGDOWN_R, (double)1);
}

_bool CState_RaxasiaP1_SwingDown_Multiple::End_Check()
{
    return m_pMonster->Get_EndAnim(m_iCurAnimIndex);;
}

void CState_RaxasiaP1_SwingDown_Multiple::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0 || m_iRouteTrack == 8)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 44.f))
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
        if (m_iCurAnimIndex == AN_SWINGDOWN_L)
        {
            if ((CurTrackPos >= 35.f && CurTrackPos <= 44.f))
            {
                m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
        else
        {
            if ((CurTrackPos >= 40.f && CurTrackPos <= 48.f))
            {
                m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
    }
}

void CState_RaxasiaP1_SwingDown_Multiple::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0 || m_iRouteTrack == 8)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 44.f))
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
        
        if (!m_bStamp)
        {
            if (CurTrackPos >= 44.f)
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, -4.25f };
                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(0) * (*m_pMonster->Get_WeaponWorldMat())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_Stamp"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });
                m_bStamp = true;
            }
        }
    }
    else if (m_iRouteTrack >= 1 && m_iRouteTrack <= 7)
    {
        if (m_iCurAnimIndex == AN_SWINGDOWN_L)
        {
            if ((CurTrackPos >= 35.f && CurTrackPos <= 44.f))
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
            if (!m_bStamp)
            {
                if (CurTrackPos >= 44.f)
                {
                    _float4x4 WorldMat{};
                    _Vec3 vPos = { 0.f, 0.f, -4.25f };
                    XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(0) * (*m_pMonster->Get_WeaponWorldMat())));
                    vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                    CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_Stamp"),
                        vPos, _Vec3{ m_pMonster->Get_TargetDir() });
                    m_bStamp = true;
                }
            }
        }
        else
        {
            if ((CurTrackPos >= 40.f && CurTrackPos <= 48.f))
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
            if (!m_bStamp)
            {
                if (CurTrackPos >= 48.f)
                {
                    _float4x4 WorldMat{};
                    _Vec3 vPos = { 0.f, 0.f, -4.25f };
                    XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(0) * (*m_pMonster->Get_WeaponWorldMat())));
                    vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                    CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_Stamp"),
                        vPos, _Vec3{ m_pMonster->Get_TargetDir() });
                    m_bStamp = true;
                }
            }
        }
    }
}

void CState_RaxasiaP1_SwingDown_Multiple::Control_Sound(_double CurTrackPos)
{

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