#include "stdafx.h"
#include "State_Raxasia_CutScene_Phase2.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"
#include "CutScene.h"

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
    m_iAnimation_Phase2 = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Raxasia_Raxasia_Phase1_C06_Renew_CINE", 1.f);

    m_pMonster->Change_Animation(m_iAnimation_Phase2, true, 0.f, 0);

    m_isStartCutScene = false;
    m_fDelay = 0.f;

    return S_OK;
}

void CState_Raxasia_CutScene_Phase2::Update(_float fTimeDelta)
{
    _int iFrame = m_pMonster->Get_Frame();

    /*  _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();

      m_fDelay += fTimeDelta;
      if (m_fDelay >= 0.1f && !m_isStartCutScene)
      {
          m_pMonster->Play_Animation();
          m_isStartCutScene = true;
      }
      else if (m_fDelay < 16.f)
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

      vMove.y = 0;*/


}

void CState_Raxasia_CutScene_Phase2::End_State()
{
}

void CState_Raxasia_CutScene_Phase2::End_Check()
{
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