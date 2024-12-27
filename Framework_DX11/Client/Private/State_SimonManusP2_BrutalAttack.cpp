#include "stdafx.h"
#include "State_SimonManusP2_BrutalAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "EffectObject.h"

CState_SimonManusP2_BrutalAttack::CState_SimonManusP2_BrutalAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_BrutalAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_BrutalAttack::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_BRUTALATTACK, false, 0.1f, 0);
    m_pMonster->Get_Model()->Set_SpeedRatio(AN_BRUTALATTACK, 0.15);
    m_iColliderResetCheckCnt = 0;
    m_bSwingSound = false;
    m_bStampSound = false;
    m_bSwing = false;
    m_bStamp = false;

    return S_OK;
}

void CState_SimonManusP2_BrutalAttack::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    if (CurTrackPos <= 190.f || 
        (CurTrackPos >= 290.f && CurTrackPos <= 350.f) ||
        (CurTrackPos >= 470.f && CurTrackPos <= 555.f))
    {
        _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 2, fTimeDelta);
    }


    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_SimonManusP2_BrutalAttack::End_State()
{

}

_bool CState_SimonManusP2_BrutalAttack::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_BRUTALATTACK);
}

void CState_SimonManusP2_BrutalAttack::Collider_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 88.f && CurTrackPos <= 115.f) ||
        (CurTrackPos >= 154.f && CurTrackPos <= 180.f) ||
        (CurTrackPos >= 205.f && CurTrackPos <= 270.f) ||
        (CurTrackPos >= 340.f && CurTrackPos <= 348.f) ||
        (CurTrackPos >= 400.f && CurTrackPos <= 412.f) ||
        (CurTrackPos >= 560.f && CurTrackPos <= 569.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.1f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }

    if (m_iColliderResetCheckCnt == 0)
    {
        if ((CurTrackPos >= 235.f && CurTrackPos <= 240.f))
        {
            ++m_iColliderResetCheckCnt;
            m_pMonster->Reset_WeaponOverlapCheck();
        }
    }
    else if (m_iColliderResetCheckCnt == 1)
    {
        if ((CurTrackPos >= 250.f && CurTrackPos <= 255.f))
        {
            ++m_iColliderResetCheckCnt;
            m_pMonster->Reset_WeaponOverlapCheck();
        }
    }

}

void CState_SimonManusP2_BrutalAttack::Effect_Check(_double CurTrackPos)
{
    if (!m_bSwing)
    {
        if ((CurTrackPos >= 88.f && CurTrackPos <= 115.f) ||
            (CurTrackPos >= 154.f && CurTrackPos <= 180.f) ||
            (CurTrackPos >= 205.f && CurTrackPos <= 270.f))
        {
            m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
            m_bSwing = true;
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else
    {
        if ((CurTrackPos > 115.f && CurTrackPos <= 120.f) ||
            (CurTrackPos > 180.f && CurTrackPos <= 185.f))
        {
            m_bSwing = false;
        }

    }


    if (!m_bStamp)
    {
        if ((CurTrackPos >= 348.f && CurTrackPos <= 353.f) ||
            (CurTrackPos >= 412.f && CurTrackPos <= 417.f) ||
            (CurTrackPos >= 569.f && CurTrackPos <= 574.f))
        {

            CEffectObject::EFFECTOBJ_DESC Desc{};
            Desc.fLifeDuration = 1.5f;
            Desc.bLoopCheck = true;
            Desc.strEffectTag = TEXT("SimonManus_Attack_Stamp");
            _float4x4 WorldMat{};

            XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(6) * (*m_pMonster->Get_WeaponWorldMat())));
            Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42, WorldMat._43 };

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_SpotEffect"), &Desc);

            m_bStamp = true;
        }
    }
    else
    {
        if ((CurTrackPos > 353.f && CurTrackPos <= 358.f) ||
            (CurTrackPos > 417.f && CurTrackPos <= 422.f) ||
            (CurTrackPos > 574.f && CurTrackPos <= 579.f))
        {
            m_bStamp = false;
        }
    }


}

void CState_SimonManusP2_BrutalAttack::Control_Sound(_double CurTrackPos)
{
    if (!m_bSwingSound)
    {
        if ((CurTrackPos >= 100.f && CurTrackPos <= 115.f) ||
            (CurTrackPos >= 160.f && CurTrackPos <= 180.f) ||
            (CurTrackPos >= 210.f && CurTrackPos <= 220.f) ||
            (CurTrackPos >= 240.f && CurTrackPos <= 245.f) ||
            (CurTrackPos >= 255.f && CurTrackPos <= 260.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));

            m_bSwingSound = true;
        }
    }
    else
    {
        if ((CurTrackPos > 115.f && CurTrackPos <= 120.f) ||
            (CurTrackPos > 180.f && CurTrackPos <= 185.f) ||
            (CurTrackPos > 220.f && CurTrackPos <= 225.f) ||
            (CurTrackPos > 245.f && CurTrackPos <= 250.f))
        {
            m_bSwingSound = false;
        }
    }

    if (!m_bStampSound)
    {
        if ((CurTrackPos >= 348.f && CurTrackPos <= 353.f) ||
            (CurTrackPos >= 412.f && CurTrackPos <= 417.f) ||
            (CurTrackPos >= 569.f && CurTrackPos <= 574.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
            m_bStampSound = true;
        }
    }
    else
    {
        if ((CurTrackPos > 351.f && CurTrackPos <= 356.f) ||
            (CurTrackPos > 415.f && CurTrackPos <= 420.f))
        {
            m_bSwingSound = false;
        }
    }

}

CState_SimonManusP2_BrutalAttack* CState_SimonManusP2_BrutalAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_BrutalAttack* pInstance = new CState_SimonManusP2_BrutalAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_BrutalAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_BrutalAttack::Free()
{
    __super::Free();
}