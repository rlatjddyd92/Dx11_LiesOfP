#include "stdafx.h"
#include "Controller_UITool.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CController_UITool)

HRESULT CController_UITool::UITool_Render()
{
	// 폰트 테스트 

	_float fX = 700.f;
	_float fY = 50.f;
	_float fGap = 1.f;

	m_pGameInstance->Render_Text(TEXT("FONT_INFO_12"), TEXT("r : INFO_12"), { fX,fY + (fGap * 0),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_INFO_18"), TEXT("r : INFO_18"), { fX,fY + (fGap * 12),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_INFO_24"), TEXT("r : INFO_24"), { fX,fY + (fGap * 30),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_INFO_36"), TEXT("r : INFO_36"), { fX,fY + (fGap * 54),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_INFO_48"), TEXT("r : INFO_48"), { fX,fY + (fGap * 90),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_INFO_60"), TEXT("r : INFO_60"), { fX,fY + (fGap * 138),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_INFO_72"), TEXT("r : INFO_72"), { fX,fY + (fGap * 198),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_TITLE_12"), TEXT("r : TITLE_12"), { fX,fY + (fGap * 270),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_TITLE_18"), TEXT("r : TITLE_18"), { fX,fY + (fGap * 282),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_TITLE_24"), TEXT("r : TITLE_24"), { fX,fY + (fGap * 300),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_TITLE_36"), TEXT("r : TITLE_36"), { fX,fY + (fGap * 324),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_TITLE_48"), TEXT("r : TITLE_48"), { fX,fY + (fGap * 360),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_TITLE_60"), TEXT("r : TITLE_60"), { fX,fY + (fGap * 408),0.f,0.f });
	m_pGameInstance->Render_Text(TEXT("FONT_TITLE_72"), TEXT("r : TITLE_72"), { fX,fY + (fGap * 468),0.f,0.f });



	return S_OK;
}

CController_UITool::CController_UITool()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CController_UITool::Initialize()
{
	string strType = {};
	for (_int i = 0; i < _int(PART_TYPE::TYPE_END); ++i)
		m_UIPartTypeName[i] = new _char[20];

	strType = "Static"; // 일반 : 이동 기능만 있는 UI
	for (_int i = 0; i <= strType.size(); ++i)
		m_UIPartTypeName[_int(PART_TYPE::TYPE_STATIC)][i] = strType[i];
	
	strType = "Bar"; // 바 : 최대치 대비 현재 수치의 비율에 따라 변화하는 UI (HP 바 등)
	for (_int i = 0; i <= strType.size(); ++i)
		m_UIPartTypeName[_int(PART_TYPE::TYPE_BAR)][i] = strType[i];

	strType = "Shading"; // 쉐이딩 : 상황에 따라 색을 변경하는 UI
	for (_int i = 0; i <= strType.size(); ++i)
		m_UIPartTypeName[_int(PART_TYPE::TYPE_SHADING)][i] = strType[i];

	strType = "Sprite"; // 스프라이트 : 스프라이트 애니메이션이 적용된 UI
	for (_int i = 0; i <= strType.size(); ++i)
		m_UIPartTypeName[_int(PART_TYPE::TYPE_SPRITE)][i] = strType[i];

	strType = "Static_Text"; // 고정 텍스트 : 변경 없이 고정된 내용을 보여 주는 UI
	for (_int i = 0; i <= strType.size(); ++i)
		m_UIPartTypeName[_int(PART_TYPE::TYPE_STATIC_TEXT)][i] = strType[i];

	strType = "Variable_Text"; // 변수 텍스트 : 연결된 변수의 내용을 텍스트로 보여 주는 UI
	for (_int i = 0; i <= strType.size(); ++i)
		m_UIPartTypeName[_int(PART_TYPE::TYPE_VARIABLE_TEXT)][i] = strType[i];


	if (FAILED(InitializeResource()))
		return E_FAIL;

	for (_int i = 0; i < m_iPartIndex_NumMax; ++i)
	{
		m_UIPartName[i] = new _char[100];
		_int iIndex = -1;

		do
		{
			++iIndex;
			m_UIPartName[i][iIndex] = m_vecUPart[i]->strUIPart_Name[iIndex];
		} while (m_vecUPart[i]->strUIPart_Name[iIndex] != '\0');
	}
		






	return S_OK;
}

HRESULT CController_UITool::InitializeResource()
{
	m_vecUPart.resize(m_iPartIndex_NumMax);

	if (FAILED(InitializeTexture()))
		return E_FAIL;

	if (FAILED(InitializeFont()))
		return E_FAIL;


	if (FAILED(LoadPart()))
		return E_FAIL;

	return S_OK;
}

HRESULT CController_UITool::SavePart()
{





	return S_OK;
}

HRESULT CController_UITool::LoadPart()
{
	// 엑셀 파일 파싱
















	// 임시 코드 

	_int iCount = 0;

	for (auto& iter : m_vecUPart)
	{
		iter = new UPART;

		string strTemp = "none";
		char czNum =  '0' + iCount;

		strTemp += czNum;

		++iCount;

		iter->strUIPart_Name = new _char[100];

		for (_int i = 0; i <= strTemp.size(); ++i)
			iter->strUIPart_Name[i] = strTemp[i];
	}
		




	return S_OK;
}

HRESULT CController_UITool::InitializeTexture()
{
	return S_OK;
}

HRESULT CController_UITool::InitializeFont()
{
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_12"), TEXT("../Bin/Resources/Fonts/Font_Info_12.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_18"), TEXT("../Bin/Resources/Fonts/Font_Info_18.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_24"), TEXT("../Bin/Resources/Fonts/Font_Info_24.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_36"), TEXT("../Bin/Resources/Fonts/Font_Info_36.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_48"), TEXT("../Bin/Resources/Fonts/Font_Info_48.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_60"), TEXT("../Bin/Resources/Fonts/Font_Info_60.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_INFO_72"), TEXT("../Bin/Resources/Fonts/Font_Info_72.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_12"), TEXT("../Bin/Resources/Fonts/Font_Title_12.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_18"), TEXT("../Bin/Resources/Fonts/Font_Title_18.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_24"), TEXT("../Bin/Resources/Fonts/Font_Title_24.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_36"), TEXT("../Bin/Resources/Fonts/Font_Title_36.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_48"), TEXT("../Bin/Resources/Fonts/Font_Title_48.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_60"), TEXT("../Bin/Resources/Fonts/Font_Title_60.spritefont"));
	m_pGameInstance->Add_Font(TEXT("FONT_TITLE_72"), TEXT("../Bin/Resources/Fonts/Font_Title_72.spritefont"));

	return S_OK;
}

void CController_UITool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (_int i = 0; i < 100; ++i)
		Safe_Delete_Array(m_UIPartName[i]);

	for (auto& iter : m_vecUPart)
	{
		Safe_Delete_Array(iter->strUIPart_Name);
		iter->vecSocketPosition.clear();
		Safe_Delete(iter);
	}

	for (_int i = 0; i < _int(PART_TYPE::TYPE_END); ++i)
		Safe_Delete_Array(m_UIPartTypeName[i]);
	

	m_vecUPart.clear();
}
