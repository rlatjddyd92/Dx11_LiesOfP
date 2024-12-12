#include "stdafx.h"
#include "State_RaxasiaP2_StepJump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_StepJump::CState_RaxasiaP2_StepJump(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_StepJump::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_StepJump::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_JUMPSTAMP_START, false, 0.1f, 0);

    m_bSwingSound = false;
    m_bStartSpot = true;
    m_vFlyMoveStack = _Vec3{};
    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP2_StepJump::Update(_float fTimeDelta)
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

        m_vTargetDir = m_pMonster->Get_TargetDir();
        m_vTargetDir.Normalize();

        if (CurTrackPos >= 87.f && CurTrackPos <= 145.f)
        {
            if (m_bStartSpot)
            {
                m_fDist = m_pMonster->Calc_Distance_XZ();
                m_bStartSpot = false;
            }

            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 1.f, fTimeDelta);

            _float fYMove = m_pMonster->Get_RigidBody()->Get_Velocity().y;

            _Vec3 vMove = m_vTargetDir * m_fDist * (((_float)CurTrackPos - 87.f) / 85.f);
            vMove.y = fYMove;
            m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vFlyMoveStack) / fTimeDelta);
            m_vFlyMoveStack = vMove;
        }
        else if (CurTrackPos >= 145.f && CurTrackPos <= 160.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 3.f, fTimeDelta);
        }

        break;

    case 1:
        //림라이트
        if (End_Check())
        {
            m_iRouteTrack = 0;
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
        
        if (CurTrackPos >= 115.f && CurTrackPos <= 145.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 3.f, fTimeDelta);
        }

        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_StepJump::End_State()
{
}

_bool CState_RaxasiaP2_StepJump::End_Check()
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


    default:
        break;
    }

    return  bEndCheck;
}

void CState_RaxasiaP2_StepJump::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 105.f) ||
            (CurTrackPos >= 110.f && CurTrackPos <= 130.f) ||
            (CurTrackPos >= 200.f && CurTrackPos <= 206.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.6f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_STRONG);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP2_StepJump::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {

        if (CurTrackPos >= 220.f)
        {
            //검에서 전기 폭발
        }
    }
    else
    {
        if (CurTrackPos >= 20.f)
        {
            //부채꼴 전기 방출
        }

    }
}

void CState_RaxasiaP2_StepJump::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP2_StepJump* CState_RaxasiaP2_StepJump::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_StepJump* pInstance = new CState_RaxasiaP2_StepJump(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_StepJump"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_StepJump::Free()
{
    __super::Free();
}