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

    return S_OK;
}

HRESULT CState_SimonManus_CutScene_Phase2::Start_State(void* pArg)
{
    m_iAnimation_Change = m_pMonster->Get_Model()->Find_AnimationIndex("AS_Manus_Phase_2_P1_Change", 1.f);
    m_pMonster->Get_Model()->Set_SpeedPerSec(m_iAnimation_Change, 31);

    m_pMonster->Change_Animation(m_iAnimation_Change, false, 0.f, 0);

    m_isChangePhase2 = false;

    m_pMonster->Set_EmissiveMask(0.1f);

    return S_OK;
}

void CState_SimonManus_CutScene_Phase2::Update(_float fTimeDelta)
{
    _int iFrame = m_pMonster->Get_Frame();

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
        }
        else if (m_pMonster->Get_EndAnim(m_iAnimation_Begod))
        {
           // m_pMonster->End_CutScene(0);
        }

        if (640 < iFrame && iFrame < 1310)
        {
            Contorl_Emissive(fTimeDelta);
        }
        else if (1340 < iFrame && iFrame < 1500)
        {
            if (!m_isSecondEmissive)
            {
                m_pMonster->Set_EmissiveMask(0.1f);
                m_isSecondEmissive = true;
            }
            Contorl_Emissive(fTimeDelta);
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

        if (!m_isCreatedHand && m_fStopedTimer > 9.29f)
        {
            m_isCreatedHand = true;

            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_CutScene_GodHand"), TEXT("Prototype_GameObject_CutScene_SimonManus_GodHand"))))
                return;
        }

        if (!m_bAnimationRestart && m_fStopedTimer > 17.3f)
        {
            m_pMonster->Play_Animation();
        }

    //ÀÌÆåÆ®
        if (!m_isCreateDome && m_fStopedTimer > 1.5f)
        {
            CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Map_SimonManus_2P"));
            m_isCreateDome = true;
        }

        if (!m_isDistortionHand && iFrame > 1655)
        {
            CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("SimonManus_ConnectGod"), _Vec3(0.f, 2.73f, 0.f));
            m_isDistortionHand = true;

            m_pMonster->Set_EmissiveMask(0.5f);
        }


    }

    Control_Dialog(iFrame);
}

void CState_SimonManus_CutScene_Phase2::End_State()
{
    m_pMonster->Active_Weapon();
}

void CState_SimonManus_CutScene_Phase2::Contorl_Emissive(_float fTimeDelta)
{
    _float fEmissive = m_pMonster->Get_EmissiveMask();

    if (fEmissive >= 0.33f)
    {
        m_isMaxEmissive = true;
        fEmissive = 0.33f;
    }
    else if (fEmissive <= 0.1f)
    {
        m_isMaxEmissive = false;
        fEmissive = 0.1f;
    }

    if (m_isMaxEmissive)
    {
        fEmissive -= 0.22f * fTimeDelta;
    }
    else
    {
        fEmissive += 0.8f * fTimeDelta;
    }

    m_pMonster->Set_EmissiveMask(fEmissive);
}

void CState_SimonManus_CutScene_Phase2::Control_Dialog(_int iFrame)
{
    if (!m_isChangePhase2)
    {
        if (!m_isShowDialog[0] && iFrame >= 283)
        {
            GET_GAMEINTERFACE->Show_Script(TEXT("ÈåÈåÈå, ÀÌ°ÍÀÌ ÁøÈ­´Ù."), TEXT("none"), 7.2f);
            m_isShowDialog[0] = true;
        }
        else if (!m_isShowDialog[1] && iFrame >= 628)
        {
            GET_GAMEINTERFACE->Show_Script(TEXT("ÀÌ ¾àÇÑ À°½ÅÀ» ¹þ¾î³ª..."), TEXT("ÁøÁ¤ÇÑ ½ÅÀÇ ÈûÀ» ¾ò´Â ³ª¸¦ ¸ñµµÇÏ¶ó!"), 24.f);
            m_isShowDialog[1] = true;
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