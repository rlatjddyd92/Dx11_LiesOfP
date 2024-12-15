#include "stdafx.h"
#include "State_RaxasiaP1_TripleSting.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

#include "Effect_Manager.h"

CState_RaxasiaP1_TripleSting::CState_RaxasiaP1_TripleSting(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_TripleSting::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_TripleSting::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    m_pMonster->Change_Animation(AN_READY, false, 0.05f, 0);

    m_bSwingSound = false;
    m_bCharge = false;

    _int iCnt = rand() % 2;
    if (iCnt == 0)
        m_iDashAnimIndex = AN_DASH_L;
    else
        m_iDashAnimIndex = AN_DASH_R;
    m_bSting = false;
    return S_OK;
}

void CState_RaxasiaP1_TripleSting::Update(_float fTimeDelta)
{
    //찌르기 115부터 한 10프나 5프동안 조금 이동시키기

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();
    switch (m_iRouteTrack)
    {
    case 0:
        if (End_Check())
        {
            ++m_iRouteTrack;
            m_bSting = false;
            m_pMonster->Change_Animation(AN_STING, false, 0.1f, 80);
        }
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3, fTimeDelta);

        break;

    case 1:
        if (CurTrackPos >= 252.f)
        {
            ++m_iRouteTrack;
            m_bSting = false;
            m_pMonster->Change_Animation(m_iDashAnimIndex, false, 0.2f, 0, true, true);
            return;
        }
        if (CurTrackPos <= 100.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        if (CurTrackPos >= 115.f && CurTrackPos <= 120.f)
        {
            _Vec3 vLook = { m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK) };

            m_pMonster->Get_RigidBody()->Set_Velocity(m_pMonster->Get_RigidBody()->Get_Velocity() + (vLook * 3.f));
        }
        break;

        //대쉬 
    case 2:
        if (CurTrackPos >= 41.f)
        {
            ++m_iRouteTrack;
            m_bSting = false;
            m_pMonster->Change_Animation(AN_STING, false, 0.4f, 40, true, true);
            return;
        }
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3, fTimeDelta);
        break;

    case 3:
        if (CurTrackPos >= 252.f)
        {
            ++m_iRouteTrack;
            m_bSting = false;
            m_pMonster->Change_Animation(m_iDashAnimIndex, false, 0.2f, 0, true, true);
            return;
        }
        if (CurTrackPos <= 100.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        if (CurTrackPos >= 115.f && CurTrackPos <= 120.f)
        {
            _Vec3 vLook = { m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK) };

            m_pMonster->Get_RigidBody()->Set_Velocity(m_pMonster->Get_RigidBody()->Get_Velocity() + (vLook * 3.f));
        }

        break;
        //대쉬
    case 4:
        if (CurTrackPos >= 41.f)
        {
            ++m_iRouteTrack;
            m_bSting = false;
            m_pMonster->Change_Animation(AN_STING, false, 0.4f, 40, true, true);
            return;
        }
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 3, fTimeDelta);
        break;

    case 5:
        if (End_Check())
        {
            m_iRouteTrack = 0;
            m_bSting = false;
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }
        if (CurTrackPos <= 100.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        if (CurTrackPos >= 115.f && CurTrackPos <= 120.f)
        {
            _Vec3 vLook = { m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK) };

            m_pMonster->Get_RigidBody()->Set_Velocity(m_pMonster->Get_RigidBody()->Get_Velocity() + (vLook * 3.f));
        }

        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP1_TripleSting::End_State()
{
    m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD);
}

_bool CState_RaxasiaP1_TripleSting::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((AN_READY) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_READY);
        }
        break;

    case 5:
        if ((AN_STING) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(AN_STING);
        }
        break;

    default:
        break;

    }
    return  bEndCheck;
}

void CState_RaxasiaP1_TripleSting::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack != 0)
    {
        if ((CurTrackPos >= 115.f && CurTrackPos <= 135.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }

    }
}

void CState_RaxasiaP1_TripleSting::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 1)
    {
        if (!m_bCharge)
        {
            if (CurTrackPos >= 20.f)
            {
                _float4x4 WorldMat{};
                _Vec3 vPos = { 0.f, 0.f, 0.f };
                XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_UFBIndex(UFB_HAND_RIGHT)) * m_pMonster->Get_Transform()->Get_WorldMatrix()));
                vPos = XMVector3TransformCoord(vPos, XMLoadFloat4x4(&WorldMat));

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_Attack_ThunderInchent"),
                    vPos, _Vec3{ m_pMonster->Get_TargetDir() });

                m_pMonster->Active_Effect(CRaxasia::EFFECT_INCHENTSWORD, true);
                m_bCharge = true;
            }
        }
    }
    else if (m_iRouteTrack == 5)
    {

        if (CurTrackPos >= 150.f)
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD);
        }
    }
}

void CState_RaxasiaP1_TripleSting::Control_Sound(_double CurTrackPos)
{

}

CState_RaxasiaP1_TripleSting* CState_RaxasiaP1_TripleSting::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_TripleSting* pInstance = new CState_RaxasiaP1_TripleSting(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_TripleSting"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_TripleSting::Free()
{
    __super::Free();
}