#include "stdafx.h"
#include "State_SimonManus_CutScene_Phase2.h"
#include "GameInstance.h"
#include "Model.h"
#include "SimonManus.h"

#include "Effect_Manager.h"

#include "GameInterface_Controller.h"

CState_SimonManus_CutScene_Phase2::CState_SimonManus_CutScene_Phase2(CFsm* pFsm, CMonster* pMonster)
    :CState{ pFsm }
    , m_pMonster{ pMonster }
{
}

HRESULT CState_SimonManus_CutScene_Phase2::Initialize(_uint iStateNum, void* pArg)
{

    m_iStateNum = iStateNum;
    //CSimonManus::FSMSTATE_DESC* pDesc = static_cast<CSimonManus::FSMSTATE_DESC*>(pArg);

    m_iKneeFrame[0] = 87;
    m_iKneeFrame[1] = 90;

    return S_OK;
}

HRESULT CState_SimonManus_CutScene_Phase2::Start_State(void* pArg)
{
    m_iAnimation_Change = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Manus_Phase_2_P1_Change", 1.f);
    m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_Change, 31);

    m_pMonster->Change_Animation(m_iAnimation_Change, false, 0.f, 0);

    m_isChangePhase2 = { false };
    m_isCreatedHand = { false };
    m_isCreateDome = { false };
    m_isDistortionHand = { false };
    m_bAnimationSpeedDown = { false };
    m_bAnimationStop = { false };
    m_bAnimationRestart = { false };
    m_isFirstEmissive = { false };
    m_isSecondEmissive = { false };
    m_isMaxEmissive = false;
    ZeroMemory(m_isShowDialog, sizeof(_bool) * 2);
    ZeroMemory(m_isKneeDown, sizeof(_bool) * 2);
    m_isHandSmoke = { false };
    m_isAura = { false };
    m_isParticle = { false };
    m_fStopedTimer = 0.f;

    m_pMonster->Set_EmissiveMask(0.12f);

    return S_OK;
}

void CState_SimonManus_CutScene_Phase2::Update(_float fTimeDelta)
{
    _int iFrame = m_pMonster->Get_Frame();
    _float fTrackPosition = (_float)m_pMonster->Get_CurrentTrackPos();
    _uint iCurAnimIndex = m_pMonster->Get_CurrentAnimIndex();

    if (!m_isChangePhase2)
    {
        if (iFrame < 105)
        {
            m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_Change, 35);
        }
        else if (m_bAnimationSpeedDown == false)
        {
            m_bAnimationSpeedDown = true;
            m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_Change, 31);
        }


        if (iFrame > 900)
        {
            m_pMonster->Change_Model(0);

            m_iAnimation_Connectgod = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Manus_Phase_2_P2_Connect_God", 1.f);
            m_iAnimation_Begod = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Manus_Phase_2_P2_be_God", 1.f);

            m_pMonster->Change_Animation(m_iAnimation_Connectgod, false, 0.1f, 0);
            m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_Connectgod, 34);
            m_isChangePhase2 = true;
        }
    }
    if (m_isChangePhase2)
    {
        if (m_pMonster->Get_EndAnim(m_iAnimation_Connectgod))
        {
            m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_CutScene_GodHand"), 0)->Set_Dead(true);
            m_pMonster->Change_Animation(m_iAnimation_Begod, false, 0.1f, 0);
            m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_Begod, 23);
        }
        else if (m_pMonster->Get_EndAnim(m_iAnimation_Begod))
        {
           // m_pMonster->End_CutScene(0);
        }

        if (iCurAnimIndex == m_iAnimation_Connectgod)
        {
            if (609 < fTrackPosition && fTrackPosition < 658)
            {
                Contorl_Emissive(fTimeDelta);
            }
            else if (760 < fTrackPosition && fTrackPosition < 1350)
            {
                if (!m_isFirstEmissive)
                {
                    m_pMonster->Set_EmissiveMask(0.1f);
                    m_isFirstEmissive = true;
                }
                Contorl_Emissive(fTimeDelta);
            }
            else if (1360 < fTrackPosition && fTrackPosition < 1700)
            {
                if (!m_isSecondEmissive)
                {
                    m_pMonster->Set_EmissiveMask(0.1f);
                    m_isSecondEmissive = true;
                }
                Contorl_Emissive(fTimeDelta);
            }
        }

        if (!m_bAnimationStop && iFrame > 1342)
        {
            m_bAnimationStop = true;
            m_pMonster->Stop_Animation();
        }
        else if (m_bAnimationStop)
        {
            m_fStopedTimer += fTimeDelta;
        }

        if (!m_isCreatedHand && m_fStopedTimer > 9.34f)
        {
            m_isCreatedHand = true;

            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_CutScene_GodHand"), TEXT("Prototype_GameObject_CutScene_SimonManus_GodHand"))))
                return;
        }

        if (!m_bAnimationRestart && m_fStopedTimer > 17.3f)
        {
            m_pMonster->Play_Animation();
        }

        if (iCurAnimIndex == m_iAnimation_Begod && m_isDistortionHand)
        {
            m_pMonster->Set_EmissiveMask(0.5f);
        }

    }

    Control_Effect(iFrame);
    Control_Dialog(iFrame);
}

void CState_SimonManus_CutScene_Phase2::End_State()
{
    m_pMonster->Active_Weapon();
}

void CState_SimonManus_CutScene_Phase2::Contorl_Emissive(_float fTimeDelta)
{
    _float fEmissive = m_pMonster->Get_EmissiveMask();

    if (fEmissive >= 0.37f)
    {
        m_isMaxEmissive = true;
        fEmissive = 0.37f;
    }
    else if (fEmissive <= 0.12f)
    {
        m_isMaxEmissive = false;
        fEmissive = 0.12f;
    }

    if (m_isMaxEmissive)
    {
        fEmissive -= 0.213f * fTimeDelta;
    }
    else
    {
        fEmissive += 0.91f * fTimeDelta;
    }

    m_pMonster->Set_EmissiveMask(fEmissive);
}

void CState_SimonManus_CutScene_Phase2::Control_Dialog(_int iFrame)
{
    if (!m_isChangePhase2)
    {
        if (!m_isShowDialog[0] && iFrame >= 283)
        {
            GET_GAMEINTERFACE->Show_Script(TEXT("흐흐흐, 이것이 진화다."), TEXT("none"), 7.2f);
            m_isShowDialog[0] = true;
        }
        else if (!m_isShowDialog[1] && iFrame >= 628)
        {
            GET_GAMEINTERFACE->Show_Script(TEXT("이 약한 육신을 벗어나..."), TEXT("진정한 신의 힘을 얻는 나를 목도하라!"), 24.f);
            m_isShowDialog[1] = true;
        }
    }
}

void CState_SimonManus_CutScene_Phase2::Control_Effect(_int iFrame)
{
    _uint iCurAnim = m_pMonster->Get_CurrentAnimIndex();

    if (!m_isChangePhase2)
    {
        if (iCurAnim == m_iAnimation_Change)
        {
            if (!m_isKneeDown[0] && iFrame >= 85)
            {
                _Vec3 vCurrentPos = m_pMonster->Calc_BoneWorldPos("Bn_R_Knee");
                vCurrentPos.y -= 0.1f;

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer_Rot(LEVEL_GAMEPLAY, TEXT("SimonManus_CutScene_SmallSmoke"), vCurrentPos);

                m_isKneeDown[0] = true;
            }
            else if (!m_isKneeDown[1] && iFrame >= 98)
            {
                _Vec3 vCurrentPos = m_pMonster->Calc_BoneWorldPos("Bn_L_Knee");
                vCurrentPos.y -= 0.1f;

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer_Rot(LEVEL_GAMEPLAY, TEXT("SimonManus_CutScene_SmallSmoke"), vCurrentPos);

                m_isKneeDown[1] = true;
            }
            else if (!m_isHandSmoke && iFrame >= 95)
            {
                _Vec3 vCurrentPos = m_pMonster->Calc_BoneWorldPos("Bn_Body_14");
                vCurrentPos.y -= 0.1f;

                CEffect_Manager::Get_Instance()->Add_Effect_ToLayer_Rot(LEVEL_GAMEPLAY, TEXT("SimonManus_CutScene_SmallSmoke"), vCurrentPos);

                m_isHandSmoke = true;
            }
        }
    }
    else
    {
        if (!m_isCreateDome && m_fStopedTimer > 1.5f)
        {
            m_pMonster->Active_Effect(CSimonManus::CUTSCENE_P2_MAP);

            m_isCreateDome = true;
        }

        if (!m_isDistortionHand && iFrame > 1655)
        {
            CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("SimonManus_ConnectGod"), _Vec3(0.f, 3.2f, 0.6f));
            m_isDistortionHand = true;
        }

        if (!m_isAura && iCurAnim == m_iAnimation_Begod && m_isDistortionHand)
        {
            static_cast<CSimonManus*>(m_pMonster)->On_Aura(true, true);

            m_isAura = true;
        }
        else if (!m_isParticle && iFrame >= 50 && m_isDistortionHand)
        {
            m_pMonster->Active_Effect(CSimonManus::CUTSCENE_P2_ARM_PARTICLE);

            m_isParticle = true;
        }
    }

}

CState_SimonManus_CutScene_Phase2* CState_SimonManus_CutScene_Phase2::Create(CFsm* pFsm, CMonster* pMonster, _uint iStateNum, void* pArg)
{
    CState_SimonManus_CutScene_Phase2* pInstance = new CState_SimonManus_CutScene_Phase2(pFsm, pMonster);

    if (FAILED(pInstance->Initialize(iStateNum, pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_SimonManus_CutScene_Phase2"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_SimonManus_CutScene_Phase2::Free()
{
    __super::Free();
}