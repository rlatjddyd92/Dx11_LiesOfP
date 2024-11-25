#include "stdafx.h"
#include "..\Public\UIPage.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage::CUIPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIPage::CUIPage(const CUIPage& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIPage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIPage::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	return S_OK;
}

void CUIPage::Priority_Update(_float fTimeDelta)
{
	// 매 프레임 초기화해야 하는 정보를 처리 
	__super::Priority_Update(fTimeDelta);
}

void CUIPage::Update(_float fTimeDelta)
{
	// 여기까지 각 페이지의 개별 적용 사항을 끝내야 함 
	__super::Update(fTimeDelta);
}

void CUIPage::Late_Update(_float fTimeDelta)
{
	// 각 페이지 별 조정 사항을 최종 반영하는 단계
	__super::Late_Update(fTimeDelta);



	if ((m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)]) && (!m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)]))
	{
		m_fTopPartMove -= m_fTopPartMove_Excel * fTimeDelta;
		if (m_fTopPartMove < 0.f)
		{
			m_fTopPartMove = 0.f;
			m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)] = false;
			m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;
		}
	}
	else if ((!m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)]) && (m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)]))
	{
		m_fTopPartMove += m_fTopPartMove_Excel * fTimeDelta;
		if (m_fTopPartMove > 1.f)
		{
			m_fTopPartMove = 1.f;
			m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)] = false;
			m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;
		}
	}

	for (auto& iter : m_vecPart)
	{
		iter->MakeDirec();

		if (iter->iParentPart_Index == -1)
		{
			if (m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)] + m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)] == 1)
				iter->fRatio = m_fTopPartMove;

			iter->MovePart({ m_fX,m_fY }, fTimeDelta);
		}
		else
		{
			iter->MovePart(m_vecPart[iter->iParentPart_Index]->fPosition, fTimeDelta);
		}

		if (m_bRender)
			if (iter->bRender)
				Input_Render_Info(*iter);
	}

	if ((m_fTopPartMove == 0.f) || (m_fTopPartMove == -1.f))
	{
		if (m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)])
			m_bRender = false;

		m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)] = false;
		m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)] = false;
	}

}

HRESULT CUIPage::Render()
{
	return S_OK;
}

void CUIPage::OpenAction()
{
	m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)] = false;
	m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = false;
	m_bRender = true;
}

void CUIPage::CloseAction()
{
	m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)] = true;
	m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)] = false;
	m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = false;
	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = false;
	m_bRender = false;
}

HRESULT CUIPage::Ready_UIPart_Group_Control()
{
	return S_OK;
}

CHECK_MOUSE CUIPage::Check_Page_Action(_float fTimeDelta)
{
	return CHECK_MOUSE::MOUSE_NONE;
}

void CUIPage::Release_Control(UG_CTRL* pCtrl)
{
	pCtrl->PartIndexlist.clear();
	Safe_Delete(pCtrl);
}

_Vec2 CUIPage::Check_Mouse_By_Part(UPART& Part)
{
	_Vec2 fPos = Part.fPosition;
	_Vec2 fSize = Part.fSize;
	return GET_GAMEINTERFACE->CheckMouse(fPos, fSize);
}

void CUIPage::UpdatePart_ByIndex(_int Index, _float fTimeDelta)
{
	m_vecPart[Index]->MakeDirec();

	if (m_vecPart[Index]->iParentPart_Index == -1)
	{
		if (m_vecPageAction[_int(PAGEACTION::ACTION_CLOSING)] + m_vecPageAction[_int(PAGEACTION::ACTION_OPENING)] == 1)
			m_vecPart[Index]->fRatio = m_fTopPartMove;

		m_vecPart[Index]->MovePart({ m_fX,m_fY }, fTimeDelta);
	}
	else
		m_vecPart[Index]->MovePart(m_vecPart[m_vecPart[Index]->iParentPart_Index]->fPosition, fTimeDelta);
}

void CUIPage::Input_Render_Info(UPART& Part)
{
	if (Part.iTexture_Index < 0)
		if ((Part.iFontIndex < 0) || (Part.iFontIndex >= _int(UI_FONT::FONT_END)))
			return;


	CUIRender_Batching::UIRENDER_INFO* pNew = new CUIRender_Batching::UIRENDER_INFO;

	pNew->bIsItem = Part.bIsItem;
	pNew->iTexture = Part.iTexture_Index;
	pNew->bIsMultiple = Part.bTexture_Color_Multiple;
	if (Part.iTexture_Index > 0)
		pNew->eText_Type = TEXT_TYPE::TEXT_END;
	else if (Part.bText_Right)
		pNew->eText_Type = TEXT_TYPE::TEXT_RIGHT;
	else if (Part.bCenter)
		pNew->eText_Type = TEXT_TYPE::TEXT_CENTER;
	else
		pNew->eText_Type = TEXT_TYPE::TEXT_LEFT;
	pNew->fTurn = Part.fTurn_Degree;
	pNew->iFont = Part.iFontIndex;
	pNew->strText = Part.strText;
	pNew->vColor_Text = Part.fTextColor;

	pNew->vColor_Text.w *= m_fTopPartMove;

	pNew->vColor_Texture = Part.fTextureColor;
	pNew->vColor_Texture.w *= m_fTopPartMove;

	pNew->vPosition = Part.fPosition;
	pNew->vSize = Part.fSize;
	pNew->fAlpha_Strash = Part.fStrash_Alpha;

	if (Part.iMoveType == _int(MOVETYPE::TYPE_BAR))
		pNew->vSize = Part.GetBarSize();

	GET_GAMEINTERFACE->Input_Render_Info(pNew);
}

void CUIPage::UpdatePart_ByControl(UG_CTRL* pCtrl)
{
	for (auto& iter : pCtrl->PartIndexlist)
	{
		m_vecPart[iter]->fRatio = pCtrl->fRatio;
		m_vecPart[iter]->bUpdate = pCtrl->bUpdate;
		m_vecPart[iter]->bRender = pCtrl->bRender;
	}
}

CUIPage* CUIPage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage* pInstance = new CUIPage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage::Clone(void* pArg)
{
	CUIPage* pInstance = new CUIPage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage::Free()
{
	__super::Free();

	m_vecPageAction.clear();

	for (auto& iter : m_vec_Group_Ctrl)
		Safe_Delete(iter);

	m_vec_Group_Ctrl.clear();
}
