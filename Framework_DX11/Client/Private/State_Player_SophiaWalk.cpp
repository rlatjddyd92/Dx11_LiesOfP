#include "stdafx.h"
#include "State_Player_SophiaWalk.h"
#include "GameInstance.h"
#include "Model.h"
#include "Player.h"
#include "Camera.h"
#include "TreasureBox.h"

CState_Player_SophiaWalk::CState_Player_SophiaWalk(CFsm* pFsm, CPlayer* pPlayer)
    :CState{ pFsm }
    , m_pPlayer{ pPlayer }
{
}

HRESULT CState_Player_SophiaWalk::Initialize(_uint iStateNum, void* pArg)
{
    FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    m_pTrackPos = pDesc->pPrevTrackPos;

    m_iAnimation_SophiaWalk = m_pPlayer->Get_Model()->Find_AnimationIndex("AS_Pino_Walking_SophiaDead_Cine", 1.f);

    m_iStateNum = iStateNum;

    m_iFootStepFrame[0] = 79;
    m_iFootStepFrame[1] = 103;
    m_iFootStepFrame[2] = 125;
    m_iFootStepFrame[3] = 147;
    m_iFootStepFrame[4] = 167;
    m_iFootStepFrame[5] = 189;
    m_iFootStepFrame[6] = 212;
    m_iFootStepFrame[7] = 232;
    m_iFootStepFrame[8] = 248;
    m_iFootStepFrame[9] = 272;
    m_iFootStepFrame[10] = 293;
    m_iFootStepFrame[11] = 313;
    m_iFootStepFrame[12] = 333;
    m_iFootStepFrame[13] = 356;

    return S_OK;
}

HRESULT CState_Player_SophiaWalk::Start_State(void* pArg)
{
    m_pPlayer->Change_Animation(m_iAnimation_SophiaWalk, false, 0.f);

    m_vRootMoveStack = _Vec3(0.f, 0.f, 0.f);

    return S_OK;
}

void CState_Player_SophiaWalk::Update(_float fTimeDelta)
{
   /* if (End_Check())
    {
        m_pPlayer->Get_Model()->ReadyDenyNextTranslate(4);
        m_pPlayer->Change_State(CPlayer::OH_IDLE);
    }*/

    _Vec3 vMove = m_pPlayer->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(4)->Translation();
    _float4x4 TransMat;
    XMStoreFloat4x4(&TransMat, m_pPlayer->Get_Model()->Get_Bones()[4]->Get_TransformationMatrix());
    //TransMat._43 = TransMat._42 = TransMat._41 = 0.f;
    TransMat._41 = 0.f;

    m_pPlayer->Get_Model()->Get_Bones()[4]->Set_TransformationMatrix(TransMat);;

    m_pPlayer->Get_Model()->Update_Bone();

    vMove = XMVector3TransformNormal(vMove, m_pPlayer->Get_Transform()->Get_WorldMatrix());
    
    vMove.y = 0;

   
    //m_pPlayer->Get_RigidBody()->Set_Velocity((vMove - m_vRootMoveStack) / fTimeDelta);

    m_vRootMoveStack = vMove;

}

void CState_Player_SophiaWalk::End_State()
{
}

_bool CState_Player_SophiaWalk::End_Check()
{
    return m_pPlayer->Get_EndAnim(m_iAnimation_SophiaWalk);
}

void CState_Player_SophiaWalk::Control_Sound()
{
}

CState_Player_SophiaWalk* CState_Player_SophiaWalk::Create(CFsm* pFsm, CPlayer* pPlayer, _uint iStateNum, void* pArg)
{
    CState_Player_SophiaWalk* pInstance = new CState_Player_SophiaWalk(pFsm, pPlayer);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Player_SophiaHand"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Player_SophiaWalk::Free()
{
    __super::Free();
}