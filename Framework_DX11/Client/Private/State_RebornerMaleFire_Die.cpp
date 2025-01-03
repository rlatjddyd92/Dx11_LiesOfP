#include "stdafx.h"
#include "State_RebornerMaleFire_Die.h"
#include "GameInstance.h"
#include "Model.h"
#include "CarcassBigA.h"

#include "Effect_Manager.h"

CState_RebornerMaleFire_Die::CState_RebornerMaleFire_Die(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RebornerMaleFire_Die::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;

    //FSM_INIT_DESC* pDesc = static_cast<FSM_INIT_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RebornerMaleFire_Die::Start_State(void* pArg)
{
    _Vec3 vUp = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_UP));
    _Vec3 vRight = XMVector3Normalize(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
    _Vec3 vTargetDir = XMVector3Normalize(m_pMonster->Get_TargetDir());


    _Vec3 vCrossUp = vRight.Cross(vTargetDir);

    _int iAnimIndex = {};

    if (vCrossUp.y <= 0)
    {
        iAnimIndex = AN_DIE_B;
        m_iDirCnt = 0;
    }
    else
    {
        iAnimIndex = AN_DIE_F;
        m_iDirCnt = 1;
    }

    m_bExplosion = false;

    m_pMonster->Change_Animation(iAnimIndex, false, 0.1f);

    return S_OK;
}

void CState_RebornerMaleFire_Die::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    //if (End_Check())
    //{
    //    //몬스터 사망
    //    m_pMonster->Change_State(CCarcassBigA::IDLE);   //임시
    //
    //}

    Effect_Check(CurTrackPos);

}

void CState_RebornerMaleFire_Die::End_State()
{
}

_bool CState_RebornerMaleFire_Die::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    if ((AN_DIE_F) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_DIE_F);
    }
    else if ((AN_DIE_B) == iCurAnim)
    {
        bEndCheck = m_pMonster->Get_EndAnim(AN_DIE_B);
    }
    else
    {

    }
    //애니메이션 번호와 일치하지 않는?다

    return bEndCheck;
}

void CState_RebornerMaleFire_Die::Effect_Check(_double CurTrackPos)
{
    //Prototype_GameObject_Explosion 175 80
    _double EffectTime = {};

    if (m_iDirCnt == 0)
    {
        EffectTime = 175.f;
    }
    else
    {
        EffectTime = 80.f;
    }
    if (!m_bExplosion)
    {
        if (CurTrackPos >= EffectTime)
        {
            _float4x4 WorldMat{};
            _Vec3 vPos = { 0.f, 0.f, 0.f };
            XMStoreFloat4x4(&WorldMat,
                (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_Model()->Get_UFBIndices(UFB_ROOT))
                    * (m_pMonster->Get_Transform()->Get_WorldMatrix())));
            vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));
            //vPos.y = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;

            CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_Stamp"),
                vPos, _Vec3{ m_pMonster->Get_TargetDir() });
            m_bExplosion = true;
        }
    }


}

CState_RebornerMaleFire_Die* CState_RebornerMaleFire_Die::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RebornerMaleFire_Die* pInstance = new CState_RebornerMaleFire_Die(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RebornerMaleFire_Die"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RebornerMaleFire_Die::Free()
{
    __super::Free();
}