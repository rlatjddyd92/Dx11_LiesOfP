#include "stdafx.h"
#include "State_Raxasia_CutScene_Meet.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"
#include "CutScene.h"
#include "Raxasia_Sword_CutScene.h"

CState_Raxasia_CutScene_Meet::CState_Raxasia_CutScene_Meet(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_Raxasia_CutScene_Meet::Initialize(_uint iStateNum, void* pArg)
{

    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_Raxasia_CutScene_Meet::Start_State(void* pArg)
{
    // 모델이 달라서 여기서 해주기
    m_iAnimation_Meet = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Raxasia_Raxasia_Phase1_C06_Renew_CINE", 1.f);
    m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_Meet, 19.0);
    
    m_pMonster->Change_Animation(m_iAnimation_Meet, false, 0.f, 0);
    m_pMonster->Stop_Animation();

    m_isStartCutScene = false;
    m_fDelay = 0.f;

    m_pCutSceneWeapon = dynamic_cast<CRaxasia*>(m_pMonster)->Get_CutSceneWeapon();

    return S_OK;
}

void CState_Raxasia_CutScene_Meet::Update(_float fTimeDelta)
{
    m_fDelay += fTimeDelta;

    if (m_fDelay > 0.3f)
        m_pMonster->Play_Animation();

    _int iFrame = m_pMonster->Get_Frame();

    if (!m_isMoveDown && iFrame > 230)
    {
        _Vec3 vOffset = _Vec3(0.03f, -0.03f, 0.f);
        _matrix PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(240.f)) * XMMatrixRotationY(XMConvertToRadians(-6.5f)) * XMMatrixRotationZ(XMConvertToRadians(31.f));
        m_pCutSceneWeapon->Get_Model()->Set_PreTranformMatrix(PreTransformMatrix);
        m_pCutSceneWeapon->Get_Transform()->Set_State(CTransform::STATE_POSITION, vOffset);

        _Vec3 vNewPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        vNewPos.x -= 14.7f;
        vNewPos.y += 1.146f;
        vNewPos.z -= 4.5f;

        m_pMonster->Get_RigidBody()->Set_GloblePose(vNewPos);

        m_isMoveDown = true;
    }

    if (!m_isPlayWeaponAni && iFrame > 470)
    {
        m_pCutSceneWeapon->Play_Animation("AS_WP_MOB_Raxasia_01_Sword_Cine_Change__Anim", 1.f);
        m_isPlayWeaponAni = true;
    }

    //if (iFrame > 50)
    //{
    //    m_pMonster->Stop_Animation();
    //}
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();

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
    //vMove.y = 0.f;
    m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vRootMoveStack) / fTimeDelta);

    m_vRootMoveStack = vMove;

    End_Check();
}

void CState_Raxasia_CutScene_Meet::End_State()
{
    m_pCutSceneWeapon = nullptr;
}

void CState_Raxasia_CutScene_Meet::End_Check()
{
    if (m_pMonster->Get_EndAnim(m_iAnimation_Meet))
    {
        m_pMonster->End_CutScene(CRaxasia::CUTSCENE_MEET);

        m_pMonster->Start_CutScene(CRaxasia::CUTSCENE_P2);
    }
}

CState_Raxasia_CutScene_Meet* CState_Raxasia_CutScene_Meet::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_Raxasia_CutScene_Meet* pInstance = new CState_Raxasia_CutScene_Meet(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Raxasia_CutScene_Meet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Raxasia_CutScene_Meet::Free()
{
    __super::Free();
}