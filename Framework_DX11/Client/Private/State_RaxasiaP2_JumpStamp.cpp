#include "stdafx.h"
#include "State_RaxasiaP2_JumpStamp.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "Effect_Manager.h"
#include "AttackObject.h"

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
    m_fMoveHeight = 0.f;
    m_fCorHeightForLand = 0.f;
    m_pMonster->Get_Model()->ReadyDenyNextTranslate(4);
    m_pMonster->SetUp_Animation(AN_INCHENT, false, 0);

    m_bSwingSound = { false };
    m_bInchentSound = { false };
    m_bEnvelopSound = { false };
    m_bStompSound = { false };

    m_bStartSpot = true;
    m_bSwing = false;
    m_bStartHeightCor = false;

    m_vTargetDir = _Vec4{};
    m_vRootMoveStack = _Vec3{};
    m_vFlyMoveStack = _Vec3{};
    m_vCurVelocity = _Vec3{};
    
    m_bStomp = false;
    m_bAccel = false;
    m_bEnvelop = false;
    m_bInchent = false;
    m_bOnMark = false;
    m_bNaviLockCheck = false;

    m_pMonster->Get_RigidBody()->Set_IsOnCell(false);
    m_pMonster->Get_RigidBody()->Set_IsLockCell(false);

    return S_OK;
}

void CState_RaxasiaP2_JumpStamp::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (!(m_iRouteTrack == 3 && CurTrackPos >= 15.f))
    {

        _Vec3 vMove = m_pMonster->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(5)->Translation();
        _float4x4 TransMat;
        XMStoreFloat4x4(&TransMat, m_pMonster->Get_Model()->Get_Bones()[4]->Get_TransformationMatrix());
        TransMat._41 = 0.f;
        TransMat._42 = 0.f;
        TransMat._43 = 0.f;

        m_pMonster->Get_Model()->Get_Bones()[4]->Set_TransformationMatrix(TransMat);;

        m_pMonster->Get_Model()->Update_Bone();

        vMove = XMVector3TransformNormal(vMove, m_pMonster->Get_Transform()->Get_WorldMatrix());
        m_fCorHeightForLand = vMove.y;
        _Vec3 vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);

        m_vCurVelocity = (vMove - m_vRootMoveStack) / fTimeDelta;

        m_pMonster->Get_RigidBody()->Set_Velocity(m_vCurVelocity);

        m_vRootMoveStack = vMove;
    }

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
            m_pMonster->Get_Model()->ReadyDenyNextTranslate(4);
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_JUMPSTAMP_START, false, 0.1f, 0);
            return;
        }

        break;
    case 1:
    {
        if (End_Check())
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }
            m_pMonster->Get_Model()->ReadyDenyNextTranslate(4);
            m_vRootMoveStack = _Vec3{0.151717f, 2.042145f, 0.f};
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_JUMPSTAMP_MIDDLE, false, 0.1f, 0);
            return;
        }
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 0.5f, fTimeDelta);

        break;
    }
        

    case 2:
    {
        if (CurTrackPos >= 20.f)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }
            m_pMonster->Get_Model()->ReadyDenyNextTranslate(4);
            ++m_iRouteTrack;

            m_fMoveHeight = m_vTargetDir.y - m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;

            m_bSwing = false;
            m_pMonster->Change_Animation(AN_JUMPSTAMP_END, false, 0.1f, 0);
            return;
        }
        if (m_bStartSpot)
        {
            if (CurTrackPos >= 15.f)
            {
                m_bStartSpot = false;

                m_vTargetDir = m_pMonster->Get_TargetDir();
            }
        }
        if (CurTrackPos <= 15.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 0.5f, fTimeDelta);
        }


        break;
    }
        

    case 3:
        if (End_Check())
        {
            m_pMonster->Get_Model()->ReadyDenyNextTranslate(4);
            m_iRouteTrack = 0;
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
        if (CurTrackPos <= 15.f)
        {
            _Vec3 vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
            _Vec3 vTargetPos = m_pMonster->Get_TargetPos();
            if (vPos.y <= vTargetPos.y + m_fCorHeightForLand * 2)
            {
                vPos.y = vTargetPos.y + m_fCorHeightForLand * 2;
                m_pMonster->Get_RigidBody()->Set_GloblePose(vPos);
                m_pMonster->Get_RigidBody()->Set_Velocity(_Vec3{});
            }
            else
            {
                _Vec3 vMove = m_vTargetDir * ((_float)CurTrackPos / 15.f);
                _float fMoveY = m_fMoveHeight * ((_float)CurTrackPos / 15.f);
                vMove.y = fMoveY;
                m_pMonster->Get_RigidBody()->Set_Velocity(m_vCurVelocity + (vMove - m_vFlyMoveStack) / fTimeDelta);
                m_vFlyMoveStack = vMove;
            }
            
        }
        else if(!m_bNaviLockCheck)
        {
            m_bNaviLockCheck = true;
            m_pMonster->Get_RigidBody()->Set_IsOnCell(true);
            m_pMonster->Get_RigidBody()->Set_IsLockCell(true);
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
    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2);
    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL);
    m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_EFFECT2);
    m_pMonster->Stop_Sound(CPawn::PAWN_SOUND_VOICE);
}

_bool CState_RaxasiaP2_JumpStamp::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_INCHENT) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_INCHENT);
        }
        break;
    case 1:
        if ((AN_JUMPSTAMP_START) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_JUMPSTAMP_START);
        }
        break;

    case 2:
        if ((AN_JUMPSTAMP_MIDDLE) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_JUMPSTAMP_MIDDLE);
        }
        break;

    case 3:
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
}

void CState_RaxasiaP2_JumpStamp::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bInchent)
        {
            if (CurTrackPos >= 45.f)
            {
                m_bInchent = true;
                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2, true);
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if (!m_bEnvelop)
        {
            if (CurTrackPos >= 97.f)
            {
                m_bEnvelop = true;
                m_pMonster->Active_Effect(CRaxasia::EFFECT_THUNDERENVELOP_SMALL, true);

                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, 0.f };
                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_UFBIndex(UFB_ROOT))
                  * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Jump"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });
                
            }
        }
    }
    else if (m_iRouteTrack == 3)
    {
        if (!m_bStomp)
        {
            if (CurTrackPos >= 18.f)
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, -1.75f };
                XMStoreFloat4x4(&WorldMat,
                    (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_Model()->Get_UFBIndices(UFB_WEAPON))
                        * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
                vPos.y = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;

                CAttackObject::ATKOBJ_DESC Desc{};
                Desc.vPos = vPos;
                Desc.vDir = _Vec3{ m_pMonster->Get_TargetDir() };
                Desc.pOwner = m_pMonster;
                Desc.vTargetPos = _Vec3{ m_pMonster->Get_TargetPos() };
                Desc.iTrack_State = 1;

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderStampMark"), &Desc);

                m_bStomp = true;
            }
        }
        //일정간격두고 마크 3개 터트리기
        if ((CurTrackPos >= 15.f && CurTrackPos <= 17.f) ||
            (CurTrackPos >= 22.f && CurTrackPos <= 24.f) || 
            (CurTrackPos >= 27.f && CurTrackPos <= 29.f))
        {
            if (!m_bOnMark)
            {
                m_bOnMark = true;

                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, 0.f };
                vPos = XMVector3TransformCoord(vPos, m_pMonster->Get_Transform()->Get_WorldMatrix());
                
                _float fvariableX = m_pGameInstance->Get_Random(0.f, 4.f) - 2.f;
                _float fvariableZ = m_pGameInstance->Get_Random(0.f, 4.f) - 2.f;
                
                vPos.x += fvariableX;
                vPos.z += fvariableZ;

                CAttackObject::ATKOBJ_DESC Desc;

                Desc.vPos = vPos;
                Desc.pOwner = m_pMonster;

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderMark"), &Desc);
                ++m_iTestCnt;
            }
        }
        else
        {
            m_bOnMark = false;
        }
    }
}

void CState_RaxasiaP2_JumpStamp::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bInchentSound)
        {
            if (CurTrackPos >= 45.f)
            {
                m_bInchentSound = true;
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT2, TEXT("SE_NPC_Raxasia_SK_WP_Sword_Lightning_Loop.wav"), true);
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if (!m_bEnvelopSound)
        {
            if (CurTrackPos >= 97.f)
            {
                m_bEnvelopSound = true;
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_VOICE, TEXT("SE_NPC_Raxasia_SK_PJ_Spark_Electric_Loop_02.wav"), true);

                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_FX_Jump_Heavy_01.wav"), false);

            }
        }
    }
    else if (m_iRouteTrack == 3)
    {
        if (!m_bStompSound)
        {
            if ((CurTrackPos >= 18.f))
            {
                m_bStompSound = true;

                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Raxasia_SK_PJ_Thunder_Explo_03.wav"), false);
            }
        }
    }
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