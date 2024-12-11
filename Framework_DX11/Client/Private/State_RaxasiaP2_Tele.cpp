#include "stdafx.h"
#include "State_RaxasiaP2_Tele.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_Tele::CState_RaxasiaP2_Tele(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Tele::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_Tele::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_RUN, false, 0.1f, 0);

    m_bSwingSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP2_Tele::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 70.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_UPPERSLASH, false, 0.1f, 0);
        }

        if (!m_bTele)
        {
            if (CurTrackPos >= 60.f)
            {
                m_bTele = true;

                _Vec3 vDir = m_pMonster->Get_TargetDir();
                _Vec3 vPos = m_pMonster->Get_TargetPos();
                vDir.Normalize();

                m_pMonster->Get_RigidBody()->Set_GloblePose(vPos - (vDir * 2));
            }
        }

        //바꾸기 직전에 플레이어 방향으로 도약
        break;

    case 1:
        if (End_Check())
        {
            if (m_iAtkCnt = 0)
            {
                m_iAtkCnt = 1;
                m_pMonster->Change_State(CRaxasia::ATKP2_TELE_LINKEDTEL);
                return;
            }
            else
            {
                m_iAtkCnt = 0;
                m_pMonster->Change_State(CRaxasia::ATKP2_TELE_JUMPLIGHTNING);
                return;
            }
        }
        break;


    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_Tele::End_State()
{
}

_bool CState_RaxasiaP2_Tele::End_Check()
{
    return  m_pMonster->Get_EndAnim(AN_UPPERSLASH);;
}

void CState_RaxasiaP2_Tele::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
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
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 105.f && CurTrackPos <= 120.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.6f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_STRONG);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP2_Tele::Effect_Check(_double CurTrackPos)
{
    if (CurTrackPos >= 35.f)
    {
        //방전
    }
}

void CState_RaxasiaP2_Tele::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP2_Tele* CState_RaxasiaP2_Tele::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Tele* pInstance = new CState_RaxasiaP2_Tele(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Tele"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Tele::Free()
{
    __super::Free();
}