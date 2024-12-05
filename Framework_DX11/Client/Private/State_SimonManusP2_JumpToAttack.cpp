#include "stdafx.h"
#include "State_SimonManusP2_JumpToAttack.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "AttackObject.h"

CState_SimonManusP2_JumpToAttack::CState_SimonManusP2_JumpToAttack(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManusP2_JumpToAttack::Initialize(_uint iStateNum, void* pArg)
{
    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    return S_OK;
}

HRESULT CState_SimonManusP2_JumpToAttack::Start_State(void* pArg)
{
    //방향 정하기 
    _int iCnt = rand() % 3;
    switch (iCnt)
    {
    case 0:
        m_iCurStartAnim = AN_JUMPTO_BACK;
        break;

    case 1:
        m_iCurStartAnim = AN_JUMPTO_LEFT;
        break;

    case 2:
        m_iCurStartAnim = AN_JUMPTO_RIGHT;
        break;

    default:
        break;
    }

    if (m_iCurStartAnim != AN_JUMPTO_BACK)
    {
        //마법일지 점프일지 정하는 부분
        if (m_pMonster->Calc_Distance_XZ() >= 7.f)
        {
            ++m_iCurStartAnim;
        }
    }

    iCnt = rand() % 2;
    if (iCnt == 0)
    {
        m_iCurLastAnim = AN_CANCLEATTACK;
    }

    m_iCurStartAnim = AN_MAGICTO_LEFT;
    m_pMonster->Change_Animation(m_iCurStartAnim, false, 0.1f, 0);

    m_bMagic = false;
    m_bJump = false;
    m_bSwing = false;
    m_bMagicSound = false;

    return S_OK;
}

void CState_SimonManusP2_JumpToAttack::Update(_float fTimeDelta)
{
    if (End_Check())
    {
        switch (m_iRouteTrack)
        {
        case 0:
            ++m_iRouteTrack;
            if (m_iCurLastAnim != AN_CANCLEATTACK)
            {
                if (m_pMonster->Calc_Distance_XZ() >= 8.f)
                {
                    m_iCurLastAnim = AN_WAVE;
                }
                else
                    m_iCurLastAnim = AN_SWING;
            }

            m_pMonster->Change_Animation(m_iCurLastAnim, false, 0, 0);
            break;

        case 1:
            m_iRouteTrack = 0;
            m_pMonster->Change_State(CSimonManus::IDLE);
            return;
            break;

        default:
            break;
        }
    }

    _double CurTrackPos = m_pMonster->Get_CurrentTrackPos();

    Collider_Check(CurTrackPos);
    Effect_Check(CurTrackPos);
    Control_Sound(CurTrackPos);
}

void CState_SimonManusP2_JumpToAttack::End_State()
{
    m_bJump = false;
}

_bool CState_SimonManusP2_JumpToAttack::End_Check()
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();
    _bool bEndCheck{ false };
    switch (m_iRouteTrack)
    {
    case 0:
        if ((m_iCurStartAnim) == iCurAnim)  //점프 방향
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iCurStartAnim);
        }
        break;

    case 1:
        if ((m_iCurLastAnim) == iCurAnim)
        {
            bEndCheck = m_pMonster->Get_EndAnim(m_iCurLastAnim);
        }
        break;

    default:
        break;
    }

    return bEndCheck;
}

void CState_SimonManusP2_JumpToAttack::Collider_Check(_double CurTrackPos)
{
    if (m_iCurLastAnim == AN_SWING)
    {
        if (m_iRouteTrack == 1)
        {
            if (CurTrackPos >= 35.f && CurTrackPos <= 65.f)
            {
                m_pMonster->Active_CurrentWeaponCollider(1);
            }
            else
            {
                m_pMonster->DeActive_CurretnWeaponCollider();
            }
        }
    }
}

void CState_SimonManusP2_JumpToAttack::Effect_Check(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bMagic)
        {
            if (m_iCurStartAnim == AN_MAGICTO_LEFT || m_iCurStartAnim == AN_MAGICTO_RIGHT)
            {
                if ((CurTrackPos >= 40.f && CurTrackPos <= 45.f))
                {
                    if (!m_pMonster->Get_EffectsLoop(CSimonManus::P2_JUMPMAGIC))
                    {
                        m_pMonster->Active_Effect(CSimonManus::P2_JUMPMAGIC);
                    }
                }
                else
                {
                    m_pMonster->DeActive_Effect(CSimonManus::P2_JUMPMAGIC);
                }

                _float fTime = 0;
                if (m_iCurStartAnim == AN_MAGICTO_LEFT)
                {
                    fTime = 63.f;
                }
                else
                {
                    fTime = 83.f;
                }

                if (CurTrackPos >= fTime)
                {
                    _Vec3 vLook = m_pMonster->Get_TargetDir();
                    _Vec3 vUp = _Vec3{ 0.f, 1.f, 0.f };
                    _Vec3 vRight = vUp.Cross(vLook);
                    vRight.Normalize();
                    vRight *= 0.1f;

                    CAttackObject::ATKOBJ_DESC Desc;
                    _float4x4 WorldMat{};
                    XMStoreFloat4x4(&WorldMat, (*m_pMonster->Get_BoneCombinedMat(m_pMonster->Get_UFBIndex(UFB_HAND_LEFT)) * (*m_pMonster->Get_Transform()->Get_WorldMatrix_Ptr())));
                    Desc.vPos = _Vec3{ WorldMat._41, WorldMat._42 - 0.2f, WorldMat._43 };
                    _Vec3 vMainDir = m_pMonster->Get_TargetPos() - Desc.vPos;
                    vMainDir.Normalize();

                    Desc.vDir = _Vec3{ vMainDir - vRight * 5 };

                    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_GoldBall"), &Desc);

                    Desc.vDir = _Vec3{ vMainDir - vRight };

                    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_GoldBall"), &Desc);

                    Desc.vDir = _Vec3{ vMainDir + vRight * 5 };

                    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_GoldBall"), &Desc);

                    Desc.vDir = _Vec3{ vMainDir + vRight };

                    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_GoldBall"), &Desc);


                    m_bMagic = true;
                }
            }
        }
    }
    else
    {
        if (m_iCurLastAnim == AN_SWING)
        {
            if (CurTrackPos >= 35.f && CurTrackPos <= 65.f)
            {
                if (!m_bSwing)
                {
                    m_pMonster->Active_Effect(CSimonManus::P1_TRAIL);
                    m_bSwing = true;
                }
            }
            else
            {
                m_pMonster->DeActive_Effect(CSimonManus::P1_TRAIL);
            }
        }
    }

}

void CState_SimonManusP2_JumpToAttack::Control_Sound(_double CurTrackPos)
{
    if (m_iRouteTrack == 0)
    {
        if (!m_bMagicSound)
        {
            if (m_iCurStartAnim == AN_MAGICTO_LEFT || m_iCurStartAnim == AN_MAGICTO_RIGHT)
            {
                if ((CurTrackPos >= 40.f && CurTrackPos <= 45.f))
                {
                    m_pMonster->Play_Sound(CPawn::PAWN_SOUND_EFFECT1, TEXT("SE_NPC_SimonManus_SK_PJ_Ergo_Wide_05.wav"));
                    m_bMagicSound = true;
                }
            }
        }
    }
}

CState_SimonManusP2_JumpToAttack* CState_SimonManusP2_JumpToAttack::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManusP2_JumpToAttack* pInstance = new CState_SimonManusP2_JumpToAttack(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManusP2_JumpToAttack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManusP2_JumpToAttack::Free()
{
    __super::Free();
}