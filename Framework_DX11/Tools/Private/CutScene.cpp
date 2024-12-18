#include "stdafx.h"
#include "CutScene.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "Controller_UITool.h"
#include "Camera.h"

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
	m_pController_UITool = CController_UITool::Get_Instance();
	Safe_AddRef(m_pController_UITool);

	m_isActive = false;
	m_pCamera = m_pGameInstance->Find_Camera(LEVEL_TOOL);
	return S_OK;
}

void CCutScene::Priority_Update(_float fTimeDelta)
{
}

void CCutScene::Update(_float fTimeDelta)
{
	if (m_bPlay && m_fTrackPosition < m_fMaxFrame)
	{
		m_fTrackPosition += fTimeDelta;
		Play_Keyframes(fTimeDelta);
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

	m_pController_UITool->Fade_In(0.f);
}

void CCutScene::Create_KeyFrame()
{
	CUTSCENE_KEYFRAME_DESC* pDesc = new CUTSCENE_KEYFRAME_DESC;
	pDesc->fTrackPosition = m_fTrackPosition;

	m_KeyFrames.push_back(pDesc);
	Sort_KeyFrame_TrackPosition();
}

void CCutScene::Load_KeyFrame(CUTSCENE_KEYFRAME_DESC pDesc)
{
	CUTSCENE_KEYFRAME_DESC* pNewDesc = new CUTSCENE_KEYFRAME_DESC;
	/*pNewDesc->fTrackPosition = pDesc.fTrackPosition;
	pNewDesc->bActived = pDesc.bActived;
	pNewDesc->ShaderDesc = pDesc.ShaderDesc;
	pNewDesc->UI_DESC = pDesc.UI_DESC;
	pNewDesc->Obj_Desc = pDesc.Obj_Desc;
	pNewDesc->Sound_Desc = pDesc.Sound_Desc;

	pNewDesc->Camera_Desc.bTeleport = pDesc.Camera_Desc.bTeleport;
	pNewDesc->Camera_Desc.bTurn = pDesc.Camera_Desc.bTurn;
	pNewDesc->Camera_Desc.bZoomIn = pDesc.Camera_Desc.bZoomIn;
	pNewDesc->Camera_Desc.bZoomOut = pDesc.Camera_Desc.bZoomOut;
	pNewDesc->Camera_Desc.bLerpMove = pDesc.Camera_Desc.bLerpMove;
	pNewDesc->Camera_Desc.mCameraWorlMatrix = pDesc.Camera_Desc.mCameraWorlMatrix;
	pNewDesc->Camera_Desc.vPitchTawRoll = pDesc.Camera_Desc.vPitchTawRoll;
	pNewDesc->Camera_Desc.fTurn_Speed = pDesc.Camera_Desc.fTurn_Speed;
	pNewDesc->Camera_Desc.fZoomDuration = pDesc.Camera_Desc.fZoomDuration;
	pNewDesc->Camera_Desc.fFovy = pDesc.Camera_Desc.fFovy;
	pNewDesc->Camera_Desc.fMoveSpeed = pDesc.Camera_Desc.fMoveSpeed;
	pNewDesc->Camera_Desc.vTargetPos = pDesc.Camera_Desc.vTargetPos;*/

	*pNewDesc = pDesc;
	pNewDesc->bActived = false;

	m_KeyFrames.push_back(pNewDesc);
}

void CCutScene::Change_NewKeyFrame(CUTSCENE_KEYFRAME_DESC pDesc, _int iIndex)
{
	CUTSCENE_KEYFRAME_DESC* pNewDesc = new CUTSCENE_KEYFRAME_DESC;
	*pNewDesc = pDesc;
	pNewDesc->bActived = false;

	m_KeyFrames[iIndex] = pNewDesc;
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
			Active_Shader(iter, fTimeDelta);
			Active_UI(iter);
			Active_Camera(iter);
		}
		if (iter->ShaderDesc.bDof_Decrease || iter->ShaderDesc.bDof_Increase)
		{
			DOF_DESC* tDesc = m_pGameInstance->Get_DOFDesc();
			if (nullptr == tDesc)
				return;

			if (iter->ShaderDesc.bDof_Increase)

			{
				m_fActiveDofValue += iter->ShaderDesc.fSpeed * fTimeDelta;
				
			}

			tDesc->fDOF = m_fActiveDofValue;
		}
	}
}

void CCutScene::Active_Shader(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc, _float fTimeDelta)
{
	DOF_DESC* tDesc = m_pGameInstance->Get_DOFDesc();
	if (nullptr == tDesc)
		return;

	tDesc->isOnDOF = pCutSceneDesc->ShaderDesc.bUseDof;

	if (tDesc->isOnDOF == false)
		return;

	tDesc->isInverse = pCutSceneDesc->ShaderDesc.bUseDof_Inverse;
	tDesc->fDOF = pCutSceneDesc->ShaderDesc.fDof;
	m_fActiveDofValue = tDesc->fDOF;
}

void CCutScene::Active_UI(CUTSCENE_KEYFRAME_DESC* pCutSceneDesc)
{
	if (pCutSceneDesc->UI_DESC.bFadeOut)
	{
		m_pController_UITool->Fade_Out(TEXT(""), TEXT(""), pCutSceneDesc->UI_DESC.fColor, pCutSceneDesc->UI_DESC.fTime);
	}
	if (pCutSceneDesc->UI_DESC.bFadeIn)
	{
		m_pController_UITool->Fade_In(pCutSceneDesc->UI_DESC.fTime);
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

void CCutScene::Sort_KeyFrame_TrackPosition()
{
	//TrakcPostition에 따라 자동 정렬
	sort(m_KeyFrames.begin(), m_KeyFrames.end(), [](const CUTSCENE_KEYFRAME_DESC* a, const CUTSCENE_KEYFRAME_DESC* b)
		{
			return a->fTrackPosition < b->fTrackPosition;
		});
}

void CCutScene::Delete_Selected_Keyframe(_int iIndex)
{
	delete m_KeyFrames[iIndex];
	m_KeyFrames[iIndex] = nullptr;
	m_KeyFrames.erase(m_KeyFrames.begin() + iIndex);
	Sort_KeyFrame_TrackPosition();
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

	Safe_Release(m_pController_UITool);
}
