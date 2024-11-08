#include "stdafx.h"
#include "..\Public\UIPage_Play.h"

#include "GameInstance.h"


CUIPage_Play::CUIPage_Play(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<UPART*>* pVecPart)
    : CUIPage{ pDevice, pContext }
{
	m_vecPart.resize(pVecPart->size());

	for (_int i = 0; i < m_vecPart.size(); ++i)
		m_vecPart[i] = (*pVecPart)[i];
}

CUIPage_Play::CUIPage_Play(const CUIPage_Play& Prototype)
    : CUIPage{ Prototype }
{
}

HRESULT CUIPage_Play::Initialize_Prototype()
{
	UI_DESC			Desc{};

	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_UIPart()))
		return E_FAIL;

    return S_OK;
}

HRESULT CUIPage_Play::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Play::Priority_Update(_float fTimeDelta)
{
}

void CUIPage_Play::Update(_float fTimeDelta)
{
}

void CUIPage_Play::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIPage_Play::Render()
{
    return S_OK;
}

HRESULT CUIPage_Play::Ready_UIPart()
{
	__super::Ready_UIPart();

	return S_OK;
}

CUIPage_Play* CUIPage_Play::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, vector<UPART*>* pVecPart)
{
	CUIPage_Play* pInstance = new CUIPage_Play(pDevice, pContext, pVecPart);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Play"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Play::Clone(void* pArg)
{
	CUIPage_Play* pInstance = new CUIPage_Play(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Play"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Play::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete_Array(iter->strUIPart_Name);
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
