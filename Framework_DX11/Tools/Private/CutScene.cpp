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
	pDesc->dTrackPosition = m_fTrackPosition;

	m_KeyFrames.push_back(pDesc);
}

void CCutScene::Play_Keyframes(_float fTimeDelta)
{
	if (m_KeyFrames.size() == 0)
		return;

	for (auto& iter : m_KeyFrames)
	{
		if (m_fTrackPosition >= iter->dTrackPosition && iter->bActived)
		{
			iter->bActived = true;
		}
	}
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
