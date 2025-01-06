#include "stdafx.h"
#include "State_RaxasiaP2_Tele_JumpLightning.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "AttackObject.h"
#include "Effect_Manager.h"

CState_RaxasiaP2_Tele_JumpLightning::CState_RaxasiaP2_Tele_JumpLightning(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Tele_JumpLightning::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_Tele_JumpLightning::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_LINKED_FIRST, false, 0.1f, 0);

    m_bSwingSound = false;
    m_bJumpForSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP2_Tele_JumpLightning::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 95.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            //m_pMonster->Change_Animation(AN_LINKED_SECOND, false, 0.5f, 55);

            m_pMonster->SetUp_Animation(AN_LINKED_SECOND, false, 55);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3.f, fTimeDelta);
        break;


    case 1:
        if (CurTrackPos >= 120.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            //m_pMonster->Change_Animation(AN_LINKED_FIRST, false, 0.5f, 50);

            m_pMonster->SetUp_Animation(AN_LINKED_FIRST, false, 50);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3.f, fTimeDelta);
        break;

    case 2:
        if (CurTrackPos >= 130.f)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_JUMPLIGHTNING, false, 0.5f, 0);

            //m_pMonster->SetUp_Animation(AN_JUMPLIGHTNING, false, 55);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3.f, fTimeDelta);
        break;

    case 3:
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2.f, fTimeDelta);
        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_Tele_JumpLightning::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
}

_bool CState_RaxasiaP2_Tele_JumpLightning::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_LINKED_SECOND) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_SECOND);
        }
        break;

    case 1:
        if ((AN_LINKED_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_FIRST);
        }
        break;

    case 2:
        if ((AN_LINKED_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_FIRST);
        }
        break;

    case 3:
        if ((AN_JUMPLIGHTNING) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_JUMPLIGHTNING);
        }
        break;

    default:
        break;
    }

    return  bEndCheck;
}

void CState_RaxasiaP2_Tele_JumpLightning::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0 || m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 80.f && CurTrackPos <= 90.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 100.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
        }
    }
}

void CState_RaxasiaP2_Tele_JumpLightning::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0 || m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 100.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 80.f && CurTrackPos <= 90.f))
        {
            m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }
        //Àü±â ÂªÅÚ ÀÌÆåÆ®
    }
    else
    {
        if (!m_bJump)
        {
            if (CurTrackPos >= 100.f)
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, 0.f };
                XMStoreFloat4x4(&WorldMat, m_pMonster->Get_Transform()->Get_WorldMatrix());
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Jump"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });

                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD, true);
                m_bJump = true;
            }
        }

        if (!m_bFire)
        {
            if (CurTrackPos >= 183.f)
            {
                m_bFire = true;

                CAttackObject::ATKOBJ_DESC Desc{};
                
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, 0.f };
                XMStoreFloat4x4(&WorldMat,
                    (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_Model()->Get_UFBIndices(UFB_WEAPON))
                        * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
                
                Desc.vPos = vPos;

                Desc.vDir = _Vec3{ m_pMonster->Get_TargetPos() - vPos };
                Desc.vDir.Normalize();

                Desc.vTargetPos = _Vec3{ m_pMonster->Get_TargetPos() };
                Desc.pOwner = m_pMonster;

                Desc.iTrack_State = 1;

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderBolt"), &Desc);
            }

        }

    }
}

void CState_RaxasiaP2_Tele_JumpLightning::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0 || m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 100.f))
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
        if ((CurTrackPos >= 80.f && CurTrackPos <= 90.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }
        //Àü±â ÂªÅÚ ÀÌÆåÆ®
    }
    else
    {
        if (!m_bJumpForSound)
        {
            if (CurTrackPos >= 100.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_FX_Jump_Heavy_01.wav"), false);

                m_bJumpForSound = true;
            }
        }

        if (!m_bSwingSound)
        {
            if ((CurTrackPos >= 193.f))
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }

        }

    }
}

CState_RaxasiaP2_Tele_JumpLightning* CState_RaxasiaP2_Tele_JumpLightning::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Tele_JumpLightning* pInstance = new CState_RaxasiaP2_Tele_JumpLightning(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Tele_JumpLightning"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Tele_JumpLightning::Free()
{
    __super::Free();
}