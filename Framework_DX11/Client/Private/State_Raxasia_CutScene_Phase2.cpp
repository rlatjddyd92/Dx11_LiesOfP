#include "stdafx.h"
#include "State_Raxasia_CutScene_Phase2.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"
#include "CutScene.h"
#include "Raxasia_Sword_CutScene.h"
#include "Weapon_Raxasia_P2_Shield.h"

CState_Raxasia_CutScene_Phase2::CState_Raxasia_CutScene_Phase2(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_Raxasia_CutScene_Phase2::Initialize(_uint iStateNum, void* pArg)
{

    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_Raxasia_CutScene_Phase2::Start_State(void* pArg)
{
    // 모델이 달라서 여기서 해주기
    m_iAnimation_Phase2 = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Raxasia_Raxasia_Phase2_C00_CINE", 1.f);
    m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_Phase2, 29.4);

    m_pMonster->Change_Animation(m_iAnimation_Phase2, false, 0.f, 0);

    m_isStartCutScene = false;
    m_fDelay = 0.f;

    m_pCutSceneWeapon = dynamic_cast<CRaxasia*>(m_pMonster)->Get_CutSceneWeapon();
    m_pShieldWeapon = dynamic_cast<CRaxasia*>(m_pMonster)->Get_ShieldWeapon();
    //m_pCutSceneWeapon->Play_Animation("AS_Sword_Raxasia_Phase2_C01_CINE");

    return S_OK;
}

void CState_Raxasia_CutScene_Phase2::Update(_float fTimeDelta)
{
    _int iFrame = m_pMonster->Get_Frame();

    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();

    if (!m_isOnGroundWeapon && iFrame > 100)    // 검 위치 땅에 고정하기
    {
        m_pCutSceneWeapon->Stop_UpdatePos();
        m_isOnGroundWeapon = true;
    }
    else if (!m_isConneectLeftHandShield && iFrame > 430)   // 방패 분리하기
    {
        _Vec3 vShieldOffset = _Vec3(0.225f, -0.325f, 0.145f);
        _matrix PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(195.f)) * XMMatrixRotationY(XMConvertToRadians(-180.f)) * XMMatrixRotationZ(XMConvertToRadians(15.f));

        m_pShieldWeapon->ChangeSocketMatrix(m_pMonster->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(36));
        m_pShieldWeapon->Get_Transform()->Set_State(CTransform::STATE_POSITION, vShieldOffset);
        m_pShieldWeapon->Get_Model()->Set_PreTranformMatrix(PreTransformMatrix);

        m_isConneectLeftHandShield = true;
    }
    else if (!m_isChangeWeaponPos && iFrame > 500)      // 검 위치 나중을 위해 위치 바꾸기
    {
        _Vec3 vPos = _Vec3(-59.28147f, -96.39721f, -27.10989f);
        _matrix PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(58.f));

        m_pCutSceneWeapon->Get_Model()->Set_PreTranformMatrix(PreTransformMatrix);
        m_pCutSceneWeapon->Set_Pos(vPos);

        m_isChangeWeaponPos = true;
    }
    else if (!m_isChangePhase2Model && iFrame > 599)      // 2 페이즈 모델로 바꾸기 
    {
        m_pMonster->Change_Model(1);
        m_isChangePhase2Model = true;
    }
    else if (!m_isReConnetWeaponBone && iFrame > 786)      // 다시 검 들고 방패 뼈 다시 세팅하기
    {
        _Vec3 vOffset = _Vec3(0.f, 0.f, 0.f);
        _matrix PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(-90.f));

        m_pCutSceneWeapon->Set_Offset(vOffset);
        m_pCutSceneWeapon->Get_Model()->Set_PreTranformMatrix(PreTransformMatrix);
        m_pCutSceneWeapon->Start_UpdatePos();

        _Vec3 vShieldOffset = _Vec3(0.66f, -0.39f, 0.21f);
       PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(280.f)) * XMMatrixRotationY(XMConvertToRadians(-190.f)) * XMMatrixRotationZ(XMConvertToRadians(100.f));

        m_pShieldWeapon->Get_Transform()->Set_State(CTransform::STATE_POSITION, vShieldOffset);
        m_pShieldWeapon->Get_Model()->Set_PreTranformMatrix(PreTransformMatrix);


        m_isReConnetWeaponBone = true;
    }
    else if(!m_isPlayWeaponChangeAnim && iFrame > 905)      // 검 애니메이션 재생하기
    {
        m_pCutSceneWeapon->Play_Animation("AS_Sword_Raxasia_Phase2_C06_CINE", 1.6f);
        m_isPlayWeaponChangeAnim = true;
    }

    

    if (iFrame > 1032)
    {
        //m_pMonster->Stop_Animation();
        //dynamic_cast<CWeapon_Raxasia_P2_Shield*>(m_pShieldWeapon)->Set_Test();
    }
     
    // 
    // 
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

    vMove = XMVector3TransformNormal(vMove, m_pMonster->Get_Transform()->Get_WorldMatrix());

    m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vRootMoveStack) / fTimeDelta);

    m_vRootMoveStack = vMove;

    //End_Check();
}

void CState_Raxasia_CutScene_Phase2::End_State()
{
    m_pCutSceneWeapon = nullptr;
}

void CState_Raxasia_CutScene_Phase2::End_Check()
{
    if (m_pMonster->Get_EndAnim(m_iAnimation_Phase2))
    {
        m_pMonster->End_CutScene(CRaxasia::CUTSCENE_MEET);
    }
}

CState_Raxasia_CutScene_Phase2* CState_Raxasia_CutScene_Phase2::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_Raxasia_CutScene_Phase2* pInstance = new CState_Raxasia_CutScene_Phase2(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_Raxasia_CutScene_Phase2"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_Raxasia_CutScene_Phase2::Free()
{
    __super::Free();
}