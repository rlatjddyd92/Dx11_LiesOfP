
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

	Setting_TestPage();

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
	if (FAILED(m_pUIPage_Ortho->Render_Ortho_UI(m_pUIRender_Client)))
		return E_FAIL;
	
	if (FAILED(m_pUIRender_Client->Render_UI(m_vecPage)))
		return E_FAIL;

	/*if (FAILED(m_pUIPage_Inven->Render_Inven_Array(m_pUIRender_Client)))
		return E_FAIL;*/

	// 여기에 나중에 Tooltip 내용 넣기

#ifdef _DEBUG
	if (m_iFonttest != 0)
		if (FAILED(m_pUIRender_Client->Render_TestFont(m_iFonttest == 1)))
			return E_FAIL;

#endif // _DEBUG

	
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
	UIControl_Common(fTimeDelta);
}

void CUIManager::UIControl_Test(_float fTimeDelta)
{
#ifdef _DEBUG
	if (KEY_TAP(KEY::P)) // Page_Play(기본 플레이 화면) 띄우기/닫기 -> 정식 기능에서는 사용하지 않음 
	{
		Open_Close_Page(UIPAGE::PAGE_PLAY);
	}

	// 상단 바 
	if (KEY_HOLD(KEY::LSHIFT)) // 최대치 조정 
	{
		if (KEY_HOLD(KEY::I)) // 체력바 표시 수치 감소
			GET_GAMEINTERFACE->Add_StatMax_Normal(STAT_NORMAL::STAT_GAUGE_HP, -100.f * fTimeDelta);
		else if (KEY_HOLD(KEY::O)) // 체력바 표시 수치 감소
			GET_GAMEINTERFACE->Add_StatMax_Normal(STAT_NORMAL::STAT_GAUGE_HP, +100.f * fTimeDelta);

		if (KEY_HOLD(KEY::K)) // 체력바 표시 수치 감소
			GET_GAMEINTERFACE->Add_StatMax_Normal(STAT_NORMAL::STAT_GAUGE_STAMINA, -100.f * fTimeDelta);
		else if (KEY_HOLD(KEY::L)) // 체력바 표시 수치 감소
			GET_GAMEINTERFACE->Add_StatMax_Normal(STAT_NORMAL::STAT_GAUGE_STAMINA, +100.f * fTimeDelta);

		if (KEY_HOLD(KEY::N)) // 체력바 표시 수치 감소
			GET_GAMEINTERFACE->Add_StatMax_Normal(STAT_NORMAL::STAT_GAUGE_REGION, -100.f * fTimeDelta);
		else if (KEY_HOLD(KEY::M)) // 체력바 표시 수치 감소
			GET_GAMEINTERFACE->Add_StatMax_Normal(STAT_NORMAL::STAT_GAUGE_REGION, +100.f * fTimeDelta);
	}
	else // 스탯 조정 
	{
		if (KEY_HOLD(KEY::I)) // 체력바 표시 수치 감소
			GET_GAMEINTERFACE->Add_Stat_Normal(STAT_NORMAL::STAT_GAUGE_HP, -100.f * fTimeDelta);
		else if (KEY_HOLD(KEY::O)) // 체력바 표시 수치 증가 
			GET_GAMEINTERFACE->Add_Stat_Normal(STAT_NORMAL::STAT_GAUGE_HP, +100.f * fTimeDelta);

		if (KEY_HOLD(KEY::K)) // 체력바 표시 수치 감소
			GET_GAMEINTERFACE->Add_Stat_Normal(STAT_NORMAL::STAT_GAUGE_STAMINA, -100.f * fTimeDelta);
		else if (KEY_HOLD(KEY::L)) // 체력바 표시 수치 증가 
			GET_GAMEINTERFACE->Add_Stat_Normal(STAT_NORMAL::STAT_GAUGE_STAMINA, +100.f * fTimeDelta);

		if (KEY_HOLD(KEY::N)) // 체력바 표시 수치 감소
			GET_GAMEINTERFACE->Add_Stat_Normal(STAT_NORMAL::STAT_GAUGE_REGION, -100.f * fTimeDelta);
		else if (KEY_HOLD(KEY::M)) // 체력바 표시 수치 증가 
			GET_GAMEINTERFACE->Add_Stat_Normal(STAT_NORMAL::STAT_GAUGE_REGION, +100.f * fTimeDelta);
	}

	// 몬스터 체력바 확인 
	if (KEY_HOLD(KEY::ALT))
	{
		if (KEY_HOLD(KEY::I))
			m_pTestData->fHP_Now -= 100.f * fTimeDelta;
		else if (KEY_HOLD(KEY::O))
			m_pTestData->fHP_Now += 100.f * fTimeDelta;

		m_pTestData->fHP_Now = max(0, m_pTestData->fHP_Now);
		m_pTestData->fHP_Now = min(m_pTestData->fHP_Now, m_pTestData->fHP_Max);

		if (KEY_TAP(KEY::K))
		{
			if (m_pTestData->bFocus)
				m_pTestData->bFocus = false;
			else
				m_pTestData->bFocus = true;
		}

		if (KEY_TAP(KEY::M))
		{
			if (m_pTestData->bSpecial_Attack)
				m_pTestData->bSpecial_Attack = false;
			else
				m_pTestData->bSpecial_Attack = true;
		}
	}


	




#endif // DEBUG

	// 아래 내용은 테스트용 변수 확인 페이지를 위한 코드
	// 릴리즈에서도 사용할 수 있도록 설정
	// TestPage 이외의 내용은 아래에 넣지 않기 


	// TestPage 조작 
	if (KEY_HOLD(KEY::LSHIFT))
	{
		if (KEY_TAP(KEY::F3)) // TestPage 팀장
		{
			if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_TEAMJANG)])
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_TEAMJANG)] = false;
			else
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_TEAMJANG)] = true;
		}
		if (KEY_TAP(KEY::F4)) // TestPage 애니메이션
		{
			if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_ANIM)])
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_ANIM)] = false;
			else
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_ANIM)] = true;
		}
		if (KEY_TAP(KEY::F5)) // TestPage 이펙트
		{
			if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_EFFECT)])
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_EFFECT)] = false;
			else
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_EFFECT)] = true;
		}
		if (KEY_TAP(KEY::F6)) // TestPage 맵
		{
			if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_MAP)])
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_MAP)] = false;
			else
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_MAP)] = true;
		}
		if (KEY_TAP(KEY::F7)) // TestPage 스탯
		{
			if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_PLAYER_STAT)])
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_PLAYER_STAT)] = false;
			else
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_PLAYER_STAT)] = true;
		}
		if (KEY_TAP(KEY::F8)) // TestPage 아이템
		{
			if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_ITEM)])
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_ITEM)] = false;
			else
				m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_ITEM)] = true;
		}
	}

	if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_TEAMJANG)])
		ShowTestPage(TEST_PAGE_NAME::NAME_TEAMJANG,
			TEXT("테스트 변수 타임델타 : "), TEST_PAGE_VALUE_TYPE::TYPE_FLOAT, &fTimeDelta);

	if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_ANIM)])
		ShowTestPage(TEST_PAGE_NAME::NAME_ANIM,
			TEXT("테스트 변수 타임델타 : "), TEST_PAGE_VALUE_TYPE::TYPE_FLOAT, &fTimeDelta);

	if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_EFFECT)])
		ShowTestPage(TEST_PAGE_NAME::NAME_EFFECT,
			TEXT("테스트 변수 타임델타 : "), TEST_PAGE_VALUE_TYPE::TYPE_FLOAT, &fTimeDelta);

	if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_MAP)])
		ShowTestPage(TEST_PAGE_NAME::NAME_MAP,
			TEXT("테스트 변수 타임델타 : "), TEST_PAGE_VALUE_TYPE::TYPE_FLOAT, &fTimeDelta);

	if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_ITEM)])
		ShowTestPage(TEST_PAGE_NAME::NAME_ITEM,
			TEXT("테스트 변수 타임델타 : "), TEST_PAGE_VALUE_TYPE::TYPE_FLOAT, &fTimeDelta);


	if (m_vecTestPageOpen[_int(TEST_PAGE_NAME::NAME_PLAYER_STAT)])
	{
		_Vec3 fHP = { GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_HP).fStat_Now , GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_HP).fStat_Max, GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_HP).fStat_Max_Limit };
		_Vec3 fST = { GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_STAMINA).fStat_Now , GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_STAMINA).fStat_Max, GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_STAMINA).fStat_Max_Limit };
		_Vec3 fRE = { GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_REGION).fStat_Now , GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_REGION).fStat_Max, GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_REGION).fStat_Max_Limit };
		_Vec3 fWE = { GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_WEIGHT).fStat_Now , GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_WEIGHT).fStat_Max, GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GAUGE_WEIGHT).fStat_Max_Limit };
		_Vec3 fGA = { GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GADRIGAIN).fStat_Now , GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GADRIGAIN).fStat_Max, GET_GAMEINTERFACE->Get_StatInfo_Normal(STAT_NORMAL::STAT_GADRIGAIN).fStat_Max_Limit };
		_int iHP = (_int)GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_BODY);
		_int iST = (_int)GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_STAMINA);
		_int iWE = (_int)GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_WEIGHT);
		_int iPO = (_int)GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_POWER);
		_int iSK = (_int)GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_SKILL);
		_int iEV = (_int)GET_GAMEINTERFACE->Get_NowStat_Normal(STAT_NORMAL::STAT_POINT_EVOLUTION);

		ShowTestPage(TEST_PAGE_NAME::NAME_PLAYER_STAT,
			TEXT("HP(현재/제한/최대치)"), TEST_PAGE_VALUE_TYPE::TYPE_VEC3, &fHP,
			TEXT("스태미나(현재/제한/최대치)"), TEST_PAGE_VALUE_TYPE::TYPE_VEC3, &fST,
			TEXT("리전(현재/제한/최대치)"), TEST_PAGE_VALUE_TYPE::TYPE_VEC3, &fRE,
			TEXT("무게(현재/제한/최대치)"), TEST_PAGE_VALUE_TYPE::TYPE_VEC3, &fWE,
			TEXT("가드리게인(현재/제한/최대치)"), TEST_PAGE_VALUE_TYPE::TYPE_VEC3, &fGA,
			TEXT("신체 스탯"), TEST_PAGE_VALUE_TYPE::TYPE_INT, &iHP,
			TEXT("지구력 스탯"), TEST_PAGE_VALUE_TYPE::TYPE_INT, &iST,
			TEXT("무게 스탯"), TEST_PAGE_VALUE_TYPE::TYPE_INT, &iWE,
			TEXT("힘 스탯"), TEST_PAGE_VALUE_TYPE::TYPE_INT, &iPO,
			TEXT("기술 스탯"), TEST_PAGE_VALUE_TYPE::TYPE_INT, &iSK
		);
	}

	// 테스트 페이지 이동
	_bool bMoving = false;
	for (_int i = 0; i < _int(TEST_PAGE_NAME::NAME_END); ++i)
	{
		if (bMoving)
			break;

		if (m_vecTestPageOpen[i])
		{
			if ((KEY_TAP(KEY::LBUTTON)) && (!m_vecTestPageMove[i]))
			{
				_Vec2 fPoint = CheckMouse(m_vecTestPage_Pos[i], m_vecTestPage_Size[i]);

				if (fPoint.x != -1)
				{
					m_vecTestPageMove[i] = true;
					m_vecTestPage_ClickPos[i] = { fPoint.x, fPoint.y };
					bMoving = true;
				}
			}
			else if (m_vecTestPageMove[i])
			{
				if (KEY_HOLD(KEY::LBUTTON))
				{
					POINT			ptMouse{};
					GetCursorPos(&ptMouse);
					ScreenToClient(g_hWnd, &ptMouse);

					_Vec2 fMove = { 0.f,0.f };

					fMove.x = ptMouse.x - m_vecTestPage_ClickPos[i].x;
					fMove.y = ptMouse.y - m_vecTestPage_ClickPos[i].y;

					m_vecTestPage_Pos[i].x += fMove.x;
					m_vecTestPage_Pos[i].y += fMove.y;

					m_vecTestPage_ClickPos[i] = { (_float)ptMouse.x, (_float)ptMouse.y };

					bMoving = true;
				}
				else
					m_vecTestPageMove[i] = false;
			}
		}
		else
			m_vecTestPageMove[i] = false;
	}
}

void CUIManager::UIControl_Common(_float fTimeDelta)
{
	if (m_pUIPage_Main->GetPageAction(PAGEACTION::ACTION_ACTIVE))
		UIControl_Main(fTimeDelta);
	else if (m_pUIPage_Loading->GetPageAction(PAGEACTION::ACTION_ACTIVE))
		UIControl_Loading(fTimeDelta);
	else if ((m_pUIPage_Play->GetPageAction(PAGEACTION::ACTION_ACTIVE)) || (m_pUIPage_Play->GetPageAction(PAGEACTION::ACTION_OPENING)))
		UIControl_Play(fTimeDelta);
	else if ((m_pUIPage_Menu->GetPageAction(PAGEACTION::ACTION_ACTIVE)) || (m_pUIPage_Menu->GetPageAction(PAGEACTION::ACTION_OPENING)))
		UIControl_Menu(fTimeDelta);
	else if ((m_pUIPage_Inven->GetPageAction(PAGEACTION::ACTION_ACTIVE)) || (m_pUIPage_Inven->GetPageAction(PAGEACTION::ACTION_OPENING)))
		UIControl_Inven(fTimeDelta);
	else if ((m_pUIPage_Equip->GetPageAction(PAGEACTION::ACTION_ACTIVE)) || (m_pUIPage_Equip->GetPageAction(PAGEACTION::ACTION_OPENING)))
		UIControl_Equip(fTimeDelta);
	else if ((m_pUIPage_Stat->GetPageAction(PAGEACTION::ACTION_ACTIVE)) || (m_pUIPage_Stat->GetPageAction(PAGEACTION::ACTION_OPENING)))
		UIControl_Stat(fTimeDelta);
	else if ((m_pUIPage_Skill->GetPageAction(PAGEACTION::ACTION_ACTIVE)) || (m_pUIPage_Skill->GetPageAction(PAGEACTION::ACTION_OPENING)))
		UIControl_Skill(fTimeDelta);
	else if (m_bIsIngame)
		OpenPage(UIPAGE::PAGE_PLAY);
}

void CUIManager::UIControl_Main(_float fTimeDelta)
{
	// 메인 페이지 필요
}

void CUIManager::UIControl_Loading(_float fTimeDelta)
{
	// 로딩 페이지 필요
}

void CUIManager::UIControl_Play(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_MENU, UIPAGE::PAGE_PLAY);
	else
	{
		m_pUIPage_Play->Check_Page_Action(fTimeDelta);
	}
}

void CUIManager::UIControl_Menu(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_MENU, UIPAGE::PAGE_PLAY);
	else
	{
		m_pUIPage_Menu->Check_Page_Action(fTimeDelta);
	}
}

void CUIManager::UIControl_Inven(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_INVEN, UIPAGE::PAGE_MENU);
	else
	{
		m_pUIPage_Inven->Check_Page_Action(fTimeDelta);
	}
}

void CUIManager::UIControl_Equip(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_EQUIP, UIPAGE::PAGE_MENU);
	else
	{
		m_pUIPage_Equip->Check_Page_Action(fTimeDelta);
	}
}

void CUIManager::UIControl_Stat(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_STAT, UIPAGE::PAGE_MENU);
	else
	{
		m_pUIPage_Stat->Check_Page_Action(fTimeDelta);
	}
}

void CUIManager::UIControl_LevelUp(_float fTimeDelta)
{
}

void CUIManager::UIControl_Skill(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_SKILL, UIPAGE::PAGE_MENU);
	else
	{
		m_pUIPage_Skill->Check_Page_Action(fTimeDelta);
	}
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

void CUIManager::SwicthPage(UIPAGE ePageA, UIPAGE ePageB)
{
	Open_Close_Page(ePageA);
	Open_Close_Page(ePageB);
}

void CUIManager::ShowTestPage(TEST_PAGE_NAME eName, _wstring DataNameA, TEST_PAGE_VALUE_TYPE eTypeA, const void* ValueA, _wstring DataNameB, TEST_PAGE_VALUE_TYPE eTypeB, const void* ValueB, _wstring DataNameC, TEST_PAGE_VALUE_TYPE eTypeC, const void* ValueC, _wstring DataNameD, TEST_PAGE_VALUE_TYPE eTypeD, const void* ValueD, _wstring DataNameE, TEST_PAGE_VALUE_TYPE eTypeE, const void* ValueE, _wstring DataNameF, TEST_PAGE_VALUE_TYPE eTypeF, const void* ValueF, _wstring DataNameG, TEST_PAGE_VALUE_TYPE eTypeG, const void* ValueG, _wstring DataNameH, TEST_PAGE_VALUE_TYPE eTypeH, const void* ValueH, _wstring DataNameI, TEST_PAGE_VALUE_TYPE eTypeI, const void* ValueI, _wstring DataNameJ, TEST_PAGE_VALUE_TYPE eTypeJ, const void* ValueJ)
{
	_int iFunc = _int(eName);

	if (!m_vecTestPageOpen[iFunc])
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

	m_pUIRender_Client->Input_TestPageInfo(eName, m_vecTestPage_Pos[iFunc], m_vecTestPage_Size[iFunc], vecName, vecValue);
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
		if (!FAILED(Make_UIPage(i)))
		{
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

			_Vec2 fPosition = { 0.f,0.f };
			ReadFile(hFile, &fPosition, sizeof(_Vec2), &dwByte, nullptr);

			m_vecPage[i]->SetUIPagePosition(fPosition);


			if (FAILED(Load_UIDataFile_Part(hFile, &dwByte, i)))
				MSG_BOX(TEXT("UI 데이터 불러오기 실패"));

			m_vecPage[i]->Ready_UIPart_Group_Control();
		}
		else 
			MSG_BOX(TEXT("UIPage 제작 필요"));
		
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
	else if (iIndex == _int(UIPAGE::PAGE_MENU))
	{
		m_pUIPage_Menu = CUIPage_Menu::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Menu);
	}
	else if (iIndex == _int(UIPAGE::PAGE_INVEN))
	{
		m_pUIPage_Inven = CUIPage_Inven::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Inven);
	}
	else if (iIndex == _int(UIPAGE::PAGE_EQUIP))
	{
		m_pUIPage_Equip = CUIPage_Equip::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Equip);
	}
	else if (iIndex == _int(UIPAGE::PAGE_STAT))
	{
		m_pUIPage_Stat = CUIPage_Stat::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Stat);
	}
	else if (iIndex == _int(UIPAGE::PAGE_LEVELUP))
	{
		m_pUIPage_LevelUp = CUIPage_LevelUp::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_LevelUp);
	}
	else if (iIndex == _int(UIPAGE::PAGE_SKILL))
	{
		m_pUIPage_Skill = CUIPage_Skill::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Skill);
	}
	else if (iIndex == _int(UIPAGE::PAGE_TOOLTIP))
	{
		m_pUIPage_ToolTip = CUIPage_ToolTip::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_ToolTip);
	}
	else if (iIndex == _int(UIPAGE::PAGE_ORTHO))
	{
		m_pUIPage_Ortho = CUIPage_Ortho::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Ortho);
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
		ReadFile(handle, &pNew->fAdjust, sizeof(_Vec2), dword, nullptr);
		ReadFile(handle, &pNew->fAdjust_End, sizeof(_Vec2), dword, nullptr);
		ReadFile(handle, &pNew->fAdjust_Start, sizeof(_Vec2), dword, nullptr);
		ReadFile(handle, &pNew->fDirec, sizeof(_Vec2), dword, nullptr);
		ReadFile(handle, &pNew->fPosition, sizeof(_Vec2), dword, nullptr);
		ReadFile(handle, &pNew->fRatio, sizeof(_float), dword, nullptr);
		ReadFile(handle, &pNew->fSize, sizeof(_Vec2), dword, nullptr);
		ReadFile(handle, &pNew->fTextColor, sizeof(_Vec4), dword, nullptr);
		ReadFile(handle, &pNew->iFontIndex, sizeof(_int), dword, nullptr);
		ReadFile(handle, &pNew->iGroupIndex, sizeof(_int), dword, nullptr);
		ReadFile(handle, &pNew->iMoveType, sizeof(_int), dword, nullptr);
		ReadFile(handle, &pNew->iParentPart_Index, sizeof(_int), dword, nullptr);
		ReadFile(handle, &pNew->iTexture_Index, sizeof(_int), dword, nullptr);
		ReadFile(handle, &pNew->fTextureColor, sizeof(_Vec4), dword, nullptr);
		ReadFile(handle, &pNew->bIsItem, sizeof(_bool), dword, nullptr);
		ReadFile(handle, &pNew->bTurn, sizeof(_bool), dword, nullptr);
		ReadFile(handle, &pNew->fTurn_Degree, sizeof(_float), dword, nullptr);

		ReadFile(handle, &pNew->bTexture_Color_Multiple, sizeof(_bool), dword, nullptr);
		ReadFile(handle, &pNew->fStrash_Alpha, sizeof(_float), dword, nullptr);
		ReadFile(handle, &pNew->bText_Right, sizeof(_bool), dword, nullptr);

		if (!pNew->bIsItem)
			m_pUIRender_Client->Make_Texture(pNew->iTexture_Index);
		else 
			m_pUIRender_Client->Make_Texture_Item(pNew->iTexture_Index);
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

void CUIManager::Setting_TestPage()
{
	m_vecTestPage_Pos.resize(_int(TEST_PAGE_NAME::NAME_END));
	m_vecTestPage_Size.resize(_int(TEST_PAGE_NAME::NAME_END));
	m_vecTestPage_ClickPos.resize(_int(TEST_PAGE_NAME::NAME_END));

	m_vecTestPageOpen.resize(_int(TEST_PAGE_NAME::NAME_END));
	m_vecTestPageMove.resize(_int(TEST_PAGE_NAME::NAME_END));

	for (_int i = 0; i < _int(TEST_PAGE_NAME::NAME_END); ++i)
	{
		m_vecTestPage_Pos[i] = { 500.f,500.f };
		m_vecTestPage_Size[i] = { 500.f,300.f };
	}

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
		_int iValue = 0;
		memcpy(&iValue, ValueA, sizeof(_int));
		_wstring strValue = to_wstring(iValue);
		pValue->push_back(strValue);
	}
	else if (eTypeA == TEST_PAGE_VALUE_TYPE::TYPE_VEC2)
	{
		_Vec2 fValue = { 0.f , 0.f };
		memcpy(&fValue, ValueA, sizeof(_Vec2));
		_wstring strTemp = to_wstring(fValue.x);
		_wstring strValue = strTemp;
		strValue += TEXT(" | ");
		strTemp = to_wstring(fValue.y);
		strValue += strTemp;
		pValue->push_back(strValue);
	}
	else if (eTypeA == TEST_PAGE_VALUE_TYPE::TYPE_VEC3)
	{
		_Vec3 fValue = { 0.f , 0.f , 0.f };
		memcpy(&fValue, ValueA, sizeof(_Vec3));
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
	else if (eTypeA == TEST_PAGE_VALUE_TYPE::TYPE_VEC4)
	{
		_Vec4 fValue = { 0.f , 0.f, 0.f, 0.f };
		memcpy(&fValue, ValueA, sizeof(_Vec4));
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

void CUIManager::Open_Close_Page(UIPAGE ePage)
{
	if (m_vecPage[_int(ePage)]->GetRender())
	{
		if ((m_vecPage[_int(ePage)]->GetPageAction(PAGEACTION::ACTION_OPENING)) || (m_vecPage[_int(ePage)]->GetPageAction(PAGEACTION::ACTION_ACTIVE)))
			ClosePage(ePage);
		else if ((m_vecPage[_int(ePage)]->GetPageAction(PAGEACTION::ACTION_CLOSING)) || (m_vecPage[_int(ePage)]->GetPageAction(PAGEACTION::ACTION_INACTIVE)))
			OpenPage(ePage);
	}
	else
		OpenPage(ePage);
}

void CUIManager::OpenPage(UIPAGE ePage)
{
	m_vecPage[_int(ePage)]->OpenAction();
}

void CUIManager::ClosePage(UIPAGE ePage)
{
	m_vecPage[_int(ePage)]->CloseAction();
}

_Vec2 CUIManager::CheckMouse(_Vec2 fPos, _Vec2 fSize)
{
	POINT			ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (ptMouse.x > fPos.x - (fSize.x * 0.5f))
		if (ptMouse.x < fPos.x + (fSize.x * 0.5f))
			if (ptMouse.y > fPos.y - (fSize.y * 0.5f))
				if (ptMouse.y < fPos.y + (fSize.y * 0.5f))
					return { (_float)ptMouse.x, (_float)ptMouse.y};

	return { -1.f,-1.f };
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

	m_vecTestPage_Pos.clear();
	m_vecTestPage_Size.clear();
	m_vecTestPage_ClickPos.clear();

	m_vecTestPageOpen.clear();
	m_vecTestPageMove.clear();

	Safe_Release(m_pUIRender_Client);
	Safe_Delete(m_pTestData);
}
