#include "stdafx.h"
#include "State_RaxasiaP2_JumpStamp.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_JumpStamp::CState_RaxasiaP2_JumpStamp(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_JumpStamp::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_JumpStamp::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_JUMPSTAMP_START, false, 0.1f, 0);

    m_bSwingSound = false;
    m_bStartSpot = true;
    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP2_JumpStamp::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_JUMPSTAMP_MIDDLE, false, 0.1f, 0);
        }

        break;

    case 1:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_JUMPSTAMP_END, false, 0.1f, 0);
        }
        if (m_bStartSpot)
        {
            if (CurTrackPos >= 40.f)
            {
                m_bStartSpot = false;
                m_vTargetDir = m_pMonster->Get_TargetDir();
            }
        }
        if (CurTrackPos <= 40.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }

        break;

    case 2:
        if (End_Check())
        {
            m_iRouteTrack = 0;
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
        if (CurTrackPos <= 15.f)
        {
            _Vec3 vMove = m_vTargetDir * ((_float)CurTrackPos / 15.f);
            m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vFlyMoveStack) / fTimeDelta);
            m_vFlyMoveStack = vMove;
        }

        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_JumpStamp::End_State()
{
}

_bool CState_RaxasiaP2_JumpStamp::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_JUMPSTAMP_START) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_JUMPSTAMP_START);
        }
        break;

    case 1:
        if ((AN_JUMPSTAMP_MIDDLE) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_JUMPSTAMP_MIDDLE);
        }
        break;

    case 2:
        if ((AN_JUMPSTAMP_END) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_JUMPSTAMP_END);
        }
        break;

    default:
        break;
    }

    return  bEndCheck;
}

void CState_RaxasiaP2_JumpStamp::Collider_Check(_double CurTrackPos)
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

void CState_RaxasiaP2_JumpStamp::Effect_Check(_double CurTrackPos)
{
    if (CurTrackPos >= 97.f)
    {
        //มกวม
    }
}

void CState_RaxasiaP2_JumpStamp::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP2_JumpStamp* CState_RaxasiaP2_JumpStamp::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_JumpStamp* pInstance = new CState_RaxasiaP2_JumpStamp(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_JumpStamp"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_JumpStamp::Free()
{
    __super::Free();
}