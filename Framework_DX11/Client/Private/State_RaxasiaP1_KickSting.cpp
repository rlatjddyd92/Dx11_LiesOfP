#include "stdafx.h"
#include "State_RaxasiaP1_KickSting.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "Effect_Manager.h"
#include "AttackObject.h"

CState_RaxasiaP1_KickSting::CState_RaxasiaP1_KickSting(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_KickSting::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_KickSting::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_KICKSTING, false, 0.1f, 0);

    m_bSwingSound = false;
    m_bStampSound = false;
    m_bSwing = false;
    m_bStampBlast = false;

    return S_OK;
}

void CState_RaxasiaP1_KickSting::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 170.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_JUMPATTACK, false, 0.1f, 0);
            return;
        }

        if (CurTrackPos <= 25.f ||
            (CurTrackPos <= 35.f && CurTrackPos <= 70.f) ||
            CurTrackPos >= 120.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    case 1:
        if (End_Check())
        {
            m_iRouteTrack = 0;
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        if (CurTrackPos <= 40.f ||
            (CurTrackPos <= 80.f && CurTrackPos <= 100.f))
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

void CState_RaxasiaP1_KickSting::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
    m_pMonster->DeActive_CurrentWeaponCollider(1);

    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD);
    m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT2);
}

_bool CState_RaxasiaP1_KickSting::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_JUMPATTACK);;
}

void CState_RaxasiaP1_KickSting::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 30.f && CurTrackPos <= 35.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(1);
        }

        if ((CurTrackPos >= 80.f && CurTrackPos <= 85.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 55.f && CurTrackPos <= 65.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
}

void CState_RaxasiaP1_KickSting::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if (!m_bChargeActive)
        {
            if (CurTrackPos >= 40.f)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD, true);
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_WP_Sword_Lightning_Loop.wav"), false);
                m_bChargeActive = true;
            }
        }

        if (!m_bStampBlast)
        {
            if (CurTrackPos >= 65.f)
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, -1.75f };
                XMStoreFloat4x4(&WorldMat,
                    (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_Model()->Get_UFBIndices(UFB_WEAPON))
                        * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
                vPos.y = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_ThunderStamp_Small"),
                    vPos, _Vec3{ 0.f, 0.f, 1.f });

                //어택오브젝트 생성 마크 후 폭발

                CAttackObject::ATKOBJ_DESC Desc;

                _Vec3 vTargetDir = m_pMonster->Get_TargetPos() - vPos;

                Desc.vPos = vPos;
                Desc.vDir = vTargetDir;
                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderStampMark"), &Desc);

                m_bStampBlast = true;
            }
        }
    }
}

void CState_RaxasiaP1_KickSting::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 80.f && CurTrackPos <= 85.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if (!m_bStampSound)
        {
            if ((CurTrackPos >= 48.f))
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"));

                m_bStampSound = true;
            }
        }
    }
}

CState_RaxasiaP1_KickSting* CState_RaxasiaP1_KickSting::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_KickSting* pInstance = new CState_RaxasiaP1_KickSting(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_KickSting"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_KickSting::Free()
{
    __super::Free();
}