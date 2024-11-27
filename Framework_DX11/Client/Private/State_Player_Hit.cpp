#include "stdafx.h"
#include "State_Player_Hit.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Hit::CState_Player_Hit(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Hit::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Hit[HIT_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_B", 2.5f);
    m_iAnimation_Hit[HIT_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_L", 2.5f);
    m_iAnimation_Hit[HIT_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_R", 2.5f);
    m_iAnimation_Hit[HIT_FB] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_FtoB", 2.5f);
    m_iAnimation_Hit[HIT_LR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_LtoR", 2.5f);
    m_iAnimation_Hit[HIT_RL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_RtoL", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Hit::Start_State(void* pArg)
{
    _Vec3* vHitPos = static_cast<_Vec3*>(pArg);
 
    HIT_TYPE eType = (HIT_TYPE)Choice_HitAnim(*vHitPos);

    if (eType == m_eHitType)
        m_pPlayer->Change_Animation(m_iAnimation_Hit[m_eHitType], false, 0.f, 0, true, true);
    else
        m_pPlayer->Change_Animation(m_iAnimation_Hit[m_eHitType], false, 0.f);

    m_eHitType = eType;

    return S_OK;
}

void CState_Player_Hit::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }
}

void CState_Player_Hit::End_State()
{
    m_pPlayer->Set_IsGuard(false);
}

_uint CState_Player_Hit::Choice_HitAnim(_Vec3 vHitPos)
{
    _Matrix PlayerWorldMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix();
    _Vec3 vHitDir = vHitPos - PlayerWorldMatrix.Translation();
    vHitDir.Normalize();

    _float fForwardDot = vHitDir.Dot(PlayerWorldMatrix.Forward()); // 앞뒤 방향
    _float fRightDot = vHitDir.Dot(PlayerWorldMatrix.Right());     // 좌우 방향

    if (fForwardDot > 0.5f)
    {
        if (fRightDot > 0.2f)
        {
            return HIT_R; // 정면 + 살짝 오른쪽
        }
        else if (fRightDot < -0.2f)
        {
            return HIT_L; // 정면 + 살짝 왼쪽
        }
        else
        {
            return HIT_FB; // 완전 정면
        }
    }
    else if (fForwardDot < -0.5f)
    {
        return HIT_B;  // 뒤쪽
    }
    else {
        if (fRightDot > 0.5f)
        {
            return HIT_RL;
        }
        else if (fRightDot < -0.5f)
        {
            return HIT_LR;
        }
    }
}

_bool CState_Player_Hit::End_Check()
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((m_iAnimation_Hit[HIT_B]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_B]);
    }
    else if ((m_iAnimation_Hit[HIT_L]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_L]);
    }
    else if ((m_iAnimation_Hit[HIT_R]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_R]);
    }
    else if ((m_iAnimation_Hit[HIT_FB]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_FB]);
    }
    else if ((m_iAnimation_Hit[HIT_LR]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_LR]);
    }
    else if ((m_iAnimation_Hit[HIT_RL]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Hit[HIT_RL]);
    }
    else
    {

    }
    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

CState_Player_Hit* CState_Player_Hit::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Hit* pInstance = new CState_Player_Hit(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Hit"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Hit::Free()
{
    __super::Free();
}