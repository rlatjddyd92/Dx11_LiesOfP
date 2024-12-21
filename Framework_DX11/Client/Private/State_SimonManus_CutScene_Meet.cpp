#include "stdafx.h"
#include "State_SimonManus_CutScene_Meet.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"
#include "CutScene.h"

#include "GameInterface_Controller.h"

CState_SimonManus_CutScene_Meet::CState_SimonManus_CutScene_Meet(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManus_CutScene_Meet::Initialize(_uint iStateNum, void* pArg)
{
    
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManus_CutScene_Meet::Start_State(void* pArg)
{
    // 모델이 달라서 여기서 해주기
    m_iAnimation_Turn = m_pMonster->Get_Model()->Find_AnimationIndex("AS_MOB_Manus_Turn_Cine", 1.f);
    m_iAnimation_Talk = m_pMonster->Get_Model()->Find_AnimationIndex("AS_MOB_Manus_Talk_Cine", 1.315f);
    m_iAnimation_Hand = m_pMonster->Get_Model()->Find_AnimationIndex("AS_MOB_Manus_Hand_Cine", 1.3159f);
    m_iAnimation_End = m_pMonster->Get_Model()->Find_AnimationIndex("AS_MOB_Manus_End_Cine", 2.f);

    m_pMonster->Change_Animation(m_iAnimation_Turn, false, 0.f, 0);
    m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_Turn, 30);

    m_isStartCutScene = false;
    m_fDelay = 0.f;

    return S_OK;
}

void CState_SimonManus_CutScene_Meet::Update(_float fTimeDelta)
{
    _int iFrame = m_pMonster->Get_Frame();
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();

    m_fDelay += fTimeDelta;
    if (m_fDelay >= 13.f && !m_isStartCutScene)
    {
        m_pMonster->Play_Animation();
        m_isStartCutScene = true;
    }
    else if (m_fDelay < 13.f)
    {
        m_pMonster->Stop_Animation();
    }

    _Vec3 vMove = m_pMonster->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr(5)->Translation();
    _float4x4 TransMat;
    XMStoreFloat4x4(&TransMat, m_pMonster->Get_Model()->Get_Bones()[5]->Get_TransformationMatrix());
    TransMat._41 = 0.f;

    m_pMonster->Get_Model()->Get_Bones()[5]->Set_TransformationMatrix(TransMat);;

    m_pMonster->Get_Model()->Update_Bone();

    vMove = XMVector3TransformNormal(vMove, m_pMonster->Get_Transform()->Get_WorldMatrix());

    vMove.y = 0;

    if (iCurAnim != m_iAnimation_End)
    {
        vMove.x = vMove.z = 0.f;
    }
    else
    {
        vMove.x *= 0.3f;
        vMove.z *= 0.3f;
    }

    m_pMonster->Get_RigidBody()->Set_Velocity((vMove - m_vRootMoveStack) / fTimeDelta);

    m_vRootMoveStack = vMove;
   
    if (iCurAnim == m_iAnimation_Hand)
    {
        _int iFrame = m_pMonster->Get_Frame();
        if (iFrame > 240)
        {
            m_pMonster->Active_Weapon();
        }
    }

    Control_Dialog(iFrame);
    End_Check();
}

void CState_SimonManus_CutScene_Meet::End_State()
{
}

void CState_SimonManus_CutScene_Meet::End_Check(_float fTimeDelta)
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();

    if (m_pMonster->Get_EndAnim(m_iAnimation_Turn))
    {
        m_fDelay2 += fTimeDelta;

        if (m_fDelay2 < 0.9f)
            return;

        m_pMonster->Change_Animation(m_iAnimation_Talk, false, 0.1f, 0); 
    }
   // else if (m_pMonster->Get_EndAnim(m_iAnimation_Talk))
    else if (m_pMonster->Get_CurrentAnimIndex() == m_iAnimation_Talk && m_pMonster->Get_Model()->Get_CurrentFrame() > 718)
    {
        m_pMonster->Change_Animation(m_iAnimation_Hand, false, 0.2f, 65);
    }
    else if (m_pMonster->Get_CurrentAnimIndex() == m_iAnimation_Hand && m_pMonster->Get_Model()->Get_CurrentFrame() > 607)
    {
        m_pMonster->Change_Animation(m_iAnimation_End, false, 0.f, 63);
    }
    else if (m_pMonster->Get_CurrentAnimIndex() == m_iAnimation_Hand && m_pMonster->Get_Model()->Get_CurrentFrame() > 560)
    {
        m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_Hand, 11);
    }
    else if (m_pMonster->Get_EndAnim(m_iAnimation_End))
    {
       // m_pMonster->End_CutScene(0);
    }
}

void CState_SimonManus_CutScene_Meet::Control_Dialog(_int iFrame)
{
    _uint iCurAnimIndex = m_pMonster->Get_CurrentAnimIndex();

    if ( !m_isShowDialog[0] && iCurAnimIndex == m_iAnimation_Turn && iFrame >= 1045)
    {
        GET_GAMEINTERFACE->Show_Script(TEXT("보라, 이것이 바로 에르고다. 실로 장엄하지 않은가!"), TEXT("none"), 11.f);
        m_isShowDialog[0] = true;
    }
    else  if (!m_isShowDialog[1] && iCurAnimIndex == m_iAnimation_Talk && iFrame >= 325)
    {
        GET_GAMEINTERFACE->Show_Script(TEXT("불멸하는 존재, 거짓없는 세계를 이루는 열쇠지"), TEXT("none"), 11.f);
        m_isShowDialog[1] = true;
    }
    else  if (!m_isShowDialog[2] && iFrame >= 1800)
    {
        GET_GAMEINTERFACE->Show_Script(TEXT("이제 그 진화의 문을 열 때가 되었다."), TEXT("none"), 7.f);
        m_isShowDialog[2] = true;
    }
    else  if (!m_isShowDialog[3] && iFrame >= 2100)
    {
        GET_GAMEINTERFACE->Show_Script(TEXT("감사하라! 이것이 신조차 삼킬 자, 나 시몬 마누스가 주는 선물이다."), TEXT("none"), 12.f);
        m_isShowDialog[3] = true;
    }
    else  if (!m_isShowDialog[4] && iFrame >= 2400)
    {
        GET_GAMEINTERFACE->Show_Script(TEXT("이제 태어날 너희의 신을 경배하라!"), TEXT("none"), 8.f);
        m_isShowDialog[4] = true;
    }
}

CState_SimonManus_CutScene_Meet* CState_SimonManus_CutScene_Meet::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManus_CutScene_Meet* pInstance = new CState_SimonManus_CutScene_Meet(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManus_CutScene_Meet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManus_CutScene_Meet::Free()
{
    __super::Free();
}