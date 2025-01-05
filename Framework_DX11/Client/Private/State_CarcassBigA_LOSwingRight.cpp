#include "stdafx.h"
#include "State_CarcassBigA_LOSwingRight.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

CState_CarcassBigA_LOSwingRight::CState_CarcassBigA_LOSwingRight(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_CarcassBigA_LOSwingRight::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_CarcassBigA_LOSwingRight::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_LOSWINGRIGHT, false, 0.1f, 0, true);

    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);
    m_isRimLight = true;
    m_bSwingSound = false;

    return S_OK;
}

void CState_CarcassBigA_LOSwingRight::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        m_pMonster->Change_State(CMonster::IDLE);
    }


    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);
    Update_Rimlight(fTimeDelta, CurTrackPos);

}

void CState_CarcassBigA_LOSwingRight::End_State()
{
    m_pMonster->Set_RimLightColor(_Vec4{ 0.f, 0.f, 0.f, 0.f });
}

_bool CState_CarcassBigA_LOSwingRight::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_LOSWINGRIGHT);
}

void CState_CarcassBigA_LOSwingRight::Collider_Check(_double CurTrackPos)
{
    if (CurTrackPos >= 65.f && CurTrackPos <= 80.f)
    {
        m_pMonster->Active_CurrentWeaponCollider(1.5, 0, HIT_TYPE::HIT_CARCASS, ATTACK_STRENGTH::ATK_STRONG);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider(0);
    }

    
}

void CState_CarcassBigA_LOSwingRight::Sound_Check(_double CurTrackPos)
{
    if (!m_bSwingSound)
    {
        if (CurTrackPos >= 65.f && CurTrackPos <= 80.f)
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_Carcass_OneArmed_SK_WS_Blunt_01.wav"), false);
            m_bSwingSound = true;
        }
    }
}

void CState_CarcassBigA_LOSwingRight::Update_Rimlight(_float fTimeDelta, _double CurTrackPos)
{
    if (m_isRimLight)
    {
        if (CurTrackPos < 85.f)
        {
            m_vRimLightColor.x = max(m_vRimLightColor.x + 0.5f * fTimeDelta, 1.f);
            m_vRimLightColor.w = max(m_vRimLightColor.w - 0.6f * fTimeDelta, 0.1f);
        }
        else
        {
            m_vRimLightColor.x = max(m_vRimLightColor.x - 0.7f * fTimeDelta, 0.f);
            m_vRimLightColor.w = min(m_vRimLightColor.w + 0.7f * fTimeDelta, 0.5f);
        }

        m_pMonster->Set_RimLightColor(m_vRimLightColor);
    }

}

CState_CarcassBigA_LOSwingRight* CState_CarcassBigA_LOSwingRight::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_CarcassBigA_LOSwingRight* pInstance = new CState_CarcassBigA_LOSwingRight(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_CarcassBigA_LOSwingRight::Free()
{
    __super::Free();
}