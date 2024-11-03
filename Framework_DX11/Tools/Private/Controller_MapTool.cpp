#include "stdafx.h"

//#include "ImGuizmo.h"
#include <io.h>
#include<fstream>

#include "Controller_MapTool.h"
#include "GameInstance.h"

#include "NonAnimModel.h"

IMPLEMENT_SINGLETON(CController_MapTool)

_float3 CController_MapTool::m_vRotation = {};

CController_MapTool::CController_MapTool()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CController_MapTool::Initialize()
{
	return S_OK;
}

HRESULT CController_MapTool::Control_Player()
{ 
	
	if (ImGui::Button("Create Player"))
	{
		//Player 생성 (한번만)
		CGameObject* pPlayer = m_pGameInstance->Find_Player(LEVEL_TOOL);
		if (pPlayer == nullptr)
		{
			CNonAnimModel::NONMODEL_DESC Desc{};
			Desc.vPosition = {0.f,0.f,0.f};
			Desc.vScale = { 1.f,1.f,1.f };
			Desc.vRotation = { 0.f,0.f,0.f };
			Desc.iRenderGroupID = 0;
			strcpy_s(Desc.szModelTag, "Prototype_Model_Test");
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Player"), TEXT("Prototype_GameObject_NonAnim"), &Desc)))
				return E_FAIL;
		}
	}

	return S_OK;
}

void CController_MapTool::Create_Map()
{
	ImGui::SeparatorText("Select Model");

	string strLayerName;
	_wstring wstrLayerName;

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("MapToolBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Map"))
		{
			if (ImGui::BeginTabBar("SelectModelType", tab_bar_flags))
			{
				if (ImGui::BeginTabItem("Monastery"))
				{
					Select_Map_Model();
					strLayerName = "Layer_Map";
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Monster"))
				{
					strLayerName = "Layer_Monster";
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Object"))
				{
					strLayerName = "Layer_InteractObject";
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			//렌더타겟 아이디 설정 가능
			static int i0 = 0;
			ImGui::InputInt("RenderTarget ID", &i0);

			//오브젝트 생성
			if (ImGui::Button("Create Model") || m_pGameInstance->Get_KeyState(C) == AWAY)
			{
				wstrLayerName.assign(strLayerName.begin(), strLayerName.end());

				CNonAnimModel::NONMODEL_DESC Desc{};
				Desc.vPosition = { 0.f,0.f,0.f };
				Desc.vScale = { 1.f,1.f,1.f };
				Desc.vRotation = { 0.f,0.f,0.f };
				Desc.iRenderGroupID = i0;
				strcpy_s(Desc.szModelTag, m_FileNames[m_iListSelectNum]);
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, wstrLayerName, TEXT("Prototype_GameObject_NonAnim"), &Desc)))
					return;

			}ImGui::SameLine();
			ImGui::Text("or Press \"C\" to Create");

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Nav"))
		{

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void CController_MapTool::Select_Obj()
{
	//임시로 무조건 플레이어가 선택되게
	if (m_pGameInstance->Find_Player(LEVEL_TOOL) != nullptr)
		m_pSelectObject = m_pGameInstance->Find_Player(LEVEL_TOOL);

	if (m_pSelectObject == nullptr)
	{
		//피킹
	}
	else
	{
		//피킹 해제만 가능하게
	}
}

void CController_MapTool::EditTransform()
{
	if (!ImGui::CollapsingHeader("Object Inform"))
		return;

	ImGui::SeparatorText("Object Transform");

	static float fScale[3] = { 1.f,1.f,1.f };
	static float fRot[3] = { 0.f,0.f,0.f };
	static float fPos[3] = { 0.f,0.f,0.f };

	ImGui::PushItemWidth(300);

	static int iSelectObj_RenderTargetID = 0;

	if (m_pPreSelectObject != m_pSelectObject)	//새로 피킹할 경우
	{
		fScale[0] = m_pSelectObject->Get_Transform()->Get_Scaled().x;
		fScale[1] = m_pSelectObject->Get_Transform()->Get_Scaled().y;
		fScale[2] = m_pSelectObject->Get_Transform()->Get_Scaled().z;

		fRot[0] = m_pSelectObject->Get_Transform()->Get_CurrentRotation().x;
		fRot[1] = m_pSelectObject->Get_Transform()->Get_CurrentRotation().y;
		fRot[2] = m_pSelectObject->Get_Transform()->Get_CurrentRotation().z;

		_Vec3 vPos = m_pSelectObject->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		fPos[0] = vPos.x;
		fPos[1] = vPos.y;
		fPos[2] = vPos.z;

		CNonAnimModel* pSelect = dynamic_cast<CNonAnimModel*>(m_pSelectObject);
		if(pSelect!= nullptr)
			iSelectObj_RenderTargetID = pSelect->Get_RenderTargetId();

		m_pPreSelectObject = m_pSelectObject;
	}
	else if(m_pSelectObject!= nullptr)	//변경한 값 적용
	{
		_float3 vPosition = {};
		vPosition.x = fPos[0];
		vPosition.y = fPos[1];
		vPosition.z = fPos[2];

		m_pSelectObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPosition);

		m_pSelectObject->Get_Transform()->Set_Scaled(fScale[0], fScale[1], fScale[2]);

		m_pSelectObject->Get_Transform()->Rotation(fRot[0], fRot[1], fRot[2]);

		CNonAnimModel* pSelect = dynamic_cast<CNonAnimModel*>(m_pSelectObject);
		if (pSelect != nullptr)
			pSelect->Set_RenderTargetId(iSelectObj_RenderTargetID);
	}

	ImGui::DragFloat3("Scale(X, Y, Z)", &fScale[0], 0.05f, 0.1f, 100.f);
	ImGui::DragFloat3("Rotation(X, Y, Z)", &fRot[0], 0.05f, -180.f, 180.f, 0);
	ImGui::DragFloat3("Position(X, Y, Z)", &fPos[0], 0.05f, -180.f, 180.f, 0);

	ImGui::InputInt("RenderTarget ID", &iSelectObj_RenderTargetID);

	ImGui::Text("");
	ImGui::PopItemWidth();
}

void CController_MapTool::ShowPickPos()
{
	if (!ImGui::CollapsingHeader("Picking Inform"))
		return;
}

void CController_MapTool::Save_Load()
{
	ImGui::Text("");
	ImGui::SeparatorText("Save / Load");

	if (ImGui::Button("Save Map"))
	{
		SaveMap();
	}
	ImGui::SameLine();
	ImGui::Text("                           ");
	ImGui::SameLine();
	if (ImGui::Button("Load Map"))
	{
		LoadMap();
	}

	ImGui::Text("");

	if (ImGui::Button("Save Nav"))
	{
		//SaveMap();
	}
	ImGui::SameLine();
	ImGui::Text("                           ");
	ImGui::SameLine();
	if (ImGui::Button("Load Nav"))
	{
		//LoadMap();
	}
}

void CController_MapTool::Select_Map_Model()
{

	static int item_selected_idx = 0; // Here we store our selected data as an index.
	static bool item_highlight = false;
	int item_highlighted_idx = -1; // Here we store our highlighted data as an index.

	//폴더 선택, 버튼 누를 때 한번만 실행
	enum Folder
	{
		Folder_Interior,
		Folder_Line,
		Folder_Structure,
		Folder_Temp
	};
	static int iFolder = 0;

	if (ImGui::RadioButton("Interior", iFolder == Folder_Interior))
	{
		iFolder = Folder_Interior;
		item_selected_idx = 0;
		Show_List(iFolder);
	} ImGui::SameLine();

	if (ImGui::RadioButton("Line", iFolder == Folder_Line))
	{
		iFolder = Folder_Line;
		item_selected_idx = 0;
		Show_List(iFolder);
	} ImGui::SameLine();

	if (ImGui::RadioButton("Structure", iFolder == Folder_Structure))
	{
		iFolder = Folder_Structure;
		item_selected_idx = 0;
		Show_List(iFolder);
	} ImGui::SameLine();

	if (ImGui::RadioButton("Temp", iFolder == Folder_Temp))
	{
		iFolder = Folder_Temp;
		item_selected_idx = 0;
		Show_List(iFolder);
	}

	////////////////////////

	if (ImGui::BeginListBox("Models"))
	{
		for (int n = 0; n < m_iListCount; n++)
		{
			const bool is_selected = (item_selected_idx == n);
			if (ImGui::Selectable(m_FileNames[n], is_selected))
				item_selected_idx = n;

			if (item_highlight && ImGui::IsItemHovered())
				item_highlighted_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();

			m_iListSelectNum = item_selected_idx;
		}
		ImGui::EndListBox();
	}
}

void CController_MapTool::Show_List(_uint iFolder)
{
	//내용물 초기화 (capacity는 그냥 냅둠)
	m_FileNames.erase(m_FileNames.begin(), m_FileNames.end());

	char szFolderFolderFullPath[128] = "../Bin/ModelData/NonAnim/Map/";    // 상대 경로
	char szFolderFolderPathReset[128] = "../Bin/ModelData/NonAnim/Map/";
	char szFolderFolderPath[128] = "../Bin/ModelData/NonAnim/Map/";
	char szFolderHandlePath[128] = "";
	char szFolderName[128] = "";
	char szDat[128] = "*.dat";

	switch (iFolder)
	{
	case 0:
		strcpy_s(szFolderName, "Interior/");
		break;
	case 1:
		strcpy_s(szFolderName, "Line/");
		break;
	case 2:
		strcpy_s(szFolderName, "Structure/");
		break;
	case 3:
		strcpy_s(szFolderName, "Temp/");
		break;
	}

	strcat_s(szFolderFolderFullPath, szFolderName);
	strcat_s(szFolderFolderPathReset, szFolderName);

	//dat파일 구분용 
	strcpy_s(szFolderHandlePath, szFolderFolderFullPath);
	strcat_s(szFolderHandlePath, szDat); //경로에서 .dat만 검색

	_finddata_t fd;
	intptr_t handle = _findfirst(szFolderHandlePath, &fd);

	if (handle == -1)
		return;

	
	int iResult = 0;
	m_iListCount = 0;
	while (iResult != -1)
	{
		m_iListCount++;

		strcpy_s(szFolderFolderPath, szFolderFolderPathReset);
		strcat_s(szFolderFolderPath , fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFolderFolderPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		//_strup : 문자열 내용을 복사해 그 주소를 저장-> 주소에 따라 문자열이 바뀌는걸 막아 모두 동일해지는걸 막음
		m_FileNames.push_back(_strdup(szFileName));

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

}

void CController_MapTool::SaveMap()
{
	const char cFile[128] = "../Bin/DataFiles/Map_Data.dat";
	ofstream fout(cFile, ios::out | ios::binary);

	//	fout.open();
	if (!fout.is_open())    // 파일 열었다면
	{
		MSG_BOX(TEXT("파일 쓰기를 실패"));
		return;
	}

	//string strUint = {};

	////전체 레이어 수 저장
	//_uint iLayerCount = m_pGameInstance->Get_Level_ObjectLayer_Count(LEVEL_GAMEPLAY);
	////fout << iLayerCount - 1 << "\n"; //카메라 레이어 제외
	////strUint = to_string(iLayerCount);
	////fout.write(strUint.c_str(), sizeof(strUint));
	////fout.write(strUint.c_str(), sizeof(strUint));
	//iLayerCount -= 2;
	//fout.write(reinterpret_cast<const char*>(&iLayerCount), sizeof(_uint));

	//_wstring sLayerTag = {};
	//string strLayerTag = {};
	//std::string::size_type strSize = {};
	//_uint ObjectCount = 0;
	//CGameObject* pGameObject = { nullptr };
	//_float4x4 ObjectWorldMatrix = {};
	//string strFloat = {};
	//string strInt = {};
	//_float3 vPos = {};

	//for (_uint i = 0; i < iLayerCount + 2; ++i)
	//{
	//	sLayerTag = m_pGameInstance->Get_LayerMap(LEVEL_GAMEPLAY, i);

	//	if (sLayerTag == TEXT("Layer_Camera") || sLayerTag == TEXT("Layer_Link"))
	//		continue;

	//	strLayerTag = std::string().assign(sLayerTag.begin(), sLayerTag.end());
	//	strSize = strLayerTag.size();
	//	fout.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize)); //문자열 사이즈 저장
	//	fout.write(strLayerTag.c_str(), strSize);	//문자열 저장

	//	ObjectCount = m_pGameInstance->Get_LayerSize(LEVEL_GAMEPLAY, sLayerTag);
	//	fout.write(reinterpret_cast<const char*>(&ObjectCount), sizeof(_uint)); //오브젝트 개수 저장

	//	//if (sLayerTag == TEXT("Layer_BackGround"))
	//	//{
	//	//	fout.write(reinterpret_cast<const char*>(&m_iFloorSizeX), sizeof(_int));	// Terrain사이즈 저장
	//	//	fout.write(reinterpret_cast<const char*>(&m_iFloorSizeZ), sizeof(_int));
	//	//}

	//	for (_uint i = 0; i < ObjectCount; ++i)
	//	{
	//		pGameObject = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, sLayerTag, i);

	//		if (sLayerTag != TEXT("Layer_BackGround"))
	//		{
	//			int iObjectType = pGameObject->Get_ObjType();	//오브젝트 타입
	//			int iListIndex = pGameObject->Get_ListIndex();	//리스트 번호

	//			fout.write(reinterpret_cast<const char*>(&iObjectType), sizeof(_int));
	//			fout.write(reinterpret_cast<const char*>(&iListIndex), sizeof(_int));
	//		}

	//		//위치 저장
	//		XMStoreFloat3(&vPos, pGameObject->Get_Transform()->Get_State(CTransform::STATE_POSITION));
	//		fout.write(reinterpret_cast<const char*>(&vPos.x), sizeof(_float));
	//		fout.write(reinterpret_cast<const char*>(&vPos.y), sizeof(_float));
	//		fout.write(reinterpret_cast<const char*>(&vPos.z), sizeof(_float));

	//		//크기 저장
	//		_float3 vScale = pGameObject->Get_Transform()->Get_Scaled();
	//		fout.write(reinterpret_cast<const char*>(&vScale.x), sizeof(_float));
	//		fout.write(reinterpret_cast<const char*>(&vScale.y), sizeof(_float));
	//		fout.write(reinterpret_cast<const char*>(&vScale.z), sizeof(_float));

	//		//회전 저장
	//		_float3 vRot = pGameObject->Get_Rotation();
	//		fout.write(reinterpret_cast<const char*>(&vRot.x), sizeof(_float));
	//		fout.write(reinterpret_cast<const char*>(&vRot.y), sizeof(_float));
	//		fout.write(reinterpret_cast<const char*>(&vRot.z), sizeof(_float));

	//		//Cell 번호 저장
	//		_int iCellnum = m_pNavigationController->Get_WhereCell(vPos);
	//		fout.write(reinterpret_cast<const char*>(&iCellnum), sizeof(_int));

	//		//Cell 방 번호 저장
	//		if (iCellnum != -1)
	//		{
	//			_uint iCellRoomNum = m_pNavigationController->Get_RoomNum(iCellnum);
	//			fout.write(reinterpret_cast<const char*>(&iCellRoomNum), sizeof(_uint));
	//		}
	//		else
	//		{
	//			_uint iCellRoomNum = 0;
	//			fout.write(reinterpret_cast<const char*>(&iCellRoomNum), sizeof(_uint));
	//		}
	//	}
	//}

	fout.close();
	MSG_BOX(TEXT("파일 쓰기를 성공"));
}

void CController_MapTool::LoadMap()
{
	const char cFile[128] = "../Bin/DataFiles/Map_Data.dat";
	ifstream fin(cFile, ios::in | ios::binary);

	//	fin.open("../Bin/Map_Data.txt");
	if (!fin.is_open())    // 파일 열었다면
	{
		MSG_BOX(TEXT("파일 읽기를 실패했어요.."));
		return;
	}

	//string line;
	//int LayerCout = { 0 };
	////getline(fin, line);
	////LayerCout = std::stoi(line);
	//fin.read(reinterpret_cast<char*>(&LayerCout), sizeof(LayerCout));

	//string strLayerTag = {};
	//std::string::size_type iStrSize = { 0 };
	//_uint iObjectCount = 0;
	//int i = 0;
	//int iObjectType = 0;
	//int iObjectListIndex = 0;
	//_float3 fPos = {};
	//_float3 fScaled = {};
	//_float3 fRot = {};
	//_int iCellNum = {};
	//_uint iCellRoomNum = {};

	//while (i < LayerCout)
	//{
	//	//getline(fin, strLayerTag);
	//	//fin >> iObjectCount;
	//	fin.read(reinterpret_cast<char*>(&iStrSize), sizeof(iStrSize));
	//	strLayerTag.resize(iStrSize);
	//	fin.read(&strLayerTag[0], iStrSize);
	//	fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(iObjectCount));

	//	for (int j = 0; j < iObjectCount; ++j)
	//	{
	//		fin.read(reinterpret_cast<char*>(&iObjectType), sizeof(iObjectType));
	//		fin.read(reinterpret_cast<char*>(&iObjectListIndex), sizeof(iObjectListIndex));

	//		fin.read(reinterpret_cast<char*>(&fPos.x), sizeof(_float));
	//		fin.read(reinterpret_cast<char*>(&fPos.y), sizeof(_float));
	//		fin.read(reinterpret_cast<char*>(&fPos.z), sizeof(_float));

	//		fin.read(reinterpret_cast<char*>(&fScaled.x), sizeof(_float));
	//		fin.read(reinterpret_cast<char*>(&fScaled.y), sizeof(_float));
	//		fin.read(reinterpret_cast<char*>(&fScaled.z), sizeof(_float));


	//		fin.read(reinterpret_cast<char*>(&fRot.x), sizeof(_float));
	//		fin.read(reinterpret_cast<char*>(&fRot.y), sizeof(_float));
	//		fin.read(reinterpret_cast<char*>(&fRot.z), sizeof(_float));

	//		fin.read(reinterpret_cast<char*>(&iCellNum), sizeof(_int));
	//		fin.read(reinterpret_cast<char*>(&iCellRoomNum), sizeof(_uint));

	//		if (strLayerTag == "Layer_Land")
	//			Read_LandObjects(iObjectType, iObjectListIndex, fPos, fScaled, fRot);
	//		else
	//			Read_NonAnim(iObjectType, iObjectListIndex, fPos, fScaled, fRot);

	//	}

	//	++i;
	//}

	fin.close();
	MSG_BOX(TEXT("파일 읽기를 성공했습니다.."));
}

void CController_MapTool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
