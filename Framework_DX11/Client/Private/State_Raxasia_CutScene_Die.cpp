#include "stdafx.h"
#include "State_Raxasia_CutScene_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"
#include "CutScene.h"

CState_Raxasia_CutScene_Die::CState_Raxasia_CutScene_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_Raxasia_CutScene_Die::Initialize(_uint iStateNum, void* pArg)
{

    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_Raxasia_CutScene_Die::Start_State(void* pArg)
{
    // 모델이 달라서 여기서 해주기
    m_iAnimation_KneeDown = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Raxasia_knee_down", 1.f);
    m_iAnimation_Die = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Raxasia_Raxasia_Dead_C00_CINE", 1.f);
    m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_KneeDown, 19.0);

    m_pMonster->Change_Animation(m_iAnimation_KneeDown, false, 0.f, 0);
    m_pMonster->Stop_Animation();

    m_isStartCutScene = false;
    m_fDelay = 0.f;

    m_pCutSceneWeapon = dynamic_cast<CRaxasia*>(m_pMonster)->Get_CutSceneWeapon();

    return S_OK;
}

void CState_Raxasia_CutScene_Die::Update(_float fTimeDelta)
{
    m_fDelay += fTimeDelta;

    if (m_fDelay > 0.3f)
        m_pMonster->Play_Animation();

    _int iFrame = m_pMonster->Get_Frame();
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();

    //if (iFrame > 50)
    //{
    //    m_pMonster->Stop_Animation();
    //}

    //m_fDelay += fTimeDelta;
    //if (m_fDelay >= 0.1f && !m_isStartCutScene)
    //{
    //    m_pMonster->Play_Animation();
    //    m_isStartCutScene = true;
    //}
    //else if (m_fDelay < 16.f)
    //{
    //    m_pMonster->Stop_Animation();
    //}


    _Vec3 vMove = m_pMonster->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(3)->Translation();
    _float4x4 TransMat;
    XMStoreFloat4x4(&TransMat, m_pMonster->Get_Model()->Get_Bones()[3]->Get_TransformationMatrix());
    TransMat._41 = TransMat._42 = TransMat._43 = 0.f;

    m_pMonster->Get_Model()->Get_Bones()[3]->Set_TransformationMatrix(TransMat);;

    m_pMonster->Get_Model()->Update_Bone();

    vMove = XMVector3TransformNormal(vMove, m_pMonster->Get_Transform()->Get_WorldMatrix()) * 0.8f;
    vMove.x =  vMove.z = 0.f;


    m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vRootMoveStack) / fTimeDelta);

    m_vRootMoveStack = vMove;

    End_Check();
}

void CState_Raxasia_CutScene_Die::End_State()
{
    m_pCutSceneWeapon = nullptr;
}

void CState_Raxasia_CutScene_Die::End_Check()
{
    if (m_pMonster->Get_EndAnim(m_iAnimation_KneeDown))
    {
        m_pMonster->Get_Transform()->Rotation(_vector{ 0, 1, 0, 0 }, XMConvertToRadians(150.f));
        m_pMonster->Change_Animation(m_iAnimation_Die, false, 0.f);
        m_pMonster->Get_Model()->Play_Animation(0.00001f);

        _Vec3 vMove = m_pMonster->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(3)->Translation();
        _float4x4 TransMat;
        XMStoreFloat4x4(&TransMat, m_pMonster->Get_Model()->Get_Bones()[3]->Get_TransformationMatrix());
        TransMat._41 = TransMat._42 = TransMat._43 = 0.f;

        m_pMonster->Get_Model()->Get_Bones()[3]->Set_TransformationMatrix(TransMat);;

        m_pMonster->Get_Model()->Update_Bone();

        vMove = XMVector3TransformNormal(vMove, m_pMonster->Get_Transform()->Get_WorldMatrix()) * 0.8f;
        vMove.x = vMove.z = 0.f;


        m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vRootMoveStack) / 0.001f);


    }
    else if (m_pMonster->Get_EndAnim(m_iAnimation_Die))
    {
        // 죽이기
    }
}

CState_Raxasia_CutScene_Die* CState_Raxasia_CutScene_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_Raxasia_CutScene_Die* pInstance = new CState_Raxasia_CutScene_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Raxasia_CutScene_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Raxasia_CutScene_Die::Free()
{
    __super::Free();
}