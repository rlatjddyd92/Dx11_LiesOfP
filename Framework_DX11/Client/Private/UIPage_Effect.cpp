#include "stdafx.h"
#include "..\Public\UIPage_Effect.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Effect::CUIPage_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Effect::CUIPage_Effect(const CUIPage_Effect& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Effect::Initialize_Prototype()
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

HRESULT CUIPage_Effect::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Effect::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Effect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if ((m_bFadeOut) && (m_fTime_Fade_Now < m_fTime_Fade_Max))
	{
		m_fTime_Fade_Now += fTimeDelta;
		if (m_fTime_Fade_Now >= m_fTime_Fade_Max)
			m_fTime_Fade_Now = m_fTime_Fade_Max;
	}
	else if ((!m_bFadeOut) && (m_fTime_Fade_Now > 0.f))
	{
		m_fTime_Fade_Now -= fTimeDelta;
		if (m_fTime_Fade_Now <= 0.f)
		{
			m_fTime_Fade_Now = 0.f;
			m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Screen)]->bRender = false;
			m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Title)]->bRender = false;
			m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Text)]->bRender = false;
		}
	}

	
	_float fFade_Ratio = m_fTime_Fade_Now / m_fTime_Fade_Max;

	if (isnan(fFade_Ratio))
		fFade_Ratio = 0.01f;

	m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Screen)]->fTextureColor.w = fFade_Ratio;
	m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Title)]->fTextColor.w = fFade_Ratio;
	m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Text)]->fTextColor.w = fFade_Ratio;

	if (m_fTime_Script_Now < m_fTime_Script_Max)
		m_fTime_Script_Now += fTimeDelta;

	if (m_fTime_Script_Now >= m_fTime_Script_Max)
	{
		m_fTime_Script_Now = 0.f;
		m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Back)]->bRender = false;
		m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Text0)]->bRender = false;
		m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Text1)]->bRender = false;
	}
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
	m_bUpdate = true;
}

void CUIPage_Effect::Late_Update(_float fTimeDelta)
{
	m_fTopPartMove = -1.f;

	__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Effect::Render()
{
	return S_OK;
}

void CUIPage_Effect::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Effect::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Effect::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Effect::Ready_UIPart_Group_Control()
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

	m_bRender = true;
	
	for (auto& iter : m_vecPart)
		iter->bRender = false;

	return S_OK;
}

_bool CUIPage_Effect::Fade_Out(_wstring strTitle, _wstring strDesc, _Vec3 vColor, _float fTime)
{
	m_bUpdate = true;

	m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Screen)]->fTextureColor = { vColor.x, vColor.y, vColor.z, 0.f };
	m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Title)]->strText = strTitle;
	m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Text)]->strText = strDesc;

	m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Screen)]->bRender = true;
	m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Title)]->bRender = true;
	m_vecPart[_int(PART_GROUP::EFFECT_FIFO_Text)]->bRender = true;

	m_fTime_Fade_Now = 0.f;
	m_fTime_Fade_Max = fTime;

	m_bFadeOut = true;

	return true;
}

_bool CUIPage_Effect::Fade_In(_float fTime)
{
	if (!m_bFadeOut)
		return false;

	m_bUpdate = true;

	if (fTime > 0.f)
		m_fTime_Fade_Now = m_fTime_Fade_Now * (fTime / m_fTime_Fade_Max);
	m_fTime_Fade_Max = fTime;

	m_bFadeOut = false;

	return true;
}

void CUIPage_Effect::Show_Script(_wstring strScript0, _wstring strScript1, _float fTime, _Vec3 vColor)
{
	m_bUpdate = true;

	m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Back)]->bRender = false;

	m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Back)]->fTextureColor = { vColor.x, vColor.y, vColor.z, m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Back)]->fTextureColor.w };
	m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Text0)]->strText = strScript0;

	m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Back)]->fRatio = 0.55f;

	m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Back)]->bRender = true;
	m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Text0)]->bRender = true;

	if (strScript1 != TEXT("none"))
	{
		m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Back)]->fRatio = 1.f;
		m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Text1)]->strText = strScript1;
		m_vecPart[_int(PART_GROUP::EFFECT_SCRIPT_Text1)]->bRender = true;
	}

	m_fTime_Script_Now = 0.f;
	m_fTime_Script_Max = fTime;
}

CUIPage_Effect* CUIPage_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Effect* pInstance = new CUIPage_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Effect::Clone(void* pArg)
{
	CUIPage_Effect* pInstance = new CUIPage_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Effect::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
