#include "stdafx.h"
#include "State_CarcassTail_LeapToAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

CState_CarcassTail_LeapToAttack::CState_CarcassTail_LeapToAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassTail_LeapToAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    m_fIdleDuration = 1.f;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_CarcassTail_LeapToAttack::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    m_fIdleTime = m_fIdleDuration;

    m_bHeadingSound = false;
    m_bSwingSound = false;

    return S_OK;
}

void CState_CarcassTail_LeapToAttack::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        if (m_iRouteTrack == 0)
        {
            //앞뒤 판단해서 꼬리 or 손으로 공격

            _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
            _Vec3 vLook = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK));
            _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());
            _Vec3 vTargetRight = vUp.Cross(vTargetDir);


            _Vec3 vCrossUp = vLook.Cross(vTargetRight);

            if (vUp.Dot(vCrossUp) >= 0)
            {
                if (rand() % 2 == 0)
                {
                    m_iLastAnim = AN_ROUTE_LAST_HAND_R;
                }
                else
                {
                    m_iLastAnim = AN_ROUTE_LAST_HAND_L;
                }

            }
            else
            {
                if (rand() % 2 == 0)
                {
                    m_iLastAnim = AN_ROUTE_LAST_TAIL_R;
                }
                else
                {
                    m_iLastAnim = AN_ROUTE_LAST_TAIL_L;
                }
            }

            m_pMonster->Change_Animation(m_iLastAnim, false, 0.1f, 0);
        }

        if (m_iRouteTrack >= 1)
        {
            m_pMonster->Change_State(CCarcassBigA::IDLE);
            return;
        }
        ++m_iRouteTrack;
    }

    if (m_iLastAnim == AN_ROUTE_LAST_HAND_L || m_iLastAnim == AN_ROUTE_LAST_HAND_R)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.5f, fTimeDelta);
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

}

void CState_CarcassTail_LeapToAttack::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_CarcassTail_LeapToAttack::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_ROUTE_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_FIRST);
        }
        break;

    case 1:
        if ((m_iLastAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iLastAnim);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_CarcassTail_LeapToAttack::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (CurTrackPos >= 50.f && CurTrackPos <= 70.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 3, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(3);
        }
    }
    else
    {
        if (CurTrackPos >= 90.f && CurTrackPos <= 105.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1.1f, 1, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
}

void CState_CarcassTail_LeapToAttack::Sound_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bHeadingSound)
        {
            if (CurTrackPos >= 50.f && CurTrackPos <= 70.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Heavy_03.wav"), false);
                m_bHeadingSound = true;
            }
        }
    }
    else
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 90.f && CurTrackPos <= 105.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_CarcassBear_SK_WS_Claw_06.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
}

CState_CarcassTail_LeapToAttack* CState_CarcassTail_LeapToAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassTail_LeapToAttack* pInstance = new CState_CarcassTail_LeapToAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_CarcassTail_LeapToAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassTail_LeapToAttack::Free()
{
    __super::Free();
}