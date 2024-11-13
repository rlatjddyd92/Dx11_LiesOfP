
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

#ifdef _DEBUG
	m_pTestData = new TESTDATA;
#endif

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

	if (m_iFonttest != 0)
		if (FAILED(m_pUIRender_Client->Render_TestFont(m_iFonttest == 1)))
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
	if (KEY_HOLD(KEY::LSHIFT))
	{
		if (KEY_HOLD(KEY::I))
		{
			GET_GAMEINTERFACE->Add_Stat_Normal(STAT_NORMAL::STAT_GAUGE_HP, -100.f * fTimeDelta);
			// 체력바 표시 수치 감소
		}
		else if (KEY_HOLD(KEY::O))
		{
			GET_GAMEINTERFACE->Add_Stat_Normal(STAT_NORMAL::STAT_GAUGE_HP, +100.f * fTimeDelta);
			// 체력바 표시 수치 증가 
		}

	}
	else
	{
		if (KEY_HOLD(KEY::I))
		{
			m_pTestData->fMax_HP_Now -= 100.f * fTimeDelta;
			if (m_pTestData->fMax_HP_Now < 0.f)
				m_pTestData->fMax_HP_Now = 0.f;
			m_pTestData->fHP_Now = min(m_pTestData->fMax_HP_Now, m_pTestData->fHP_Now);
			// 체력바 최대치 감소
		}
		else if (KEY_HOLD(KEY::O))
		{
			m_pTestData->fMax_HP_Now += 100.f * fTimeDelta;
			if (m_pTestData->fMax_HP_Now > m_pTestData->fMax_HP_Limit)
				m_pTestData->fMax_HP_Now = m_pTestData->fMax_HP_Limit;
			// 체력바 최대치 증가
		}
	}


	// 하단 아이템 

	if (KEY_TAP(KEY::T))
	{
		m_pUIPage_Play->Move_SelectCtrl(true);
	}

	if (KEY_TAP(KEY::G))
	{
		m_pUIPage_Play->Move_SelectCtrl(false);
	}


	// 폰트 테스트

	if (KEY_TAP(KEY::F2))
	{
		++m_iFonttest;
		if (m_iFonttest > 1)
			m_iFonttest = -1;
	}

	// TestPage 조작 
	if (KEY_TAP(KEY::F3))
	{
		if (m_bTestPageOpen)
			m_bTestPageOpen = false;
		else
			m_bTestPageOpen = true;
	}



	if (m_bTestPageOpen)
	{
		


		/*_float3 fHP = { GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_HP).fStat_Now , GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_HP).fStat_Max, GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_HP).fStat_Max_Limit };
		_float3 fST = { GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_STAMINA).fStat_Now , GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_STAMINA).fStat_Max, GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_STAMINA).fStat_Max_Limit };
		_float3 fRE = { GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_REGION).fStat_Now , GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_REGION).fStat_Max, GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_REGION).fStat_Max_Limit };
		_float3 fWE = { GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_WEIGHT).fStat_Now , GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_WEIGHT).fStat_Max, GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_WEIGHT).fStat_Max_Limit };
		_float3 fGA = { GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GADRIGAIN).fStat_Now , GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GADRIGAIN).fStat_Max, GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GADRIGAIN).fStat_Max_Limit };
		_int iHP = GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_BODY);
		_int iST = GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_STAMINA);
		_int iWE = GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_WEIGHT);
		_int iPO = GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_POWER);
		_int iSK = GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_SKILL);
		_int iEV = GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_EVOLUTION);

		ShowTestPage(
			TEXT("HP(현재/제한/최대치)"), TEST_PAGE_VALUE_TYPE::TYPE_FLOAT3, &fHP,
			TEXT("스태미나(현재/제한/최대치)"), TEST_PAGE_VALUE_TYPE::TYPE_FLOAT3, &fST,
			TEXT("리전(현재/제한/최대치)"), TEST_PAGE_VALUE_TYPE::TYPE_FLOAT3, &fRE,
			TEXT("무게(현재/제한/최대치)"), TEST_PAGE_VALUE_TYPE::TYPE_FLOAT3, &fWE,
			TEXT("가드리게인(현재/제한/최대치)"), TEST_PAGE_VALUE_TYPE::TYPE_FLOAT3, &fGA,
			TEXT("신체 스탯"), TEST_PAGE_VALUE_TYPE::TYPE_INT, &iHP,
			TEXT("지구력 스탯"), TEST_PAGE_VALUE_TYPE::TYPE_INT, &iST,
			TEXT("무게 스탯"), TEST_PAGE_VALUE_TYPE::TYPE_INT, &iWE,
			TEXT("힘 스탯"), TEST_PAGE_VALUE_TYPE::TYPE_INT, &iPO,
			TEXT("기술 스탯"), TEST_PAGE_VALUE_TYPE::TYPE_INT, &iSK
		);*/

		if ((KEY_TAP(KEY::LBUTTON)) && (!m_bTestPageMove))
		{
			POINT			ptMouse{};
			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);

			if (ptMouse.x > m_fTestPage_Pos.x - (m_fTestPage_Size.x* 0.5f))
				if (ptMouse.x < m_fTestPage_Pos.x + (m_fTestPage_Size.x * 0.5f))
					if (ptMouse.y > m_fTestPage_Pos.y - (m_fTestPage_Size.y * 0.5f))
						if (ptMouse.y < m_fTestPage_Pos.y + (m_fTestPage_Size.y * 0.5f))
						{
							m_bTestPageMove = true;
							m_fTestPage_ClickPos = { (_float)ptMouse.x, (_float)ptMouse.y };
						}
		}
		else if (m_bTestPageMove)
		{
			if(KEY_HOLD(KEY::LBUTTON))
			{
				POINT			ptMouse{};
				GetCursorPos(&ptMouse);
				ScreenToClient(g_hWnd, &ptMouse);

				_float2 fMove = { 0.f,0.f };

				fMove.x = ptMouse.x - m_fTestPage_ClickPos.x;
				fMove.y = ptMouse.y - m_fTestPage_ClickPos.y;

				m_fTestPage_Pos.x += fMove.x;
				m_fTestPage_Pos.y += fMove.y;

				m_fTestPage_ClickPos = { (_float)ptMouse.x, (_float)ptMouse.y };
			}
			else
				m_bTestPageMove = false;
		}
	}
	else 
		m_bTestPageMove = false;



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
	m_pTestData->fHP_Now = GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_GAUGE_HP);
	m_pUIPage_Play->SetRatio_HPBarMax(m_pTestData->fMax_HP_Now / m_pTestData->fMax_HP_Limit);
	m_pUIPage_Play->SetRatio_HPBarFill(m_pTestData->fHP_Now / m_pTestData->fMax_HP_Limit);
}

void CUIManager::UIControl_Inven(_float fTimeDelta)
{
}

void CUIManager::OpenMainPage()
{
	// 게임 첫 시작 화면 세팅 
	//m_vecPage[_int(UIPAGE::PAGE_MAIN)]->SetRender(true);

}

void CUIManager::OpenLoadingPage()
{
	//for (auto& iter : m_vecPage)
	//	iter->SetRender(false);

	//m_vecPage[_int(UIPAGE::PAGE_LOADING)]->SetRender(true);
}

void CUIManager::SwicthPage(UIPAGE eNextPage)
{
	for (auto& iter : m_vecPage)
		iter->CloseAction();

	m_vecPage[_int(eNextPage)]->OpenAction();
}

void CUIManager::ShowTestPage(_wstring DataNameA, TEST_PAGE_VALUE_TYPE eTypeA, const void* ValueA, _wstring DataNameB, TEST_PAGE_VALUE_TYPE eTypeB, const void* ValueB, _wstring DataNameC, TEST_PAGE_VALUE_TYPE eTypeC, const void* ValueC, _wstring DataNameD, TEST_PAGE_VALUE_TYPE eTypeD, const void* ValueD, _wstring DataNameE, TEST_PAGE_VALUE_TYPE eTypeE, const void* ValueE, _wstring DataNameF, TEST_PAGE_VALUE_TYPE eTypeF, const void* ValueF, _wstring DataNameG, TEST_PAGE_VALUE_TYPE eTypeG, const void* ValueG, _wstring DataNameH, TEST_PAGE_VALUE_TYPE eTypeH, const void* ValueH, _wstring DataNameI, TEST_PAGE_VALUE_TYPE eTypeI, const void* ValueI, _wstring DataNameJ, TEST_PAGE_VALUE_TYPE eTypeJ, const void* ValueJ)
{
	if (!m_bTestPageOpen)
		return;

	vector<_wstring> vecName;
	vector<_wstring> vecValue;

	if (ValueA)
		InputTestPageInfo(&vecName, &vecValue, DataNameA, eTypeA, ValueA);
	if (ValueB)
		InputTestPageInfo(&vecName, &vecValue, DataNameB, eTypeB, ValueB);
	if (ValueC)
		InputTestPageInfo(&vecName, &vecValue, DataNameC, eTypeC, ValueC);
	if (ValueD)
		InputTestPageInfo(&vecName, &vecValue, DataNameD, eTypeD, ValueD);
	if (ValueE)
		InputTestPageInfo(&vecName, &vecValue, DataNameE, eTypeE, ValueE);
	if (ValueF)
		InputTestPageInfo(&vecName, &vecValue, DataNameF, eTypeF, ValueF);
	if (ValueG)
		InputTestPageInfo(&vecName, &vecValue, DataNameG, eTypeG, ValueG);
	if (ValueH)
		InputTestPageInfo(&vecName, &vecValue, DataNameH, eTypeH, ValueH);
	if (ValueI)
		InputTestPageInfo(&vecName, &vecValue, DataNameI, eTypeI, ValueI);
	if (ValueJ)
		InputTestPageInfo(&vecName, &vecValue, DataNameJ, eTypeJ, ValueJ);

	m_pUIRender_Client->Input_TestPageInfo(m_fTestPage_Pos, m_fTestPage_Size, vecName, vecValue);
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

	for (_int i = 0; i < _int(UIPAGE::PAGE_END); ++i)
	{
		if (FAILED(Make_UIPage(i)))
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

		m_vecPage[i]->Ready_UIPart_Group_Control();
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
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Main);
	}
	else if (iIndex == _int(UIPAGE::PAGE_LOADING))
	{
		m_pUIPage_Loading = CUIPage_Loading::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Loading);
	}
	else if (iIndex == _int(UIPAGE::PAGE_PLAY))
	{
		m_pUIPage_Play = CUIPage_Play::Create(m_pDevice, m_pContext);
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

void CUIManager::InputTestPageInfo(vector<_wstring>* pName, vector<_wstring>* pValue, _wstring DataNameA, TEST_PAGE_VALUE_TYPE eTypeA, const void* ValueA)
{
	pName->push_back(DataNameA);

	if (eTypeA == TEST_PAGE_VALUE_TYPE::TYPE_FLOAT)
	{
		_float fValue = 0.f;
		memcpy(&fValue, ValueA, sizeof(_float));
		_wstring strValue = to_wstring(fValue);
		pValue->push_back(strValue);
	}
	else if (eTypeA == TEST_PAGE_VALUE_TYPE::TYPE_INT)
	{
		_float fValue = 0.f;
		memcpy(&fValue, ValueA, sizeof(_float));
		_wstring strValue = to_wstring(fValue);
		pValue->push_back(strValue);
	}
	else if (eTypeA == TEST_PAGE_VALUE_TYPE::TYPE_FLOAT2)
	{
		_float2 fValue = { 0.f , 0.f };
		memcpy(&fValue, ValueA, sizeof(_float2));
		_wstring strTemp = to_wstring(fValue.x);
		_wstring strValue = strTemp;
		strValue += TEXT(" | ");
		strTemp = to_wstring(fValue.y);
		strValue += strTemp;
		pValue->push_back(strValue);
	}
	else if (eTypeA == TEST_PAGE_VALUE_TYPE::TYPE_FLOAT3)
	{
		_float3 fValue = { 0.f , 0.f , 0.f };
		memcpy(&fValue, ValueA, sizeof(_float3));
		_wstring strTemp = to_wstring(fValue.x);
		_wstring strValue = strTemp;
		strValue += TEXT(" | ");
		strTemp = to_wstring(fValue.y);
		strValue += strTemp;
		strValue += TEXT(" | ");
		strTemp = to_wstring(fValue.z);
		strValue += strTemp;
		pValue->push_back(strValue);
	}
	else if (eTypeA == TEST_PAGE_VALUE_TYPE::TYPE_FLOAT4)
	{
		_float4 fValue = { 0.f , 0.f, 0.f, 0.f };
		memcpy(&fValue, ValueA, sizeof(_float4));
		_wstring strTemp = to_wstring(fValue.x);
		_wstring strValue = strTemp;
		strValue += TEXT(" | ");
		strTemp = to_wstring(fValue.y);
		strValue += strTemp;
		strValue += TEXT(" | ");
		strTemp = to_wstring(fValue.z);
		strValue += strTemp;
		strValue += TEXT(" | ");
		strTemp = to_wstring(fValue.w);
		strValue += strTemp;
		pValue->push_back(strValue);
	}
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
	Safe_Delete(m_pTestData);
}
