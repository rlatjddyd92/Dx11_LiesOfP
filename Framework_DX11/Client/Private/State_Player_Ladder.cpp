#include "stdafx.h"
#include "State_Player_Ladder.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Weapon.h"
#include "Ladder.h"

CState_Player_Ladder::CState_Player_Ladder(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Ladder::Initialize(_uint iStateNum, void* pArg)
{
    // 올라가유
    m_iAnimation_Ladder[LADDER_UP][0] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Start_Up", 2.f);
    m_iAnimation_Ladder[LADDER_UP][1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Up_Left", 2.f);
    m_iAnimation_Ladder[LADDER_UP][2] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Up_Right", 2.f);
    m_iAnimation_Ladder[LADDER_UP][3] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_End_Up_Left", 2.f);
    m_iAnimation_Ladder[LADDER_UP][4] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_End_Up_Right", 2.f);

    // 내려가유
    m_iAnimation_Ladder[LADDER_DOWN][0] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Start_Down", 2.f);
    m_iAnimation_Ladder[LADDER_DOWN][1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Down_Left", 2.f);
    m_iAnimation_Ladder[LADDER_DOWN][2] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Down_Right", 2.f);
    m_iAnimation_Ladder[LADDER_DOWN][3] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_End_Down_Left", 2.f);
    m_iAnimation_Ladder[LADDER_DOWN][4] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_End_Down_Right", 2.f);

    // 기다려유
    m_iAnimation_LadderIdle[0] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Idle_Left", 2.f);
    m_iAnimation_LadderIdle[1] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Ladder_Idle_Right", 2.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    m_iSoundFrame = 30;

    return S_OK;
}

HRESULT CState_Player_Ladder::Start_State(void* pArg)
{
    CLadder* pLadder = static_cast<CLadder*>(pArg);

    //_Vec3* vLadderPos = static_cast<_Vec3*>(pArg);
    Choice_UpDown(pLadder);
    m_isInputW = false;
    m_isInputS = false;
    
    m_pPlayer->Change_Animation(m_iAnimation_Ladder[m_eUpDownType][0], false, 0.f);

    m_pPlayer->Get_RigidBody()->Set_IsOnCell(false);
    m_pPlayer->Get_RigidBody()->Set_IsLockCell(false);

    m_pPlayer->Disappear_Weapon();

    return S_OK;
}

void CState_Player_Ladder::Update(_float fTimeDelta)
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();

    if (KEY_TAP(KEY::W) || KEY_HOLD(KEY::W))
    {
        m_isInputW = true;
        m_isInputS = false;
    }
    else if (KEY_TAP(KEY::S) || KEY_HOLD(KEY::S))
    {
        m_isInputS = true;
        m_isInputW = false;
    }

    if (m_pPlayer->Get_EndAnim(iCurAnim))
    {
        if (iCurAnim == m_iAnimation_Ladder[LADDER_UP][3] || iCurAnim == m_iAnimation_Ladder[LADDER_UP][4]
            || iCurAnim == m_iAnimation_Ladder[LADDER_DOWN][3] || iCurAnim == m_iAnimation_Ladder[LADDER_DOWN][4])
        {
            m_pPlayer->Change_State(CPlayer::OH_IDLE);
        }
        else
        {
            if (iCurAnim == m_iAnimation_Ladder[LADDER_DOWN][0])
            {
                m_iHandIndex -= 1;
                _Vec3 vLadderLook = m_pLadder->Get_Transform()->Get_State(CTransform::STATE_LOOK);
                m_pPlayer->Get_Transform()->Set_NewLook(-vLadderLook);
            }
            m_iHandIndex = ((m_iHandIndex + 1) % 2);
            m_pPlayer->Change_Animation(m_iAnimation_LadderIdle[m_iHandIndex], true, 0.f);
        }

        m_isInputW = false;
        m_isInputS = false;
    }
    else if (iCurAnim == m_iAnimation_LadderIdle[0]
        || iCurAnim == m_iAnimation_LadderIdle[1])
    {
        if (m_isInputW)
        {
            _Vec3 vDiff = m_pLadder->Get_LadderUpPos() - m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
            _float fDiff = vDiff.Length();
            if (m_pPlayer->Get_IsLadderEnd() && vDiff.Length() <= 2.f)
            {
                m_iHandIndex = ((m_iHandIndex) % 2) + 3;
                m_eUpDownType = LADDER_UP;
                m_pPlayer->Change_Animation(m_iAnimation_Ladder[m_eUpDownType][m_iHandIndex], false, 0.f);
            }
            else
            {
                m_iHandIndex = ((m_iHandIndex + 1) % 2) + 1;
                m_eUpDownType = LADDER_UP;
                m_pPlayer->Change_Animation(m_iAnimation_Ladder[m_eUpDownType][m_iHandIndex], false, 0.f);
            }
        }
        else if (m_isInputS)
        {
            _Vec3 vDiff = m_pLadder->Get_LadderDonwPos() - m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
            _float fDiff = vDiff.Length();
            if (m_pPlayer->Get_IsLadderEnd() && vDiff.Length() <= 2.f)
            {
                m_iHandIndex = ((m_iHandIndex) % 2) + 3;
                m_eUpDownType = LADDER_DOWN;
                m_pPlayer->Change_Animation(m_iAnimation_Ladder[m_eUpDownType][m_iHandIndex], false, 0.f);
            }
            else
            {
                m_iHandIndex = ((m_iHandIndex + 1) % 2) + 1;
                m_eUpDownType = LADDER_DOWN;
                m_pPlayer->Change_Animation(m_iAnimation_Ladder[m_eUpDownType][m_iHandIndex], false, 0.f);
            }
        }

        m_isInputW = false;
        m_isInputS = false;
    }
    else if (iCurAnim != m_iAnimation_Ladder[LADDER_UP][0] && iCurAnim != m_iAnimation_Ladder[LADDER_DOWN][0]
        && iCurAnim != m_iAnimation_Ladder[LADDER_UP][3] && iCurAnim != m_iAnimation_Ladder[LADDER_DOWN][3]
        && iCurAnim != m_iAnimation_Ladder[LADDER_UP][4] && iCurAnim != m_iAnimation_Ladder[LADDER_DOWN][4])
    {
        _Vec3 vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        if (m_eUpDownType == LADDER_UP)
            vPlayerPos.y += 0.95f * fTimeDelta;
        else
            vPlayerPos.y -= 0.95f * fTimeDelta;

        m_pPlayer->Get_RigidBody()->Set_GloblePose(vPlayerPos);
    }

    if (KEY_TAP(KEY::Q))
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
}

void CState_Player_Ladder::End_State()
{
    _Vec3 vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    m_pPlayer->Get_Navigation()->Research_Cell(vPlayerPos);

    m_pPlayer->Get_RigidBody()->Set_IsOnCell(true);
    m_pPlayer->Get_RigidBody()->Set_IsLockCell(true);

    m_pPlayer->Appear_Weapon();
}

void CState_Player_Ladder::Choice_UpDown(CLadder* pLadder)
{
    m_pLadder = pLadder;

    _Vec3 vLadderPos = pLadder->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _Vec3 vLadderLook = pLadder->Get_Transform()->Get_State(CTransform::STATE_LOOK);
    _Vec3 vUpPos = pLadder->Get_LadderUpPos();
    _Vec3 vDownPos = pLadder->Get_LadderDonwPos();
    _Vec3 vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _Vec3 vLadderColliderPos;
    
    if ((vUpPos - vPlayerPos).Length() < (vDownPos - vPlayerPos).Length())
        vLadderColliderPos = vUpPos;
    else
        vLadderColliderPos = vDownPos;

    _Vec3 vInitPos;
    _float fHeightDiff = vLadderPos.y - vPlayerPos.y;
    if (fHeightDiff  < -1.5f)
    {
        m_eUpDownType = LADDER_DOWN;
        m_iHandIndex = 1;

        vLadderLook.Normalize();
        m_pPlayer->Get_Transform()->Set_NewLook(vLadderLook);
        vLadderColliderPos -= vLadderLook * 1.15f;
        vInitPos.x = vLadderColliderPos.x;
        vInitPos.z = vLadderColliderPos.z;

        if (pLadder->Get_LadderTag() == "LV_Ladder_MetalWood_01_KSJ")// 긴놈
        {
            vInitPos.y = vPlayerPos.y;
        }
        else
        {
            vInitPos.y = vPlayerPos.y + 0.1f;
        }
    }
    else
    {
        m_eUpDownType = LADDER_UP;
        m_iHandIndex = 0;

        vLadderLook.Normalize();
        m_pPlayer->Get_Transform()->Set_NewLook(-vLadderLook);

        vInitPos.x = vLadderColliderPos.x;
        vInitPos.z = vLadderColliderPos.z;

        if (pLadder->Get_LadderTag() == "LV_Ladder_MetalWood_01_KSJ")// 긴놈
        {
            vInitPos.y = vPlayerPos.y + 0.05f;
        }
        else
        {
            vInitPos.y = vPlayerPos.y - 0.05f;
        }
    }
    m_pPlayer->Get_RigidBody()->Set_GloblePose(vInitPos);
}

_bool CState_Player_Ladder::End_Check()
{
    return true;
}

void CState_Player_Ladder::Control_Sound()
{
    _int iFrame = m_pPlayer->Get_Frame();

    if ((iFrame == m_iSoundFrame || iFrame == m_iSoundFrame + 1) && !m_isPlaySound)
    {
        m_pPlayer->Play_CurrentWeaponSound(CWeapon::WEP_SOUND_EFFECT1, TEXT("SE_PC_SK_FX_Rapier_1H_H_FableArts_Parry_01.wav"));
        m_isPlaySound = true;
    }
    else
    {
        m_isPlaySound = false;
    }
}

CState_Player_Ladder* CState_Player_Ladder::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Ladder* pInstance = new CState_Player_Ladder(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Ladder"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Ladder::Free()
{
    __super::Free();
}