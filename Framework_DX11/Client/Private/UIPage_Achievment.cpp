#include "stdafx.h"
#include "..\Public\UIPage_Achievment.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Achievment::CUIPage_Achievment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Achievment::CUIPage_Achievment(const CUIPage_Achievment& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Achievment::Initialize_Prototype()
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

	m_fTopPartMove = -1.f;

	return S_OK;
}

HRESULT CUIPage_Achievment::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Achievment::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Achievment::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Achievment::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
}

HRESULT CUIPage_Achievment::Render()
{
	return S_OK;
}

void CUIPage_Achievment::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Achievment::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Achievment::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Achievment::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	m_vec_Group_Ctrl.resize(_int(PART_GROUP::GROUP_END));

	for (auto& iter : m_vec_Group_Ctrl)
		iter = new UG_CTRL;

	for (_int i = 0; i < m_vecPart.size(); ++i)
	{
		if (m_vecPart[i]->iGroupIndex != -1)
			m_vec_Group_Ctrl[m_vecPart[i]->iGroupIndex]->PartIndexlist.push_back(i);
	}

	for (auto& iter : m_vecPart)
		iter->bRender = false;

	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;

	m_bRender = false;

	return S_OK;
}

CUIPage_Achievment* CUIPage_Achievment::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Achievment* pInstance = new CUIPage_Achievment(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Achievment"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Achievment::Clone(void* pArg)
{
	CUIPage_Achievment* pInstance = new CUIPage_Achievment(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Achievment"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Achievment::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
