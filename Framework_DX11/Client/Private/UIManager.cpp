
#include "stdafx.h"
#include "..\Public\UIManager.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CUIManager::CUIManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUIManager::CUIManager(const CUIManager& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CUIManager::Initialize_Prototype()
{
	// UI 매니저는 클론 안 함, 여기서 모든 세팅 끝내기 

	UI_DESC			Desc{};

	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pUIRender_Client = CUIRender_Client::Create(m_pDevice, m_pContext);
	//Safe_AddRef(m_pUIRender_Client);

	if (FAILED(Load_UIDataFile()))
		return E_FAIL;



	return S_OK;
}

HRESULT CUIManager::Initialize(void* pArg)
{
	

	return S_OK;
}

void CUIManager::Priority_Update(_float fTimeDelta)
{
	for (auto& iter : m_vecPage)
		if (iter->GetUpdate())
			iter->Priority_Update(fTimeDelta);
}

void CUIManager::Update(_float fTimeDelta)
{
	for (auto& iter : m_vecPage)
		if (iter->GetUpdate())
			iter->Update(fTimeDelta);
}

void CUIManager::Late_Update(_float fTimeDelta)
{
	// 렌더 최종 결정 
	_int iCountOpen = 0;

	for (auto& iter : m_vecPage)
	{
		if (iter->GetUpdate())
			iter->Late_Update(fTimeDelta);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIManager::Render()
{
	if (FAILED(m_pUIRender_Client->Render_UI(m_vecPage)))
		return E_FAIL;

	return S_OK;
}

void CUIManager::Update_UIManager(_float fTimeDelta)
{
	Priority_Update(fTimeDelta);
	Update(fTimeDelta);
	Update_UIControl(fTimeDelta);
	Late_Update(fTimeDelta);
}

void CUIManager::Update_UIControl(_float fTimeDelta)
{
	// 조작 관련 모든 것 
	// 마우스, 키보드를 통한 UI 조작은 모두 여기 작성한다 

	UIControl_Test(fTimeDelta);
	//UIControl_Main(fTimeDelta);
	//UIControl_Loading(fTimeDelta);
	UIControl_Play(fTimeDelta);


}

void CUIManager::UIControl_Test(_float fTimeDelta)
{
#ifdef _DEBUG
	if (KEY_TAP(KEY::P))
	{
		if (m_vecPage[_int(UIPAGE::PAGE_PLAY)]->GetRender())
		{
			if (m_vecPage[_int(UIPAGE::PAGE_PLAY)]->GetPageAction(PAGEACTION::ACTION_OPENING))
			{
				m_vecPage[_int(UIPAGE::PAGE_PLAY)]->CloseAction();
			}
			else if (m_vecPage[_int(UIPAGE::PAGE_PLAY)]->GetPageAction(PAGEACTION::ACTION_CLOSING))
			{
				m_vecPage[_int(UIPAGE::PAGE_PLAY)]->OpenAction();
			}
		}
		else
			m_vecPage[_int(UIPAGE::PAGE_PLAY)]->OpenAction();
	}

	// 상단 바 
	if (!KEY_HOLD(KEY::LSHIFT))
	{
		if (KEY_HOLD(KEY::I))
		{
			GET_GAMEINTERFACE->GetTestData()->fMax_HP_Now -= 100.f * fTimeDelta;
			if (GET_GAMEINTERFACE->GetTestData()->fMax_HP_Now < 0.f)
				GET_GAMEINTERFACE->GetTestData()->fMax_HP_Now = 0.f;
			GET_GAMEINTERFACE->GetTestData()->fHP_Now = min(GET_GAMEINTERFACE->GetTestData()->fMax_HP_Now, GET_GAMEINTERFACE->GetTestData()->fHP_Now);
			// 체력바 최대치 감소
		}
		else if (KEY_HOLD(KEY::O))
		{
			GET_GAMEINTERFACE->GetTestData()->fMax_HP_Now += 100.f * fTimeDelta;
			if (GET_GAMEINTERFACE->GetTestData()->fMax_HP_Now > GET_GAMEINTERFACE->GetTestData()->fMax_HP_Limit)
				GET_GAMEINTERFACE->GetTestData()->fMax_HP_Now = GET_GAMEINTERFACE->GetTestData()->fMax_HP_Limit;
			// 체력바 최대치 증가
		}
	}
	else
	{
		if (KEY_HOLD(KEY::I))
		{
			GET_GAMEINTERFACE->GetTestData()->fHP_Now -= 100.f * fTimeDelta;
			if (GET_GAMEINTERFACE->GetTestData()->fHP_Now < 0.f)
				GET_GAMEINTERFACE->GetTestData()->fHP_Now = 0.f;
			// 체력바 표시 수치 감소
		}
		else if (KEY_HOLD(KEY::O))
		{
			GET_GAMEINTERFACE->GetTestData()->fHP_Now += 100.f * fTimeDelta;
			if (GET_GAMEINTERFACE->GetTestData()->fHP_Now > GET_GAMEINTERFACE->GetTestData()->fMax_HP_Now)
				GET_GAMEINTERFACE->GetTestData()->fHP_Now = GET_GAMEINTERFACE->GetTestData()->fMax_HP_Now;
			// 체력바 표시 수치 증가 
		}
	}


#endif // DEBUG

	
	


}

void CUIManager::UIControl_Main(_float fTimeDelta)
{
}

void CUIManager::UIControl_Loading(_float fTimeDelta)
{
}

void CUIManager::UIControl_Play(_float fTimeDelta)
{
}

void CUIManager::UIControl_Inven(_float fTimeDelta)
{
}

void CUIManager::OpenMainPage()
{
	// 게임 첫 시작 화면 세팅 
	m_vecPage[_int(UIPAGE::PAGE_MAIN)]->SetRender(true);

}

void CUIManager::OpenLoadingPage()
{
	for (auto& iter : m_vecPage)
		iter->SetRender(false);

	m_vecPage[_int(UIPAGE::PAGE_LOADING)]->SetRender(true);
}

void CUIManager::SwicthPage(UIPAGE eNextPage)
{
	for (auto& iter : m_vecPage)
		iter->CloseAction();

	m_vecPage[_int(eNextPage)]->OpenAction();
}

HRESULT CUIManager::Load_UIDataFile()
{
	// 여기부터 
	wstring fileName = TEXT("../Bin/DataFiles/UIData.dat");
	WCHAR* TempName = new WCHAR[fileName.size() + 1];
	for (_int i = 0; i <= fileName.size(); ++i)
		TempName[i] = fileName[i];

	HANDLE hFile = CreateFile(TempName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte(0);

	// Page의 수 
	_int iPageCount = 0;
	ReadFile(hFile, &iPageCount, sizeof(_int), &dwByte, nullptr);
	m_vecPage.resize(_int(UIPAGE::PAGE_END));

	for (_int i= 0; i < _int(UIPAGE::PAGE_END); ++i)
	{
		if(FAILED(Make_UIPage(i)))
			return E_FAIL;

		_wstring strName = {};
		while (true)
		{
			_char szText = {};
			ReadFile(hFile, &szText, sizeof(_char), &dwByte, nullptr);
			strName += (_tchar)szText;
			if (szText == '\0')
				break;
		}
		m_vecPage[i]->SetUIPageName(strName);

		_float2 fPosition = { 0.f,0.f };
		ReadFile(hFile, &fPosition, sizeof(_float2), &dwByte, nullptr);

		m_vecPage[i]->SetUIPagePosition(fPosition);

		if (FAILED(Load_UIDataFile_Part(hFile, &dwByte, i)))
			MSG_BOX(TEXT("UI 데이터 불러오기 실패"));
	}

	CloseHandle(hFile);

	Safe_Delete_Array(TempName);



	return S_OK;
}

HRESULT CUIManager::Make_UIPage(_int iIndex)
{
	if (iIndex == _int(UIPAGE::PAGE_MAIN))
	{
		m_pUIPage_Main = CUIPage_Main::Create(m_pDevice, m_pContext);
		m_pUIPage_Main->Ready_UIPart_Group_Control();
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Main);
	}
	else if (iIndex == _int(UIPAGE::PAGE_LOADING))
	{
		m_pUIPage_Loading = CUIPage_Loading::Create(m_pDevice, m_pContext);
		m_pUIPage_Loading->Ready_UIPart_Group_Control();
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Loading);
	}
	else if (iIndex == _int(UIPAGE::PAGE_PLAY))
	{
		m_pUIPage_Play = CUIPage_Play::Create(m_pDevice, m_pContext);
		m_pUIPage_Play->Ready_UIPart_Group_Control();
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Play);
	}

	if (m_vecPage[iIndex] == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CUIManager::Load_UIDataFile_Part(HANDLE handle, DWORD* dword, _int iIndex)
{
	// 여기부터 

	_int iPartCount = 0;
	ReadFile(handle, &iPartCount, sizeof(_int), dword, nullptr);

	for (_int i = 0; i < iPartCount; ++i)
	{
		CUIPage::UPART* pNew = new CUIPage::UPART;

		ReadFile(handle, &pNew->bBarDirecX, sizeof(_int), dword, nullptr);
		ReadFile(handle, &pNew->bCenter, sizeof(_bool), dword, nullptr);
		ReadFile(handle, &pNew->fAdjust, sizeof(_float2), dword, nullptr);
		ReadFile(handle, &pNew->fAdjust_End, sizeof(_float2), dword, nullptr);
		ReadFile(handle, &pNew->fAdjust_Start, sizeof(_float2), dword, nullptr);
		ReadFile(handle, &pNew->fDirec, sizeof(_float2), dword, nullptr);
		ReadFile(handle, &pNew->fPosition, sizeof(_float2), dword, nullptr);
		ReadFile(handle, &pNew->fRatio, sizeof(_float), dword, nullptr);
		ReadFile(handle, &pNew->fSize, sizeof(_float2), dword, nullptr);
		ReadFile(handle, &pNew->fTextColor, sizeof(_float4), dword, nullptr);
		ReadFile(handle, &pNew->iFontIndex, sizeof(_int), dword, nullptr);
		ReadFile(handle, &pNew->iGroupIndex, sizeof(_int), dword, nullptr);
		ReadFile(handle, &pNew->iMoveType, sizeof(_int), dword, nullptr);
		ReadFile(handle, &pNew->iParentPart_Index, sizeof(_int), dword, nullptr);
		ReadFile(handle, &pNew->iTexture_Index, sizeof(_int), dword, nullptr);
		ReadFile(handle, &pNew->fTextureColor, sizeof(_float4), dword, nullptr);
		m_pUIRender_Client->Make_Texture(pNew->iTexture_Index);
		_wstring strName = {};
		while (true)
		{
			_char szText = {};
			ReadFile(handle, &szText, sizeof(_char), dword, nullptr);
			strName += (_tchar)szText;
			if (szText == '\0')
				break;
		}
		pNew->strUIPart_Name = strName;

		_wstring strText = {};
		while (true)
		{
			_tchar szText = {};
			ReadFile(handle, &szText, sizeof(_tchar), dword, nullptr);
			strText += szText;
			if (szText == '\0')
				break;
		}
		pNew->strText = strText;

		m_vecPage[iIndex]->InputPart(pNew);
	}

	return S_OK;
}

CUIManager* CUIManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIManager* pInstance = new CUIManager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUIManager::Clone(void* pArg)
{
	CUIManager* pInstance = new CUIManager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIManager::Free()
{
	__super::Free();


	for (auto& iter : m_vecPage)
		Safe_Release(iter);

	m_vecPage.clear();

	Safe_Release(m_pUIRender_Client);
}
