#include "stdafx.h"
#include "State_SimonManusP1_Stamp.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "AttackObject.h"
#include "FollowedEffect.h"

CState_SimonManusP1_Stamp::CState_SimonManusP1_Stamp(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP1_Stamp::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP1_Stamp::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    if (m_pMonster->Calc_Distance_XZ() >= 4.f)
    {
        m_pMonster->Change_Animation(AN_STAMP_MOVE, false, 0.1f, 0);
        m_iCurStartAnim = AN_STAMP_MOVE;
    }
    else
    {
        m_pMonster->Change_Animation(AN_STAMP, false, 0.1f, 0);
        m_iCurStartAnim = AN_STAMP;
    }
    m_bStamp = false;
    m_bBlast = false;
    m_bWeaponSpin = false;
    m_bStampEffect = false;
    m_bAbsorb = false;
    m_bStampSound = false;
    return S_OK;
}

void CState_SimonManusP1_Stamp::Update(_float fTimeDelta)
{

    if (!m_bStamp)
    {
        _float fTime = 120.f;
        if (m_iCurStartAnim == AN_STAMP_MOVE)
        {
            fTime = 140.f;
        }
        
        if (fTime <= m_pMonster->Get_CurrentTrackPos())
        {
            m_pMonster->Change_Animation(AN_SPINBLAST, false, 0, 0);
            m_pMonster->Change_WeaponAnimation(2, false, 0.f);
            ++m_iRouteTrack;
            m_bStamp = true;
        }
    }


    if (End_Check())
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
}

void CState_SimonManusP1_Stamp::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);

    m_pMonster->Change_WeaponAnimation(0, false, 0.1f);
    m_bWeaponSpin = false;
    m_bStamp = false;
}

_bool CState_SimonManusP1_Stamp::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((m_iCurStartAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iCurStartAnim);
        }
        break;

    case 1:
    case 2:
        if ((AN_SPINBLAST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_SPINBLAST);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_SimonManusP1_Stamp::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (m_iCurStartAnim == AN_STAMP)
        {
            if (CurTrackPos >= 40.f && CurTrackPos <= 70.f)
            {
                m_pMonster->Active_CurrentWeaponCollider(1.1f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
            }
            else
            {
                m_pMonster->DeActive_CurrentWeaponCollider(0);
            }
        }
        else
        {
            if (CurTrackPos >= 85.f && CurTrackPos <= 95.f)
            {
                m_pMonster->Active_CurrentWeaponCollider(1.1f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
            }
            else
            {
                m_pMonster->DeActive_CurrentWeaponCollider(0);
            }
        }
        
    }
}

void CState_SimonManusP1_Stamp::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        _float fTime{};
        if (m_iCurStartAnim == AN_STAMP_MOVE)
        {
            fTime = 94.f;
        }
        else
        {
            fTime = 70.f;
        }

        if ((CurTrackPos >= fTime && CurTrackPos <= fTime + 10.f))
        {
            if (!m_bStampEffect)
            {
                CEffectObject::EFFECTOBJ_DESC Desc{};
                Desc.fLifeDuration = 1.5f;
                Desc.strEffectTag = TEXT("SimonManus_Attack_Stamp");
                _float4x4 WorldMat{};
                Desc.bLoopCheck = true;

                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(6) * (*m_pMonster->Get_WeaponWorldMat())));
                Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42, WorldMat._43 };

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_SpotEffect"), &Desc);

                m_bStampEffect = true;
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if (!m_bBlast)
        {
            if (CurTrackPos > 125.f)
            {
                m_bBlast = true;
                CAttackObject::ATKOBJ_DESC Desc;
                _float4x4 WorldMat{};
                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(6) * (*m_pMonster->Get_WeaponWorldMat())));
                Desc.vPos = _Vec3{ WorldMat ._41, WorldMat._42, WorldMat._43 };

                Desc.pOwner = m_pMonster;

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_StampBlast"), &Desc);
            }
        }

        if (!m_bAbsorb)
        {
            if (CurTrackPos >= 40.f)
            {
                CFollowedEffect::FOLLOWEFFOBJ_DESC Desc{};
                Desc.fLifeDuration = 1.5f;
                Desc.bLoopCheck = true;
                Desc.strEffectTag = TEXT("SimonManus_Attack_ChargeStamp");
                _float4x4 WorldMat{};

                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(6) * (*m_pMonster->Get_WeaponWorldMat())));
                Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42, WorldMat._43 };

                Desc.pParentWorldMat = m_pMonster->Get_WeaponWorldMat();
                Desc.pBoneCombinedMat = m_pMonster->Get_WeaponBoneCombinedMat(6);

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_FollowedEffect"), &Desc);

                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
                m_bAbsorb = true;
            }
        }
        if (!m_bWeaponSpin)
        {
            if (m_pMonster->Get_WeaponAnimEnd(2))
            {
                m_pMonster->Change_WeaponAnimation(3, true, 0.f, 0);
                
                m_bWeaponSpin = true;
            }
        }
    }
}

void CState_SimonManusP1_Stamp::Control_Sound(_double CurTrackPos)
{
    if (!m_bStampSound)
    {
        _float fTime{};
        if (m_iCurStartAnim == AN_STAMP_MOVE)
            fTime = 94.f;
        else
            fTime = 70.f;

        if ((CurTrackPos >= fTime && CurTrackPos <= fTime + 10.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
            m_bStampSound = true;
        }
    }


}

CState_SimonManusP1_Stamp* CState_SimonManusP1_Stamp::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP1_Stamp* pInstance = new CState_SimonManusP1_Stamp(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP1_Stamp"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP1_Stamp::Free()
{
    __super::Free();
}