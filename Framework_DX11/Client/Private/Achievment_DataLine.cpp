#include "stdafx.h"
#include "UIPage.h"
#include "..\Public\Achievment_DataLine.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CAchievment_DataLine::CAchievment_DataLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage_Achievment{ pDevice , pContext }
{
}

CAchievment_DataLine::CAchievment_DataLine(const CAchievment_DataLine& Prototype)
	: CUIPage_Achievment{ Prototype }
{
}

HRESULT CAchievment_DataLine::Initialize_Data(vector<struct CUIPage::UIPART_INFO*>& vecOrigin, struct CUIPage::SCROLL_INFO* Scroll)
{
	for (auto& iter : vecOrigin)
	{
		_int iGroup = iter->iGroupIndex;

		if (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_GOAL_LINE)
			m_vecBackCtrl.push_back(iter);
		else if (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_GOAL_ICON)
			m_vecIconCtrl.push_back(iter);
		else if (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_GOAL_TITLE)
			m_vecTitleCtrl.push_back(iter);
		else if (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_GOAL_DESC)
			m_vecDescCtrl.push_back(iter);
		else if (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_GOAL_COUNT)
			m_vecCountCtrl.push_back(iter);
		else if (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_GOAL_RESULT)
			m_vecResultCtrl.push_back(iter);
		else if (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_POPUP_FRAME)
			m_vecPopup_BackCtrl.push_back(iter);
		else if (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_POPUP_ICON)
			m_vecPopup_IconCtrl.push_back(iter);
		else if (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_POPUP_TITLE)
			m_vecPopup_TitleCtrl.push_back(iter);
		else if (iGroup == (_int)CUIPage_Achievment::PART_GROUP::GROUP_POPUP_DESC)
			m_vecPopup_DescCtrl.push_back(iter);
	}

	m_vBackColor_Origin = m_vecBackCtrl[1]->fTextureColor;
	m_vIconColor_Origin = m_vecIconCtrl.back()->fTextureColor;
	m_vTitleColor_Origin = m_vecTitleCtrl.back()->fTextColor;
	m_vDescColor_Origin = m_vecDescCtrl.back()->fTextColor;

	m_pScroll = Scroll;

	return S_OK;
}

void CAchievment_DataLine::Input_Data(vector<_wstring>& Data)
{
	m_iAchievment_Index = stoi(Data[0]);
	m_iICon_Index = stoi(Data[1]);
	m_strTitle = Data[2];
	m_strDesc = Data[3];
	m_iGoal = stoi(Data[4]);

	m_fPosY_By_Index = m_iAchievment_Index * m_vecBackCtrl[1]->fSize.y * 1.3f;
}

_bool CAchievment_DataLine::Update_Line(_float fTimeDelta, _float fAdjustY, _float fAlpha, _bool bIsRender)
{
	m_fTopPartMove = fAlpha;

	if (bIsRender == true)
		Set_Before_LineRender(fAdjustY, fAlpha);

	if (m_bIsComplete == true)
		return true;
	else
		return false;
}

void CAchievment_DataLine::Update_PopUp(_float fTimeDelta, _float fPopupHeight)
{
	if (m_vLifeTime.x > 0.f)
	{
		Set_Before_PopUpRender(fTimeDelta, fPopupHeight);
	}
}

void CAchievment_DataLine::Set_Before_LineRender(_float fAdjustY, _float fAlpha)
{
	m_vecBackCtrl.front()->fPosition.y += -fAdjustY + m_fPosY_By_Index;

	if (m_pScroll->Check_Is_Render_Y(m_vecBackCtrl.front()->fPosition.y, m_vecBackCtrl.front()->fSize.y) == false)
		return;

	m_vecIconCtrl.back()->iTexture_Index = m_iICon_Index;
	m_vecTitleCtrl.back()->strText = m_strTitle;
	m_vecDescCtrl.back()->strText = m_strDesc;
	_wstring _strCount = to_wstring(m_iNow);
	_strCount += TEXT(" / ");
	_strCount += to_wstring(m_iGoal);
	m_vecCountCtrl.back()->strText = _strCount;
	m_vecResultCtrl.back()->strText = m_bIsComplete == true ? TEXT("달성") : TEXT("미달성");

	Set_LinePart();
	
	if (m_bIsComplete == false)
	{
		m_vecBackCtrl[1]->fTextureColor = { 0.3f,0.3f,0.3f,1.f };
		m_vecIconCtrl.back()->fTextureColor = { 0.3f,0.3f,0.3f,1.f };
		m_vecTitleCtrl.back()->fTextColor = { 0.2f,0.2f,0.2f,1.f };
		m_vecDescCtrl.back()->fTextColor = { 0.1f,0.1f,0.1f,1.f };
	}

	Render_Line(fAlpha);

	if (m_bIsComplete == false)
	{
		m_vecBackCtrl[1]->fTextureColor = m_vBackColor_Origin;
		m_vecIconCtrl.back()->fTextureColor = m_vIconColor_Origin;
		m_vecTitleCtrl.back()->fTextColor = m_vTitleColor_Origin;
		m_vecDescCtrl.back()->fTextColor = m_vDescColor_Origin;
	}

	m_vecBackCtrl.front()->fPosition.y -= -fAdjustY + m_fPosY_By_Index;
}

void CAchievment_DataLine::Set_Before_PopUpRender(_float fTimeDelta, _float fPopupHeight)
{
	m_vecPopup_BackCtrl.front()->fRatio = fPopupHeight;
	Determine_PopupAlpha(fTimeDelta);
	m_vecPopup_IconCtrl.back()->iTexture_Index = m_iICon_Index;
	m_vecPopup_TitleCtrl.back()->strText = m_strTitle;
	m_vecPopup_DescCtrl.back()->strText = m_strDesc;

	m_vecPopup_BackCtrl.front()->MovePart({ 640,360 }, fTimeDelta);

	Set_PopupPart();
	Render_Popup();
}

void CAchievment_DataLine::Set_LinePart()
{
	Set_PartArray(&m_vecBackCtrl, m_vecBackCtrl[0]->fPosition, 1);

	_Vec2 vPosFrame = m_vecBackCtrl[1]->fPosition;

	Set_PartArray(&m_vecIconCtrl, vPosFrame);
	Set_PartArray(&m_vecTitleCtrl, vPosFrame);
	Set_PartArray(&m_vecDescCtrl, vPosFrame);
	Set_PartArray(&m_vecCountCtrl, vPosFrame);
	Set_PartArray(&m_vecResultCtrl, vPosFrame);
}

void CAchievment_DataLine::Set_PopupPart()
{
	_Vec2 vPosFrame = m_vecPopup_BackCtrl[0]->fPosition;

	Set_PartArray(&m_vecPopup_BackCtrl, vPosFrame, 1);
	Set_PartArray(&m_vecPopup_IconCtrl, vPosFrame);
	Set_PartArray(&m_vecPopup_TitleCtrl, vPosFrame);
	Set_PartArray(&m_vecPopup_DescCtrl, vPosFrame);
}

void CAchievment_DataLine::Render_Line(_float fAlpha)
{
	Render_PartArray(&m_vecBackCtrl, false);
	Render_PartArray(&m_vecIconCtrl, false);
	Render_PartArray(&m_vecTitleCtrl, false);
	Render_PartArray(&m_vecDescCtrl, false);
	Render_PartArray(&m_vecCountCtrl, false);
	Render_PartArray(&m_vecResultCtrl, false);
}

_bool CAchievment_DataLine::Check_AllowPopup()
{






	return true;
}

void CAchievment_DataLine::Determine_PopupAlpha(_float fTimeDelta)
{
	_float fResult = 1.f;

	m_vLifeTime.x += fTimeDelta;

	if (m_vLifeTime.x >= m_vLifeTime.y * 2.f + m_vLifeTime.z)
	{
		fResult = 0.f;
		m_vLifeTime = { 0.f,0.f,0.f };
	}
	else if (m_vLifeTime.x >= m_vLifeTime.y * 1.f + m_vLifeTime.z)
	{
		fResult = m_vLifeTime.x - (m_vLifeTime.y * 1.f + m_vLifeTime.z);
		fResult = (m_vLifeTime.y - fResult) / m_vLifeTime.y;
	}
	else if (m_vLifeTime.x >= m_vLifeTime.y)
	{
		fResult = 1.f;
	}
	else if (m_vLifeTime.x < m_vLifeTime.y)
	{
		fResult = m_vLifeTime.x / m_vLifeTime.y;
	}

	m_fPopup_AlphaNow = fResult;
}

void CAchievment_DataLine::Render_Popup()
{
	if (Check_AllowPopup() == false)
		return;

	Render_PartArray(&m_vecPopup_BackCtrl, true);
	Render_PartArray(&m_vecPopup_IconCtrl, true);
	Render_PartArray(&m_vecPopup_TitleCtrl, true);
	Render_PartArray(&m_vecPopup_DescCtrl, true);
}

void CAchievment_DataLine::Set_PartArray(vector<struct CUIPage::UIPART_INFO*>* pArray, _Vec2& vPosFrame, _int iStart)
{
	vector<struct CUIPage::UIPART_INFO*>::iterator iter = pArray->begin();

	for (_int i = 0; i < iStart; ++i)
		++iter;

	(*iter)->MovePart(vPosFrame, 1.f);
	for (_int i = 1 + iStart; i < pArray->size(); ++i)
	{
		_Vec2 vPos = (*iter)->fPosition;
		++iter;
		(*iter)->MovePart(vPos, 1.f);
	}	
}

void CAchievment_DataLine::Render_PartArray(vector<struct CUIPage::UIPART_INFO*>* pArray, _bool bIsPopup)
{
	vector<struct CUIPage::UIPART_INFO*>::iterator iter = pArray->begin();

	for (_int i = 0; i < pArray->size(); ++i)
	{
		if (bIsPopup == true)
		{
			_float fOrigin = m_fTopPartMove;
			m_fTopPartMove = m_fPopup_AlphaNow;
			Input_Render_Info(*(*iter));
			m_fTopPartMove = fOrigin;
			++iter;
		}
		else
		{
			Input_Render_Info(*(*iter), SCROLL_AREA::SCROLL_ACHIEVMENT);
			++iter;
		}
	}
}

HRESULT CAchievment_DataLine::Initialize_Prototype()
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

HRESULT CAchievment_DataLine::Initialize(void* pArg)
{
	return S_OK;
}

void CAchievment_DataLine::Priority_Update(_float fTimeDelta)
{
}

void CAchievment_DataLine::Update(_float fTimeDelta)
{
}

void CAchievment_DataLine::Late_Update(_float fTimeDelta)
{
}

HRESULT CAchievment_DataLine::Render()
{
	return S_OK;
}

void CAchievment_DataLine::OpenAction()
{
}

void CAchievment_DataLine::CloseAction()
{
}

CHECK_MOUSE CAchievment_DataLine::Check_Page_Action(_float fTimeDelta)
{
	return CHECK_MOUSE();
}

HRESULT CAchievment_DataLine::Ready_UIPart_Group_Control()
{
	return S_OK;
}

CAchievment_DataLine* CAchievment_DataLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAchievment_DataLine* pInstance = new CAchievment_DataLine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAchievment_DataLine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAchievment_DataLine::Clone(void* pArg)
{
	CAchievment_DataLine* pInstance = new CAchievment_DataLine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CAchievment_DataLine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAchievment_DataLine::Free()
{
	__super::Free();

	m_vecBackCtrl.clear();
	m_vecIconCtrl.clear();
	m_vecTitleCtrl.clear();
	m_vecDescCtrl.clear();
	m_vecCountCtrl.clear();
	m_vecResultCtrl.clear();
	m_vecPopup_BackCtrl.clear();
	m_vecPopup_IconCtrl.clear();
	m_vecPopup_TitleCtrl.clear();
	m_vecPopup_DescCtrl.clear();
}
