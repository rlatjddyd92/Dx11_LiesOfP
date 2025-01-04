#include "stdafx.h"
#include "State_Player_Die.h"
#include "State_Player_Teleport.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "GameInterface_Controller.h"
#include "Effect_Manager.h"

CState_Player_Die::CState_Player_Die(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Die[HIT_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_Dead_B", 2.5f);
    m_iAnimation_Die[HIT_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_Dead_F", 2.5f);
    m_iAnimation_Die[DOWN_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Dead_B", 2.5f);
    m_iAnimation_Die[DOWN_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Dead_F", 2.5f);
    m_iAnimation_Die[CURSE] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Curse_Dead", 2.5f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Die::Start_State(void* pArg)
{
    DIE_DESC* pDesc = static_cast<DIE_DESC*>(pArg);

    m_iAnimIndex = Choice_DieAnim(pDesc);

    m_pPlayer->Change_Animation(m_iAnimation_Die[m_iAnimIndex], false, 0.1f);
    m_pPlayer->Damaged(9999999.f);

    m_pPlayer->Change_Weapon();
    m_pPlayer->Combine_Scissor();
    m_pPlayer->SetUp_Die();

    m_isDeadEnd = false;
    m_isFadeOut = false;
    m_isAppearStartEffect = false;

    m_vRimLightColor = _Vec4(0.f, 0.f, 0.f, 0.5f);

    m_fDieTime = 0.f;
    m_fDissloveRatio = 0.f;

    m_bSoundPlay = false;
    m_bSoundPlay2 = true;

    switch (m_iAnimIndex)
    {
    case HIT_B:
        m_iSoundPlayFrame = 106;
        break;
    case  HIT_F:
        m_iSoundPlayFrame = 80;
        break;
    case DOWN_B:
        m_iSoundPlayFrame = 180;
        break;
    case  DOWN_F:
        m_iSoundPlayFrame = 120;
        break;
    case CURSE:
        m_iSoundPlayFrame = 350;
        break;
    }

    return S_OK;
}

void CState_Player_Die::Update(_float fTimeDelta)
{
    if (m_isDeadEnd)
    {
        if(m_isAppearStartEffect)
        {
            m_fDissloveRatio += fTimeDelta;
            m_vRimLightColor.z = max(m_vRimLightColor.z + fTimeDelta, 1.f);
            m_vRimLightColor.w = max(m_vRimLightColor.w - 0.6f * fTimeDelta, 0.1f);
        }

        m_fDieTime += fTimeDelta;
        if (!m_isAppearStartEffect)
        {
            //CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Teleport_Depart"), (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION));
            m_pPlayer->On_DissolveEffect(CPlayer::DISSOLVE_DEAD, true);
            m_isAppearStartEffect = true;
        }
        else if (m_fDieTime > 0.4f && !m_isFadeOut)
        {
            GET_GAMEINTERFACE->Fade_Out(TEXT(""), TEXT(""));
            m_isFadeOut = true;
        }
        else if (m_fDieTime > 1.5f && m_isFadeOut)
        {
            CState_Player_Teleport::TELEPORT_DESC Desc{};
            Desc.isDie = true;

            m_pPlayer->Change_State(CPlayer::TELEPORT, &Desc);
        }
    }

    //죽으면 이전 별바라기로
    if (End_Check())
    {
        m_isDeadEnd = true;
    }

    m_pPlayer->Set_DissloveRatio(m_fDissloveRatio);
    m_pPlayer->Set_RimLightColor(m_vRimLightColor);

    if (m_bSoundPlay == false)
    {
        m_bSoundPlay = true;
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT1, TEXT("VO_PC_Die_HL3_04.wav"));
    }

    _int iFrame = m_pPlayer->Get_Frame();

    if (iFrame >= m_iSoundPlayFrame && m_bSoundPlay2)
    {
        m_pPlayer->Play_Sound(CPlayer::PAWN_SOUND_EFFECT2, TEXT("SE_PC_MT_Land_Die_Heavy_01.wav"));
        m_bSoundPlay2 = false;
    }

}

void CState_Player_Die::End_State()
{
    m_pPlayer->Recovery_HP(9999999.f);
    m_pPlayer->SetUp_Die();

    m_pPlayer->Set_IsGuard(false);
    m_pPlayer->Set_IsArm(false);
    m_pPlayer->Set_IsParry(false);
}

_uint CState_Player_Die::Choice_DieAnim(DIE_DESC* pDesc)
{  
    if (nullptr != pDesc)
    {
        if (pDesc->vHitPos.Length() > 0.f)
        {
            _Matrix PlayerWorldMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix();
            _Vec3 vHitDir = pDesc->vHitPos - PlayerWorldMatrix.Translation();
            vHitDir.Normalize();

            _float fForwardDot = vHitDir.Dot(PlayerWorldMatrix.Forward()); // 앞뒤 방향

            if (fForwardDot > 0.f)
            {
                return HIT_F;
            }
            else if (fForwardDot < -0.f)
            {
                return HIT_B;  // 뒤쪽
            }
        }
        // 넘어져 있다는 상태임
        else if (m_pFsm->Get_PrevState() == CPlayer::HIT)
        {
            if (pDesc->isFront)
            {
                return DOWN_F;
            }
            else
            {
                return DOWN_B;
            }
        }
    }

    return HIT_F;
}

_bool CState_Player_Die::End_Check()
{
    _uint iCurAnim = m_pPlayer->Get_CurrentAnimIndex();

    return m_pPlayer->Get_EndAnim(iCurAnim);
}

CState_Player_Die* CState_Player_Die::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_Die* pInstance = new CState_Player_Die(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_Die::Free()
{
    __super::Free();
}