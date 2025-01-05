#include "stdafx.h"
#include "State_SimonManusP1_Charge_SwingDown.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "AttackObject.h"

CState_SimonManusP1_Charge_SwingDown::CState_SimonManusP1_Charge_SwingDown(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_Charge_SwingDown::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_Charge_SwingDown::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_CHARGESWINGDOWN, false, 0.1f, 0);

    m_bChargeSwing = true;
    m_bStampSound = false;
    return S_OK;
}

void CState_SimonManusP1_Charge_SwingDown::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_SimonManusP1_Charge_SwingDown::End_State()
{

}

_bool CState_SimonManusP1_Charge_SwingDown::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_CHARGESWINGDOWN);
}

void CState_SimonManusP1_Charge_SwingDown::Collider_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 225.f && CurTrackPos <= 245.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

void CState_SimonManusP1_Charge_SwingDown::Effect_Check(_double CurTrackPos)
{
    if (m_bCharging)
    {
        if (CurTrackPos >= 10.f)//차징 
        {
            //차지 이펙트 set loop
            m_pMonster->Active_Effect(CSimonManus::P1_CHARGEHAMMER);

            m_bCharging = false;
        }
    }
    else if (CurTrackPos >= 220.f)
    {
        m_pMonster->DeActive_Effect(CSimonManus::P1_CHARGEHAMMER);
    }

    if (m_bChargeSwing)
    {
        if (CurTrackPos >= 240.f)
        {
            m_bChargeSwing = false;
            CAttackObject::ATKOBJ_DESC Desc;
            _float4x4 WorldMat{};
            XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(6) * (*m_pMonster->Get_WeaponWorldMat())));
            Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42 - 0.2f, WorldMat._43 };

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_ChargeSwing"), &Desc);
        }
    }
}

void CState_SimonManusP1_Charge_SwingDown::Control_Sound(_double CurTrackPos)
{
    if (!m_bStampSound)
    {
        if (CurTrackPos >= 240.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
            m_bStampSound = true;
        }
    }
}

CState_SimonManusP1_Charge_SwingDown* CState_SimonManusP1_Charge_SwingDown::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_Charge_SwingDown* pInstance = new CState_SimonManusP1_Charge_SwingDown(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_Charge_SwingDown"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_Charge_SwingDown::Free()
{
    __super::Free();
}