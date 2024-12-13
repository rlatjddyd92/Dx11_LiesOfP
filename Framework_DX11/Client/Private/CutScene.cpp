#include "stdafx.h"
#include "CutScene.h"
#include "GameInstance.h"
#include "GameInterface_Controller.h"
#include "Camera.h"
#include "Camera_Manager.h"
#include "Pawn.h"
#include "SimonManus.h"

CCutScene::CCutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CCutScene::CCutScene(const CCutScene& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CCutScene::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCutScene::Initialize(void* pArg)
{
	m_isActive = false;

	m_pCamera = CCamera_Manager::Get_Instance()->Find_Camera(TEXT("Camera_Free"));

	m_pObjects[PLAYER] = static_cast<CPawn*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
//	m_pObjects[BOSS1]  = static_cast<CPawn*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Raxasia"), 0));
	

	return S_OK;
}

void CCutScene::Priority_Update(_float fTimeDelta)
{
}

void CCutScene::Update(_float fTimeDelta)
{
	if (m_bFirstStart)
		First_Setting();

	if (m_bPlay && m_fTrackPosition < m_fMaxFrame)
	{
		m_fTrackPosition += fTimeDelta;
		Play_Keyframes(fTimeDelta);
	}
	else if (m_fTrackPosition > m_fMaxFrame)
	{
		m_bPlay = false;
		m_bFinished = true;
	}
	
	if(m_bFinished && m_bFinishe_Setting == false)
	{
		End_Setting();
	}
}

void CCutScene::Late_Update(_float fTimeDelta)
{
}

void CCutScene::Keyframe_Actived_Reset()
{
	if (m_KeyFrames.size() == 0)
		return;

	for (auto& iter : m_KeyFrames)
	{
		iter->bActived = false;
	}
}

void CCutScene::Create_KeyFrame()
{
	CUTSCENE_KEYFRAME_DESC* pDesc = new CUTSCENE_KEYFRAME_DESC;
	pDesc->fTrackPosition = m_fTrackPosition;

	m_KeyFrames.push_back(pDesc);
}

void CCutScene::Play_Keyframes(_float fTimeDelta)
{
	if (m_KeyFrames.size() == 0)
		return;

	for (auto& iter : m_KeyFrames)
	{
		if (m_fTrackPosition >= iter->fTrackPosition && iter->bActived == false)
		{
			iter->bActived = true;

			Active_Shader(iter);
			Active_UI(iter);
			Active_Camera(iter);
			Active_Obj(iter);
			Active_Sound(iter);
		}
	}
}

void CCutScene::Active_Shader(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc)
{
	DOF_DESC* tDesc = m_pGameInstance->Get_DOFDesc();
	if (nullptr == tDesc)
		return;

	tDesc->isOnDOF = pCutSceneDesc->ShaderDesc.bUseDof;
	tDesc->fDOF = pCutSceneDesc->ShaderDesc.fDof;
}

void CCutScene::Active_UI(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc)
{
	//UI 숨겨야 함
	if (pCutSceneDesc->UI_DESC.bFadeOut)
	{
		GET_GAMEINTERFACE->Fade_Out(TEXT(""), TEXT(""), pCutSceneDesc->UI_DESC.fColor, pCutSceneDesc->UI_DESC.fTime);
	}
	if (pCutSceneDesc->UI_DESC.bFadeIn)
	{
		GET_GAMEINTERFACE->Fade_In(pCutSceneDesc->UI_DESC.fTime);
		GET_GAMEINTERFACE->UIPart_Off();
	}
}

void CCutScene::Active_Camera(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc)
{
	if (pCutSceneDesc->Camera_Desc.bTeleport)
	{
		m_pCamera->Get_Transform()->Set_WorldMatrix(pCutSceneDesc->Camera_Desc.mCameraWorlMatrix);
	}
	if (pCutSceneDesc->Camera_Desc.bTurn)
	{
		m_pCamera->Start_Turn(pCutSceneDesc->Camera_Desc.fTurn_Speed, pCutSceneDesc->Camera_Desc.vPitchTawRoll);
	}
	if (pCutSceneDesc->Camera_Desc.bZoomIn)
	{
		m_pCamera->ZoomIn(pCutSceneDesc->Camera_Desc.fFovy, pCutSceneDesc->Camera_Desc.fZoomDuration);
	}
	if (pCutSceneDesc->Camera_Desc.bZoomOut)
	{
		m_pCamera->ZoomOut(pCutSceneDesc->Camera_Desc.fZoomDuration);
	}
	if (pCutSceneDesc->Camera_Desc.bLerpMove)
	{
		m_pCamera->Start_MoveLerp(pCutSceneDesc->Camera_Desc.vTargetPos, pCutSceneDesc->Camera_Desc.fMoveSpeed);
	}
	if (pCutSceneDesc->Camera_Desc.bShake)
	{
		m_pCamera->Start_PosShake(pCutSceneDesc->Camera_Desc.fShakePower, pCutSceneDesc->Camera_Desc.fShakeDuration);
	}
}

void CCutScene::Active_Obj(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc)
{
	if (pCutSceneDesc->Obj_Desc.bUseObj[PLAYER])
	{
		m_pObjects[PLAYER]->Change_State(*pCutSceneDesc->Obj_Desc.iStateNum);
	}
	if (pCutSceneDesc->Obj_Desc.bUseObj[BOSS1])
	{
		m_pObjects[BOSS1]->Change_State(*pCutSceneDesc->Obj_Desc.iStateNum);
	}
	if (pCutSceneDesc->Obj_Desc.bUseObj[BOSS2])
	{
		m_pObjects[BOSS2]->Change_State(*pCutSceneDesc->Obj_Desc.iStateNum);
	}
}

void CCutScene::Active_Sound(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc)
{	
	if (pCutSceneDesc->Sound_Desc.bStopBGM)
		m_pGameInstance->Stop_BGM();

	if(pCutSceneDesc->Sound_Desc.bChangeBGM)
	{
		switch (m_iIndex)
		{
		case SOPHIA_ENTER:
			m_pGameInstance->Play_BGM(TEXT("MU_MS_Monastery_B_Loop.wav"), &g_fBGMVolume);
			break;
		case SOPHIA_DEAD:
			m_pGameInstance->Play_BGM(TEXT("MU_MS_Monastery_B_Loop.wav"), &g_fBGMVolume);
			break;
		default:
			break;
		}
	}
}

void CCutScene::First_Setting()
{
	m_bFirstStart = false;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));

	GET_GAMEINTERFACE->UIPart_Off();

	_Vec3 vInitPos = {};

	switch (m_iIndex)
	{
	case SOPHIA_DEAD:
		pPlayer->Disappear_Weapon();
		break;
	case BOSS2_MEET:
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_SimonManus"), TEXT("Prototype_GameObject_SimonManus"))))
			return;

		vInitPos = _Vec3(16.606f, 0.104f, -16.498f);
		pPlayer->Get_RigidBody()->Set_GloblePose(vInitPos);
		pPlayer->Get_Navigation()->Research_Cell(vInitPos);
		pPlayer->Get_Transform()->Rotation(0.f, -45.f, 0.f);
		pPlayer->Disappear_Weapon();
		//m_pObjects[BOSS2] = static_cast<CPawn*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_SimonManus"), 0));
		break;
	case BOSS2_PHASE2:
		pPlayer->IsActive(false);
		static_cast<CSimonManus*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_SimonManus"), 0))->Start_CutScene(1);
		break;
	}
}

void CCutScene::End_Setting()
{
	m_bFinishe_Setting = true;

	//UI 살려야 함
	GET_GAMEINTERFACE->Fade_In(0.f);
	GET_GAMEINTERFACE->UIPart_On();

	CCamera_Manager::Get_Instance()->Change_Camera(TEXT("Camera_Player"));

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));

	if (m_iIndex == SOPHIA_ENTER)
	{
		pPlayer->Appear_Weapon();
		pPlayer->Get_Model()->ReadyDenyNextTranslate(4);
		pPlayer->Change_State(CPlayer::OH_IDLE);
		pPlayer->Get_Navigation()->Move_to_Cell(pPlayer->Get_RigidBody(), 1178);
	}
	else if (m_iIndex == SOPHIA_DEAD)
	{
		pPlayer->Appear_Weapon();
		pPlayer->Get_Model()->ReadyDenyNextTranslate(4);
		pPlayer->Change_State(CPlayer::OH_IDLE);
		pPlayer->Get_Navigation()->Move_to_Cell(pPlayer->Get_RigidBody(), 1178);
	}
	else if (m_iIndex == BOSS2_MEET)
	{
		dynamic_cast<CCutScene*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), BOSS2_MEET2))->Start_Play();
	}
	else if (m_iIndex == BOSS2_MEET2)
	{
		dynamic_cast<CCutScene*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), BOSS2_MEET3))->Start_Play();
	}
	else if (m_iIndex == BOSS2_MEET3)
	{
		pPlayer->Change_State(CPlayer::OH_IDLE);
		pPlayer->Get_Navigation()->Move_to_Cell(pPlayer->Get_RigidBody(), 119);
		pPlayer->Appear_Weapon();
		static_cast<CPawn*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_SimonManus"), 0))->End_CutScene(0);
	}
	else if (m_iIndex == BOSS2_PHASE2)
	{
		pPlayer->IsActive(true);
		//static_cast<CPawn*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_SimonManus"), 0))->End_CutScene(1);
	}
	pPlayer->Get_Camera()->Move_PlayerBackPos();
}

void CCutScene::Load_KeyFrame(CUTSCENE_KEYFRAME_DESC pDesc)
{
	CUTSCENE_KEYFRAME_DESC* pNewDesc = new CUTSCENE_KEYFRAME_DESC;
	*pNewDesc = pDesc;
	pNewDesc->bActived = false;
	
	m_KeyFrames.push_back(pNewDesc);
}

void CCutScene::Start_Play()
{
	CCamera_Manager::Get_Instance()->Change_Camera(TEXT("Camera_Free"));
	m_isActive = true;
	m_bPlay = true;
}

CCutScene* CCutScene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCutScene* pInstance = new CCutScene(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCutScene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCutScene::Clone(void* pArg)
{
	CCutScene* pInstance = new CCutScene(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCutScene::Free()
{
	__super::Free();

	for (auto& iter : m_KeyFrames)
	{
		delete iter;
		iter = nullptr;
	}
}
