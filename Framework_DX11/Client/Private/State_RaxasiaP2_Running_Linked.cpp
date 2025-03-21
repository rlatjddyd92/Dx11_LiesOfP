#include "stdafx.h"
#include "State_RaxasiaP2_Running_Linked.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "AttackObject.h"
#include "Effect_Manager.h"

CState_RaxasiaP2_Running_Linked::CState_RaxasiaP2_Running_Linked(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Running_Linked::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_Running_Linked::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_LINKED_FIRST, false, 0.1f, 0);
    
    m_bRunningWise = *static_cast<_bool*>(pArg);

    m_bSpeedController = false;
    m_bSwingSound = false;
    m_bAccelForSound = false;
    m_bJumpForSound = false;

    m_bFire = false;
    m_bSwing = false;
    m_bEnvelop = false;
    m_bAccel = false;
    m_bJump = false;

    return S_OK;
}

void CState_RaxasiaP2_Running_Linked::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 120)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bEnvelop = false;
            m_bSpeedController = false;
            m_bAccel = false;
            m_bSwingSound = false;
            m_bAccelForSound = false;
            
            //m_pMonster->Change_Animation(AN_LINKED_SECOND, false, 0.2f, 45);
            m_pMonster->SetUp_Animation(AN_LINKED_SECOND, false, 45);
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 4.f, fTimeDelta);
        if (!m_bSpeedController)
        {
            if (CurTrackPos >= 80)
            {
                m_bSpeedController = true;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_LINKED_FIRST, (_double)0.5);
            }
        }
        else
        {
            if (CurTrackPos >= 85)
            {
                m_bSpeedController = false;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_LINKED_FIRST, (_double)1);
            }
        }

        if (CurTrackPos >= 30.f && CurTrackPos <= 75.f)
        {
            _Vec3 vDir = m_pMonster->Get_TargetDir();

            vDir.Normalize();
        
            if (m_pMonster->Calc_Distance_XZ() >= 2.5f)
            {
                m_pMonster->Get_RigidBody()->Add_Velocity(vDir * 2);
            }
            else
            {
                m_pMonster->Get_RigidBody()->Add_Velocity(-(vDir * 2));
            }
        }


        else if (CurTrackPos >= 75.5f && CurTrackPos <= 79.5f)
        {
            _Vec3 vTargetPos = m_pMonster->Get_TargetPos();

            _Vec3 vDir = m_pMonster->Get_TargetDir();
            _Vec3 vUp{ 0, 1, 0 };

            _Vec3 vRight = vUp.Cross(vDir);
            vRight.Normalize();
            _Vec3 vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);

            if (m_bRunningWise)
            {
                vPos += (vRight * m_fAccelSpeed * fTimeDelta);
            }
            else
            {
                vPos -= (vRight * m_fAccelSpeed * fTimeDelta);
            }
            vDir = vTargetPos - vPos;
            vDir.Normalize();

            m_fDistance = m_pMonster->Calc_Distance_XZ();

            vDir *= m_fDistance;
            _Vec3 vTemp{};
            vTemp = vDir;
            vTemp.Normalize();
            if (m_fDistance <= 4.55)
            {
                vTemp *= -15.f;
            }
            else if (m_fDistance >= 4.65)
            {
                vTemp *= 15.f;
            }

            m_pMonster->Get_RigidBody()->Set_GloblePose(vTargetPos - vDir + (vTemp * fTimeDelta));
        }
        break;

    case 1:
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bSwingSound = false;
            m_bAccelForSound = false;
            m_pMonster->Change_Animation(AN_JUMPLIGHTNING, false, 0.1f, 90);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 4.f, fTimeDelta);

        if (!m_bSpeedController)
        {
            if (CurTrackPos >= 70)
            {
                m_bSpeedController = true;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_LINKED_SECOND, (_double)0.5);
            }
        }
        else
        {
            if (CurTrackPos >= 78)
            {
                m_bSpeedController = false;
                m_pMonster->Get_Model()->Set_SpeedRatio(AN_LINKED_SECOND, (_double)1);
            }
        }

        if (CurTrackPos >= 78.f && CurTrackPos <= 84.f)
        {
            _Vec3 vDir = m_pMonster->Get_TargetDir();
            _Vec3 vVelo = m_pMonster->Get_RigidBody()->Get_Velocity();
            vDir.Normalize();

            if (m_pMonster->Calc_Distance_XZ() >= 1.5f)
            {
                m_pMonster->Get_RigidBody()->Set_Velocity(vVelo + vDir * 3);
            }
            else
            {
                m_pMonster->Get_RigidBody()->Set_Velocity(vVelo - (vDir * 3));
            }
        }

        if (CurTrackPos >= 69.5f && CurTrackPos <= 76.f)
        {
            _Vec3 vTargetPos = m_pMonster->Get_TargetPos();

            _Vec3 vDir = m_pMonster->Get_TargetDir();
            _Vec3 vUp{ 0, 1, 0 };

            _Vec3 vRight = vUp.Cross(vDir);
            vRight.Normalize();
            _Vec3 vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);

            if (!m_bRunningWise)
            {
                vPos += (vRight * m_fAccelSpeed * fTimeDelta);
            }
            else
            {
                vPos -= (vRight * m_fAccelSpeed * fTimeDelta);
            }

            vDir = vTargetPos - vPos;
            vDir.Normalize();

            m_fDistance = m_pMonster->Calc_Distance_XZ();

            vDir *= m_fDistance;
            _Vec3 vTemp{};
            if (CurTrackPos >= 5.f)
            {
                vTemp = vDir;
                vTemp.Normalize();
                if (m_fDistance <= 4.55)
                {
                    vTemp *= -15.f;
                }
                else if (m_fDistance >= 4.65)
                {
                    vTemp *= 15.f;
                }
            }

            m_pMonster->Get_RigidBody()->Set_GloblePose(vTargetPos - vDir + (vTemp * fTimeDelta));
        }
        break;

    case 2:
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

void CState_RaxasiaP2_Running_Linked::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
    m_pMonster->DeActive_CurrentWeaponCollider(1);

    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL);
}

_bool CState_RaxasiaP2_Running_Linked::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_LINKED_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_FIRST);
        }
        break;

    case 1:
        if ((AN_LINKED_SECOND) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_LINKED_SECOND);
        }
        break;

    case 2:
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

void CState_RaxasiaP2_Running_Linked::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 100.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
            m_pMonster->Active_Debuff(0, 0.1f);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
            m_pMonster->DeActive_Debuff(0);
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 80.f && CurTrackPos <= 90.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.6f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
            m_pMonster->Active_Debuff(0, 0.1f);
        }
        else
        {
            m_pMonster->DeActive_CurrentWeaponCollider(0);
            m_pMonster->DeActive_Debuff(0);
        }
    }
    
}

void CState_RaxasiaP2_Running_Linked::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
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

        if ((CurTrackPos >= 75.f && CurTrackPos <= 79.5f))
        {
            if (!m_bAccel)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
                m_bAccel = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
        }

    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 80.f && CurTrackPos <= 90.f))
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

        if ((CurTrackPos >= 69.f && CurTrackPos <= 76.f))
        {
            if (!m_bAccel)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL, true);
                m_bAccel = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
        }
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
            if (CurTrackPos >= 180.f)
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

        if (CurTrackPos >= 230.f)
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_BIG);
        }
    }
}

void CState_RaxasiaP2_Running_Linked::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 100.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }

        }

        if ((CurTrackPos >= 75.f && CurTrackPos <= 79.5f))
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

        if ((CurTrackPos >= 69.f && CurTrackPos <= 76.f))
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
    else
    {
        if (!m_bJumpForSound)
        {
            if (CurTrackPos >= 100.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_FX_Jump_Heavy_01.wav"), false);
               
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

CState_RaxasiaP2_Running_Linked* CState_RaxasiaP2_Running_Linked::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Running_Linked* pInstance = new CState_RaxasiaP2_Running_Linked(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Running_Linked"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Running_Linked::Free()
{
    __super::Free();
}