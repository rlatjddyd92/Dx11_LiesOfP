#include "stdafx.h"
#include "State_Player_Rapier_RAttack00.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "Effect_Manager.h"
#include "Weapon.h"

CState_Player_Rapier_RAttack00::CState_Player_Rapier_RAttack00(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Rapier_RAttack00::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_RapierSA1 = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_O_Rapier_SA1", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pIsEndAnim = pDesc->pIsEndAnim;
    m_pResetRootMove = pDesc->pIsResetRootMove;
    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iChangeFrame = 45;
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Rapier_RAttack00::Start_State(void* pArg)
{
    if(m_pFsm->Get_PrevState() == CPlayer::OH_IDLE)
        m_pPlayer->Change_Animation(m_iAnimation_RapierSA1, false);
    else
        m_pPlayer->Change_Animation(m_iAnimation_RapierSA1, false, 0.1f, 5);

    m_isInputLButton = false;
    m_isInputRButton = false;
    m_fRButtonTime = 0.f;

    return S_OK;
}

void CState_Player_Rapier_RAttack00::Update(_float fTimeDelta)
{
    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame == 30)
    {
        _Vec3 vPos = m_pPlayer->Get_CurrentWeapon()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        _Vec3 vRot = _Vec3(0.f, 0.f, 0.f);
        _Vec3 vScale = _Vec3(1.f, 1.f, 1.f);
        CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Rapier_Normal"), vPos, vRot, vScale);
    }

    if (iFrame < m_iChangeFrame)
    {
        if (KEY_TAP(KEY::LBUTTON))
        {
            m_isInputLButton = true;
            m_isInputRButton = false;
        }
        else if (KEY_TAP(KEY::RBUTTON))
        {
            m_isInputRButton = true;
            m_isInputLButton = false;
            m_fRButtonTime = 0.f;
        }
        else if (KEY_HOLD(KEY::RBUTTON))
        {
            m_fRButtonTime += fTimeDelta;
        }
    }

    if (m_iChangeFrame < iFrame && iFrame < m_iChangeFrame + 15)
    {
        if (m_isInputLButton)
            m_pPlayer->Change_State(CPlayer::RAPIER_LATTACK0);
        else if (m_isInputRButton)
        {
            if (m_fRButtonTime > 0.15f)
                m_pPlayer->Change_State(CPlayer::RAPIER_CHARGE);
            else
                m_pPlayer->Change_State(CPlayer::RAPIER_RATTACK0);
        }
    }
    else if (End_Check())
    {
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }
}

void CState_Player_Rapier_RAttack00::End_State()
{
}

_bool CState_Player_Rapier_RAttack00::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_RapierSA1);
}

CState_Player_Rapier_RAttack00* CState_Player_Rapier_RAttack00::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Rapier_RAttack00* pInstance = new CState_Player_Rapier_RAttack00(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Rapier_RAttack00"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Rapier_RAttack00::Free()
{
    __super::Free();
}