#include "stdafx.h"
#include "State_Player_Arm_Counter.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"

CState_Player_Arm_Counter::CState_Player_Arm_Counter(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Arm_Counter::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_Arm_Counter = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Arm_GuardEx_Guard_Counter", 2.6f);

    m_iStateNum = iStateNum;

    m_iColliderStartFrame = 96;
    m_iColliderEndFrame = 101;

    m_iActiveEffectFrame = 10;
    m_iDeActiveEffectFrame = 90;

    m_isChangeLoop = false;

    return S_OK;
}

HRESULT CState_Player_Arm_Counter::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_Arm_Counter, false, 0.15f, 0, true, true);

    m_isActiveEffect = false;

    return S_OK;
}

void CState_Player_Arm_Counter::Update(_float fTimeDelta)
{
    _int iFrmae = m_pPlayer->Get_Frame();

    if (End_Check())
    {
        m_isChangeLoop = true;
        m_pPlayer->Change_State(CPlayer::ARM_LOOP);
        return;
    }

    Control_Collider();
    Control_Effect(iFrmae);
}

void CState_Player_Arm_Counter::End_State()
{
    if (!m_isChangeLoop)
        m_pPlayer->Get_Model()->Set_RemoteTuning(true);
}

_bool CState_Player_Arm_Counter::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_Arm_Counter);
}

void CState_Player_Arm_Counter::Control_Collider()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iColliderStartFrame <= iFrame && iFrame <= m_iColliderEndFrame)
    {
        if (m_pPlayer->Active_Arm())
        {
            m_pPlayer->Decrease_Stamina(30.f);
        }
    }
    else
    {
        m_pPlayer->DeActive_Arm();
    }
}

void CState_Player_Arm_Counter::Control_Effect(_int iFrame)
{
    if (!m_isActiveEffect && m_iActiveEffectFrame <= iFrame)
    {
        m_pPlayer->Active_Effect(CPlayer::EFFECT_ARM_COUNTER_CHARGE);
        m_isActiveEffect = true;
    }
    else if (m_isActiveEffect && m_iDeActiveEffectFrame <= iFrame)
    {
        m_pPlayer->DeActive_Effect(CPlayer::EFFECT_ARM_COUNTER_CHARGE);
    }
}

CState_Player_Arm_Counter* CState_Player_Arm_Counter::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Arm_Counter* pInstance = new CState_Player_Arm_Counter(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Arm_Counter"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Arm_Counter::Free()
{
    __super::Free();
}
