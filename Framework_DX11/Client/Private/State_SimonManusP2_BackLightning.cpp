#include "stdafx.h"
#include "State_SimonManusP2_BackLightning.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"
#include "AttackObject.h"

CState_SimonManusP2_BackLightning::CState_SimonManusP2_BackLightning(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_BackLightning::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_BackLightning::Start_State(void* pArg)
{
    m_pMonster->Change_Animation(AN_BACKLIGHTNING, false, 0.1f, 0);
    m_bLightning = false;

    return S_OK;
}

void CState_SimonManusP2_BackLightning::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->SetUp_Animation(8, true, 0, true);
        m_pMonster->Get_Model()->Play_Animation(0);
        m_pMonster->Get_Transform()->Look_Dir(-(m_pMonster->Get_Transform()->Get_State(CTransform::STATE_LOOK)));
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
}

void CState_SimonManusP2_BackLightning::End_State()
{
    m_pMonster->DeActive_CurrentWeaponCollider(0);
}

_bool CState_SimonManusP2_BackLightning::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_BACKLIGHTNING);
}

void CState_SimonManusP2_BackLightning::Effect_Check(_double CurTrackPos)
{
    if ((CurTrackPos >= 50.f && CurTrackPos <= 51.f) ||
        (CurTrackPos >= 52.f && CurTrackPos <= 53.f) ||
        (CurTrackPos >= 54.f && CurTrackPos <= 55.f))
    {
        if (!m_bLightning)
        {
            m_bLightning = true;

            CAttackObject::ATKOBJ_DESC Desc;
            _float4x4 WorldMat{};
            XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_UFBIndex(UFB_HAND_LEFT)) * (*m_pMonster->Get_Transform()->Get_WorldMatrix_Ptr())));
            Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42, WorldMat._43 };

            Desc.vPos.y = m_pMonster->Get_Transform()->Get_State(CTransform::STATE_POSITION).y;

            Desc.pOwner = m_pMonster;

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_LightningImpact"), &Desc);
            //손 위치 아래에 생성
        }
    }
    else
    {
        m_bLightning = false;
    }
}

void CState_SimonManusP2_BackLightning::Control_Sound(_double CurTrackPos)
{
    if (!m_bStingSound)
    {
        if ((CurTrackPos >= 105.f && CurTrackPos <= 110.f) ||
            (CurTrackPos >= 175.f && CurTrackPos <= 180.f))
        {
            m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
            m_bStingSound = true;
        }
    }
    else
    {
        if (CurTrackPos > 115.f && CurTrackPos <= 120.f)
        {
            m_bStingSound = false;
        }
    }
}

CState_SimonManusP2_BackLightning* CState_SimonManusP2_BackLightning::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_BackLightning* pInstance = new CState_SimonManusP2_BackLightning(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_BackLightning"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_BackLightning::Free()
{
    __super::Free();
}