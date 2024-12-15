#include "stdafx.h"
#include "State_RaxasiaP1_Sting_Spread.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "Effect_Manager.h"

CState_RaxasiaP1_Sting_Spread::CState_RaxasiaP1_Sting_Spread(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_Sting_Spread::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_Sting_Spread::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_STING, false, 0.1f, 0);

    m_bSwingSound = false;
    m_bLockOn = false;
    m_bResetAtkList = false;
    m_bCharge = false;
    m_bSwing = false;
    m_bStamp = false;
    m_bSpread = false;

    return S_OK;
}

void CState_RaxasiaP1_Sting_Spread::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 65.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_BACKJUMP, false, 0.1f, 0);
            return;
        }

        if (CurTrackPos <= 50.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    case 1:
        if (CurTrackPos >= 40.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_STING, false, 0.1f, 65);
            return;
        }

        break;

    case 2:
        if (CurTrackPos >= 260.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_SWING_ANDSPREAD, false, 0.1f, 65);
            return;
        }
        if (CurTrackPos <= 110.f)
        {//플레이어방향 이동
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }
        else if (CurTrackPos >= 117.f && CurTrackPos <= 120.f)
        {
            if (!m_bLockOn)
            {
                m_vLockVec = { m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK) };
                m_vLockVec.Normalize();
                m_bLockOn = true;
            }
            
            m_pMonster->Get_RigidBody()->Set_Velocity(m_pMonster->Get_RigidBody()->Get_Velocity() + (m_vLockVec * 40));
        }

        break;

    case 3:
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        if (CurTrackPos <= 130.f ||
            (CurTrackPos >= 170.f && CurTrackPos <= 215.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }
        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP1_Sting_Spread::End_State()
{
    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD);
}

_bool CState_RaxasiaP1_Sting_Spread::End_Check()
{
    return  m_pMonster->Get_EndAnim(AN_SWING_ANDSPREAD);
}

void CState_RaxasiaP1_Sting_Spread::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 115.f && CurTrackPos <= 135.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack == 3)
    {
        if ((CurTrackPos >= 85.f && CurTrackPos <= 140.f) ||
            (CurTrackPos >= 210.f && CurTrackPos <= 220.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }

        if (!m_bResetAtkList)
        {
            if (CurTrackPos >= 110.f)
            {
                m_bResetAtkList = true;
            }
        }
    }
}

void CState_RaxasiaP1_Sting_Spread::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bCharge)
        {
            if (CurTrackPos >= 20.f)
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, 0.f };
                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_UFBIndex(UFB_HAND_RIGHT)) * m_pMonster->Get_Transform()->Get_WorldMatrix()));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_ThunderInchent"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });

                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD, true);
                m_bCharge = true;
            }
        }
    }
    else if (m_iRouteTrack == 3)
    {
        if (CurTrackPos >= 85.f && CurTrackPos <= 140.f)
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

        if (m_bCharge)
        {
            if (CurTrackPos <= 10.f ||
                (CurTrackPos >= 280.f))
            {
                m_bCharge = false;
                m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD);
            }
        }
        else
        {
            if (CurTrackPos>= 175 && CurTrackPos <= 180.f)
            {
                m_bCharge = true;
                m_bSpeedController = true;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_SWING_ANDSPREAD, (double)0.5);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD, true);
            }
        }

        if (m_bSpeedController)
        {
            if (CurTrackPos >= 205.f)
            {
                m_bSpeedController = false;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_SWING_ANDSPREAD, (double)1);
            }
        }

        if (!m_bStamp)
        {
            if (CurTrackPos >= 219.f)
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, -4.25f };
                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(0) * (*m_pMonster->Get_WeaponWorldMat())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_ThunderStamp"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });
                m_bStamp = true;
            }
        }

        if (!m_bSpread)
        {
            if (CurTrackPos >= 250.f)
            {
                //_float4x4 WorldMat{};
                //_Vec3 vPos = { 0.f, 0.f, -4.25f };
                //XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(0) * (*m_pMonster->Get_WeaponWorldMat())));
                //vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
                //
                //CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_ThunderSpread"),
                //    vPos, _Vec3{ m_pMonster->Get_TargetDir() });
                //
                //m_bSpread = true;
            }
        }


    }
}

void CState_RaxasiaP1_Sting_Spread::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP1_Sting_Spread* CState_RaxasiaP1_Sting_Spread::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_Sting_Spread* pInstance = new CState_RaxasiaP1_Sting_Spread(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_Sting_Spread"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_Sting_Spread::Free()
{
    __super::Free();
}