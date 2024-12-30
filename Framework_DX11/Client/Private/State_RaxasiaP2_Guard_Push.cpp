#include "stdafx.h"
#include "State_RaxasiaP2_Guard_Push.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP2_Guard_Push::CState_RaxasiaP2_Guard_Push(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP2_Guard_Push::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RaxasiaP2_Guard_Push::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_START, false, 0.1f, 0);

    m_bSwingSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP2_Guard_Push::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSwing = false;
            m_pMonster->Change_Animation(AN_SHIELDATTACK, false, 0.02f, 0);
            return;
        }

        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
       

        break;

    case 1:
        if (End_Check())
        {
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
        
        if (CurTrackPos <= 75)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP2_Guard_Push::End_State()
{
}

_bool CState_RaxasiaP2_Guard_Push::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_START) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_START);
        }
        break;

    case 1:
        if ((AN_SHIELDATTACK) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_SHIELDATTACK);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_RaxasiaP2_Guard_Push::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 60.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 1, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP2_Guard_Push::Effect_Check(_double CurTrackPos)
{

}

void CState_RaxasiaP2_Guard_Push::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 35.f && CurTrackPos <= 60.f))
        {
            if (!m_bSwingSound)
            {
                if ((CurTrackPos >= 35.f))
                {
                    m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Blunt_L.wav"), false);
                    m_bSwingSound = true;
                }
            }
        }
    }
}

CState_RaxasiaP2_Guard_Push* CState_RaxasiaP2_Guard_Push::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP2_Guard_Push* pInstance = new CState_RaxasiaP2_Guard_Push(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP2_Guard_Push"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP2_Guard_Push::Free()
{
    __super::Free();
}