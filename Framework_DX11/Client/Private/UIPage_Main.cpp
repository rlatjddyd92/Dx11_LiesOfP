#include "stdafx.h"
#include "..\Public\UIPage_Main.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"
#include "Level_Loading.h"

CUIPage_Main::CUIPage_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{
	
}

CUIPage_Main::CUIPage_Main(const CUIPage_Main& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Main::Initialize_Prototype()
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

	m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = true;

	return S_OK;
}

HRESULT CUIPage_Main::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Main::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Main::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Main::Late_Update(_float fTimeDelta)
{
	if (m_fOpening_Progress < 1.f)
		m_fOpening_Progress += fTimeDelta * 0.5f;
	
	if (m_fOpening_Progress > 1.f)
		m_fOpening_Progress = 1.f;

	Main_Update(fTimeDelta);


	//__super::Late_Update(fTimeDelta);
}

HRESULT CUIPage_Main::Render()
{
	return S_OK;
}

void CUIPage_Main::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Main::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Main::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);
	Check_Mouse(fTimeDelta);
	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Main::Ready_UIPart_Group_Control()
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
	m_fTopPartMove = 1.f;

	return S_OK;
}


void CUIPage_Main::Check_Mouse(_float fTimeDelta)
{
	const UPART* pMouse = m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::MAIN_MOUSE))];

	if (GET_GAMEINTERFACE->CheckMouse(pMouse->fPosition, pMouse->fSize).x != -1.f)
	{
		m_fMouse_Effect_Ratio += fTimeDelta;
		m_fMouse_Effect_Ratio = min(m_fMouse_Effect_Ratio, 1.f);

		if (KEY_TAP(KEY::LBUTTON))
		{
			if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
				return;
			else
			{
				m_bRender = false;
				m_bUpdate = false;
				m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = false;
				m_vecPageAction[_int(PAGEACTION::ACTION_INACTIVE)] = true;
			}
		}
	}
	else
	{
		m_fMouse_Effect_Ratio -= fTimeDelta;
		m_fMouse_Effect_Ratio = max(m_fMouse_Effect_Ratio, 0.f);
	}
}

void CUIPage_Main::Main_Update(_float fTimeDelta)
{
	// 배경 0~1 번 파트는 처음부터 알파 최대값
	// 나머지 파트는 알파 0부터 서서히 올라가기  

	if (m_fOpening_Progress < 1.f)
	{
		m_fOpening_Progress += fTimeDelta * 0.3f;

		for (_int i = 0; i < _int(PART_GROUP::GROUP_END); ++i)
		{
			if (i <= _int(PART_GROUP::MAIN_STATIC))
			{
				for (auto& iter : m_vec_Group_Ctrl[i]->PartIndexlist)
					Input_Render_Info(*m_vecPart[iter]);
			}
			else
			{
				if ((i == _int(PART_GROUP::MAIN_FX)) || (i == _int(PART_GROUP::MAIN_TEXT_FX)))
				{
					for (auto& iter : m_vec_Group_Ctrl[i]->PartIndexlist)
					{
						_float fOrigin_Red = m_vecPart[iter]->fTextureColor.x;
						_float fDiff = m_vecPart[iter]->fTextureColor.x - m_vecPart[iter]->fTextureColor.y;
						m_vecPart[iter]->fTextureColor.x = m_vecPart[iter]->fTextureColor.y + fDiff * m_fMouse_Effect_Ratio;
						Input_Render_Info(*m_vecPart[iter]);
						m_vecPart[iter]->fTextureColor.x = fOrigin_Red;
					}
				}
				else
				{
					for (auto& iter : m_vec_Group_Ctrl[i]->PartIndexlist)
					{
						_float fOrigin_Alpha = m_vecPart[iter]->fTextureColor.w;
						m_vecPart[iter]->fTextureColor.w *= m_fOpening_Progress;
						Input_Render_Info(*m_vecPart[iter]);
						m_vecPart[iter]->fTextureColor.w = fOrigin_Alpha;
					}
				}
			}	
		}
	}
	else
	{
		for (_int i = 0; i < _int(PART_GROUP::GROUP_END); ++i)
		{
			if ((i == _int(PART_GROUP::MAIN_FX)) || (i == _int(PART_GROUP::MAIN_TEXT_FX)))
			{
				for (auto& iter : m_vec_Group_Ctrl[i]->PartIndexlist)
				{
					_float fOrigin_Red = m_vecPart[iter]->fTextureColor.x;
					_float fDiff = m_vecPart[iter]->fTextureColor.x - m_vecPart[iter]->fTextureColor.y;
					m_vecPart[iter]->fTextureColor.x = m_vecPart[iter]->fTextureColor.y + fDiff * m_fMouse_Effect_Ratio;
					Input_Render_Info(*m_vecPart[iter]);
					m_vecPart[iter]->fTextureColor.x = fOrigin_Red;
				}
			}
			else
			{
				for (auto& iter : m_vec_Group_Ctrl[i]->PartIndexlist)
					Input_Render_Info(*m_vecPart[iter]);
			}
		}
	}
}

CUIPage_Main* CUIPage_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Main* pInstance = new CUIPage_Main(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Main"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Main::Clone(void* pArg)
{
	CUIPage_Main* pInstance = new CUIPage_Main(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Main"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Main::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	m_vecPart.clear();
}
