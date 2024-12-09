
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

	// 24-22-23 김성용 : UI 전용 쉐이더 추가 
#pragma region UI_Shader 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_Change_Color"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI_Change_Color.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_Multiple_Color"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI_Multiple_Color.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_Normal"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI_Normal.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI_Master"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI_Master.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
#pragma endregion

	m_pUIRender_Batching = CUIRender_Batching::Create(m_pDevice, m_pContext);

	m_pUIRender_Batching->Set_Scroll_Area(SCROLL_AREA::SCROLL_NONE, { m_fX,m_fY }, { m_fSizeX,m_fSizeY });

	if (FAILED(Load_UIDataFile()))
		return E_FAIL;

	Setting_TestPage();

	m_vecTestPageInfo.resize(_int(TEST_PAGE_NAME::NAME_END));
	for (auto& iter : m_vecTestPageInfo)
	{
		iter.resize(10);

		for (auto& iterIndex : iter)
			iterIndex.resize(2);
	}


	m_vecTestPage_Pos.resize(_int(TEST_PAGE_NAME::NAME_END));
	m_vecTestPage_Size.resize(_int(TEST_PAGE_NAME::NAME_END));


	m_pTestData = new TESTDATA;





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

	if (m_pUIPage_Play->GetRender())
		m_pUIPage_Ortho->Late_Update(fTimeDelta);

	m_pUIPage_Effect->SetUpdate(true);

	for (_int i = 0; i < _int(UIPAGE::PAGE_END); ++i)
	{
		if (i == _int(UIPAGE::PAGE_ORTHO))
			continue;
		if (i == _int(UIPAGE::PAGE_EFFECT))
			continue;
		if (m_vecPage[i]->GetPageAction(PAGEACTION::ACTION_INACTIVE))
			continue;

		if (m_vecPage[i]->GetUpdate())
			m_vecPage[i]->Late_Update(fTimeDelta);
	}

	m_pUIPage_Effect->Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIManager::Render()
{
	if (FAILED(m_pUIRender_Batching->Render()))
		return E_FAIL;

#ifdef _DEBUG

#endif // _DEBUG

	m_pUIRender_Batching->Select_Scroll_Area(SCROLL_AREA::SCROLL_NONE);
	return S_OK;
}

void CUIManager::Update_UIManager(_float fTimeDelta)
{
	Priority_Update(fTimeDelta);
	Update(fTimeDelta);
	Update_UIControl(fTimeDelta);
	Late_Update(fTimeDelta);
	Update_TestPage(fTimeDelta);
}

void CUIManager::Update_UIControl(_float fTimeDelta)
{
	// 조작 관련 모든 것 
	// 마우스, 키보드를 통한 UI 조작은 모두 여기 작성한다 

	UIControl_Common(fTimeDelta);
}

void CUIManager::Update_TestPage(_float fTimeDelta)
{


}

void CUIManager::UIControl_Test(_float fTimeDelta)
{

}

void CUIManager::UIControl_Talking(_float fTimeDelta)
{
}

void CUIManager::UIControl_Popup(_float fTimeDelta)
{
}

void CUIManager::UIControl_Common(_float fTimeDelta)
{
	//m_bIsPlayPageMaintain = true;



	if (m_pUIPage_Effect->Check_Fade() > 0.f)
		m_bIsPlayPageMaintain = false;
	if ((m_pUIPage_Talking->GetPageAction(PAGEACTION::ACTION_ACTIVE)) || (m_pUIPage_Talking->GetPageAction(PAGEACTION::ACTION_OPENING)))
		m_bIsPlayPageMaintain = false;

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
	else if ((m_pUIPage_Option->GetPageAction(PAGEACTION::ACTION_ACTIVE)) || (m_pUIPage_Option->GetPageAction(PAGEACTION::ACTION_OPENING)))
		UIControl_Option(fTimeDelta);
	else if ((m_pUIPage_Skill->GetPageAction(PAGEACTION::ACTION_ACTIVE)) || (m_pUIPage_Skill->GetPageAction(PAGEACTION::ACTION_OPENING)))
		UIControl_Skill(fTimeDelta);
	else if ((m_bIsIngame) && (m_bIsPlayPageMaintain))
		OpenPage(UIPAGE::PAGE_PLAY);
}

void CUIManager::UIControl_Main(_float fTimeDelta)
{
	m_pUIPage_Main->Check_Page_Action(fTimeDelta);

	if (m_pUIPage_Main->GetPageAction(PAGEACTION::ACTION_INACTIVE))
	{
		m_pUIPage_Loading->GetPageAction(PAGEACTION::ACTION_ACTIVE);
		m_pUIPage_Loading->SetUpdate(true);
		m_pUIPage_Loading->SetRender(true);
	}

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
		m_eNowPage = UIPAGE::PAGE_PLAY;
		m_pUIPage_Play->Check_Page_Action(fTimeDelta);
	}
}

void CUIManager::UIControl_Menu(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_MENU, UIPAGE::PAGE_PLAY);
	else
	{
		m_eNowPage = UIPAGE::PAGE_MENU;
		m_pUIPage_Menu->Check_Page_Action(fTimeDelta);
	}
}

void CUIManager::UIControl_Inven(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_INVEN, UIPAGE::PAGE_MENU);
	else
	{
		m_eNowPage = UIPAGE::PAGE_INVEN;
		m_eNow_Active_Func = ITEM_FUNC::FUNC_END;
		m_pUIPage_Inven->Check_Page_Action(fTimeDelta);

		if (m_pUIPage_Inven->Get_ItemAction_Info()->eAction_Array_Type == INVEN_ARRAY_TYPE::TYPE_END)
			m_pUIPage_ItemInfo->Off_ItemAction();

		m_pUIPage_ItemInfo->Set_Active_ItemInfo(true, UIPAGE::PAGE_INVEN);
		m_pUIPage_ItemInfo->Check_Page_Action(fTimeDelta);
		m_eNow_Active_Func = m_pUIPage_ItemInfo->Get_Active_Func();

		GET_GAMEINTERFACE->Adjust_Spec();
	}
}

void CUIManager::UIControl_Equip(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_EQUIP, UIPAGE::PAGE_MENU);
	else
	{
		m_eNowPage = UIPAGE::PAGE_EQUIP;
		m_eNow_Active_Func = ITEM_FUNC::FUNC_END;
		m_pUIPage_Equip->Check_Page_Action(fTimeDelta);

		if (m_pUIPage_Equip->Get_ItemAction_Info()->eAction_Equip_Slot == EQUIP_SLOT::EQUIP_END)
			m_pUIPage_ItemInfo->Off_ItemAction();

		m_pUIPage_ItemInfo->Set_Active_ItemInfo(true, UIPAGE::PAGE_EQUIP);
		m_pUIPage_ItemInfo->Check_Page_Action(fTimeDelta);
		m_eNow_Active_Func = m_pUIPage_ItemInfo->Get_Active_Func();

		GET_GAMEINTERFACE->Adjust_Spec();
	}
}

void CUIManager::UIControl_Stat(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_STAT, UIPAGE::PAGE_MENU);
	else
	{
		m_eNowPage = UIPAGE::PAGE_STAT;
		m_pUIPage_Stat->Check_Page_Action(fTimeDelta);
	}
}

void CUIManager::UIControl_Option(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_OPTION, UIPAGE::PAGE_MENU);
	else
	{
		m_eNowPage = UIPAGE::PAGE_OPTION;
		m_pUIPage_Option->Check_Page_Action(fTimeDelta);
	}
}

void CUIManager::UIControl_Skill(_float fTimeDelta)
{
	if (KEY_TAP(KEY::ESC))
		SwicthPage(UIPAGE::PAGE_SKILL, UIPAGE::PAGE_MENU);
	else
	{
		m_eNowPage = UIPAGE::PAGE_SKILL;
		m_pUIPage_Skill->Check_Page_Action(fTimeDelta);
	}
}



void CUIManager::OpenMainPage()
{
	m_vecPage[_int(UIPAGE::PAGE_MAIN)]->Update(true);
	m_vecPage[_int(UIPAGE::PAGE_MAIN)]->SetRender(true);
	m_pUIPage_Main->Open_Main_Page();
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

	if (ValueA)
		InputTestPageInfo(eName, DataNameA, eTypeA, ValueA, 0);
	if (ValueB)
		InputTestPageInfo(eName, DataNameB, eTypeB, ValueB, 1);
	if (ValueC)
		InputTestPageInfo(eName, DataNameC, eTypeC, ValueC, 2);
	if (ValueD)
		InputTestPageInfo(eName, DataNameD, eTypeD, ValueD, 3);
	if (ValueE)
		InputTestPageInfo(eName, DataNameE, eTypeE, ValueE, 4);
	if (ValueF)
		InputTestPageInfo(eName, DataNameF, eTypeF, ValueF, 5);
	if (ValueG)
		InputTestPageInfo(eName, DataNameG, eTypeG, ValueG, 6);
	if (ValueH)
		InputTestPageInfo(eName, DataNameH, eTypeH, ValueH, 7);
	if (ValueI)
		InputTestPageInfo(eName, DataNameI, eTypeI, ValueI, 8);
	if (ValueJ)
		InputTestPageInfo(eName, DataNameJ, eTypeJ, ValueJ, 9);



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
	m_pGameInstance->Get_CurLevel();

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
	else if (iIndex == _int(UIPAGE::PAGE_SKILL))
	{
		m_pUIPage_Skill = CUIPage_Skill::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Skill);
	}
	else if (iIndex == _int(UIPAGE::PAGE_OPTION))
	{
		m_pUIPage_Option = CUIPage_Option::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Option);
	}
	else if (iIndex == _int(UIPAGE::PAGE_ITEMINFO))
	{
		m_pUIPage_ItemInfo = CUIPage_ItemInfo::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_ItemInfo);
	}
	else if (iIndex == _int(UIPAGE::PAGE_ORTHO))
	{
		m_pUIPage_Ortho = CUIPage_Ortho::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Ortho);
	}
	else if (iIndex == _int(UIPAGE::PAGE_TEST))
	{
		m_pUIPage_Test = CUIPage_Test::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Test);
	}
	else if (iIndex == _int(UIPAGE::PAGE_EFFECT))
	{
		m_pUIPage_Effect = CUIPage_Effect::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Effect);
	}
	else if (iIndex == _int(UIPAGE::PAGE_TALKING))
	{
		m_pUIPage_Talking = CUIPage_Talking::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Talking);
	}
	else if (iIndex == _int(UIPAGE::PAGE_POPUP))
	{
		m_pUIPage_Popup = CUIPage_Popup::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Popup);
	}
	else if (iIndex == _int(UIPAGE::PAGE_INFORM))
	{
		m_pUIPage_Inform = CUIPage_Inform::Create(m_pDevice, m_pContext);
		m_vecPage[iIndex] = static_cast<CUIPage*>(m_pUIPage_Inform);
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

		ReadFile(handle, &pNew->vTexture_Range, sizeof(_Vec4), dword, nullptr);
		ReadFile(handle, &pNew->vTexture_Angle, sizeof(_Vec2), dword, nullptr);

		if (!pNew->bIsItem)
			m_pUIRender_Batching->Make_Texture(pNew->iTexture_Index);
		else
			m_pUIRender_Batching->Make_Texture_Item(pNew->iTexture_Index);
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

void CUIManager::InputTestPageInfo(TEST_PAGE_NAME eName, _wstring DataNameA, TEST_PAGE_VALUE_TYPE eTypeA, const void* ValueA, _int iIndex)
{
	m_vecTestPageInfo[_int(eName)][iIndex][0] = DataNameA;

	if (eTypeA == TEST_PAGE_VALUE_TYPE::TYPE_FLOAT)
	{
		_float fValue = 0.f;
		memcpy(&fValue, ValueA, sizeof(_float));
		_wstring strValue = to_wstring(fValue);
		m_vecTestPageInfo[_int(eName)][iIndex][1] = strValue;
	}
	else if (eTypeA == TEST_PAGE_VALUE_TYPE::TYPE_INT)
	{
		_int iValue = 0;
		memcpy(&iValue, ValueA, sizeof(_int));
		_wstring strValue = to_wstring(iValue);
		m_vecTestPageInfo[_int(eName)][iIndex][1] = strValue;
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
		m_vecTestPageInfo[_int(eName)][iIndex][1] = strValue;
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
		m_vecTestPageInfo[_int(eName)][iIndex][1] = strValue;
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
		m_vecTestPageInfo[_int(eName)][iIndex][1] = strValue;
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
	m_pUIPage_Popup->Off_Popup();

	if (ePage != UIPAGE::PAGE_END)
		m_vecPage[_int(ePage)]->OpenAction();
}

void CUIManager::ClosePage(UIPAGE ePage)
{
	m_pUIPage_Popup->Off_Popup();

	if (ePage != UIPAGE::PAGE_END)
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
					return { (_float)ptMouse.x, (_float)ptMouse.y };

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

	Safe_Release(m_pUIRender_Batching);

	for (auto& iter : m_vecTestPageInfo)
	{
		for (auto& iterIndex : iter)
			iterIndex.clear();

		iter.clear();
	}
	Safe_Release(m_pGameInstance);

	m_vecTestPageInfo.clear();

	Safe_Delete(m_pTestData);
}