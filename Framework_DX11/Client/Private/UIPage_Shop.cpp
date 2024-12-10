#include "stdafx.h"
#include "..\Public\UIPage_Shop.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Shop::CUIPage_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Shop::CUIPage_Shop(const CUIPage_Shop& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Shop::Initialize_Prototype()
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

HRESULT CUIPage_Shop::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Shop::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Shop::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Shop::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
}

HRESULT CUIPage_Shop::Render()
{
	return S_OK;
}

void CUIPage_Shop::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Shop::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Shop::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Shop::Ready_UIPart_Group_Control()
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

	m_bRender = false;

	return S_OK;
}

void CUIPage_Shop::Action_Tab(_float fTimeDelta)
{
}

void CUIPage_Shop::Action_Scroll(_float fTimeDelta)
{
}

void CUIPage_Shop::Action_Cell(_float fTimeDelta)
{
}

void CUIPage_Shop::Action_Focus(_float fTimeDelta)
{
}

void CUIPage_Shop::Update_Tab(_float fTimeDelta)
{
}

void CUIPage_Shop::Update_Scroll(_float fTimeDelta)
{
}

void CUIPage_Shop::Update_Cell(_float fTimeDelta)
{
}

void CUIPage_Shop::Update_Focus(_float fTimeDelta)
{
}

CUIPage_Shop* CUIPage_Shop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Shop* pInstance = new CUIPage_Shop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Shop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Shop::Clone(void* pArg)
{
	CUIPage_Shop* pInstance = new CUIPage_Shop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Shop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Shop::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
