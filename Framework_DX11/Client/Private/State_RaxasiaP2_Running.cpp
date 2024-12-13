#include "stdafx.h"
#include "State_RaxasiaP2_Running.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

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

    m_bSwingSound = false;

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

        if (m_pMonster->Calc_Distance_XZ() >= 3.f)
        {
            _Vec3 vVelo = m_pMonster->Get_RigidBody()->Get_Velocity();
            m_pMonster->Get_RigidBody()->Set_Velocity(vVelo + vDir);
        }
        break;
    }

    case 1:
    {
        if (m_iNextStateNum == CRaxasia::ATKP2_RUNNING_LINKED)
        {
            if (End_Check())
            {
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

            vDir *= m_fDistance;
            if (m_pMonster->Calc_Distance_XZ() >= 3.f)
            {
                _Vec3 vVelo = m_pMonster->Get_RigidBody()->Get_Velocity();
                m_pMonster->Get_RigidBody()->Set_Velocity(vVelo + vDir);
            }
            m_pMonster->Get_RigidBody()->Set_GloblePose(vTargetPos - vDir);
        }
        else if (CurTrackPos <= 160)
        {
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
        if ((CurTrackPos >= 150.f && CurTrackPos <= 160.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }

        if ((CurTrackPos >= 170.f && CurTrackPos <= 180.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP2_Running::Effect_Check(_double CurTrackPos)
{

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