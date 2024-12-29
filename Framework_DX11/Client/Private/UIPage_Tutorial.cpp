#include "stdafx.h"
#include "..\Public\UIPage_Tutorial.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

#include "UITutorial_Info.h";
#include "UITutorial_Guide.h";
#include "UITutorial_Timing.h";
#include "UITutorial_Result.h";


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
	__super::Late_Update(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
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

void CUIPage_Tutorial::NextMission()
{
}

void CUIPage_Tutorial::NextGuide()
{
}

void CUIPage_Tutorial::ShowResult()
{
}

void CUIPage_Tutorial::OffResult()
{
}

void CUIPage_Tutorial::ShowTiming(KEY eKey, _float fTime)
{
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

	return S_OK;
}

void CUIPage_Tutorial::Initialize_Tutorial()
{
	m_pInfo = CUITutorial_Info::Create(m_pDevice, m_pContext);
	m_pGuide = CUITutorial_Guide::Create(m_pDevice, m_pContext);
	m_pTiming = CUITutorial_Timing::Create(m_pDevice, m_pContext);
	m_pResult = CUITutorial_Result::Create(m_pDevice, m_pContext);






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

	Safe_Release(m_pInfo);
	Safe_Release(m_pGuide);
	Safe_Release(m_pTiming);
	Safe_Release(m_pResult);

	m_vecPart.clear();
}
