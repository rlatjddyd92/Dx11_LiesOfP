#include "stdafx.h"
#include "State_SimonManusP2_Route1.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "EffectObject.h"

CState_SimonManusP2_Route1::CState_SimonManusP2_Route1(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_Route1::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_Route1::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    m_bStampEffect = false;
    m_bStampSound = false;
    m_bSwingSound = false;
    return S_OK;
}

void CState_SimonManusP2_Route1::Update(_float fTimeDelta)
{
    if (End_Check())
    {

        switch (m_iRouteTrack)
        {
        case 0:
            m_pMonster->Change_Animation(AN_ROUTE_MIDDLE, false, 0.0f, 0);
            m_bSwingSound = false;
            break;

        case 1:
            m_pMonster->Change_Animation(AN_ROUTE_LAST, false, 0.1f, 0);
            break;

        case 2:
            m_pMonster->Change_State(CSimonManus::IDLE);
            return;
            break;

        default:
            break;
        }
        ++m_iRouteTrack;
    }
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(fTimeDelta, CurTrackPos);
    Effect_Check(CurTrackPos);

}

void CState_SimonManusP2_Route1::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP2_Route1::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_ROUTE_FIRST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_FIRST);
        }
        break;

    case 1:
        if ((AN_ROUTE_MIDDLE) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_MIDDLE);
        }
        break;

    case 2:
        if ((AN_ROUTE_LAST) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_ROUTE_LAST);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_SimonManusP2_Route1::Collider_Check(_float fTimeDelta, _double CurTrackPos)
{
    if (m_iRouteTrack == 0) //AN_ROUTE_FIRST, 쓰러지면서 하는 스윙
    {
        if (CurTrackPos <= 50.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 5, fTimeDelta);
        }

        if (CurTrackPos >= 60 && CurTrackPos <= 85.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else if(m_iRouteTrack == 1)       //어보이드 스윙
    {
        if (CurTrackPos >= 120 && CurTrackPos <= 180.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
    else       //스탬프
    {
        if (CurTrackPos >= 60 && CurTrackPos <= 75.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_SimonManusP2_Route1::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0) //AN_ROUTE_FIRST, 쓰러지면서 하는 스윙
    {
        if ((CurTrackPos >= 60 && CurTrackPos <= 85.f))
        {
            if (!m_pMonster->Get_EffectsLoop(CSimonManus::P1_TRAIL))
            {
                m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
        }
    }
    else if (m_iRouteTrack == 1)       //어보이드 스윙
    {
        if ((CurTrackPos >= 120 && CurTrackPos <= 180.f))
        {
            if (!m_pMonster->Get_EffectsLoop(CSimonManus::P1_TRAIL))
            {
                m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
        }
    }
    else       //스탬프
    {
        if (CurTrackPos >= 60 && CurTrackPos <= 75.f)
        {
            if (!m_bStampEffect)
            {
                CEffectObject::EFFECTOBJ_DESC Desc{};
                Desc.fLifeDuration = 1.5f;
                Desc.strEffectTag = TEXT("SimonManus_Attack_Stamp");
                _float4x4 WorldMat{};

                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_WeaponBoneCombinedMat(6) * (*m_pMonster->Get_WeaponWorldMat())));
                Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42, WorldMat._43 };

                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_SpotEffect"), &Desc);

                m_bStampEffect = true;
            }
        }
    }
}

void CState_SimonManusP2_Route1::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0) //AN_ROUTE_FIRST, 쓰러지면서 하는 스윙
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 70 && CurTrackPos <= 75.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));

                m_bSwingSound = true;
            }
        }
    }
    else if(m_iRouteTrack == 1)      //어보이드 스윙
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 160 && CurTrackPos <= 165.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));

                m_bSwingSound = true;
            }
        }
    }
    else     //스탬프
    {
        if (!m_bStampSound)
        {
            if (CurTrackPos >= 160 && CurTrackPos <= 165.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));

                m_bStampSound = true;
            }
        }
    }
}

CState_SimonManusP2_Route1* CState_SimonManusP2_Route1::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_Route1* pInstance = new CState_SimonManusP2_Route1(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_Route1"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_Route1::Free()
{
    __super::Free();
}