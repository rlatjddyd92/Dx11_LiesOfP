#include "stdafx.h"
#include "State_RaxasiaP1_TripleSting.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP1_TripleSting::CState_RaxasiaP1_TripleSting(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_TripleSting::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_TripleSting::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_READY, false, 0.1f, 0);

    m_bSwingSound = false;

    m_bSting = false;
    return S_OK;
}

void CState_RaxasiaP1_TripleSting::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();
    //18, 20, 21, 24
    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSting = false;
            m_pMonster->Change_Animation(AN_STING, false, 0.1f, 0);
        }

        break;

    case 1:
        if (CurTrackPos >= 230.f)
        {
            m_iRouteTrack = 0;
            m_bSting = false;
            m_pMonster->Change_Animation(AN_STING, false, 0.1f, 0);
            return;
        }

        break;

    case 2:
        if (CurTrackPos >= 230.f)
        {
            m_iRouteTrack = 0;
            m_bSting = false;
            m_pMonster->Change_Animation(AN_STING, false, 0.1f, 0);
            return;
        }

        break;

    case 3:
        if (End_Check())
        {
            m_iRouteTrack = 0;
            m_bSting = false;
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

void CState_RaxasiaP1_TripleSting::End_State()
{
}

_bool CState_RaxasiaP1_TripleSting::End_Check()
{
    return false;
}

void CState_RaxasiaP1_TripleSting::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack != 0)
    {
        if ((CurTrackPos >= 115.f && CurTrackPos <= 135.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }

    }
}

void CState_RaxasiaP1_TripleSting::Effect_Check(_double CurTrackPos)
{
}

void CState_RaxasiaP1_TripleSting::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP1_TripleSting* CState_RaxasiaP1_TripleSting::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_TripleSting* pInstance = new CState_RaxasiaP1_TripleSting(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_TripleSting"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_TripleSting::Free()
{
    __super::Free();
}