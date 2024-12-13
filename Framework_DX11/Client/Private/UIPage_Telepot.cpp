#include "stdafx.h"
#include "..\Public\UIPage_Telepot.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Telepot::CUIPage_Telepot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Telepot::CUIPage_Telepot(const CUIPage_Telepot& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Telepot::Initialize_Prototype()
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

HRESULT CUIPage_Telepot::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Telepot::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Telepot::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Telepot::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
}

HRESULT CUIPage_Telepot::Render()
{
	return S_OK;
}

void CUIPage_Telepot::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Telepot::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Telepot::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Telepot::Ready_UIPart_Group_Control()
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

CUIPage_Telepot* CUIPage_Telepot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Telepot* pInstance = new CUIPage_Telepot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Telepot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Telepot::Clone(void* pArg)
{
	CUIPage_Telepot* pInstance = new CUIPage_Telepot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Telepot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Telepot::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
