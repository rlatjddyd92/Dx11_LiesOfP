#include "stdafx.h"
#include "..\Public\UITutorial_Guide.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUITutorial_Guide::CUITutorial_Guide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage_Tutorial{ pDevice, pContext }
{

}

CUITutorial_Guide::CUITutorial_Guide(const CUITutorial_Guide& Prototype)
	: CUIPage_Tutorial{ Prototype }
{
}

HRESULT CUITutorial_Guide::Initialize_Prototype()
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

HRESULT CUITutorial_Guide::Initialize(void* pArg)
{


	return S_OK;
}

void CUITutorial_Guide::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUITutorial_Guide::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUITutorial_Guide::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
}

HRESULT CUITutorial_Guide::Render()
{
	return S_OK;
}

void CUITutorial_Guide::OpenAction()
{
	__super::OpenAction();
}

void CUITutorial_Guide::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUITutorial_Guide::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUITutorial_Guide::Ready_UIPart_Group_Control()
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

void CUITutorial_Guide::Set_Guide(vector<struct CUIPage::UIPART_INFO*>& vecOrigin)
{
	_int iIndex = 0;
	for (auto& iter : vecOrigin)
	{
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_GUIDE))
			break;

		++iIndex;
	}

	m_pSharedPointer_Frame = vecOrigin[iIndex];
	++iIndex;
	m_pSharedPointer_Title = vecOrigin[iIndex];

	for (_int i = 0; i < 4; ++i)
	{
		++iIndex;
		m_vecSharedPointer_Ctrl_Upper.push_back(vecOrigin[iIndex]);
	}

	for (_int i = 0; i < 4; ++i)
	{
		++iIndex;
		m_vecSharedPointer_Ctrl_Middle.push_back(vecOrigin[iIndex]);
	}

	for (_int i = 0; i < 4; ++i)
	{
		++iIndex;
		m_vecSharedPointer_Ctrl_Lower.push_back(vecOrigin[iIndex]);
	}
}

void CUITutorial_Guide::Update_Guide(TUTO_CHAPTER& NowData, _float fTimeDelta)
{
	if (m_iNowChapter == NowData.iCapterIndex)
		return;

	m_iNowChapter = NowData.iCapterIndex;

	m_pSharedPointer_Title->strText = NowData.strTitle;

	_int iCount = 0;
	iCount += Set_Crtl_Guide(&m_vecSharedPointer_Ctrl_Upper, NowData, 0);
	iCount += Set_Crtl_Guide(&m_vecSharedPointer_Ctrl_Middle, NowData, 1);
	iCount += Set_Crtl_Guide(&m_vecSharedPointer_Ctrl_Lower, NowData, 2);

	if (iCount == 3) m_pSharedPointer_Frame->fRatio = 1.f;
	if (iCount == 2) m_pSharedPointer_Frame->fRatio = 0.75f;
	if (iCount == 1) m_pSharedPointer_Frame->fRatio = 0.55f;
}

CUITutorial_Guide* CUITutorial_Guide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUITutorial_Guide* pInstance = new CUITutorial_Guide(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUITutorial_Popup"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUITutorial_Guide::Clone(void* pArg)
{
	CUITutorial_Guide* pInstance = new CUITutorial_Guide(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUITutorial_Popup"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUITutorial_Guide::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();

	m_vecSharedPointer_Ctrl_Upper.clear();
	m_vecSharedPointer_Ctrl_Middle.clear();
	m_vecSharedPointer_Ctrl_Lower.clear();
}
