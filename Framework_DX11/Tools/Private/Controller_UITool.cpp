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
	//UIPart_Render();
	m_pUIRender->Late_Update(0.f);


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

		// 사이즈
		ImGui::PushItemWidth(100.f);
		ImGui::Text("PageCenter");
		ImGui::SameLine();
		ImGui::InputFloat("x", &m_vecPageInfo[m_iNowSelectNum]->fPosition.x);
		ImGui::SameLine();
		ImGui::InputFloat("y", &m_vecPageInfo[m_iNowSelectNum]->fPosition.y);

		ImGui::NewLine();
	}

	if (ImGui::CollapsingHeader("Part Setting"))
		UIPart_Edit();
}

void CController_UITool::UIPart_Edit()
{
	ImGui::Text("PartCount : ");
	ImGui::SameLine();

	_int iCount = (_int)m_vecPageInfo[m_iNowSelectNum]->vecPart.size();
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

	ImGui::InputInt("SelectPart", &m_vecPageInfo[m_iNowSelectNum]->iNowSelectPart);

	if ((m_vecPageInfo[m_iNowSelectNum]->iNowSelectPart >= 0) && ((m_vecPageInfo[m_iNowSelectNum]->iNowSelectPart < m_vecPageInfo[m_iNowSelectNum]->vecPart.size())))
	{
		UPART* pNow = m_vecPageInfo[m_iNowSelectNum]->vecPart[m_vecPageInfo[m_iNowSelectNum]->iNowSelectPart];

		ImGui::PushItemWidth(100.f);
		// 부모 파트 정보

		ImGui::SeparatorText("SetInfo");

		ImGui::Text("ParentIndex");
		ImGui::SameLine();
		ImGui::InputInt("Parent", &pNow->iParentPart_Index);
		ImGui::SameLine();
		if (pNow->iParentPart_Index == -1)
			ImGui::Text("Page(root Part)");
		else if ((pNow->iParentPart_Index < 0) || (pNow->iParentPart_Index >= (_int)m_vecPageInfo[m_iNowSelectNum]->vecPart.size()))
			ImGui::Text("※ WrongNum");
		else
			ImGui::Text(m_vecPageInfo[m_iNowSelectNum]->vecPart[pNow->iParentPart_Index]->strUIPart_Name);

		// 사이즈
		ImGui::Text("Size");
		ImGui::SameLine();
		ImGui::InputFloat("SizeX", &pNow->fSize.x);
		ImGui::SameLine();
		ImGui::InputFloat("SizeY", &pNow->fSize.y);

		ImGui::SeparatorText("Texture");
		// 텍스쳐
		ImGui::Text("Texture");
		ImGui::SameLine();
		ImGui::InputInt("Tex: ", &pNow->iTexture_Index);
		ImGui::SameLine();
		if (pNow->iTexture_Index == -1)
			ImGui::Text("none(NoTexture)");
		else if ((pNow->iTexture_Index < 0) || (pNow->iTexture_Index >= m_pUIRender->GetTextureCount()))
			ImGui::Text("※ WrongNum");
		else
			ImGui::Text(m_pUIRender->GetTextureTag(pNow->iTexture_Index));

		ImGui::SeparatorText("Text");
		// 텍스트 
		ImGui::Text("TextFont");
		ImGui::SameLine();
		ImGui::InputInt("Font: ", &pNow->iFontIndex);
		ImGui::SameLine();
		if (pNow->iFontIndex == _int(CUIRender::UI_FONT::FONT_END))
			ImGui::Text("none(NoText)");
		else if ((pNow->iFontIndex < 0) || (pNow->iFontIndex >= (_int)CUIRender::UI_FONT::FONT_END))
			ImGui::Text("※ WrongNum");
		else
			ImGui::Text(m_pUIRender->GetTextFontTag(pNow->iFontIndex));
		ImGui::InputText("Text", m_InputText, sizeof(m_InputText));
		if (ImGui::Button("InputText"))
		{
			_int iIndex = -1;

			pNow->szText = new _tchar[100];
			do
			{
				++iIndex;
				pNow->szText[iIndex] = m_InputText[iIndex];
			} while (m_InputText[iIndex] != '\0');
		}

		ImGui::SeparatorText("Position");
		// 위치
		ImGui::Text("Type");
		ImGui::SameLine();
		ImGui::Combo("MoveType", &pNow->iMoveType, m_szPositionType, 3);

		if (pNow->iMoveType == _int(MOVETYPE::TYPE_STATIC))
		{
			ImGui::Text("Adjust");
			ImGui::SameLine();
			ImGui::InputFloat("Adj.x", &pNow->fAdjust.x);
			ImGui::SameLine();
			ImGui::InputFloat("Adj.y", &pNow->fAdjust.y);
		}
		else
		{
			ImGui::Text("Adjust_Start");
			ImGui::SameLine();
			ImGui::InputFloat("Adj_S.x", &pNow->fAdjust_Start.x);
			ImGui::SameLine();
			ImGui::InputFloat("Adj_S.y", &pNow->fAdjust_Start.y);

			ImGui::Text("Adjust_End");
			ImGui::SameLine();
			ImGui::InputFloat("Adj_E.x", &pNow->fAdjust_End.x);
			ImGui::SameLine();
			ImGui::InputFloat("Adj_E.y", &pNow->fAdjust_End.y);

			if (pNow->iMoveType == _int(MOVETYPE::TYPE_BAR))
				ImGui::Combo("Direc", &pNow->bBarDirecX, m_szBarDirec, 2);

			if (ImGui::Button("InputAdjust"))
			{
				pNow->MakeDirec();
			}

			ImGui::Text("SetPosition");
			ImGui::SameLine();
			ImGui::SliderFloat("Slider", &pNow->fRatio, 0.f, 1.f);
		}

		
	}




	for (auto& iter : m_vecPageInfo[m_iNowSelectNum]->vecPart)
	{
		iter->MakeDirec();
		
		if (iter->iParentPart_Index == -1)
			iter->MovePart(m_vecPageInfo[m_iNowSelectNum]->fPosition);
		else 
			iter->MovePart(m_vecPageInfo[m_iNowSelectNum]->vecPart[iter->iParentPart_Index]->fPosition);
	}

	Safe_Delete_Array(tTemp);
	Safe_Delete_Array(szCount);
	

	ImGui::NewLine();

}

void CController_UITool::AddNewPage()
{
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

HRESULT CController_UITool::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	m_pUIRender = CUIRender::Create(pDevice, pContext);
	//m_pUIRender->SetUITool(this);
	//Safe_AddRef(m_pUIRender);

	InitializeResource();

	return S_OK;
}

HRESULT CController_UITool::InitializeResource()
{
	

	if (FAILED(LoadPart()))
		return E_FAIL;

	return S_OK;
}

HRESULT CController_UITool::SavePart()
{
	/*string fileName = pPath;
	WCHAR* TempName = new WCHAR[fileName.size()];
	for (_int i = 0; i <= fileName.size(); ++i)
		TempName[i] = fileName[i];

	HANDLE hFile = CreateFile(TempName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD dwByte(0);


	WriteFile(hFile, &iSize, sizeof(_int), &dwByte, nullptr);

	CloseHandle(hFile);*/


	return S_OK;
}

HRESULT CController_UITool::LoadPart()
{
	//string fileName = "../Bin/Resources/Textures/UI/UIPage.dat";
	//WCHAR* TempName = new WCHAR[fileName.size() + 1];
	//memset(TempName, 0, sizeof(TempName));
	//for (_int i = 0; i <= fileName.size(); ++i)
	//	TempName[i] = fileName[i];

	//HANDLE hFile = CreateFile(TempName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//if (INVALID_HANDLE_VALUE == hFile)
	//	return;
	//DWORD dwByte(0); // 예외 처리 변수


	//ReadFile(hFile, &tTemp.eCon_Type, sizeof(CONTAINER), &dwByte, nullptr);




	//CloseHandle(hFile);








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


HRESULT CController_UITool::InitializeComponent()
{
	/* FOR.Com_Shader */






	return S_OK;
}

void CController_UITool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);



	for (auto& iter : m_vecPageInfo)
	{
		Safe_Delete_Array(iter->strUIPage_Name);

		for (auto& iterPart : iter->vecPart)
		{
			Safe_Delete_Array(iterPart->strUIPart_Name);
			Safe_Delete(iterPart);
		}

		iter->vecPart.clear();

		Safe_Delete(iter);
	}

	for (_int i = 0; i < 100; ++i)
		Safe_Delete_Array(m_ArrPageName[i]);


	Safe_Release(m_pUIRender);
	
}
