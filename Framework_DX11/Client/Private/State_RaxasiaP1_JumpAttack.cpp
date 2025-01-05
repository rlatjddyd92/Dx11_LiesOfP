#include "stdafx.h"
#include "State_RaxasiaP1_JumpAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "Raxasia.h"

CState_RaxasiaP1_JumpAttack::CState_RaxasiaP1_JumpAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_RaxasiaP1_JumpAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_RaxasiaP1_JumpAttack::Start_State(void* pArg)
{
    m_iRouteTrack = 0;
    _int iCnt = rand() % 2;
    if (iCnt == 0)
    {
        m_iCurAnimIndex = AN_JUMPATTACK_L;
        m_pMonster->Change_Animation(AN_JUMPATTACK_L, false, 0.1f, 3);
    }
    else
    {
        m_iCurAnimIndex = AN_JUMPATTACK_R;
        m_pMonster->Change_Animation(AN_JUMPATTACK_R, false, 0.1f, 3);
    }

    m_bSwingSound = false;

    m_bSwing = false;
    return S_OK;
}

void CState_RaxasiaP1_JumpAttack::Update(_float fTimeDelta)
{
    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    switch (m_iRouteTrack)
    {
    case 0:
    {
        _float fTime = 0;
        if (m_iCurAnimIndex == AN_JUMPATTACK_L)
        {
            fTime = 170.f;
            if (CurTrackPos >= 50.f && CurTrackPos <= 60.f)
            {
                m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
            }
        }
        else
        {
            fTime = 140.f;
            if (CurTrackPos >= 55.f && CurTrackPos <= 65.f)
            {
                m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
            }
        }

        if ((CurTrackPos >= 5.f && CurTrackPos <= 45.f) ||
            (CurTrackPos >= 85.f && CurTrackPos <= 140.f))
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }

        if (CurTrackPos >= fTime)
        {
            if (m_pMonster->Get_TargetDead())
            {
                m_pMonster->Change_Animation(8, true, 0.5f, 0, true);
                m_pMonster->Change_State(CMonster::IDLE);
                return;
            }

            ++m_iRouteTrack;
            m_bSwing = false;
            m_bSwingSound = false;
            m_pMonster->Change_Animation(AN_SLASH, false, 0.1f, 3);
            return;
        }

        break;
    }

    case 1:
        if (End_Check())
        {
            m_iRouteTrack = 0;
            m_pMonster->Change_State(CRaxasia::IDLE);
            return;
        }

        if (CurTrackPos <= 40.f || CurTrackPos >= 70.f)
        {
            m_pMonster->Get_Transform()->LookAt_Lerp_NoHeight(m_pMonster->Get_TargetDir(), 1.f, fTimeDelta);
        }
        break;

    default:
        break;
    }

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);

}

void CState_RaxasiaP1_JumpAttack::End_State()
{
}

_bool CState_RaxasiaP1_JumpAttack::End_Check()
{
    return m_pMonster->Get_EndAnim(AN_SLASH);;
}

void CState_RaxasiaP1_JumpAttack::Collider_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (m_iCurAnimIndex == AN_JUMPATTACK_L)
        {
            if ((CurTrackPos >= 60.f && CurTrackPos <= 75.f))
            {
                m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
        else
        {
            if ((CurTrackPos >= 65.f && CurTrackPos <= 80.f))
            {
                m_pMonster->Active_CurrentWeaponCollider(1.2f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_WEAK);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 45.f && CurTrackPos <= 60.f))
        {
            m_pMonster->Active_CurrentWeaponCollider(1.3f, 0, HIT_TYPE::HIT_METAL, ATTACK_STRENGTH::ATK_NORMAL);
        }
        else
        {
            m_pMonster->DeActive_CurretnWeaponCollider();
        }
    }
}

void CState_RaxasiaP1_JumpAttack::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (m_iCurAnimIndex == AN_JUMPATTACK_L)
        {
            if ((CurTrackPos >= 60.f && CurTrackPos <= 75.f))
            {
                if (!m_bSwing)
                {
                    m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
                    m_bSwing = true;
                }
            }
            else
            {
                m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
            }
        }
        else
        {
            if ((CurTrackPos >= 65.f && CurTrackPos <= 80.f))
            {
                if (!m_bSwing)
                {
                    m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
                    m_bSwing = true;
                }
            }
            else
            {
                m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 45.f && CurTrackPos <= 60.f))
        {
            if (!m_bSwing)
            {
                m_pMonster->Active_Effect(CRaxasia::EFFECT_SWING, true);
                m_bSwing = true;
            }
        }
        else
        {
            m_pMonster->DeActive_Effect(CRaxasia::EFFECT_SWING);
        }
    }
}

void CState_RaxasiaP1_JumpAttack::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (m_iCurAnimIndex == AN_JUMPATTACK_L)
        {
            if ((CurTrackPos >= 60.f && CurTrackPos <= 75.f))
            {
                if (!m_bSwingSound)
                {
                    m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                    m_bSwingSound = true;
                }
            }
        }
        else
        {
            if ((CurTrackPos >= 65.f && CurTrackPos <= 80.f))
            {
                if (!m_bSwingSound)
                {
                    m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                    m_bSwingSound = true;
                }
            }
        }
    }
    else if (m_iRouteTrack == 1)
    {
        if ((CurTrackPos >= 45.f && CurTrackPos <= 60.f))
        {
            if (!m_bSwingSound)
            {
                m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SK_WS_BroadSword_06.wav"), false);
                m_bSwingSound = true;
            }
        }
    }

}

CState_RaxasiaP1_JumpAttack* CState_RaxasiaP1_JumpAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_RaxasiaP1_JumpAttack* pInstance = new CState_RaxasiaP1_JumpAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_RaxasiaP1_JumpAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_RaxasiaP1_JumpAttack::Free()
{
    __super::Free();
}