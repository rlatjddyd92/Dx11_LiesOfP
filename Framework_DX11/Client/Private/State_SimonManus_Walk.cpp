#include "stdafx.h"
#include "State_SimonManus_Walk.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

CState_SimonManus_Walk::CState_SimonManus_Walk(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManus_Walk::Initialize(_uint iStateNum, void* pArg)
{
   //m_iAnimation_Idle = m_pMonster->Get_Model()->Get_AnimationIndex("Kurama_Idle_Loop");
    m_iStateNum = iStateNum;
    m_fIdleDuration = 3.3f;

    return S_OK;
}

HRESULT CState_SimonManus_Walk::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(33, true);;


    return S_OK;
}

void CState_SimonManus_Walk::Update(_float fTimeDelta)
{

    _vector vPos = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    _vector vCamPos = m_pGameInstance->Get_CamPosition_Vec4();//임시사용 캠 포지션
    if (!(XMVectorGetX(vPos) == XMVectorGetX(vCamPos))
        || !(XMVectorGetZ(vPos) == XMVectorGetZ(vCamPos)))
    {
        _int iDir = m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(vCamPos - vPos, 1, fTimeDelta);
        switch (iDir)
        {
        case -1:
            m_pMonster->Change_Animation(30, true, true);
            break;

        case 0:
            m_pMonster->Change_Animation(33, true, true);
            break;

        case 1:
            m_pMonster->Change_Animation(31, true, true);
            break;

        default:
            break;
        }
        m_pMonster->Get_Transform()->Go_Straight(fTimeDelta);
    }


    if (m_pMonster->Calc_Distance_XZ() <= 5.f)
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
    }
}

void CState_SimonManus_Walk::End_State()
{
}

CState_SimonManus_Walk* CState_SimonManus_Walk::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManus_Walk* pInstance = new CState_SimonManus_Walk(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManus_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManus_Walk::Free()
{
    __super::Free();
}