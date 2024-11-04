#include "stdafx.h"
#include "Controller_UITool.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CController_UITool)

void CController_UITool::UITool_Edit()
{
	UIPage_Edit();
}

HRESULT CController_UITool::UITool_Render()
{
	//FontTest();

	UISocket_Render();
	UIPart_Render();

	return S_OK;
}

void CController_UITool::UIPage_Edit()
{
	if (ImGui::CollapsingHeader("Page Setting"))
	{
		if (m_vecPageInfo.empty())
		{
			ImGui::Text("Empty Pagelist");
			return;
		}

		ImGui::Text("Now Page : ");
		ImGui::SameLine();
		ImGui::Text(m_vecPageInfo[m_iNowSelectNum]->strUIPage_Name);

		ImGuiComboFlags Flag = 0;

		ImGui::Combo("SelectPage", &m_iNowSelectNum, m_ArrPageName, 100);

		ImGui::InputText("PageName", m_InputPageName, sizeof(m_InputPageName));
		if (ImGui::Button("ChangeName"))
		{
			_int iIndex = -1;

			do
			{
				++iIndex;
				m_vecPageInfo[m_iNowSelectNum]->strUIPage_Name[iIndex] = m_InputPageName[iIndex];
				m_ArrPageName[m_iNowSelectNum][iIndex] = m_InputPageName[iIndex];
			} while (m_InputPageName[iIndex] != '\0');
		}

		ImGui::NewLine();
	}

	if (ImGui::CollapsingHeader("Socket Setting"))
		UISocket_Edit();

	if (ImGui::CollapsingHeader("Part Setting"))
		UIPart_Edit();
}

void CController_UITool::UISocket_Edit()
{
	

	ImGui::Text("SocketCount : ");
	ImGui::SameLine();

	_int iCount = m_vecPageInfo[m_iNowSelectNum]->vecSocket.size();
	_tchar* tTemp = new _tchar[5];
	swprintf(tTemp, 5, L"%d", iCount);
	_char* szCount = new _char[5];
	for (_int i = 0; i < 5; ++i)
		szCount[i] = tTemp[i];

	ImGui::Text(szCount);

	ImGui::PushItemWidth(100.f);

	ImGui::SeparatorText("AddSocket");
	ImGui::InputText("SocketName", m_InputSocketName, sizeof(m_InputSocketName));
	if (ImGui::Button("AddSocket"))
	{
		_int iIndex = -1;

		USOCKET* pNew = new USOCKET;
		pNew->strUISocket_Name = new _char[100];
		do
		{
			++iIndex;
			pNew->strUISocket_Name[iIndex] = m_InputSocketName[iIndex];
		} while (m_InputSocketName[iIndex] != '\0');

		m_vecPageInfo[m_iNowSelectNum]->vecSocket.push_back(pNew);
	}
	ImGui::SeparatorText("Socketlist");
	
	ImGui::PushItemWidth(50.f);

	for (auto& iter : m_vecPageInfo[m_iNowSelectNum]->vecSocket)
	{
		ImGui::Text(iter->strUISocket_Name);
		ImGui::SameLine();
		ImGui::InputFloat("X", &iter->fPosition.x);
		ImGui::SameLine();
		ImGui::InputFloat("Y", &iter->fPosition.y);

	}





	Safe_Delete_Array(tTemp);
	Safe_Delete_Array(szCount);

	ImGui::NewLine();
}

void CController_UITool::UIPart_Edit()
{

	ImGui::Text("PartCount : ");
	ImGui::SameLine();

	_int iCount = m_vecPageInfo[m_iNowSelectNum]->vecPart.size();
	_tchar* tTemp = new _tchar[5];
	swprintf(tTemp, 5, L"%d", iCount);
	_char* szCount = new _char[5];
	for (_int i = 0; i < 5; ++i)
		szCount[i] = tTemp[i];

	ImGui::Text(szCount);

	ImGui::PushItemWidth(100.f);

	ImGui::SeparatorText("AddPart");
	ImGui::InputText("PartName", m_InputPartName, sizeof(m_InputPartName));
	if (ImGui::Button("AddPart"))
	{
		_int iIndex = -1;

		UPART* pNew = new UPART;
		pNew->strUIPart_Name = new _char[100];
		do
		{
			++iIndex;
			pNew->strUIPart_Name[iIndex] = m_InputPartName[iIndex];
		} while (m_InputPartName[iIndex] != '\0');

		m_vecPageInfo[m_iNowSelectNum]->vecPart.push_back(pNew);
	}

	ImGui::SeparatorText("Partlist");

	for (auto& iter : m_vecPageInfo[m_iNowSelectNum]->vecPart)
	{
		ImGui::Text(iter->strUIPart_Name);
		ImGui::SameLine();
		ImGui::PushItemWidth(50.f);
		ImGui::InputFloat("X", &iter->fSize.x);
		ImGui::SameLine();
		ImGui::InputFloat("Y", &iter->fSize.y);
		ImGui::SameLine();
		ImGui::PushItemWidth(70.f);
		ImGui::InputInt("Soc", &iter->iSocket_Index);
		ImGui::SameLine();
		ImGui::InputInt("Tex", &iter->iTexture_Index);


		ImGui::SameLine();
		if ((iter->iSocket_Index < 0) || (iter->iSocket_Index >= m_vecPageInfo[m_iNowSelectNum]->vecSocket.size()))
			ImGui::Text(" ※InvalidSocket");
		ImGui::SameLine();
		if ((iter->iTexture_Index < 0) || (iter->iTexture_Index >= m_vecTextureInfo.size()))
			ImGui::Text(" ※InvalidTexture");

	}





	Safe_Delete_Array(tTemp);
	Safe_Delete_Array(szCount);

	ImGui::NewLine();

}

void CController_UITool::AddNewPage()
{
}

void CController_UITool::UISocket_Render()
{
	// 소켓 위치와 좌표, 이름 표시 필요 
	// * <- Center_1 (100.4, 302.5)

	for (auto& iter : m_vecPageInfo[m_iNowSelectNum]->vecSocket)
	{
		
	}


}

void CController_UITool::UIPart_Render()
{
}

void CController_UITool::FontTest()
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


}

CController_UITool::CController_UITool()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CController_UITool::Initialize()
{
	InitializeResource();

	return S_OK;
}

HRESULT CController_UITool::InitializeResource()
{
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
	// 테스트 코드 
	m_vecPageInfo.resize(m_iPageNum);

	for (_int i = 0; i < m_iPageNum; ++i)
	{
		UPAGE* pNew = new UPAGE;
		pNew->strUIPage_Name = new _char[100];
		m_ArrPageName[i] = new _char[100];

		_char czTest[10] = "none";

		for (_int j = 0; j <= 9; ++j)
		{
			pNew->strUIPage_Name[j] = czTest[j];
			m_ArrPageName[i][j] = czTest[j];
		}

		m_vecPageInfo[i] = pNew;
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

HRESULT CController_UITool::InitializeComponent()
{
	/* FOR.Com_Shader */






	return S_OK;
}

void CController_UITool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (auto& iter : m_vecTextureInfo)
	{
		Safe_Release(iter->Texture);
		Safe_Delete_Array(iter->strTexturePath);
		Safe_Delete_Array(iter->strTextureTag);
		Safe_Delete(iter);
	}

	m_vecTextureInfo.clear();

	for (auto& iter : m_vecPageInfo)
	{
		Safe_Delete_Array(iter->strUIPage_Name);

		for (auto& iterPart : iter->vecPart)
		{
			Safe_Delete_Array(iterPart->strUIPart_Name);
			Safe_Delete(iterPart);
		}

		iter->vecPart.clear();

		for (auto& iterSocket : iter->vecSocket)
		{
			Safe_Delete_Array(iterSocket->strUISocket_Name);
			Safe_Delete(iterSocket);
		}

		iter->vecSocket.clear();
		Safe_Delete(iter);
	}

	for (_int i = 0; i < 100; ++i)
		Safe_Delete_Array(m_ArrPageName[i]);

	//Safe_Delete_Array(m_ArrPageName);

	m_vecTextureInfo.clear();

	
}
