#include "stdafx.h"
#include "CutScene.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"

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
}

void CCutScene::Create_KeyFrame()
{
	CUTSCENE_DESC* pDesc = new CUTSCENE_DESC;
	pDesc->fTrackPosition = m_fTrackPosition;

	m_KeyFrames.push_back(pDesc);
	Sort_KeyFrame_TrackPosition();
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
		}
	}
}

void CCutScene::Active_Shader(CUTSCENE_DESC* pCutSceneDesc)
{
	DOF_DESC* tDesc = m_pGameInstance->Get_DOFDesc();
	if (nullptr == tDesc)
		return;

	tDesc->isOnDOF = pCutSceneDesc->ShaderDesc.bUseDof;
	tDesc->fDOF = pCutSceneDesc->ShaderDesc.fDof;
}

void CCutScene::Active_UI(CUTSCENE_DESC* pCutSceneDesc)
{
}

void CCutScene::Sort_KeyFrame_TrackPosition()
{
	//TrakcPostition에 따라 자동 정렬
	sort(m_KeyFrames.begin(), m_KeyFrames.end(), [](const CUTSCENE_DESC* a, const CUTSCENE_DESC* b)
		{
			return a->fTrackPosition < b->fTrackPosition;
		});
}

void CCutScene::Delete_Selected_Keyframe(_int iIndex)
{
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
}
