#include "stdafx.h"
#include "..\Public\UITutorial_Timing.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUITutorial_Timing::CUITutorial_Timing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage_Tutorial{ pDevice, pContext }
{

}

CUITutorial_Timing::CUITutorial_Timing(const CUITutorial_Timing& Prototype)
	: CUIPage_Tutorial{ Prototype }
{
}

HRESULT CUITutorial_Timing::Initialize_Prototype()
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

HRESULT CUITutorial_Timing::Initialize(void* pArg)
{


	return S_OK;
}

void CUITutorial_Timing::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUITutorial_Timing::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUITutorial_Timing::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);
}

HRESULT CUITutorial_Timing::Render()
{
	return S_OK;
}

void CUITutorial_Timing::OpenAction()
{
	__super::OpenAction();
}

void CUITutorial_Timing::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUITutorial_Timing::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUITutorial_Timing::Ready_UIPart_Group_Control()
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

void CUITutorial_Timing::Set_Timing(vector<struct CUIPage::UIPART_INFO*>& vecOrigin)
{
	_int iIndex = 0;


	for (auto& iter : vecOrigin)
	{
		if (iter->iGroupIndex == _int(PART_GROUP::GROUP_TIMING))
		{
			if (iIndex == 0) m_pSharedPointer_Frame = iter;
			if (iIndex == 1) m_pSharedPointer_Gauge = iter;
			if (iIndex == 2) m_pSharedPointer_Icon = iter;
			++iIndex;
		}
	}



}

void CUITutorial_Timing::Start_Timing(KEY eKey, _float fTime)
{
	if (eKey == KEY::LSHIFT)
		m_pSharedPointer_Icon->iTexture_Index = 250;
	else if (eKey == KEY::LBUTTON)
		m_pSharedPointer_Icon->iTexture_Index = 302;
	else if (eKey == KEY::RBUTTON)
		m_pSharedPointer_Icon->iTexture_Index = 305;

	m_vTime = { 0.f,fTime };

	m_bActive = true;
}

_bool CUITutorial_Timing::Update_Timing(_float fTimeDelta)
{
	if (m_bActive == false)
		return false;

	m_vTime.x += fTimeDelta;

	if (m_vTime.x >= m_vTime.y)
	{
		m_vTime = { 0.f,0.f };
		m_bActive = false;
		return false;
	}

	_float fAngle = (m_vTime.x / m_vTime.y) * 360.f;

	m_pSharedPointer_Gauge->vTexture_Angle.x = -90.f;

	if (fAngle <= 270.f)
		m_pSharedPointer_Gauge->vTexture_Angle.y = fAngle - 90.f;
	else
		m_pSharedPointer_Gauge->vTexture_Angle.y = (fAngle - 270.f) - 180.f;

	return true;
}

CUITutorial_Timing* CUITutorial_Timing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUITutorial_Timing* pInstance = new CUITutorial_Timing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUITutorial_Timing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUITutorial_Timing::Clone(void* pArg)
{
	CUITutorial_Timing* pInstance = new CUITutorial_Timing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUITutorial_Timing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUITutorial_Timing::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
