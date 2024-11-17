#include "stdafx.h"
#include "..\Public\UIPage_LevelUp.h"

#include "GameInstance.h"


CUIPage_LevelUp::CUIPage_LevelUp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_LevelUp::CUIPage_LevelUp(const CUIPage_LevelUp& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_LevelUp::Initialize_Prototype()
{
	// 모든 UIPage는 클론 안 함, 여기서 모든 세팅 끝내기 

	UI_DESC			Desc{};

	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_vecPageAction.resize(_int(PAGEACTION::ACTION_END));

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CUIPage_LevelUp::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_LevelUp::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_LevelUp::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_LevelUp::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_LevelUp::Render()
{
	return S_OK;
}

void CUIPage_LevelUp::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_LevelUp::CloseAction()
{
	__super::CloseAction();
}

HRESULT CUIPage_LevelUp::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	return S_OK;
}

CUIPage_LevelUp* CUIPage_LevelUp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_LevelUp* pInstance = new CUIPage_LevelUp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_LevelUp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_LevelUp::Clone(void* pArg)
{
	CUIPage_LevelUp* pInstance = new CUIPage_LevelUp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_LevelUp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_LevelUp::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
