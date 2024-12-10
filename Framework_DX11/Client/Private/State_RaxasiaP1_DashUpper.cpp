#include "stdafx.h"
#include "State_RaxasiaP1_DashUpper.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

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

    m_bSwingSound = false;

    m_bSwing = false;
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
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        if (CurTrackPos >= 30.f)
        {
            _float fDist = m_pMonster->Calc_Distance_XZ();

            if (fDist <= 5.f)
            {
                _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
                _Vec3 vLook = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK));
                _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());

                _Vec3 vCrossUp = vLook.Cross(vTargetDir);

                if (fabs(vUp.Dot(vCrossUp)) <= 0.3f)
                {
                    ++m_iRouteTrack;
                    m_pMonster->Change_Animation(AN_DASHUPPER_ATTACK, false, 0.1f, 0);
                }
            }

            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }
        break;

    case 1:
        if (CurTrackPos >= 130.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_TRIPLELINK_FIRST, false, 0.1f, 0);
        }
        break;

    case 2:
        if (CurTrackPos >= 70.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_TRIPLELINK_SECOND, false, 0.1f, 0);
        }
        break;

    case 3:
        if (CurTrackPos >= 70.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_TRIPLELINK_LAST, false, 0.1f, 0);
        }
        break;

    case 4:
        if (End_Check())
        {
            m_bSwing = false;
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

}

void CState_RaxasiaP1_DashUpper::End_State()
{
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
        if ((AN_TRIPLELINK_LAST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_TRIPLELINK_LAST);
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
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack == 2)
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
    else if (m_iRouteTrack == 3)
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
    else if (m_iRouteTrack == 4)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 44.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP1_DashUpper::Effect_Check(_double CurTrackPos)
{
}

void CState_RaxasiaP1_DashUpper::Control_Sound(_double CurTrackPos)
{

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