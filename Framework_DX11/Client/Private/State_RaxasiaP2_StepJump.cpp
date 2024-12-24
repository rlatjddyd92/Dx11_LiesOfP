#include "stdafx.h"
#include "State_RaxasiaP2_StepJump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "AttackObject.h"
#include "Effect_Manager.h"

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
    m_pMonster->Change_Animation(AN_INCHENT, false, 0.1f, 0);

    m_bSwingSound = false;
    m_bStartSpot = true;
    m_vFlyMoveStack = _Vec3{};
    m_bSwing = false;
    m_bInchent = false;
    m_bEnvelop = false;
    m_bStomp = false;
    m_bSWingDown = false;
    m_bSpread = false;

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
            m_pMonster->Change_Animation(AN_JUMPSTAMP_START, false, 0.1f, 0);
        }
        break;
    case 1:
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
                m_fDist -= 4.f;
                if (m_fDist < 2.f)
                {
                    m_fDist = 2.f;
                }
                m_bStartSpot = false;
            }

            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 1.f, fTimeDelta);

            _float fYMove = m_pMonster->Get_RigidBody()->Get_Velocity().y;

            _Vec3 vMove = m_vTargetDir * m_fDist * (((_float)CurTrackPos - 87.f) / 145.f);
            vMove.y = fYMove;
            m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vFlyMoveStack) / fTimeDelta);
            m_vFlyMoveStack = vMove;
        }
        else if (CurTrackPos >= 145.f && CurTrackPos <= 160.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_vTargetDir, 3.f, fTimeDelta);
        }

        break;

    case 2:
        //림라이트
        if (End_Check())
        {
            m_iRouteTrack = 0;
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        if (CurTrackPos <= 45.f || 
            (CurTrackPos >= 105.f && CurTrackPos <= 180.f))
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


    default:
        break;
    }

    return  bEndCheck;
}

void CState_RaxasiaP2_StepJump::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
    }
    else if (m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 105.f) ||
            (CurTrackPos >= 110.f && CurTrackPos <= 130.f) ||
            (CurTrackPos >= 200.f && CurTrackPos <= 206.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
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
        if (!m_bInchent)
        {
            if (CurTrackPos >= 45.f && CurTrackPos <= 55.f)
            {
                m_bInchent = true;
                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2, true);
            }
        }
    }
    if (m_iRouteTrack == 1)
    {
        if (!m_bEnvelop)
        {
            if (CurTrackPos >= 87.f)
            {
                m_bEnvelop = true;
            }
        }

        if (!m_bStomp)
        {
            if (CurTrackPos >= 175.f && CurTrackPos <= 211.f)
            {
                m_bStomp = true;
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, -1.75f };
                XMStoreFloat4x4(&WorldMat,
                    (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_Model()->Get_UFBIndices(UFB_WEAPON))
                        * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
                vPos.y = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_ThunderStamp_Small"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });

                //찍으면서 마크 생성 후 폭발

                CAttackObject::ATKOBJ_DESC Desc;

                _Vec3 vTargetDir = m_pMonster->Get_TargetPos() - vPos;

                Desc.vPos = vPos;
                Desc.vDir = vTargetDir;
                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderStampMark"), &Desc);

            }
        }
        else if (CurTrackPos >= 220.f)
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2);
            m_bInchent = false;
        }
    }
    else
    {
        if ((CurTrackPos >= 90.f && CurTrackPos <= 130.f) ||
            (CurTrackPos >= 200.f && CurTrackPos <= 206.f))
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

        if (!m_bInchent)
        {
            if ((CurTrackPos >= 190.f))
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, 0.f };
                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_UFBIndex(UFB_HAND_RIGHT)) * m_pMonster->Get_Transform()->Get_WorldMatrix()));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_ThunderInchent"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });

                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2, true);
                //내려찍기
                m_bInchent = true;
            }
        }
        else if (CurTrackPos >= 230.f)
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2);
        }

        if (!m_bSWingDown)
        {
            if ((CurTrackPos >= 206.f && CurTrackPos <= 211.f))
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, -1.75f };
                XMStoreFloat4x4(&WorldMat,
                    (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_Model()->Get_UFBIndices(UFB_WEAPON))
                        * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
                vPos.y = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_ThunderStamp"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });

                CAttackObject::ATKOBJ_DESC Desc;

                _Vec3 vTargetDir = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK);
                vTargetDir.Normalize();
                _Vec3 vRight = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
                vRight.Normalize();


                Desc.vPos = vPos;
                Desc.vDir = vTargetDir;
                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderSpread"), &Desc);

                Desc.vPos = vPos;
                Desc.vDir = vTargetDir + vRight * 0.3f;
                Desc.vDir.Normalize();
                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderSpread"), &Desc);

                Desc.vPos = vPos;
                Desc.vDir = vTargetDir - vRight * 0.3f;
                Desc.vDir.Normalize();
                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderSpread"), &Desc);

                Desc.vPos = vPos;
                Desc.vDir = vTargetDir + vRight * 0.6f;
                Desc.vDir.Normalize();
                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderSpread"), &Desc);

                Desc.vPos = vPos;
                Desc.vDir = vTargetDir - vRight * 0.6f;
                Desc.vDir.Normalize();
                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderSpread"), &Desc);

            }
        }

        if (!m_bSpread)
        {
            if ((CurTrackPos >= 200.f && CurTrackPos <= 206.f))
            {
                m_bSpread = true;
            }
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