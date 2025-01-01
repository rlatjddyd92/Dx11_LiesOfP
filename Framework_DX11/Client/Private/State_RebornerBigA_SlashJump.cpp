#include "stdafx.h"
#include "State_RebornerBigA_SlashJump.h"
#include "GameInstance.h"
#include "Model.h"
#include "Monster.h"
#include "Camera.h"

#include "CurruptedStrongArm_Puppet.h"

CState_RebornerBigA_SlashJump::CState_RebornerBigA_SlashJump(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerBigA_SlashJump::Initialize(_uint iStateNum, void* pArg)
{
    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_RebornerBigA_SlashJump::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_SWIPATTACK, false, 0.1f, 0, true);
    m_bSwingSound = { false };
    return S_OK;
}

void CState_RebornerBigA_SlashJump::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (End_Check())
    {
        m_pMonster->SetUp_Animation(51, true, 0, true);
        m_pMonster->Get_Model()->Play_Animation(0);
        m_pMonster->Get_Transform()->Look_Dir(-(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK)));
        m_pMonster->Change_State(CMonster::IDLE);
        return;
    }

    Collider_Check(CurTrackPos);
    Sound_Check(CurTrackPos);

    //if (CurTrackPos <= 45.f)
    //{
    //    m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1, fTimeDelta);
    //}

}

void CState_RebornerBigA_SlashJump::End_State()
{
}

_bool CState_RebornerBigA_SlashJump::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_SWIPATTACK);
}

void CState_RebornerBigA_SlashJump::Collider_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 50.f && CurTrackPos <= 57.f))
    {
        m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
    }
    else
    {
        m_pMonster->DeActive_CurretnWeaponCollider();
    }
}

void CState_RebornerBigA_SlashJump::Sound_Check(_double CurTrackPos)
{
    if (!m_bSwingSound)
    {
        if ((CurTrackPos >= 50.f && CurTrackPos <= 57.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_Staff_03.wav"), false);
            m_bSwingSound = true;
        }
    }
}

CState_RebornerBigA_SlashJump* CState_RebornerBigA_SlashJump::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerBigA_SlashJump* pInstance = new CState_RebornerBigA_SlashJump(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerBigA_SlashJump"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerBigA_SlashJump::Free()
{
    __super::Free();
}