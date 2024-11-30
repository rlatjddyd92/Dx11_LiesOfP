#include "stdafx.h"
#include "State_SimonManusP1_Stamp.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "AttackObject.h"

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
    m_bStamp = true;
    m_bBlast = false;
    m_bWeaponSpin = false;
    return S_OK;
}

void CState_SimonManusP1_Stamp::Update(_float fTimeDelta)
{

    if (m_bStamp)
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
            m_bStamp = false;
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
}

void CState_SimonManusP1_Stamp::End_State()
{
    m_pMonster->DeActive_Effect(CSimonManus::P1_STAMP);
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
                m_pMonster->Active_CurrentWeaponCollider(1);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
        else
        {
            if (CurTrackPos >= 85.f && CurTrackPos <= 95.f)
            {
                m_pMonster->Active_CurrentWeaponCollider(1);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
        
    }
}

void CState_SimonManusP1_Stamp::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 40.f && CurTrackPos <= 80.f))
        {
            if (!m_pMonster->Get_EffectsLoop(CSimonManus::P1_STAMP))
            {
                m_pMonster->Active_Effect(CSimonManus::P1_STAMP);
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P1_STAMP);
            m_pMonster->DeActive_Effect(CSimonManus::P1_CHARGESTAMP);
        }

        if (!m_bBlast)
        {
            if (CurTrackPos > 125.f)
            {
                m_bBlast = true;
                CAttackObject::ATKOBJ_DESC Desc;
                _float4x4 WorldMat{};
                XMStoreFloat4x4(&WorldMat, *m_pMonster->Get_WeaponWorldMat());
                Desc.vPos = _Vec3{ WorldMat ._41, WorldMat._42, WorldMat._43 };
                XMVector3TransformCoord(Desc.vPos, *m_pMonster->Get_WeaponBoneCombinedMat(6));
                m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_StampBlast"), &Desc);
            }
        }

        if (!m_bWeaponSpin)
        {
            if (m_pMonster->Get_WeaponAnimEnd(2))
            {
                m_pMonster->Change_WeaponAnimation(3, true, 0.f, 0);
                m_pMonster->Active_Effect(CSimonManus::P1_CHARGESTAMP);
                m_bWeaponSpin = true;
            }
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