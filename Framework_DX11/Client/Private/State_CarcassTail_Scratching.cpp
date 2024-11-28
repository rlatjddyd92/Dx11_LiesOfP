#include "stdafx.h"
#include "State_CarcassTail_Scratching.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassTail_Scratchig::CState_CarcassTail_Scratchig(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_Scratchig::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_Scratchig::Start_State(void* pArg)
{
    m_iCurAnim = 1;     //왼 오 구분하게 만들기

    m_pMonster->Change_Animation(m_iCurAnim, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CarcassTail_Scratchig::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    Collider_Check();

}

void CState_CarcassTail_Scratchig::End_State()
{
}

_bool CState_CarcassTail_Scratchig::End_Check()
{
    return m_pMonster->Get_EndAnim(m_iCurAnim);
}

void CState_CarcassTail_Scratchig::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if ((CurTrackPos >= 90.f && CurTrackPos <= 115.f) ||
        (CurTrackPos >= 135.f && CurTrackPos <= 165.f) ||
        (CurTrackPos >= 225.f && CurTrackPos <= 245.f) ||
        (CurTrackPos >= 275.f && CurTrackPos <= 295.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1, 0);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(0);
    }

    if ((CurTrackPos >= 82.f && CurTrackPos <= 100.f) ||
        (CurTrackPos >= 150.f && CurTrackPos <= 165.f) ||
        (CurTrackPos >= 185.f && CurTrackPos <= 195.f) ||
        (CurTrackPos >= 205.f && CurTrackPos <= 225.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1, 1);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(1);
    }
}

CState_CarcassTail_Scratchig* CState_CarcassTail_Scratchig::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_Scratchig* pInstance = new CState_CarcassTail_Scratchig(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_Scratchig"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_Scratchig::Free()
{
    __super::Free();
}