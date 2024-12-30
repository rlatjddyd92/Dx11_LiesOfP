#include "stdafx.h"
#include "State_Player_OH_Dash.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_OH_Dash::CState_Player_OH_Dash(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_OH_Dash::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Dash[DASH_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_Normal_F", 2.5f);
    m_iAnimation_Dash[DASH_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_Normal_B", 2.5f);
    m_iAnimation_Dash[DASH_FOCUS_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_FocusHurt_F", 2.5f);
    m_iAnimation_Dash[DASH_FOCUS_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_FocusHurt_B", 2.5f);
    m_iAnimation_Dash[DASH_FOCUS_L] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_FocusHurt_L", 2.5f);
    m_iAnimation_Dash[DASH_FOCUS_R] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Dash_FocusHurt_R", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_OH_Dash::Start_State(void* pArg)
{
    Select_DashAnimation();

    m_pPlayer->Set_IsInvicible(true);
    m_pPlayer->Set_MoveSpeed(5.f);

    //m_pPlayer->Get_RigidBody()->Set_Friction(_float3(10.f, 0.f, 10.f));
  
    m_isPlaySound = false;
    m_isPlaySound2 = false;

    return S_OK;
}

void CState_Player_OH_Dash::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }

    Control_Sound();
}

void CState_Player_OH_Dash::End_State()
{
    m_pPlayer->Set_IsInvicible(false);
}

void CState_Player_OH_Dash::Select_DashAnimation()
{
    if (m_pPlayer->Get_IsLockOn())
    {
        if (m_pPlayer->Key_Hold(KEY::W))
        {
            m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_FOCUS_F], false, 0.f);
            m_iDashTypeNum = DASH_FOCUS_F;
        }
        else if (m_pPlayer->Key_Hold(KEY::A))
        {
            m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_FOCUS_L], false, 0.f);
            m_iDashTypeNum = DASH_FOCUS_L;
        }
        else if (m_pPlayer->Key_Hold(KEY::D))
        {
            m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_FOCUS_R], false, 0.f);
            m_iDashTypeNum = DASH_FOCUS_R;
        }
        else
        {
            m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_FOCUS_B], false, 0.f);
            m_iDashTypeNum = DASH_FOCUS_B;
        }
    }
    else
    {
        if (m_pFsm->Get_PrevState() == CPlayer::OH_RUN || m_pFsm->Get_PrevState() == CPlayer::OH_SPRINT)
        {
            m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_F], false, 0.f);
            m_iDashTypeNum = DASH_F;
            return;
        }

        m_pPlayer->Change_Animation(m_iAnimation_Dash[DASH_B], false, 0.f);
        m_iDashTypeNum = DASH_B;
    }

    Control_Invicible();
}

_bool CState_Player_OH_Dash::End_Check()
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((m_iAnimation_Dash[DASH_FOCUS_F]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Dash[DASH_FOCUS_F]);
    }
    else if ((m_iAnimation_Dash[DASH_FOCUS_B]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Dash[DASH_FOCUS_B]);
    }
    else if ((m_iAnimation_Dash[DASH_FOCUS_L]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Dash[DASH_FOCUS_L]);
    }
    else if ((m_iAnimation_Dash[DASH_FOCUS_R]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Dash[DASH_FOCUS_R]);
    }
    else if ((m_iAnimation_Dash[DASH_F]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Dash[DASH_F]);
    }
    else if ((m_iAnimation_Dash[DASH_B]) == iCurAnim)
    {
        bEndCheck = m_pPlayer->Get_EndAnim(m_iAnimation_Dash[DASH_B]);
    }
    else
    {

    }
        //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

void CState_Player_OH_Dash::Control_Invicible()
{
    m_iCurAnim = m_pPlayer->Get_CurrentAnimIndex();
    _int iFrame = m_pPlayer->Get_Frame();

    _bool isInviclible = false;

    if (m_iCurAnim == m_iAnimation_Dash[DASH_F])
    {
        if (10 <= iFrame && iFrame <= 50)
        {
            isInviclible = true;
        }
    }
    else if (m_iCurAnim == m_iAnimation_Dash[DASH_B])
    {
        if (2 <= iFrame && iFrame <= 30)
        {
            isInviclible = true;
        }
    }
    else if (m_iCurAnim == m_iAnimation_Dash[DASH_FOCUS_F]
        || m_iCurAnim == m_iAnimation_Dash[DASH_FOCUS_B])
    {
        if (2 <= iFrame && iFrame <= 40)
        {
            isInviclible = true;
        }
    }
    else if (m_iCurAnim == m_iAnimation_Dash[DASH_FOCUS_L]
        || m_iCurAnim == m_iAnimation_Dash[DASH_FOCUS_R])
    {
        if (2 <= iFrame && iFrame <= 40)
        {
            isInviclible = true;
        }
    }

    m_pPlayer->Set_IsInvicible(isInviclible);
}

void CState_Player_OH_Dash::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (m_iDashTypeNum == DASH_F)
    {
        if (iFrame >= 5 && !m_isPlaySound)
        {
            m_isPlaySound = true;
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Rustle_Dash_Normal_HL1_01.wav"));
        }
    }
    else if (m_iDashTypeNum == DASH_B)
    {
        if (iFrame >= 4 && !m_isPlaySound)
        {
            m_isPlaySound = true;
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Rustle_Dash_Normal_HL1_03.wav"));
        }
    }   
    else if (m_iDashTypeNum == DASH_FOCUS_F)
    {
        if (iFrame >= 5 && !m_isPlaySound)
        {
            m_isPlaySound = true;
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Rustle_Dash_Normal_HL1_01.wav"));
        }
        if (iFrame >= 20 && !m_isPlaySound2)
        {
            m_isPlaySound2 = true;
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_BodyFall_Dirt_Slide_01.wav"));
        }
    } 
    else if (m_iDashTypeNum == DASH_FOCUS_B)
    {
        if (iFrame >= 0 && !m_isPlaySound)
        {
            m_isPlaySound = true;
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Rustle_Dash_Normal_HL1_01.wav"));
        }
        if (iFrame >= 15 && !m_isPlaySound2)
        {
            m_isPlaySound2 = true;
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_BodyFall_Dirt_Slide_01.wav"));
        }
    }
    else if (m_iDashTypeNum == DASH_FOCUS_L)
    {
        if (iFrame >= 0 && !m_isPlaySound)
        {
            m_isPlaySound = true;
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Rustle_Dash_Normal_HL1_01.wav"));
        }
        if (iFrame >= 20 && !m_isPlaySound2)
        {
            m_isPlaySound2 = true;
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_BodyFall_Dirt_Slide_01.wav"));
        }
    }
    else if (m_iDashTypeNum == DASH_FOCUS_R)
    {
        if (iFrame >= 0 && !m_isPlaySound)
        {
            m_isPlaySound = true;
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_Rustle_Dash_Normal_HL1_01.wav"));
        }
        if (iFrame >= 15 && !m_isPlaySound2)
        {
            m_isPlaySound2 = true;
            m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("SE_PC_MT_BodyFall_Dirt_Slide_01.wav"));
        }
    }
}

CState_Player_OH_Dash* CState_Player_OH_Dash::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_OH_Dash* pInstance = new CState_Player_OH_Dash(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_OH_Dash"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_OH_Dash::Free()
{
    __super::Free();
}