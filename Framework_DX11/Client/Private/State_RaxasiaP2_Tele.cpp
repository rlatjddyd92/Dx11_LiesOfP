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
    m_pMonster->Change_Animation(AN_RUN, true, 0.1f, 0);

    m_iAtkCnt = 0;
    m_bSpeedController = false;

    m_bSwingSound = false;
    m_bAccelSound = false;
    m_bEnvelopSound = false;

    m_bTele = false;
    m_bSwing = false;
    m_bEnvelop = false;
    m_bTeleEffect = false;

    return S_OK;
}

void CState_RaxasiaP2_Tele::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (!m_bSpeedController)
        {
            if (CurTrackPos >= 10.f && CurTrackPos <= 14.f)
            {
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_RUN, (double)0.2f);
                m_bSpeedController = true;
            }
        }
        else
        {
            if (CurTrackPos >= 15.f)
            {
                if (!m_bTele)
                {
                    m_bTele = true;

                    _Vec3 vDir = m_pMonster->Get_TargetDir();
                    _Vec3 vPos = m_pMonster->Get_TargetPos();
                    vDir.Normalize();

                    m_pMonster->Get_RigidBody()->Set_GloblePose(vPos - (vDir * 2));
                }
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_RUN, (double)1.f);
                m_bSpeedController = false;
            }
        }

        if (CurTrackPos >= 19.f)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_UPPERSLASH, false, 0.1f, 0);
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        break;

    case 1:
        if (End_Check())
        {
            if (m_iAtkCnt == 0)
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
        if (CurTrackPos <= 15.f)
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

void CState_RaxasiaP2_Tele::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);

    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL);
    m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_VOICE);
}

_bool CState_RaxasiaP2_Tele::End_Check()
{
    return  m_pMonster->Get_EndAnim(AN_UPPERSLASH);;
}

void CState_RaxasiaP2_Tele::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 15.f && CurTrackPos <= 40.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
            m_pMonster->Active_Debuff(0, 0.3f);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
            m_pMonster->DeActive_Debuff(0);
        }
    }
}

void CState_RaxasiaP2_Tele::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
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
    else
    {
        if (!m_bEnvelop)
        {
            if (CurTrackPos >= 14.f)
            {
                m_bEnvelop = false;
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL, true);
            }
        }

        if (!m_bTeleEffect)
        {
            if (CurTrackPos >= 14.f)
            {
                m_bTeleEffect = false;
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
            }
        }
        if (CurTrackPos >= 15.5f)
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
        }
    }
}

void CState_RaxasiaP2_Tele::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 15.f && CurTrackPos <= 40.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }
        else
        {
            m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT1);
        }
    }
    else
    {
        if (!m_bEnvelopSound)
        {
            if (CurTrackPos >= 14.f)
            {
                m_bEnvelopSound = false;
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_VOICE, TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Electric_Loop_02.wav"), true);
            }
        }

        if (!m_bAccelSound)
        {
            if (CurTrackPos >= 14.f)
            {
                m_bAccelSound = true;
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_FX_Blink_Spark_Foot_03.wav"), true);
            }
        }
        if (CurTrackPos >= 15.5f)
        {
            m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT2);
        }
    }
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