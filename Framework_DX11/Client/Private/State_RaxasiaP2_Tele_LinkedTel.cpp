#include "stdafx.h"
#include "State_RaxasiaP2_Tele_LinkedTel.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_Tele_LinkedTel::CState_RaxasiaP2_Tele_LinkedTel(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Tele_LinkedTel::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_Tele_LinkedTel::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->SetUp_Animation(AN_LINKED_SECOND, false, 0);
    m_bAccel = false;
    m_bSwingSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP2_Tele_LinkedTel::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();
    //0~1 긴텔 붙여야함
    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 120)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bAccel = false;
            m_pMonster->SetUp_Animation(AN_LINKED_FIRST, false, 45);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);

        if (CurTrackPos >= 73.5f && CurTrackPos <= 77.5f)
        {
            _Vec3 vTargetPos = m_pMonster->Get_TargetPos();

            _Vec3 vDir = m_pMonster->Get_TargetDir();
            _Vec3 vUp{ 0, 1, 0 };

            _Vec3 vRight = vUp.Cross(vDir);
            vRight.Normalize();
            _Vec3 vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);

            //우측으로
            vPos -= (vRight * m_fAccelSpeed * fTimeDelta);

            vDir = vTargetPos - vPos;
            vDir.Normalize();

            m_fDistance = m_pMonster->Calc_Distance_XZ();

            vDir *= m_fDistance;
            _Vec3 vTemp{};
            vTemp = vDir;
            vTemp.Normalize();
            if (m_fDistance <= 3.05)
            {
                vTemp *= -15.f;
            }
            else if (m_fDistance >= 3.15)
            {
                vTemp *= 15.f;
            }

            m_pMonster->Get_RigidBody()->Set_GloblePose(vTargetPos - vDir + (vTemp * fTimeDelta));
        }
        break;

    case 1:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->SetUp_Animation(AN_UPPERSLASH, false, 0);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);

        if (CurTrackPos >= 62.5f && CurTrackPos <= 73.f)
        {
            _Vec3 vTargetPos = m_pMonster->Get_TargetPos();

            _Vec3 vDir = m_pMonster->Get_TargetDir();
            _Vec3 vUp{ 0, 1, 0 };

            _Vec3 vRight = vUp.Cross(vDir);
            vRight.Normalize();
            _Vec3 vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);

            //좌측으로
            vPos += (vRight * m_fAccelSpeed * fTimeDelta);

            vDir = vTargetPos - vPos;
            vDir.Normalize();

            m_fDistance = m_pMonster->Calc_Distance_XZ();

            vDir *= m_fDistance;
            _Vec3 vTemp{};
            if (CurTrackPos >= 5.f)
            {
                vTemp = vDir;
                vTemp.Normalize();
                if (m_fDistance <= 3.05)
                {
                    vTemp *= -15.f;
                }
                else if (m_fDistance >= 3.15)
                {
                    vTemp *= 15.f;
                }
            }

            m_pMonster->Get_RigidBody()->Set_GloblePose(vTargetPos - vDir + (vTemp * fTimeDelta));
        }
        break;

    case 2:
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        if (CurTrackPos <= 10.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_Tele_LinkedTel::End_State()
{
}

_bool CState_RaxasiaP2_Tele_LinkedTel::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_LINKED_SECOND) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_SECOND);
        }
        break;

    case 1:
        if ((AN_LINKED_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_FIRST);
        }
        break;

    case 2:
        if ((AN_UPPERSLASH) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_UPPERSLASH);
        }
        break;

    default:
        break;
    }

    return  bEndCheck;
}

void CState_RaxasiaP2_Tele_LinkedTel::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0 )
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 100.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
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
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else
    {
        if ((CurTrackPos >= 15.f && CurTrackPos <= 40.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP2_Tele_LinkedTel::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 100.f))
        {
            m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }
        
        if ((CurTrackPos >= 73.f && CurTrackPos <= 77.5f))
        {
            if (!m_bAccel)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
                m_bAccel = true;
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
            m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }

        if ((CurTrackPos >= 62.f && CurTrackPos <= 73.f))
        {
            if (!m_bAccel)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
                m_bAccel = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
        }

    }
    else
    {
        if ((CurTrackPos >= 15.f && CurTrackPos <= 40.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }
    }
}

void CState_RaxasiaP2_Tele_LinkedTel::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP2_Tele_LinkedTel* CState_RaxasiaP2_Tele_LinkedTel::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Tele_LinkedTel* pInstance = new CState_RaxasiaP2_Tele_LinkedTel(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Tele_LinkedTel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Tele_LinkedTel::Free()
{
    __super::Free();
}