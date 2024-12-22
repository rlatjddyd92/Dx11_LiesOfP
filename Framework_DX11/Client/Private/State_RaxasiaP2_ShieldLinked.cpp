#include "stdafx.h"
#include "State_RaxasiaP2_ShieldLinked.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "AttackObject.h"
#include "Effect_Manager.h"

CState_RaxasiaP2_ShieldLinked::CState_RaxasiaP2_ShieldLinked(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_ShieldLinked::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_ShieldLinked::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_SHIELDATTACK_TWICE, false, 0.1f, 0);

    m_bSwingSound = false;

    m_bCharge = false;
    m_bSwing = false;

    return S_OK;
}

void CState_RaxasiaP2_ShieldLinked::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (CurTrackPos >= 255)
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_START, false, 0.1f, 0);
            return;
        }

        if (CurTrackPos <= 110.f || 
            (CurTrackPos >= 155.f && CurTrackPos <= 165)||
            CurTrackPos >= 205.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    case 1:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_SHIELDATTACK, false, 0.1f, 0);
        }
        
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);

        break;

    case 2:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_JUMPMAGIC, false, 0.1f, 0);
            return;
        }

        if (CurTrackPos <= 25.f ||
            CurTrackPos >= 70.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }
        break;

    case 3:
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);

        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_ShieldLinked::End_State()
{
    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2);
}

_bool CState_RaxasiaP2_ShieldLinked::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_SHIELDATTACK_TWICE) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_SHIELDATTACK_TWICE);
        }
        break;

    case 1:
        if ((AN_START) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_START);
        }
        break;

    case 2:
        if ((AN_SHIELDATTACK) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_SHIELDATTACK);
        }
        break;

    case 3:
        if ((AN_JUMPMAGIC) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_JUMPMAGIC);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_ShieldLinked::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if ((CurTrackPos >= 130.f && CurTrackPos <= 145.f) ||
            (CurTrackPos >= 175.f && CurTrackPos <= 195.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.2f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
    else if (m_iRouteTrack == 2)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 50.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.4f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider(1);
        }
    }
}

void CState_RaxasiaP2_ShieldLinked::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bCharge)
        {
            if (CurTrackPos >= 60.f)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD_P2, true);
                m_bCharge = true;
            }
        }
    }
    else if (m_iRouteTrack == 3)
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
            if (CurTrackPos >= 195.f)
            {
                m_bFire = true;

                CAttackObject::ATKOBJ_DESC Desc{};

                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, -1.75f };
                XMStoreFloat4x4(&WorldMat,
                    (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_Model()->Get_UFBIndices(UFB_WEAPON))
                        * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                Desc.vPos = vPos;

                Desc.vDir = _Vec3{ m_pMonster->Get_TargetDir() };
                Desc.vDir.Normalize();

                Desc.vTargetPos = _Vec3{ m_pMonster->Get_TargetPos() };
                Desc.pOwner = m_pMonster;

                Desc.iTrack_State = 1;

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ThunderBolt"), &Desc);
            }

        }
    }
}

void CState_RaxasiaP2_ShieldLinked::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP2_ShieldLinked* CState_RaxasiaP2_ShieldLinked::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_ShieldLinked* pInstance = new CState_RaxasiaP2_ShieldLinked(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_ShieldLinked"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_ShieldLinked::Free()
{
    __super::Free();
}