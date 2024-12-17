#include "stdafx.h"
#include "State_RaxasiaP2_Running_Linked.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "Effect_Manager.h"

CState_RaxasiaP2_Running_Linked::CState_RaxasiaP2_Running_Linked(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Running_Linked::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_Running_Linked::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_LINKED_FIRST, false, 0.1f, 0);
    
    m_bRunningWise = *static_cast<_bool*>(pArg);

    m_bSpeedController = false;
    m_bSwingSound = false;

    m_bSwing = false;
    m_bEnvelop = false;
    m_bAccel = false;
    m_bJump = false;

    return S_OK;
}

void CState_RaxasiaP2_Running_Linked::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 120)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bEnvelop = false;
            m_bSpeedController = false;
            m_bAccel = false;
            m_pMonster->Change_Animation(AN_LINKED_SECOND, false, 0.2f, 45);
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        if (!m_bSpeedController)
        {
            if (CurTrackPos >= 80)
            {
                m_bSpeedController = true;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_LINKED_FIRST, (_double)0.5);
            }
        }
        else
        {
            if (CurTrackPos >= 85)
            {
                m_bSpeedController = false;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_LINKED_FIRST, (_double)1);
            }
        }

        if (CurTrackPos >= 20.f && CurTrackPos <= 23.f)
        {
            _Vec3 vTargetPos = m_pMonster->Get_TargetPos();

            _Vec3 vDir = m_pMonster->Get_TargetDir();
            _Vec3 vUp{ 0, 1, 0 };

            _Vec3 vRight = vUp.Cross(vDir);
            vRight.Normalize();
            _Vec3 vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);

            if (m_bRunningWise)
            {
                vPos += (vRight * m_fAccelSpeed * fTimeDelta);
            }
            else
            {
                vPos -= (vRight * m_fAccelSpeed * fTimeDelta);
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
                if (m_fDistance < 3.f)
                {
                    vTemp *= -3.f;
                }
                else
                {
                    vTemp *= 3.f;
                }
            }

            m_pMonster->Get_RigidBody()->Set_GloblePose(vTargetPos - vDir + (vTemp * fTimeDelta));
        }
        break;

    case 1:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_JUMPLIGHTNING, false, 0.1f, 90);
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);

        if (!m_bSpeedController)
        {
            if (CurTrackPos >= 70)
            {
                m_bSpeedController = true;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_LINKED_SECOND, (_double)0.5);
            }
        }
        else
        {
            if (CurTrackPos >= 75)
            {
                m_bSpeedController = false;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_LINKED_SECOND, (_double)1);
            }
        }

        if (CurTrackPos >= 15.f && CurTrackPos <= 18.f)
        {
            _Vec3 vTargetPos = m_pMonster->Get_TargetPos();

            _Vec3 vDir = m_pMonster->Get_TargetDir();
            _Vec3 vUp{ 0, 1, 0 };

            _Vec3 vRight = vUp.Cross(vDir);
            vRight.Normalize();
            _Vec3 vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);

            if (!m_bRunningWise)
            {
                vPos += (vRight * m_fAccelSpeed * fTimeDelta);
            }
            else
            {
                vPos -= (vRight * m_fAccelSpeed * fTimeDelta);
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
                if (m_fDistance < 1.f)
                {
                    vTemp *= -3.f;
                }
                else
                {
                    vTemp *= 3.f;
                }
            }

            m_pMonster->Get_RigidBody()->Set_GloblePose(vTargetPos - vDir + (vTemp * fTimeDelta));
        }
        break;

    case 2:
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        break;


    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_Running_Linked::End_State()
{
    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_BIG);
}

_bool CState_RaxasiaP2_Running_Linked::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_LINKED_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_FIRST);
        }
        break;

    case 1:
        if ((AN_LINKED_SECOND) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_SECOND);
        }
        break;

    case 2:
        if ((AN_JUMPLIGHTNING) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_JUMPLIGHTNING);
        }
        break;

    default:
        break;
    }

    return  bEndCheck;
}

void CState_RaxasiaP2_Running_Linked::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0 || m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 100.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 80.f && CurTrackPos <= 90.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.6f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_STRONG);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    
}

void CState_RaxasiaP2_Running_Linked::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 100.f))
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
        }

        if ((CurTrackPos >= 20.f && CurTrackPos <= 60.f))
        {
            if (!m_bAccel)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
        }

    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 80.f && CurTrackPos <= 90.f))
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
        }

        if ((CurTrackPos >= 15.f && CurTrackPos <= 55.f))
        {
            if (!m_bAccel)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
        }
    }
    else
    {
        if (!m_bJump)
        {
            if (CurTrackPos >= 100.f)
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, 0.f };
                XMStoreFloat4x4(&WorldMat, m_pMonster->Get_Transform()->Get_WorldMatrix());
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Jump"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });

                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD, true);
                m_bJump = true;
            }
        }

        if (CurTrackPos >= 230.f)
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_BIG);
        }
    }
}

void CState_RaxasiaP2_Running_Linked::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP2_Running_Linked* CState_RaxasiaP2_Running_Linked::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Running_Linked* pInstance = new CState_RaxasiaP2_Running_Linked(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Running_Linked"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Running_Linked::Free()
{
    __super::Free();
}