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

    m_iAnimation_Down[DOWN_DRAG_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Drag_Intro_B", 2.5f);
    m_iAnimation_Down[DOWN_STAMP_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Stamp_Intro_B", 2.5f);
    m_iAnimation_Down[DOWN_DRAG_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Drag_Intro_F", 2.5f);
    m_iAnimation_Down[DOWN_STAMP_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Stamp_Intro_F", 2.5f);

    m_iAnimation_GetUp[0] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Getup_B", 2.5f);
    m_iAnimation_GetUp[1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Getup_F", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Hit::Start_State(void* pArg)
{
    HIT_DESC* pDesc = static_cast<HIT_DESC*>(pArg);
 
    m_isDown = pDesc->isDown;
    if (m_isDown)
    {
        DOWN_TYPE eType = (DOWN_TYPE)Choice_DonwAnim(pDesc->vHitPos);
        if(eType != m_eHitType)
            m_pPlayer->Change_Animation(m_iAnimation_Down[eType], false, 0.1f);

        m_eDownType = eType;
    }
    else
    {
        HIT_TYPE eType = (HIT_TYPE)Choice_HitAnim(pDesc->vHitPos);

        if (eType == m_eHitType)
            m_pPlayer->Change_Animation(m_iAnimation_Hit[eType], false, 0.f, 0, true, true);
        else
            m_pPlayer->Change_Animation(m_iAnimation_Hit[eType], false, 0.f);

        m_eHitType = eType;
    }

    return S_OK;
}

void CState_Player_Hit::Update(_float fTimeDelta)
{
    if (m_isDown)
    {

    }
    else
    {
        if (End_Check())
        {
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        }
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

    m_isHitFront = true;

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
        m_isHitFront = false;
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

    return HIT_FB;
}

_uint CState_Player_Hit::Choice_DonwAnim(_Vec3 vHitPos)
{
    _Matrix PlayerWorldMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix();
    _Vec3 vHitDir = vHitPos - PlayerWorldMatrix.Translation();
    vHitDir.Normalize();

    _Vec3 vRight = PlayerWorldMatrix.Right();
    _Vec3 vUp = PlayerWorldMatrix.Up();
    _Vec3 vLook = PlayerWorldMatrix.Forward();

    _float fUpDot = vHitDir.Dot(vUp);
    _float fForwardDot = vHitDir.Dot(vLook);

    m_isHitFront = true;

    // 위쪽에서 때림
    if (fUpDot > 0.7f) 
    {
        if (fForwardDot > 0.5f)
        {
            return DOWN_STAMP_F;
        }
        else
        {
            return DOWN_STAMP_B;
        }
    }
    else
    {
        if (fForwardDot > 0.5f)
        {
            return DOWN_DRAG_F;
        }
        else
        {
            return DOWN_DRAG_B;
        }
    }

    return DOWN_STAMP_F;
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