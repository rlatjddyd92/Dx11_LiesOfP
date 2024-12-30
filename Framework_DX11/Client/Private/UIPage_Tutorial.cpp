#include "stdafx.h"
#include "..\Public\UIPage_Tutorial.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

#include "UITutorial_Info.h";
#include "UITutorial_Guide.h";
#include "UITutorial_Timing.h";
#include "UITutorial_Result.h";
#include "UITutorial_Popup.h";

CUIPage_Tutorial::CUIPage_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Tutorial::CUIPage_Tutorial(const CUIPage_Tutorial& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Tutorial::Initialize_Prototype()
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

HRESULT CUIPage_Tutorial::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Tutorial::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Tutorial::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Tutorial::Late_Update(_float fTimeDelta)
{
	Update_Tutorial_Info(fTimeDelta);
	Update_Tutorial_Guide(fTimeDelta);
	Update_Tutorial_Timing(fTimeDelta);
	Update_Tutorial_Result(fTimeDelta);
	Update_Tutorial_Popup(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);

	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Tutorial::Render()
{
	return S_OK;
}

void CUIPage_Tutorial::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Tutorial::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Tutorial::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

void CUIPage_Tutorial::Close_Popup()
{
	m_bPopupOpen = false;
}

void CUIPage_Tutorial::Open_Popup()
{
	m_bPopupOpen = true;
}

void CUIPage_Tutorial::Update_Tutorial_Info(_float fTimeDelta)
{





}

void CUIPage_Tutorial::Update_Tutorial_Guide(_float fTimeDelta)
{





}

void CUIPage_Tutorial::Update_Tutorial_Timing(_float fTimeDelta)
{
	if (KEY_TAP(KEY::M))
		ShowTiming(KEY::LSHIFT, 2.f);

	if (m_pTiming->Update_Timing(fTimeDelta) == false)
		m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TIMING)]->bRender = false;
}

void CUIPage_Tutorial::Update_Tutorial_Result(_float fTimeDelta)
{



}

void CUIPage_Tutorial::Update_Tutorial_Popup(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ENTER))
		m_bPopupOpen = !m_bPopupOpen;

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_POPUP)]->bRender = m_bPopupOpen;
}

void CUIPage_Tutorial::ShowTiming(KEY eKey, _float fTime)
{
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TIMING)]->bRender = true;

	m_pTiming->Start_Timing(eKey, fTime);
}

HRESULT CUIPage_Tutorial::Ready_UIPart_Group_Control()
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
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;

	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_TIMING)]->bRender = false;
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_RESULT)]->bRender = false;

	Initialize_Tutorial();

	return S_OK;
}

void CUIPage_Tutorial::Initialize_Tutorial()
{
	for (_int i = 0; i < 4; ++i)
	{
		CUITutorial_Info* pNew = CUITutorial_Info::Create(m_pDevice, m_pContext);
		m_vecInfo.push_back(pNew);
	}

	m_pGuide = CUITutorial_Guide::Create(m_pDevice, m_pContext);
	m_pTiming = CUITutorial_Timing::Create(m_pDevice, m_pContext);
	m_pResult = CUITutorial_Result::Create(m_pDevice, m_pContext);
	m_pPopup = CUITutorial_Popup::Create(m_pDevice, m_pContext);

	m_pTiming->Set_Timing(m_vecPart);
}

void CUIPage_Tutorial::Update_Tutorial()
{






}

CUIPage_Tutorial* CUIPage_Tutorial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Tutorial* pInstance = new CUIPage_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Tutorial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Tutorial::Clone(void* pArg)
{
	CUIPage_Tutorial* pInstance = new CUIPage_Tutorial(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Tutorial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Tutorial::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	for (auto& iter : m_vecInfo)
	{
		Safe_Release(iter);
	}

	m_vecInfo.clear();

	Safe_Release(m_pGuide);
	Safe_Release(m_pTiming); 
	Safe_Release(m_pResult);
	Safe_Release(m_pPopup);

	m_vecPart.clear();
}
