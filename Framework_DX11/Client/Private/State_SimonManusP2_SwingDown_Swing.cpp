#include "stdafx.h"
#include "State_SimonManusP2_SwingDown_Swing.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "EffectObject.h"

CState_SimonManusP2_SwingDown_Swing::CState_SimonManusP2_SwingDown_Swing(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_SwingDown_Swing::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_SwingDown_Swing::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_ROUTE_FIRST, false, 0.1f, 0);

    m_isSwing = true;

    _int iCnt = rand() % 2;

    switch (iCnt)
    {
    case 0:
        m_iCurLastAnim = AN_ROUTE_LAST_L;
        break;

    case 1:
        m_iCurLastAnim = AN_ROUTE_LAST_R;
        break;

    default:
        break;
    }

    m_bStampSound = false;
    m_bSwingSound = false;
    m_bSwing = false;

    return S_OK;
}

void CState_SimonManusP2_SwingDown_Swing::Update(_float fTimeDelta)
{
    if (m_isSwing)
    {
        if (130 >= m_pMonster->Get_CurrentTrackPos())
        {
            m_pMonster->Change_Animation(m_iCurLastAnim, false, 0, 0);
            ++m_iRouteTrack;
            m_isSwing = false;
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

void CState_SimonManusP2_SwingDown_Swing::End_State()
{
    m_iRouteTrack = 0;
}

_bool CState_SimonManusP2_SwingDown_Swing::End_Check()
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
        if ((m_iCurLastAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iCurLastAnim);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_SimonManusP2_SwingDown_Swing::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if (m_iCurLastAnim == AN_ROUTE_LAST_L)
        {
            if (CurTrackPos >= 45.f && CurTrackPos <= 95.f)
            {
                m_pMonster->Active_CurrentWeaponCollider(1);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
        else    //루트 R일때
        {
            if (CurTrackPos >= 45 && CurTrackPos <= 80.f)
            {
                m_pMonster->Active_CurrentWeaponCollider(1);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
    }
    else    //처음 공격일때
    {
        if (CurTrackPos >= 60.f && CurTrackPos <= 75.f)
        {
            m_pMonster->Active_CurrentWeaponCollider(1);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_SimonManusP2_SwingDown_Swing::Effect_Check(_double CurTrackPos)
{
    if(m_iRouteTrack == 0)    //처음 공격일때
    {
        if (CurTrackPos >= 60.f && CurTrackPos <= 75.f)
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
    else
    {
        if (m_iCurLastAnim == AN_ROUTE_LAST_L)
        {
            if (CurTrackPos >= 45.f && CurTrackPos <= 95.f)
            {
                if (!m_bSwing)
                {
                    m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
                    m_bSwing = true;
                }
            }
            else
            {
                m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
            }
        }
        else    //루트 R일때
        {
            if (CurTrackPos >= 45 && CurTrackPos <= 80.f)
            {
                if (!m_bSwing)
                {
                    m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
                    m_bSwing = false;
                }
            }
            else
            {
                m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
            }
        }
    }
}

void CState_SimonManusP2_SwingDown_Swing::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bStampSound)
        {
            if (CurTrackPos >= 75.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
                m_bStampSound = true;
            }
        }
    }
    else
    {
        if (!m_bSwingSound)
        {
            if (CurTrackPos >= 45.f && CurTrackPos <= 50.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_M_02.wav"), true);
                m_bSwingSound = true;
            }
            if (CurTrackPos >= 65.f)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_PC_SK_Smash_Crystal_Stone_H_03.wav"));
                m_bSwingSound = true;
            }
        }
        else
        {
            if (CurTrackPos > 55.f && CurTrackPos <= 60.f)
            {
                m_bSwingSound = false;
            }
        }
    }
}

CState_SimonManusP2_SwingDown_Swing* CState_SimonManusP2_SwingDown_Swing::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_SwingDown_Swing* pInstance = new CState_SimonManusP2_SwingDown_Swing(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_SwingDown_Swing"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_SwingDown_Swing::Free()
{
    __super::Free();
}