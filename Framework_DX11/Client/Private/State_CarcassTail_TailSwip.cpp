#include "stdafx.h"
#include "State_CarcassTail_TailSwip.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassTail_TailSwip::CState_CarcassTail_TailSwip(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_TailSwip::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_TailSwip::Start_State(void* pArg)
{
    m_iCurAnim = 1;     //왼 오 구분하게 만들기

    m_pMonster->Change_Animation(m_iCurAnim, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CarcassTail_TailSwip::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    Collider_Check();

}

void CState_CarcassTail_TailSwip::End_State()
{
}

_bool CState_CarcassTail_TailSwip::End_Check()
{
    return m_pMonster->Get_EndAnim(m_iCurAnim);
}

void CState_CarcassTail_TailSwip::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if ((CurTrackPos >= 110.f && CurTrackPos <= 135.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.1f, 2, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        m_pMonster->Active_CurrentWeaponCollider(1.1f, 4, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(2);
    }
}

CState_CarcassTail_TailSwip* CState_CarcassTail_TailSwip::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_TailSwip* pInstance = new CState_CarcassTail_TailSwip(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_TailSwip"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_TailSwip::Free()
{
    __super::Free();
}