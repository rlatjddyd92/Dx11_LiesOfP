#include "stdafx.h"
#include "State_RaxasiaP2_Declare_War.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "Effect_Manager.h"
#include "AttackObject.h"

CState_RaxasiaP2_Declare_War::CState_RaxasiaP2_Declare_War(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Declare_War::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP2_Declare_War::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_HOWLLING, false, 0.1f, 50);

    m_bStartSpot = true;
    m_vFlyMoveStack = _Vec3{};
    m_bSwingSound = false;
    m_bSwing = false;
    m_bJump = false;
    m_bStart = false;
    m_bEnvelop = false;
    m_bDive = false;
    m_bLanding = false;
    m_bHovering = false;
    m_bEndFire = false;

    m_bJumpForSound = false;
    m_bStartForSound = false;
    m_bDiveForSound = false;
    m_bEnvelopForSound = false;

    m_iThunderCnt = 0;
    m_fTimeStack_ThunderBolt = 0.f;
    m_fTimeStack_Lightning = 0.f;
    m_fHoveringTimeStack = 0.f;

    m_vFogSpot = {};
    m_fHeight = {};

    return S_OK;
}

void CState_RaxasiaP2_Declare_War::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }
            ++m_iRouteTrack;
            m_bSwing = false;
            m_bStart = false;

            m_bJumpForSound = false;
            m_bStartForSound = false;
            m_bDiveForSound = false;
            m_bEnvelopForSound = false;

            m_pMonster->Change_Animation(AN_DECLARE_WAR, false, 0.1f, 0);
            return;
        }
        
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        
        break;

    case 1:
    {
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        m_vTargetDir = m_pMonster->Get_TargetDir();
        m_vTargetDir.Normalize();

        if (CurTrackPos >= 295.f && CurTrackPos <= 315.f)
        {
        
            if (m_bStartSpot)
            {
                m_fDist = m_pMonster->Calc_Distance_XZ();
                if (m_fDist < 4.f)
                {
                    m_fDist = 4.f;
                }
                m_bStartSpot = false;
            }
        
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 1.f, fTimeDelta);
        
            _float fYMove = m_pMonster->Get_RigidBody()->Get_Velocity().y;
        
            _Vec3 vMove = m_vTargetDir * m_fDist * (((_float)CurTrackPos - 295.f) / 20.f);
            vMove.y = fYMove;
         
            m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vFlyMoveStack) / fTimeDelta);
            m_vFlyMoveStack = vMove;
        }
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        break;
    }
        
        

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos, fTimeDelta);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_Declare_War::End_State()
{
    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD);
    m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_VOICE);
}

_bool CState_RaxasiaP2_Declare_War::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_HOWLLING) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_HOWLLING);
        }
        break;

    case 1:
        if ((AN_DECLARE_WAR) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_DECLARE_WAR);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_Declare_War::Collider_Check(_double CurTrackPos)
{
}

void CState_RaxasiaP2_Declare_War::Effect_Check(_double CurTrackPos, _float fTimeDelta)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bStart)
        {
            if (CurTrackPos >= 50.f)
            {
                m_bStart = true;
                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2, true);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_HOWLING, true);
            }
        }
        else
        {
            if (CurTrackPos >= 300.f)
            {
                m_pMonster->DeActive_Effect(CRaxasia::EFFECT_HOWLING);
            }
        }
    }
    else
    {
        if (!m_bStart)
        {
            m_fHeight = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;
            m_bStart = true;
            m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERCHARGE_GROUND, true);
        }

        if (!m_bJump)
        {
            if (CurTrackPos >= 68.f)
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, 0.f };
                XMStoreFloat4x4(&WorldMat, m_pMonster->Get_Transform()->Get_WorldMatrix());
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_SuperJump"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });

                vPos.y += 15.f;

                m_vFogSpot = vPos;
                m_vFogSpot.y -= 2.f;

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_Fog"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });

                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD, true);
                m_bJump = true;
                m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERCHARGE_GROUND);
            }
        }
        else if (!m_bDive)
        {
            if (m_fLightningTime <= m_fTimeStack_Lightning)
            {
                m_fTimeStack_Lightning -= m_fLightningTime;

                CAttackObject::ATKOBJ_DESC Desc;

                Desc.vPos = (_Vec3)m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
                Desc.vPos.y = m_fHeight;

                _float fvariableX = m_pGameInstance->Get_Random(0.f, 7.f) - 3.5f;
                _float fvariableZ = m_pGameInstance->Get_Random(0.f, 7.f) - 3.5f;
                
                Desc.vPos.x += fvariableX;
                Desc.vPos.z += fvariableZ;

                Desc.pOwner = m_pMonster;
                //vPos로 드래그 마크
                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderMark"), &Desc);

            }
            else
            {
                m_fTimeStack_Lightning += fTimeDelta;
            }
        }

        if (!m_bEndFire)
        {

            if (!m_bHovering)
            {
                if (CurTrackPos >= 170.f && CurTrackPos <= 250.f)
                {
                    m_bHovering = true;
                }
            }
            else
            {
                if (m_fThunderBoltTime <= m_fTimeStack_ThunderBolt)
                {
                    m_fTimeStack_ThunderBolt -= m_fThunderBoltTime;
                    //썬더볼트 생성

                    _float4x4 WorldMat{};
                    _Vec3 vPos = m_vFogSpot;

                    _Vec3 vTargetDir = m_pMonster->Get_TargetDir();
                    vTargetDir.Normalize();

                    _Vec3 vRight = vTargetDir.Cross(_Vec3{ 0.f, 1.f, 0.f });

                    _float fvariableX = m_pGameInstance->Get_Random(0.f, 7.f) - 3.5f;
                    _float fvariableZ = m_pGameInstance->Get_Random(0.f, 2.f) - 1.f;

                    vPos += vRight * fvariableX;
                    vPos += vTargetDir * fvariableZ;
                    vPos += _Vec3{ 0.f, -0.5f, 0.f } *fvariableZ;


                    CAttackObject::ATKOBJ_DESC Desc{};
                    Desc.vPos = vPos;

                    fvariableX = m_pGameInstance->Get_Random(0.f, 2.f) - 1.f;
                    _float fvariableY = m_pGameInstance->Get_Random(0.f, 2.f) - 1.f;

                    _Vec3 vTargetPos = m_pMonster->Get_TargetPos();
                    _Vec3 vDir = vTargetPos - m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
                    vDir.Normalize();

                    _Vec3 vUp = { 0.f, 1.f, 0.f };
                    vRight = vUp.Cross(vDir);
                    vUp = vDir.Cross(vRight);

                    vDir += (vUp * fvariableY);
                    vDir.Normalize();
                    vDir += (vRight * fvariableX);
                    vDir.Normalize();

                    Desc.vDir = vDir;
                    Desc.vDir.Normalize();

                    Desc.vTargetPos = _Vec3{ m_pMonster->Get_TargetPos() };
                    Desc.pOwner = m_pMonster;

                    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderBolt"), &Desc);

                    ++m_iThunderCnt;
                }
                else
                {
                    m_fTimeStack_ThunderBolt += fTimeDelta;
                }

                if (m_iThunderCnt < 6)
                {
                    if (CurTrackPos >= 250.f)
                    {
                        m_pMonster->Get_Model()->Set_CurrentTrackPosition((_double)170.f);
                        m_pMonster->Get_Model()->Set_CurrentTrackPosition_Boundary((_double)170.f);
                    }
                }
                else
                {
                    m_bEndFire = true;
                }
            }
        }
        else if (m_bHovering)
        {
            if (m_fHoveringTimeStack >= m_fHoveringDuration)
            {
                m_bHovering = false;
            }
            else
            {
                if (CurTrackPos >= 250.f)
                {
                    m_pMonster->Get_Model()->Set_CurrentTrackPosition((_double)170.f);
                    m_pMonster->Get_Model()->Set_CurrentTrackPosition_Boundary((_double)170.f);
                }
                m_fHoveringTimeStack += fTimeDelta;
            }
        }


        if (!m_bEnvelop)
        {
            if (CurTrackPos >= 150.f)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERENVELOP_BIG, true);
                m_bEnvelop = true;
            }
        }

        if (!m_bDive)
        {
            if (CurTrackPos >= 255.f)
            {
                m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_BIG);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL);
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERACCEL);
                m_bDive = true;
            }
        }
        else
        {
            if (!m_bLanding)
            {
                if (CurTrackPos >= 320.f)
                {
                    _float4x4 WorldMat{};
                    _Vec3 vPos = { 0.f, 0.f, 0.f };
                    XMStoreFloat4x4(&WorldMat, m_pMonster->Get_Transform()->Get_WorldMatrix());
                    vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                    CAttackObject::ATKOBJ_DESC Desc{};
                    Desc.vPos = vPos;
                    Desc.vDir = _Vec3{ m_pMonster->Get_TargetDir() };
                    Desc.pOwner = m_pMonster;
                    Desc.vTargetPos = _Vec3{ m_pMonster->Get_TargetPos() };

                    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderLanding"), &Desc);

                    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL);
                    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERACCEL);
                    m_bLanding = true;
                }
            }
        }

    }

}

void CState_RaxasiaP2_Declare_War::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bStartForSound)
        {
            if (CurTrackPos >= 50.f)
            {
                m_bStartForSound = true;
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_VOICE, TEXT("SE_NPC_Raxasia_SK_WP_Sword_Lightning_Loop.wav"), true);
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("VO_NPC_Raxasia_P2_Roar_05.wav"), false);   //하울링
            }
        }
    }
    else
    {
        if (!m_bStartForSound)
        {
            m_bStartForSound = true;
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_FX_Spark_Charge_01.wav"), true);
        }

        if (!m_bJumpForSound)
        {
            if (CurTrackPos >= 68.f)
            {
                m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT1);

                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_FX_Spark_Jump_01.wav"), false);

                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_FX_Cloud_01.wav"), false);

                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_VOICE, TEXT("SE_NPC_Raxasia_SK_WP_Sword_Lightning_Loop.wav"), true);
                m_bJumpForSound = true;
            }
        }

        if (!m_bEnvelopForSound)
        {
            if (CurTrackPos >= 150.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Electric_Loop_02.wav"), true);
                m_bEnvelopForSound = true;
            }
        }

        if (!m_bDiveForSound)
        {
            if (CurTrackPos >= 255.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Electric_Loop_02.wav"), true);
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_FX_Blink_Spark_Foot_03.wav"), true);

                m_bDiveForSound = true;
            }
        }
        else
        {
            if (CurTrackPos >= 320.f)
            {
                m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT1);
                m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT2);
            }
        }

    }
}

CState_RaxasiaP2_Declare_War* CState_RaxasiaP2_Declare_War::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Declare_War* pInstance = new CState_RaxasiaP2_Declare_War(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Declare_War"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Declare_War::Free()
{
    __super::Free();
}