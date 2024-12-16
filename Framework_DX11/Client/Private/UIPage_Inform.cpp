#include "stdafx.h"
#include "..\Public\UIPage_Inform.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Inform::CUIPage_Inform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Inform::CUIPage_Inform(const CUIPage_Inform& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Inform::Initialize_Prototype()
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

HRESULT CUIPage_Inform::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Inform::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Inform::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Inform::Late_Update(_float fTimeDelta)
{
	if (fTimeDelta > 0.2f)
		fTimeDelta = 0.01f;

	Update_Region(fTimeDelta);
	Update_Inform(fTimeDelta);
	Update_Heart(fTimeDelta);

	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Inform::Render()
{
	return S_OK;
}

void CUIPage_Inform::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Inform::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Inform::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Inform::Ready_UIPart_Group_Control()
{
	__super::Ready_UIPart_Group_Control();

	for (auto& iter : m_vecPart)
		iter->bRender = false;
	m_bUpdate = true;
	m_bRender = true;
	m_fTopPartMove = -1.f;
	return S_OK;
}

void CUIPage_Inform::Show_Region_Info(_wstring strName, _wstring strDesc, _float fTime_Emerge, _float fTime_Show)
{
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;
	m_bUpdate = true;
	m_fTopPartMove = -1.f;
	m_vLifeTime_Region = { 0.f, fTime_Emerge , fTime_Show };

	for (_int i = _int(PART_GROUP::INFORM_Region_Fx); i <= _int(PART_GROUP::INFORM_Region_Desc); ++i)
	{
		m_vecPart[i]->bRender = true;
		m_vecPart[i]->fTextureColor.w = 0.f;
		m_vecPart[i]->fTextColor.w = 0.f;
	}
	
	m_vecPart[_int(PART_GROUP::INFORM_Region_Title)]->strText = strName;
	m_vecPart[_int(PART_GROUP::INFORM_Region_Desc)]->strText = strDesc;
}

void CUIPage_Inform::Show_Inform(INFORM_MESSAGE eInform, _float fTime_Emerge, _float fTime_Show)
{
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;
	m_bUpdate = true;
	m_fTopPartMove = -1.f;
	m_vLifeTime_Inform = { 0.f, fTime_Emerge , fTime_Show };

	_int iIndex = _int(PART_GROUP::INFORM_Dead) + _int(eInform);

	for (_int i = _int(PART_GROUP::INFORM_Common_Fx); i <= _int(PART_GROUP::INFORM_Stargazer); ++i)
	{
		if ((i == _int(PART_GROUP::INFORM_Common_Fx)) || (i == iIndex))
		{
			m_vecPart[i]->bRender = true;
			m_vecPart[i]->fTextureColor.w = 0.f;
		}
	}

	m_vecPart[_int(PART_GROUP::INFORM_Common_Fx)]->bRender = true;
	m_vecPart[iIndex]->bRender = true;
}

void CUIPage_Inform::Show_Heart(_wstring strScript, _float fTime_Emerge, _float fTime_Show)
{
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;
	m_bUpdate = true;
	m_fTopPartMove = -1.f;
	m_vLifeTime_Heart = { 0.f, fTime_Emerge , fTime_Show };

	for (_int i = _int(PART_GROUP::INFORM_Heart); i <= _int(PART_GROUP::INFORM_Text_Front); ++i)
	{
		m_vecPart[i]->bRender = true;
		m_vecPart[i]->fTextureColor.w = 0.f;
		m_vecPart[i]->fTextColor.w = 0.f;
	}
	
	m_vecPart[_int(PART_GROUP::INFORM_Text_Back)]->strText = strScript;
	m_vecPart[_int(PART_GROUP::INFORM_Text_Front)]->strText = strScript;
}

void CUIPage_Inform::Update_Region(_float fTimeDelta)
{
	_float fRatio = Check_Ratio(&m_vLifeTime_Region, fTimeDelta);

	if (fRatio == -1.f)
	{
		for (_int i = _int(PART_GROUP::INFORM_Region_Fx); i <= _int(PART_GROUP::INFORM_Region_Desc); ++i)
			m_vecPart[i]->bRender = false;

	}	
	else
	{
		for (_int i = _int(PART_GROUP::INFORM_Region_Fx); i <= _int(PART_GROUP::INFORM_Region_Desc); ++i)
		{
			m_vecPart[i]->fTextureColor.w = fRatio;
			m_vecPart[i]->fTextColor.w = fRatio;
		}
	}
}

void CUIPage_Inform::Update_Inform(_float fTimeDelta)
{
	_float fRatio = Check_Ratio(&m_vLifeTime_Inform, fTimeDelta);

	if (fRatio == -1.f)
	{
		for (_int i = _int(PART_GROUP::INFORM_Common_Fx); i <= _int(PART_GROUP::INFORM_Stargazer); ++i)
			m_vecPart[i]->bRender = false;

	}
	else
	{
		for (_int i = _int(PART_GROUP::INFORM_Common_Fx); i <= _int(PART_GROUP::INFORM_Stargazer); ++i)
		{
			m_vecPart[i]->fTextureColor.w = fRatio;
			m_vecPart[i]->fTextColor.w = fRatio;
		}
	}
}

void CUIPage_Inform::Update_Heart(_float fTimeDelta)
{
	_float fRatio = Check_Ratio(&m_vLifeTime_Heart, fTimeDelta);

	if (fRatio == -1.f)
	{
		for (_int i = _int(PART_GROUP::INFORM_Heart); i <= _int(PART_GROUP::INFORM_Text_Front); ++i)
			m_vecPart[i]->bRender = false;

	}
	else
	{
		for (_int i = _int(PART_GROUP::INFORM_Heart); i <= _int(PART_GROUP::INFORM_Text_Front); ++i)
		{
			m_vecPart[i]->fTextureColor.w = fRatio;
			m_vecPart[i]->fTextColor.w = fRatio;
			m_vecPart[i]->fRatio = fRatio;
		}
	}
}

_float CUIPage_Inform::Check_Ratio(_Vec3* vLifeTime, _float fTimeDelta)
{
	_float fResult = 1.f;

	vLifeTime->x += fTimeDelta;

	if (vLifeTime->x >= vLifeTime->y * 2.f + vLifeTime->z)
	{
		fResult = -1.f;
		*vLifeTime = { 0.f,0.f,0.f };
	}
	else if (vLifeTime->x >= vLifeTime->y * 1.f + vLifeTime->z)
	{
		fResult = vLifeTime->x - (vLifeTime->y * 1.f + vLifeTime->z);
		fResult = (vLifeTime->y - fResult) / vLifeTime->y;
	}
	else if (vLifeTime->x >= vLifeTime->y)
	{
		fResult = 1.f;
	}
	else if (vLifeTime->x < vLifeTime->y)
	{
		fResult = vLifeTime->x / vLifeTime->y;
	}


	return fResult;
}

CUIPage_Inform* CUIPage_Inform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Inform* pInstance = new CUIPage_Inform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Inform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Inform::Clone(void* pArg)
{
	CUIPage_Inform* pInstance = new CUIPage_Inform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Inform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Inform::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
