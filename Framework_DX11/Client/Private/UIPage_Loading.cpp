#include "stdafx.h"
#include "..\Public\UIPage_Loading.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIPage_Loading::CUIPage_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIPage{ pDevice, pContext }
{

}

CUIPage_Loading::CUIPage_Loading(const CUIPage_Loading& Prototype)
	: CUIPage{ Prototype }
{
}

HRESULT CUIPage_Loading::Initialize_Prototype()
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

	m_bRender = false;
	m_bUpdate = false;
	m_fTopPartMove = 1.f;

	return S_OK;
}

HRESULT CUIPage_Loading::Initialize(void* pArg)
{


	return S_OK;
}

void CUIPage_Loading::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUIPage_Loading::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUIPage_Loading::Late_Update(_float fTimeDelta)
{
	if (!m_bLoading_Scene)
		Credit_Update(fTimeDelta);
	else
		Loading_Update(fTimeDelta);
}

HRESULT CUIPage_Loading::Render()
{
	return S_OK;
}

void CUIPage_Loading::OpenAction()
{
	__super::OpenAction();
}

void CUIPage_Loading::CloseAction()
{
	__super::CloseAction();
}

CHECK_MOUSE CUIPage_Loading::Check_Page_Action(_float fTimeDelta)
{
	__super::Check_Page_Action(fTimeDelta);

	return CHECK_MOUSE::MOUSE_NONE;
}

HRESULT CUIPage_Loading::Ready_UIPart_Group_Control()
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

	m_vecCreadit.resize(5);

	vector<vector<_wstring>> vecBuffer;
	m_pGameInstance->LoadDataByFile("../Bin/DataFiles/Credit_Info.csv", &vecBuffer);

	for (_int i = 0; i < 5; ++i)
	{
		CREADIT* pNew = new CREADIT;

		pNew->strTitle = vecBuffer[i + 1][0];
		pNew->strName = vecBuffer[i + 1][1];
		pNew->strRole_First = vecBuffer[i + 1][2];
		pNew->strRole_Second = vecBuffer[i + 1][3];

		m_vecCreadit[i] = pNew;
	}

	__super::Array_Control(_int(PART_GROUP::GROUP_LOADING_STATIC), _int(PART_GROUP::GROUP_LOADING_NOSE_PROGRESS), CTRL_COMMAND::COM_RENDER, false);

	m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CREADIT_LEADER))]->strText = m_vecCreadit[m_iCreadit_Count]->strTitle;
	m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CREADIT_NAME))]->strText = m_vecCreadit[m_iCreadit_Count]->strName;
	m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CREADIT_ROLE_0))]->strText = m_vecCreadit[m_iCreadit_Count]->strRole_First;
	m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CREADIT_ROLE_1))]->strText = m_vecCreadit[m_iCreadit_Count]->strRole_Second;

	m_bRender = false;

	return S_OK;
}

void CUIPage_Loading::Credit_Update(_float fTimeDelta)
{
	if (m_fShowTime_Now == 0.f)
	{
		m_fAppear += fTimeDelta;

		if (m_fAppear >= 1.f)
		{
			m_fAppear = 1.f;
			m_fShowTime_Now += fTimeDelta;
		}
	}
	else if (m_fAppear == 1.f)
	{
		m_fShowTime_Now += fTimeDelta;
		if (m_fShowTime_Now >= m_fShowTime_Max)
		{
			m_fAppear -= fTimeDelta;
		}
	}
	else if (m_fAppear >= 0.f)
	{
		m_fAppear -= fTimeDelta;
		if (m_fAppear <= 0.f)
			m_fInverval += fTimeDelta;
	}
	else if (m_fInverval > 0.f)
	{
		m_fInverval += fTimeDelta;
		if (m_fInverval >= 1.f)
		{
			m_fAppear = 0.f;
			m_fInverval = 0.f;
			m_fShowTime_Now = 0.f;
			
			++m_iCreadit_Count;

			if (m_iCreadit_Count == 5)
			{
				__super::Array_Control(_int(PART_GROUP::GROUP_CREADIT_STATIC), _int(PART_GROUP::GROUP_CREADIT_ROLE_1), CTRL_COMMAND::COM_RENDER, false);
				__super::Array_Control(_int(PART_GROUP::GROUP_LOADING_STATIC), _int(PART_GROUP::GROUP_LOADING_NOSE_PROGRESS), CTRL_COMMAND::COM_RENDER, true);
				m_bLoading_Scene = true;
				return;
			}
			else
			{
				m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CREADIT_LEADER))]->strText = m_vecCreadit[m_iCreadit_Count]->strTitle;
				m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CREADIT_NAME))]->strText = m_vecCreadit[m_iCreadit_Count]->strName;
				m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CREADIT_ROLE_0))]->strText = m_vecCreadit[m_iCreadit_Count]->strRole_First;
				m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_CREADIT_ROLE_1))]->strText = m_vecCreadit[m_iCreadit_Count]->strRole_Second;
			}
		}
	}

	for (_int i = _int(PART_GROUP::GROUP_CREADIT_STATIC); i <= _int(PART_GROUP::GROUP_CREADIT_ROLE_1); ++i)
	{
		if (m_fInverval == 0.f)
		{
			for (auto& iter : m_vec_Group_Ctrl[i]->PartIndexlist)
			{
				m_vecPart[iter]->fTextColor.w = m_fAppear;
				m_vecPart[iter]->fTextureColor.w = m_fAppear;

				if ((m_vecPart[iter]->iFontIndex == 14) || (m_vecPart[iter]->strText[0] != 'x'))
					__super::Input_Render_Info(*m_vecPart[iter]);
			}
		}
	}
}

void CUIPage_Loading::Loading_Update(_float fTimeDelta)
{
	m_vec_Group_Ctrl[_int(PART_GROUP::GROUP_LOADING_NOSE)]->fRatio = m_fLoading_Progress;
	m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_LOADING_NOSE_TEXT))]->strText = m_strLoading_Message;
	m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_LOADING_NOSE_PROGRESS))]->strText = to_wstring(_int(m_fLoading_Progress*100.f)) + TEXT("%");

	if (m_fLoading_Progress == 1.f)
	{
		m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_LOADING_NOSE_TEXT))]->Set_RedText();
		m_vecPart[__super::Get_Front_PartIndex_In_Control(_int(PART_GROUP::GROUP_LOADING_NOSE_PROGRESS))]->Set_RedText();
	}

	for (auto& iter : m_vec_Group_Ctrl)
		__super::UpdatePart_ByControl(iter);

	__super::Late_Update(fTimeDelta);
}

CUIPage_Loading* CUIPage_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIPage_Loading* pInstance = new CUIPage_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIPage_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIPage_Loading::Clone(void* pArg)
{
	CUIPage_Loading* pInstance = new CUIPage_Loading(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIPage_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIPage_Loading::Free()
{
	__super::Free();

	for (auto& iter : m_vecPart)
	{
		Safe_Delete(iter);
	}

	for (auto& iter : m_vecCreadit)
		Safe_Delete(iter);

	m_vecCreadit.clear();

	m_vecPart.clear();
}
