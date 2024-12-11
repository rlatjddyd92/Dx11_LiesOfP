#include "stdafx.h"
#include "State_Player_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"

CState_Player_Die::CState_Player_Die(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iAnimation_Die[HIT_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_Dead_B", 2.f);
    m_iAnimation_Die[HIT_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Hit_Dead_F", 2.f);
    m_iAnimation_Die[DOWN_B] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Dead_B", 2.f);
    m_iAnimation_Die[DOWN_F] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Down_Dead_F", 2.f);
    m_iAnimation_Die[CURSE] = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Curse_Dead", 2.f);

    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_Player_Die::Start_State(void* pArg)
{
    DIE_DESC* pDesc = static_cast<DIE_DESC*>(pArg);

    _int iAnim = Choice_DieAnim(pDesc);

    m_pPlayer->Change_Animation(m_iAnimation_Die[iAnim], false, 0.1f);
    m_pPlayer->Get_Player_Stat().vGauge_Hp.x = 0.f;

    m_pPlayer->Change_Weapon();
    m_pPlayer->Combine_Scissor();

    return S_OK;
}

void CState_Player_Die::Update(_float fTimeDelta)
{
    //죽으면 이전 별바라기로
}

void CState_Player_Die::End_State()
{
    m_pPlayer->Set_IsGuard(false);
}

_uint CState_Player_Die::Choice_DieAnim(DIE_DESC* pDesc)
{  
    // 넘어져 있다는 상태임
    if (m_pFsm->Get_PrevState() == CPlayer::HIT)
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
    else if(pDesc->vHitPos.Length() > 0.f)
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