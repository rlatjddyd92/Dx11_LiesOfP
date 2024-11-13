#include "stdafx.h"
#include "Controller_UITool.h"
#include "GameInstance.h"
#include <fstream>
#include <sstream>

IMPLEMENT_SINGLETON(CController_UITool)

void CController_UITool::UITool_Edit()
{
	if (ImGui::Button("SaveUIData"))
	{
		if (FAILED(SavePage()))
		{
			MSG_BOX(TEXT("Failed to Save Page"));
		}
		else if (FAILED(SavePart()))
		{
			MSG_BOX(TEXT("Failed to Save Part"));
		}
		else 
			MSG_BOX(TEXT("UI 파일 저장 완료"));
	}

	ImGui::SameLine();

	if (ImGui::Button("LoadUIData"))
	{
		if (MessageBox(nullptr, L"UI 파일을 불러와 현재 내용에 덮어씁니다.", L"UI 데이터 불러오기", MB_OKCANCEL))
		{
			EraseUIData();

			// 불러오기

			if (FAILED(LoadPage()))
			{
				MSG_BOX(TEXT("Failed to Load Page"));
			}
			else if (FAILED(LoadPart()))
			{
				MSG_BOX(TEXT("Failed to Load Part"));
			}
			else
				MSG_BOX(TEXT("UI 파일 불러오기 완료"));

		}

		// 취소
	}

	ImGui::SameLine();

	if (ImGui::Button("MakeClientDataFile"))
	{
		// 클라이언트 쪽으로 dat 파일 만들어 내보내기

		if (FAILED(MakeClientData_Page()))
			MSG_BOX(TEXT("UI 데이터 생성 실패"));
		else 
			MSG_BOX(TEXT("Client 프로젝트에 UI 데이터 생성 완료"));
	}

	UIPage_Edit();
}

HRESULT CController_UITool::UITool_Render()
{
	//FontTest();
	//UIPart_Render();
	m_pUIRender->Late_Update(0.f);


	return S_OK;
}

void CController_UITool::Show_StaticSystemMessage(_wstring Text)
{
}

void CController_UITool::Off_StaticSystemMessage()
{
}

void CController_UITool::Show_SystemMessage(_wstring Text, _float fTime)
{
}

void CController_UITool::Show_MouseInfo(_wstring DataNameA, _float* DataA, _wstring DataNameB, _float* DataB, _wstring DataNameC, _float* DataC, _wstring DataNameD, _float* DataD)
{
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

		ImGui::Text("Background");
		ImGui::SameLine();
		ImGui::InputInt("Back", &m_iBackground);
		ImGui::SameLine();
		if (m_iBackground == -1)
			ImGui::Text("none(NoTexture)");
		else if ((m_iBackground < 0) || (m_iBackground >= m_pUIRender->GetTextureCount()))
			ImGui::Text("※ WrongNum");
		else
			ImGui::Text(m_pUIRender->GetTextureTag(m_iBackground));
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
		szCount[i] = (_char)tTemp[i];

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

		ImGui::Text("GroupIndex");
		ImGui::SameLine();
		ImGui::InputInt("Group", &pNow->iGroupIndex);

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

		ImGui::DragFloat("R", &pNow->fTextureColor.x);
		ImGui::SameLine();
		ImGui::DragFloat("G", &pNow->fTextureColor.y);
		ImGui::SameLine();
		ImGui::DragFloat("B", &pNow->fTextureColor.z);
		ImGui::SameLine();
		ImGui::DragFloat("A", &pNow->fTextureColor.w);



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
			ImGui::DragFloat("Adj_E.x", &pNow->fAdjust_End.x);
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
	

	if (FAILED(LoadPage()))
		return E_FAIL;

	if (FAILED(LoadPart()))
		return E_FAIL;

	return S_OK;
}

HRESULT CController_UITool::SavePage()
{
	vector<vector<_wstring>> vecBuffer;

	vecBuffer.resize(_int(UIPAGE::PAGE_END) + 1);

	vecBuffer[0].resize(m_DataTag_Page.size());
	vecBuffer[0] = m_DataTag_Page;

	for (_int i = 1; i < vecBuffer.size(); ++i)
	{
		vecBuffer[i].resize(m_DataTag_Page.size());
		vecBuffer[i][0] = to_wstring(i - 1);
		_int iIndex = -1;
		do
		{
			++iIndex;
			vecBuffer[i][1] += (_tchar)(m_vecPageInfo[i - 1]->strUIPage_Name)[iIndex];
		} while ((_tchar)(m_vecPageInfo[i - 1]->strUIPage_Name)[iIndex] != '\0');
		
		vecBuffer[i][2] = to_wstring(m_vecPageInfo[i - 1]->fPosition.x);
		vecBuffer[i][3] = to_wstring(m_vecPageInfo[i - 1]->fPosition.y);
	}

	if (FAILED(m_pGameInstance->SaveDataByFile("../Bin/DataFiles/UIPageSpec.csv", vecBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CController_UITool::LoadPage()
{
	vector<vector<_wstring>> vecBuffer;
	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/UIPageSpec.csv", &vecBuffer)))
		return E_FAIL;

	m_DataTag_Page.resize(vecBuffer[0].size());
	m_DataTag_Page = vecBuffer[0];

	for (_int i = 1; i < vecBuffer.size(); ++i)
	{
		UPAGE* pNew = new UPAGE;

		pNew->strUIPage_Name = new _char[vecBuffer[i][1].size() + 1];
		m_ArrPageName[i - 1] = new _char[vecBuffer[i][1].size() + 1];
		for (_int j = 0; j <= vecBuffer[i][1].size();++j)
		{
			pNew->strUIPage_Name[j] = (_char)vecBuffer[i][1][j];
			m_ArrPageName[i - 1][j] = (_char)vecBuffer[i][1][j];
		}
			

		pNew->fPosition.x = stof(vecBuffer[i][2]);
		pNew->fPosition.y = stof(vecBuffer[i][3]);

		m_vecPageInfo.push_back(pNew);
	}

	return S_OK;
}

HRESULT CController_UITool::SavePart()
{
	vector<vector<_wstring>> vecBuffer;

	vecBuffer.push_back(m_DataTag_Part);

	for (_int i = 0; i < _int(UIPAGE::PAGE_END); ++i)
	{
		for (_int j = 0; j < m_vecPageInfo[i]->vecPart.size(); ++j)
		{
			vector<_wstring> vecPart;

			UPART* pNow = m_vecPageInfo[i]->vecPart[j];

			vecPart.push_back(to_wstring(i));
			vecPart.push_back(to_wstring(j));

			_int iIndex = -1;
			_wstring strName;
			do
			{
				++iIndex;
				strName += (_tchar)(pNow->strUIPart_Name)[iIndex];
			} while ((_tchar)(pNow->strUIPart_Name)[iIndex] != '\0');

			vecPart.push_back(strName);
			vecPart.push_back(to_wstring(pNow->iParentPart_Index));
			vecPart.push_back(to_wstring(pNow->fSize.x));
			vecPart.push_back(to_wstring(pNow->fSize.y));
			vecPart.push_back(to_wstring(pNow->fPosition.x));
			vecPart.push_back(to_wstring(pNow->fPosition.y));
			vecPart.push_back(to_wstring(pNow->iGroupIndex));
			vecPart.push_back(to_wstring(pNow->fAdjust.x));
			vecPart.push_back(to_wstring(pNow->fAdjust.y));
			vecPart.push_back(to_wstring(pNow->iMoveType));
			vecPart.push_back(to_wstring(pNow->fAdjust_Start.x));
			vecPart.push_back(to_wstring(pNow->fAdjust_Start.y));
			vecPart.push_back(to_wstring(pNow->fAdjust_End.x));
			vecPart.push_back(to_wstring(pNow->fAdjust_End.y));
			vecPart.push_back(to_wstring(pNow->fDirec.x));
			vecPart.push_back(to_wstring(pNow->fDirec.y));
			vecPart.push_back(to_wstring(pNow->fRatio));
			vecPart.push_back(to_wstring(pNow->bBarDirecX));
			vecPart.push_back(to_wstring(pNow->iTexture_Index));
			vecPart.push_back(to_wstring(pNow->fTextureColor.x));
			vecPart.push_back(to_wstring(pNow->fTextureColor.y));
			vecPart.push_back(to_wstring(pNow->fTextureColor.z));
			vecPart.push_back(to_wstring(pNow->fTextureColor.w));
			vecPart.push_back(to_wstring(pNow->iFontIndex));

			iIndex = -1;
			_wstring strText;

			if (pNow->szText == nullptr)
			{
				strText = TEXT("\0");
			}
			else
			{
				do
				{
					++iIndex;
					strText += (pNow->szText)[iIndex];
				} while ((pNow->szText)[iIndex] != '\0');
			}

			vecPart.push_back(strText);

			vecPart.push_back(to_wstring(pNow->bCenter));
			vecPart.push_back(to_wstring(pNow->fTextColor.x));
			vecPart.push_back(to_wstring(pNow->fTextColor.y));
			vecPart.push_back(to_wstring(pNow->fTextColor.z));
			vecPart.push_back(to_wstring(pNow->fTextColor.w));

			vecBuffer.push_back(vecPart);
		}
	}

	if (FAILED(m_pGameInstance->SaveDataByFile("../Bin/DataFiles/UIPartSpec.csv", vecBuffer)))
		return E_FAIL;


	return S_OK;
}

HRESULT CController_UITool::LoadPart()
{
	vector<vector<_wstring>> vecBuffer;
	if (FAILED(m_pGameInstance->LoadDataByFile("../Bin/DataFiles/UIPartSpec.csv", &vecBuffer)))
		return E_FAIL;

	m_DataTag_Part.resize(vecBuffer[0].size());
	m_DataTag_Part = vecBuffer[0];

	for (_int i = 1; i < vecBuffer.size(); ++i)
	{
		UPART* pNew = new UPART;

		pNew->strUIPart_Name = new _char[vecBuffer[i][2].size() + 1];
		for (_int j = 0; j <= vecBuffer[i][2].size();++j)
			pNew->strUIPart_Name[j] = (_char)vecBuffer[i][2][j];

		pNew->iParentPart_Index = stoi(vecBuffer[i][3]);
		pNew->fSize = { stof(vecBuffer[i][4]), stof(vecBuffer[i][5]) };
		pNew->fPosition = { stof(vecBuffer[i][6]), stof(vecBuffer[i][7]) };
		pNew->iGroupIndex = stoi(vecBuffer[i][8]);
		pNew->fAdjust = { stof(vecBuffer[i][9]), stof(vecBuffer[i][10]) };
		pNew->iMoveType = stoi(vecBuffer[i][11]);
		pNew->fAdjust_Start = { stof(vecBuffer[i][12]), stof(vecBuffer[i][13]) };
		pNew->fAdjust_End = { stof(vecBuffer[i][14]), stof(vecBuffer[i][15]) };
		pNew->fDirec = { stof(vecBuffer[i][16]), stof(vecBuffer[i][17]) };
		pNew->fRatio = stof(vecBuffer[i][18]);
		pNew->bBarDirecX = stoi(vecBuffer[i][19]);
		pNew->iTexture_Index = stoi(vecBuffer[i][20]);
		pNew->fTextureColor = { stof(vecBuffer[i][21]) , stof(vecBuffer[i][22]) , stof(vecBuffer[i][23]) , stof(vecBuffer[i][24]) };
		pNew->iFontIndex = stoi(vecBuffer[i][25]);
		
		pNew->szText = new _tchar[vecBuffer[i][26].size() + 1];
		for (_int j = 0; j <= vecBuffer[i][26].size();++j)
			pNew->szText[j] = vecBuffer[i][26][j];

		pNew->bCenter = stoi(vecBuffer[i][27]);
		pNew->fTextColor = { stof(vecBuffer[i][28]) , stof(vecBuffer[i][29]) , stof(vecBuffer[i][30]) , stof(vecBuffer[i][31]) };

		pNew->MakeDirec();
		if (pNew->iParentPart_Index == -1)
			pNew->MovePart(m_vecPageInfo[stoi(vecBuffer[i][0])]->fPosition);
		else
			pNew->MovePart(m_vecPageInfo[stoi(vecBuffer[i][0])]->vecPart[pNew->iParentPart_Index]->fPosition);

		m_vecPageInfo[stoi(vecBuffer[i][0])]->vecPart.push_back(pNew);
	}

	return S_OK;
}


HRESULT CController_UITool::InitializeComponent()
{
	/* FOR.Com_Shader */






	return S_OK;
}

HRESULT CController_UITool::EraseUIData()
{
	for (auto& iter : m_vecPageInfo)
	{
		Safe_Delete_Array(iter->strUIPage_Name);

		for (auto& iterPart : iter->vecPart)
		{
			Safe_Delete_Array(iterPart->strUIPart_Name);
			Safe_Delete_Array(iterPart->szText);
			Safe_Delete(iterPart);
		}

		iter->vecPart.clear();

		Safe_Delete(iter);
	}

	for (_int i = 0; i < _int(UIPAGE::PAGE_END); ++i)
		Safe_Delete_Array(m_ArrPageName[i]);

	m_DataTag_Page.clear();
	m_DataTag_Part.clear();

	return S_OK;
}

HRESULT CController_UITool::MakeClientData_Page()
{
	wstring fileName = TEXT("../../Client/Bin/DataFiles/UIData.dat");
	WCHAR* TempName = new WCHAR[fileName.size() + 1];
	for (_int i = 0; i <= fileName.size(); ++i)
		TempName[i] = fileName[i];

	HANDLE hFile = CreateFile(TempName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte(0);

	// Page의 수 
	_int iPageCount = (_int)m_vecPageInfo.size();
	WriteFile(hFile, &iPageCount, sizeof(_int), &dwByte, nullptr);

	for (auto& iter : m_vecPageInfo)
	{
		_int iIndexPageName = -1;

		do
		{
			++iIndexPageName;
			WriteFile(hFile, &iter->strUIPage_Name[iIndexPageName], sizeof(_char), &dwByte, nullptr);
		} while (iter->strUIPage_Name[iIndexPageName] != '\0');

		WriteFile(hFile, &iter->fPosition, sizeof(_float2), &dwByte, nullptr);
		
		if (FAILED(MakeClientData_Part(hFile , &dwByte, &iter->vecPart)))
			MSG_BOX(TEXT("UI 데이터 생성 실패"));
	}

	CloseHandle(hFile);

	Safe_Delete_Array(TempName);

	return S_OK;
}

HRESULT CController_UITool::MakeClientData_Part(HANDLE handle, DWORD* dword, vector<UPART*>* pPart)
{
	_int iPartCount = (_int)pPart->size();
	WriteFile(handle, &iPartCount, sizeof(_int), dword, nullptr);

	for (auto& iter : (*pPart))
	{
		WriteFile(handle, &iter->bBarDirecX, sizeof(_int), dword, nullptr);
		WriteFile(handle, &iter->bCenter, sizeof(_bool), dword, nullptr);
		WriteFile(handle, &iter->fAdjust, sizeof(_float2), dword, nullptr);
		WriteFile(handle, &iter->fAdjust_End, sizeof(_float2), dword, nullptr);
		WriteFile(handle, &iter->fAdjust_Start, sizeof(_float2), dword, nullptr);
		WriteFile(handle, &iter->fDirec, sizeof(_float2), dword, nullptr);
		WriteFile(handle, &iter->fPosition, sizeof(_float2), dword, nullptr);
		WriteFile(handle, &iter->fRatio, sizeof(_float), dword, nullptr);
		WriteFile(handle, &iter->fSize, sizeof(_float2), dword, nullptr);
		WriteFile(handle, &iter->fTextColor, sizeof(_float4), dword, nullptr);
		WriteFile(handle, &iter->iFontIndex, sizeof(_int), dword, nullptr);
		WriteFile(handle, &iter->iGroupIndex, sizeof(_int), dword, nullptr);
		WriteFile(handle, &iter->iMoveType, sizeof(_int), dword, nullptr);
		WriteFile(handle, &iter->iParentPart_Index, sizeof(_int), dword, nullptr);
		WriteFile(handle, &iter->iTexture_Index, sizeof(_int), dword, nullptr);
		WriteFile(handle, &iter->fTextureColor, sizeof(_float4), dword, nullptr);

		_int iIndexPartName = -1;

		do
		{
			++iIndexPartName;
			WriteFile(handle, &iter->strUIPart_Name[iIndexPartName], sizeof(_char), dword, nullptr);
		} while (iter->strUIPart_Name[iIndexPartName] != '\0');

		_int iIndexText = -1;

		do
		{
			++iIndexText;
			WriteFile(handle, &iter->szText[iIndexText], sizeof(_tchar), dword, nullptr);
		} while (iter->szText[iIndexText] != '\0');
	}

	return S_OK;
}

void CController_UITool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	EraseUIData();

	Safe_Release(m_pUIRender);
	
}
