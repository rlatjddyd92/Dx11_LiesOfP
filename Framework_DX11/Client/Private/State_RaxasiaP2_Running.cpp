#include "stdafx.h"
#include "State_RaxasiaP2_Running.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "Effect_Manager.h"

CState_RaxasiaP2_Running::CState_RaxasiaP2_Running(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Running::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_Running::Start_State(void* pArg)
{
    m_iRouteTrack = 0;

    _int iCnt = rand() % 2;
    if (iCnt == 0)
    {
        m_bRunningWise = false;
        m_iDashAnimIndex = AN_DASH_L;
    }
    else
    {
        m_bRunningWise = true;
        m_iDashAnimIndex = AN_DASH_R;
    }
    m_pMonster->Change_Animation(m_iDashAnimIndex, false, 0.1f, 0);

    iCnt = rand() % 2;
    if (iCnt == 0)
    {
        m_iNextStateNum = CRaxasia::ATKP2_RUNNING_LINKED;
    }
    else
    {
        m_iNextStateNum = CRaxasia::ATKP2_RUNNING_FURY;
    }
    m_iNextStateNum = CRaxasia::ATKP2_RUNNING_LINKED;

    m_bSwingSound = false;
    m_bShieldAttack = false;
    m_bSwing = false;

    return S_OK;
}

void CState_RaxasiaP2_Running::Update(_float fTimeDelta)
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
            m_fDistance = m_pMonster->Calc_Distance_XZ();

            m_pMonster->Change_Animation(AN_RUNNING, false, 0.2f, 0);
            return;
        }
        _Vec3 vDir = m_pMonster->Get_TargetDir();
        vDir.Normalize();
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(_Vec4{ vDir }, 2.f, fTimeDelta);

        break;
    }

    case 1:
    {
        if (m_iNextStateNum == CRaxasia::ATKP2_RUNNING_LINKED)
        {
            if (End_Check())
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL, true);
                m_pMonster->Change_State(CRaxasia::ATKP2_RUNNING_LINKED, &m_bRunningWise);
                return;
            }
        }
        else
        {
            if (CurTrackPos >= 225.f)
            {
                m_pMonster->Change_State(CRaxasia::ATKP2_RUNNING_FURY, &m_bRunningWise);
                return;
            }
        }

        if (CurTrackPos <= 120.f)
        {
            _Vec3 vTargetPos = m_pMonster->Get_TargetPos();

            _Vec3 vDir = m_pMonster->Get_TargetDir();
            _Vec3 vUp{ 0, 1, 0 };

            _Vec3 vRight = vUp.Cross(vDir);
            vRight.Normalize();
            vRight *= 2.f;

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
                if (m_fDistance < 3.f)
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
            _Vec3 vDir = m_pMonster->Get_TargetDir();
            _Vec3 vVelo = m_pMonster->Get_RigidBody()->Get_Velocity();
            vDir.Normalize();

            if (m_pMonster->Calc_Distance_XZ() >= 1.5f)
            {
                m_pMonster->Get_RigidBody()->Set_Velocity(vVelo + vDir * 3);
            }
            else
            {
                m_pMonster->Get_RigidBody()->Set_Velocity(vVelo - ( vDir * 3));
            }
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3.f, fTimeDelta);
        }
        break;
    }

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_Running::End_State()
{
}

_bool CState_RaxasiaP2_Running::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((m_iDashAnimIndex) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iDashAnimIndex);
        }
        break;

    case 1:
        if ((AN_RUNNING) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_RUNNING);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_Running::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
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
}

void CState_RaxasiaP2_Running::Effect_Check(_double CurTrackPos)
{

    if (m_iRouteTrack == 1)
    {
        if (!m_bSwing)
        {
            if ((CurTrackPos >= 145.f && CurTrackPos <= 160.f))
            {
                m_bSwing = true;
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING);
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
                m_bShieldAttack = true;
            }
        }
    }

}

void CState_RaxasiaP2_Running::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP2_Running* CState_RaxasiaP2_Running::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Running* pInstance = new CState_RaxasiaP2_Running(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Running"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Running::Free()
{
    __super::Free();
}