#include "stdafx.h"
#include "State_Player_Hit.h"
#include "State_Player_Die.h"
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
    m_iAnimation_Hit[HIT_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_B", 2.6f);
    m_iAnimation_Hit[HIT_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_L", 2.6f);
    m_iAnimation_Hit[HIT_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_R", 2.6f);
    m_iAnimation_Hit[HIT_FB] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_FtoB", 2.6f);
    m_iAnimation_Hit[HIT_LR] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_LtoR", 2.6f);
    m_iAnimation_Hit[HIT_RL] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_RtoL", 2.6f);

    m_iAnimation_Down[DOWN_DRAG_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Drag_Intro_B", 2.3f);
    m_iAnimation_Down[DOWN_STAMP_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Stamp_Intro_B", 2.3f);
    m_iAnimation_Down[DOWN_DRAG_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Drag_Intro_F", 2.3f);
    m_iAnimation_Down[DOWN_STAMP_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Stamp_Intro_F", 2.3f);

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
            m_pPlayer->Change_Animation(m_iAnimation_Down[eType], false, 0.05f);

        m_pPlayer->Set_IsInvicible(true);

        m_eDownType = eType;
    }
    else
    {
        HIT_TYPE eType = (HIT_TYPE)Choice_HitAnim(pDesc->vHitPos);

        if (eType == m_eHitType)
            m_pPlayer->Change_Animation(m_iAnimation_Hit[eType], false, 0.05f, 0, true, true);
        else
            m_pPlayer->Change_Animation(m_iAnimation_Hit[eType], false, 0.05f);

        m_eHitType = eType;
        m_iChangeFrame = m_pPlayer->Get_Model()->Get_LastFrame_CurrentAnim(m_iAnimation_Hit[m_eHitType]) - 30;
    }

    
    m_pPlayer->Change_Weapon();
    m_pPlayer->Combine_Scissor();

    _float fSoundNum = m_pGameInstance->Get_Random(0.f,3.f);
    Random_Sound_Play(fSoundNum);

    return S_OK;
}

void CState_Player_Hit::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_isDown)
    {
        if (End_Check())
        {
            if (m_pPlayer->Get_Player_Stat().vGauge_Hp.x <= 0.f)
            {
                _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
                CState_Player_Die::DIE_DESC Desc{};
                Desc.isFront = true;

                if (m_iAnimation_Down[DOWN_DRAG_B] == iCurAnim
                    || m_iAnimation_Down[DOWN_STAMP_B] == iCurAnim)
                {
                    Desc.isFront = false;
                }
                m_pPlayer->Change_State(CPlayer::DIE, &Desc);
            }
            else
            {
                _int iType = m_eDownType / 2;
                m_pPlayer->Change_State(CPlayer::GETUP, &iType);
            }
        }
    }
    else
    {
        if (m_iChangeFrame > iFrame)
        {
            if (m_pPlayer->Key_Hold(KEY::LSHIFT))
            {
                _uint iWeponType = m_pPlayer->Get_WeaponType();

                if (iWeponType < 2)
                    m_pPlayer->Change_State(CPlayer::OH_GUARD);
                else
                    m_pPlayer->Change_State(CPlayer::TH_GUARD);
            }
        }

        if (End_Check())
        {
            _uint iWeponType = m_pPlayer->Get_WeaponType();

            if (iWeponType < 2)
                m_pPlayer->Change_State(CPlayer::OH_IDLE);
            else
                m_pPlayer->Change_State(CPlayer::TH_IDLE);
        }
    }
}

void CState_Player_Hit::End_State()
{
    m_pPlayer->Set_IsInvicible(false);
    m_pPlayer->Set_IsGuard(false);
    m_pPlayer->Stop_Sound(CPlayer::PAWN_SOUND_EFFECT1);
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
    _float fForwardDot = vHitDir.Dot(-vLook);

    m_isHitFront = true;

    // 위쪽에서 때림
    if (fUpDot > 0.7f && vHitPos.y - PlayerWorldMatrix.Translation().y > 1.5f)
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
        if (fForwardDot > 0.f)
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
    else if(m_iAnimation_Down[DOWN_DRAG_B] == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Down[DOWN_DRAG_B]);
    } 
    else if (m_iAnimation_Down[DOWN_STAMP_B] == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Down[DOWN_STAMP_B]);
    }
    else if (m_iAnimation_Down[DOWN_DRAG_F] == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Down[DOWN_DRAG_F]);
    }
    else if (m_iAnimation_Down[DOWN_STAMP_F] == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Down[DOWN_STAMP_F]);
    }

    return bEndCheck;
}

void CState_Player_Hit::Random_Sound_Play(_float fSoundNum)
{
    if (fSoundNum > 2.f)
    {
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("VO_PC_Dmg_HL4_02.wav"));
    }
    else if (fSoundNum > 1.f)
    {
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("VO_PC_Dmg_HL4_03.wav"));
    }
    else
    {
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("VO_PC_Dmg_HL4_07.wav"));
    }
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