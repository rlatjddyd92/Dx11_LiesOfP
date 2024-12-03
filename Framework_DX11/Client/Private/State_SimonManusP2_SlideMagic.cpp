#include "stdafx.h"
#include "State_SimonManusP2_SlideMagic.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "AttackObject.h"

CState_SimonManusP2_SlideMagic::CState_SimonManusP2_SlideMagic(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_SlideMagic::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_SlideMagic::Start_State(void* pArg)
{
    _int iCnt = rand() % 2;
    if (iCnt == 0)
    {
        m_iCurAnim = AN_SLIDE_LEFT;
    }
    else
    {
        m_iCurAnim = AN_SLIDE_RIGHT;
    }

    _Vec3 vUp = {0.f, 1.f, 0.f};
    _Vec3 vLook = (m_pMonster->Get_TargetDir());
    _Vec3 vRight{};

    if (m_iCurAnim == AN_SLIDE_LEFT)
    {
        vRight = vLook.Cross(vUp);
    }
    else
    {
        vRight = vUp.Cross(vLook);
    }
    m_vGoalDir = vLook + vRight;
    m_vGoalDir.Normalize();
    
    m_iMagicCnt = 0;

    m_pMonster->Change_Animation(m_iCurAnim, false, 0.1f, 0);

    return S_OK;
}

void CState_SimonManusP2_SlideMagic::Update(_float fTimeDelta)
{
    if (End_Check())//애니메이션의 종료 받아오도록 해서 어택이 종료된 시점에
    {
        m_pMonster->Change_State(CSimonManus::IDLE);
        return;
    }
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    if (CurTrackPos <= 70.f)
    {
        m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(_Vec4{ m_vGoalDir }, 2, fTimeDelta);

    }

    Effect_Check(CurTrackPos);
}

void CState_SimonManusP2_SlideMagic::End_State()
{

}

_bool CState_SimonManusP2_SlideMagic::End_Check()
{
    return m_pMonster->Get_EndAnim(m_iCurAnim);
}

void CState_SimonManusP2_SlideMagic::Effect_Check(_double CurTrackPos)
{
    if (m_iCurAnim == AN_SLIDE_LEFT)
    {
        if ((CurTrackPos >= 30.f && CurTrackPos <= 50.f))
        {
            if (!m_pMonster->Get_EffectsLoop(CSimonManus::P2_SLIDEMAGIC))
            {
                m_pMonster->Active_Effect(CSimonManus::P2_SLIDEMAGIC);
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P2_SLIDEMAGIC);
        }
    }
    else
    {
        if ((CurTrackPos >= 15.f && CurTrackPos <= 55.f))
        {
            if (!m_pMonster->Get_EffectsLoop(CSimonManus::P2_SLIDEMAGIC))
            {
                m_pMonster->Active_Effect(CSimonManus::P2_SLIDEMAGIC);
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CSimonManus::P2_SLIDEMAGIC);
        }
    }

    _bool bMagicCheck = false;
    switch (m_iMagicCnt)
    {
    case 0:
        if (72.f <= CurTrackPos)
        {
            ++m_iMagicCnt;
            bMagicCheck = true;
        }
        break;

    case 1:
        if (91.f <= CurTrackPos)
        {
            ++m_iMagicCnt;
            bMagicCheck = true;
        }
        break;

    case 2:
        if (100.f <= CurTrackPos)
        {
            ++m_iMagicCnt;
            bMagicCheck = true;
        }
        break;

    case 3:
        if (119.f <= CurTrackPos)
        {
            ++m_iMagicCnt;
            bMagicCheck = true;
        }
        break;

    default:
        break;
    }

    if (bMagicCheck)
    {
        CAttackObject::ATKOBJ_DESC Desc;
        _float4x4 WorldMat{};
        XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_UFBIndex(UFB_HAND_LEFT)) *(*m_pMonster->Get_Transform()->Get_WorldMatrix_Ptr())));
        Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42 - 0.2f, WorldMat._43 };
        Desc.vDir = _Vec3{ XMVector3Normalize(m_pMonster->Get_TargetPos() - Desc.vPos) };

        m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_SlideMagic"), &Desc);

        m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_FX_Ground_Exp_L_03.wav"));
        
    }

}

void CState_SimonManusP2_SlideMagic::Control_Sound(_double CurTrackPos)
{
    if (!m_bMagicSound)
    {
        if (m_iCurAnim == AN_SLIDE_LEFT)
        {
            if ((CurTrackPos >= 30.f && CurTrackPos <= 40.f))
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SimonManus_SK_PJ_Ergo_Wait_05.wav"));
                m_bMagicSound = true;
            }
        }
        else
        {
            if ((CurTrackPos >= 15.f && CurTrackPos <= 25.f))
            {
            }
        }
    }
}

CState_SimonManusP2_SlideMagic* CState_SimonManusP2_SlideMagic::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_SlideMagic* pInstance = new CState_SimonManusP2_SlideMagic(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_SlideMagic"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_SlideMagic::Free()
{
    __super::Free();
}