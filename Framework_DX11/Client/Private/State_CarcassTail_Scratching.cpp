#include "stdafx.h"
#include "State_CarcassTail_Scratching.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassTail_Scratching::CState_CarcassTail_Scratching(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_Scratching::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassTail_Scratching::Start_State(void* pArg)
{
    m_iCurAnim = AN_SCRATCHING_R;     //왼 오 구분하게 만들기

    m_pMonster->Change_Animation(m_iCurAnim, false, 0.1f, 0, true);

    return S_OK;
}

void CState_CarcassTail_Scratching::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos < 80.f)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.5f, fTimeDelta);
    }

    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }

    Collider_Check();

}

void CState_CarcassTail_Scratching::End_State()
{
}

_bool CState_CarcassTail_Scratching::End_Check()
{
    return m_pMonster->Get_EndAnim(m_iCurAnim);
}

void CState_CarcassTail_Scratching::Collider_Check()
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if ((CurTrackPos >= 85.f && CurTrackPos <= 115.f))
    {
        if (m_iCurAnim == AN_SCRATCHING_L)
        {
            m_pMonster->Active_CurrentWeaponCollider(1, 0, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->Active_CurrentWeaponCollider(1, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
    }
    else
    {
        if (m_iCurAnim == AN_SCRATCHING_L)
        {
            m_pMonster->DeActive_CurretnWeaponCollider(0);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
}

CState_CarcassTail_Scratching* CState_CarcassTail_Scratching::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_Scratching* pInstance = new CState_CarcassTail_Scratching(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_Scratching"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_Scratching::Free()
{
    __super::Free();
}