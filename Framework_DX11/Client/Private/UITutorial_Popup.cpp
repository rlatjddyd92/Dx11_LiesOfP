#include "stdafx.h"
#include "..\Public\UITutorial_Popup.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUITutorial_Popup::CUITutorial_Popup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage_Tutorial{ pDevice, pContext }
{

}

CUITutorial_Popup::CUITutorial_Popup(const CUITutorial_Popup& Prototype)
	: CUIPage_Tutorial{ Prototype }
{
}

HRESULT CUITutorial_Popup::Initialize_Prototype()
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

HRESULT CUITutorial_Popup::Initialize(void* pArg)
{


	return S_OK;
}

void CUITutorial_Popup::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUITutorial_Popup::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUITutorial_Popup::Late_Update(_float fTimeDelta)
{
	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);

	__super::Late_Update(fTimeDelta);
}

HRESULT CUITutorial_Popup::Render()
{
	return S_OK;
}

void CUITutorial_Popup::OpenAction()
{
	__super::OpenAction();
}

void CUITutorial_Popup::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUITutorial_Popup::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUITutorial_Popup::Ready_UIPart_Group_Control()
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

void CUITutorial_Popup::Set_Popup(vector<struct CUIPage::UIPART_INFO*>& vecOrigin)
{
	_int iIndex = 0;
	for (auto& iter : vecOrigin)
	{
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_POPUP))
			break;

		++iIndex;
	}

	for (_int i = 0; i < 7; ++i)
	{
		m_vecSharedPointer_Frame.push_back(vecOrigin[iIndex]);
		++iIndex;
	}

	m_pSharedPointer_Title = m_vecSharedPointer_Frame[m_vecSharedPointer_Frame.size() - 2];

	for (_int i = 0; i < 4; ++i)
	{
		m_vecSharedPointer_Ctrl_Upper.push_back(vecOrigin[iIndex]);
		++iIndex;
	}

	for (_int i = 0; i < 4; ++i)
	{
		m_vecSharedPointer_Ctrl_Middle.push_back(vecOrigin[iIndex]);
		++iIndex;
	}

	for (_int i = 0; i < 4; ++i)
	{
		m_vecSharedPointer_Ctrl_Lower.push_back(vecOrigin[iIndex]);
		++iIndex;
	}

	m_pSharedPointer_DescA = vecOrigin[iIndex];
	++iIndex;
	m_pSharedPointer_DescB = vecOrigin[iIndex];
	++iIndex;

	for (_int i = 0; i < 3; ++i)
	{
		m_vecSharedPointer_Frame.push_back(vecOrigin[iIndex]);
		++iIndex;
	}
}

void CUITutorial_Popup::Update_Popup(TUTO_CHAPTER& NowData, _float fTimeDelta)
{
	m_pSharedPointer_Title->strText = NowData.strTitle;

	Set_Crtl_Guide(&m_vecSharedPointer_Ctrl_Upper, NowData, 0);
	Set_Crtl_Guide(&m_vecSharedPointer_Ctrl_Middle, NowData, 1);
	Set_Crtl_Guide(&m_vecSharedPointer_Ctrl_Lower, NowData, 2);

	if (NowData.strDescA[0] != 'n')
	{
		m_pSharedPointer_DescA->strText = NowData.strDescA;
		m_pSharedPointer_DescA->bRender = true;
	}
	else 
		m_pSharedPointer_DescA->bRender = false;
		
	if (NowData.strDescB[0] != 'n')
	{
		m_pSharedPointer_DescB->strText = NowData.strDescB;
		m_pSharedPointer_DescB->bRender = true;
	}
	else
		m_pSharedPointer_DescB->bRender = false;
}

CUITutorial_Popup* CUITutorial_Popup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUITutorial_Popup* pInstance = new CUITutorial_Popup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUITutorial_Popup"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUITutorial_Popup::Clone(void* pArg)
{
	CUITutorial_Popup* pInstance = new CUITutorial_Popup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUITutorial_Popup"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUITutorial_Popup::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();

	m_vecSharedPointer_Frame.clear();
	
	m_vecSharedPointer_Ctrl_Upper.clear();
	m_vecSharedPointer_Ctrl_Middle.clear();
	m_vecSharedPointer_Ctrl_Lower.clear();
}
