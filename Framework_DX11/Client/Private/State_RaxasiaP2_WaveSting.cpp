#include "stdafx.h"
#include "State_RaxasiaP2_WaveSting.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "AttackObject.h"

CState_RaxasiaP2_WaveSting::CState_RaxasiaP2_WaveSting(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_WaveSting::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_WaveSting::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_CHARGE, false, 0.1f, 50);
    m_bResetRim = false;
    m_bControlRim = false;

    m_fGoalRimAlpha = 0.1f;
    m_fCurtRimAlpha = 1.f;

    m_bSwingSound = false;
    m_bAccelForSound = false;

    m_bSwing = false;
    m_bWave = false;
    m_bEnvelop = false;
    m_bAccel = false;

    return S_OK;
}

void CState_RaxasiaP2_WaveSting::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_WAVE, false, 0.02f, 50);
            return;
        }
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);

        break;

    case 1:
        if (CurTrackPos >= 150.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_RUN, true, 0.02f, 0);
            m_pMonster->Get_Model()->Set_SpeedRatio(AN_RUN, (double)1.5);
            return;
        }
        if (CurTrackPos <= 60.f ||
            (CurTrackPos >= 95.f && CurTrackPos <= 105))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }

        break;

    case 2:
    {
        _float fDist = m_pMonster->Calc_Distance_XZ();
        if (fDist <= 4.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bControlRim = true;
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL);
            m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT1);
            m_pMonster->Change_Animation(AN_STING, false, 0.02f, 50);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);

        break;
    }

    case 3:
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
        
        if (CurTrackPos <= 155.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        }

        if (!m_bResetRim)
        {
            if (CurTrackPos >= 180.f)
            {
                m_fGoalRimAlpha = 1.f;
                m_bResetRim = true;
            }
        }

        break;
    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

    if (m_bControlRim)
    {
        Update_Rimlight();
    }

}

void CState_RaxasiaP2_WaveSting::End_State()
{
    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2);
    m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_VOICE);
}

_bool CState_RaxasiaP2_WaveSting::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_CHARGE) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_CHARGE);
        }
        break;

    case 3:
        if ((AN_STING) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_STING);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_WaveSting::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 60.f && CurTrackPos <= 90.f)||
            (CurTrackPos >= 110.f && CurTrackPos <= 135.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if (m_iRouteTrack == 3)
    {
        if ((CurTrackPos >= 160.f && CurTrackPos <= 170.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.6f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_STRONG);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP2_WaveSting::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bCharge)
        {
            if (CurTrackPos >= 35.f)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2, true);
                m_bCharge = true;
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if (!m_bWave)
        {
            if ((CurTrackPos >= 75.f && CurTrackPos <= 80.f) ||
                (CurTrackPos >= 110.f && CurTrackPos <= 115.f))
            {
                //웨이브 공격 생성
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, -1.75f };
                XMStoreFloat4x4(&WorldMat,
                    (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_Model()->Get_UFBIndices(UFB_WEAPON))
                        * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
                vPos.y = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;
                //어택오브젝트 생성 마크 후 폭발

                CAttackObject::ATKOBJ_DESC Desc;

                _Vec3 vTargetDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);

                Desc.vPos = vPos;
                Desc.vDir = vTargetDir;

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderWave"), &Desc);

            }
        }
        else
        {
            if (CurTrackPos >= 95.f && CurTrackPos <= 105.f)
            {
                m_bWave = false;
            }

        }
    }
    else if (m_iRouteTrack == 2)
    {
        if (!m_bEnvelop)
        {
            if (CurTrackPos >= 15.f)
            {
                m_bEnvelop = true;
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Electric_Loop_02.wav"), true);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL, true);
            }
        }

        if (CurTrackPos >= 19.f && CurTrackPos <= 21.f)
        {
            if (!m_bAccel)
            {
                m_bAccel = true;
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
        }
    }
}

void CState_RaxasiaP2_WaveSting::Update_Rimlight()
{
    if (m_fCurtRimAlpha != m_fGoalRimAlpha)
    {
        m_fCurtRimAlpha += (m_fGoalRimAlpha - m_fCurtRimAlpha) / 15;
        m_pMonster->Set_RimLightColor(_Vec4{ 0.9f, 0.f, 0.f, m_fCurtRimAlpha });
        if (abs(m_fGoalRimAlpha - m_fCurtRimAlpha) < 0.1f)
        {
            m_fCurtRimAlpha = m_fGoalRimAlpha;
            if (m_fGoalRimAlpha == 1.f)
            {
                m_pMonster->Set_RimLightColor(_Vec4{ 0.f, 0.f, 0.f, 1.f });
            }
        }
    }
}

void CState_RaxasiaP2_WaveSting::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bCharge)
        {
            if (CurTrackPos >= 35.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_VOICE, TEXT("SE_NPC_Raxasia_SK_WP_Sword_Lightning_Loop.wav"), true);
                m_bCharge = true;
            }
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if (CurTrackPos >= 19.f && CurTrackPos <= 21.f)
        {
            if (!m_bAccelForSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_FX_Blink_Spark_Foot_03.wav"), true);
                m_bAccelForSound = true;
            }
        }
        else
        {
            m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT2);
        }
    }
}

CState_RaxasiaP2_WaveSting* CState_RaxasiaP2_WaveSting::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_WaveSting* pInstance = new CState_RaxasiaP2_WaveSting(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_WaveSting"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_WaveSting::Free()
{
    __super::Free();
}