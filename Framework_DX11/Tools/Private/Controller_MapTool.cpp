#include "stdafx.h"

//#include "ImGuizmo.h"
#include <io.h>
#include<fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Controller_MapTool.h"
#include "GameInstance.h"
#include "NavigationController.h"

#include "NonAnimModel.h"
#include "Decal.h"

IMPLEMENT_SINGLETON(CController_MapTool)

_float3 CController_MapTool::m_vRotation = {};

CController_MapTool::CController_MapTool()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CController_MapTool::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pNavigationController = CNavigationController::Get_Instance();
	if (nullptr == m_pNavigationController)
		return E_FAIL;

	m_pNavigationController->Initialize(m_pDevice, m_pContext);

	//m_pGizmoDesc = new tagGizmoDesc;

	return S_OK;
}

HRESULT CController_MapTool::Control_Player()
{ 
	ImGui::Text("");
	if (ImGui::Button("Create Player"))
	{
		//Player 생성 (한번만)
		CGameObject* pPlayer = m_pGameInstance->Find_Player(LEVEL_TOOL);
		if (pPlayer == nullptr)
		{
			CNonAnimModel::NONMODEL_DESC Desc{};
			Desc.vPosition = m_vPickPos;
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

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("MapToolBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Map"))
		{
			Map_Menu();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Nav"))
		{
			Nav_Menu();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Light"))
		{
			Light_Menu();

			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem("Decal"))
		{
			Decal_Menu();

			ImGui::EndTabItem();
		}

		m_pNavigationController->Render();

		ImGui::EndTabBar();
	}
}

void CController_MapTool::Pick_Object()
{
	if (!ImGui::CollapsingHeader("Object Inform"))
		return;

	//오브젝트 피킹 작동
	if (m_pGameInstance->Get_KeyState(RBUTTON) == AWAY)
	{
		m_iPre_Picked_ID = m_iPickObject_ID;

		m_pGameInstance->Picking_Object(&m_iPickObject_ID);

		if (m_iPickObject_ID != 0)	//0은 허공을 선택한 것
		{
			if (m_pSelectObject != nullptr)	//처음 피킹한게 아닐때만
			{
				dynamic_cast<CNonAnimModel*>(m_pSelectObject)->Set_Selected(false);
				m_pPreSelectObject = m_pSelectObject;
			}

			if (m_iPre_Picked_ID != m_iPickObject_ID)	//새로 고른거다
			{
				Find_PickObject();

				CNonAnimModel* pSelect = dynamic_cast<CNonAnimModel*>(m_pSelectObject);
				if (pSelect != nullptr)
				{
					//선택한게 조명인 경우
					if (pSelect->Get_isLight())
					{
						_Vec3 vObjPos = pSelect->Get_Transform()->Get_State(CTransform::STATE_POSITION);
						_Vec4 vPos = { vObjPos.x,vObjPos.y,vObjPos.z, 1.f };
						
						m_iSelectedLightIndex = m_pGameInstance->Find_Light_Index(vPos);
					}
				}
			}
			else
			{
				//같은걸 다시 고르면 선택 해제
				m_pSelectObject = nullptr;
				m_iSelectedLightIndex = -1;
				m_iPickObject_ID = 0;
			}

		}
	}

	ImGui::SeparatorText("Object Transform");

	static _Vec3 vScale = { 1.f,1.f,1.f };
	static _Vec3 vRot = { 0.f,0.f,0.f };
	static _Vec3 vPos = { 0.f,0.f,0.f };

	ImGui::PushItemWidth(300);

	static int iSelectObj_RenderTargetID = 0;
	static bool bShadow = false;

	if (m_pPreSelectObject != m_pSelectObject && m_pSelectObject != nullptr)	//새로 피킹할 경우 Obj 정보를 띄우자
	{
		vScale = m_pSelectObject->Get_Transform()->Get_Scaled();
		vRot = m_pSelectObject->Get_Transform()->Get_CurrentRotation();
		vPos = (_Vec3)m_pSelectObject->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		CNonAnimModel* pSelect = dynamic_cast<CNonAnimModel*>(m_pSelectObject);
		if (pSelect != nullptr)
		{
			iSelectObj_RenderTargetID = pSelect->Get_RenderTargetId();
			bShadow = pSelect->Get_bShadow();
		}

		m_pPreSelectObject = m_pSelectObject;
	}
	else if (m_pSelectObject != nullptr)	//Imgui에서 변경한 값 적용
	{
		m_pSelectObject->Get_Transform()->Set_Scaled(vScale.x, vScale.y, vScale.z);

		m_pSelectObject->Get_Transform()->Rotation(vRot.x, vRot.y, vRot.z);

		m_pSelectObject->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPos);

		CNonAnimModel* pSelect = dynamic_cast<CNonAnimModel*>(m_pSelectObject);
		if (pSelect != nullptr)
		{
			pSelect->Set_RenderTargetId(iSelectObj_RenderTargetID);
			pSelect->Set_bShadow(bShadow);

			//선택한게 조명인 경우
			if (pSelect->Get_isLight())
			{
				if(m_iSelectedLightIndex != -1)
				{
					LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(m_iSelectedLightIndex);
					pLightDesc->vPosition = _Vec4(vPos.x, vPos.y, vPos.z, 1.f);
				}
			}
		}

	}

#pragma region ImGuiZimo
	enum GIZMO
	{
		POSITION, ROTATION, SCALE
	};
	static int iGizmoOperation = 0;

	if (ImGui::RadioButton("Pos", iGizmoOperation == POSITION))
	{
		iGizmoOperation = POSITION;
		m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	} ImGui::SameLine();

	if (ImGui::RadioButton("Rot", iGizmoOperation == ROTATION))
	{
		iGizmoOperation = ROTATION;
		m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::ROTATE;
	} ImGui::SameLine();

	if (ImGui::RadioButton("Scale", iGizmoOperation == SCALE))
	{
		iGizmoOperation = SCALE;
		m_tGizmoDesc.CurrentGizmoOperation = ImGuizmo::SCALE;
	}

	_Matrix ViewMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	_Matrix ProjMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);

	if (m_pSelectObject != nullptr)
	{
		_Matrix GizmoWorldMatrix = m_pSelectObject->Get_Transform()->Get_WorldMatrix();

		//ImGuiZimo

		// 행렬 요소 재조합
		ImGuizmo::RecomposeMatrixFromComponents(&vPos.x, &vRot.x, &vScale.x, (_float*)&GizmoWorldMatrix);

		if (ImGuizmo::Manipulate(&ViewMatrix._11, &ProjMatrix._11			// 뷰, 투영행렬
			, m_tGizmoDesc.CurrentGizmoOperation			// Tr, Rt, Sc
			, m_tGizmoDesc.CurrentGizmoMode				// WORLD, LOCAL
			, (_float*)&GizmoWorldMatrix					// 객체의 월드 행렬
			, NULL							// 그냥 NULL 고정하자
			, m_tGizmoDesc.bUseSnap ? &m_tGizmoDesc.snap[0] : NULL	// 위의 틱당 단위 설정들
			, m_tGizmoDesc.boundSizing ? m_tGizmoDesc.bounds : NULL
			, m_tGizmoDesc.boundSizingSnap ? m_tGizmoDesc.boundsSnap : NULL))
		{
			m_pSelectObject->Get_Transform()->Set_WorldMatrix(GizmoWorldMatrix);
		}

		// 행렬 요소 분해
		ImGuizmo::DecomposeMatrixToComponents((_float*)&GizmoWorldMatrix, &vPos.x, &vRot.x, &vScale.x);
	}
#pragma endregion

	ImGui::DragFloat3("Scale(X, Y, Z)", (_float*)&vScale, 0.05f, 0.1f, 100.f);
	ImGui::DragFloat3("Rotation(X, Y, Z)", (_float*)&vRot, 0.05f, -180.f, 180.f, 0);
	ImGui::DragFloat3("Position(X, Y, Z)", (_float*)&vPos, 0.05f, -5000.f, 5000.f, 0);

	ImGui::InputInt("RenderTarget ID", &iSelectObj_RenderTargetID);
	ImGui::Checkbox("Render Shadow", &bShadow);

	ImGui::PopItemWidth();

	if (ImGui::Button("Delete"))
	{
		m_iPre_Picked_ID = m_iPickObject_ID;
		m_iPickObject_ID = 0;
		m_pSelectObject->Set_Dead(true);
		m_pPreSelectObject = m_pSelectObject;

		//선택한게 조명인 경우
		CNonAnimModel* pSelect = dynamic_cast<CNonAnimModel*>(m_pSelectObject);
		if (pSelect != nullptr)
		{
			if (pSelect->Get_isLight() && m_iSelectedLightIndex != -1)
			{
				m_pGameInstance->Delete_Light(m_iSelectedLightIndex);
				m_iSelectedLightIndex = -1;
			}
		}
		m_pSelectObject = nullptr;
	}

	ImGui::Text("");
}

void CController_MapTool::PickPos()
{
	if (!ImGui::CollapsingHeader("Picking Inform"))
		return;

	if(m_pGameInstance->Get_KeyState(RBUTTON) == AWAY)
		m_pGameInstance->Picking(&m_vPickPos);

	static float fRot[3] = { 0.f,0.f,0.f};
	fRot[0] = m_vPickPos.x;
	fRot[1] = m_vPickPos.y;
	fRot[2] = m_vPickPos.z;

	ImGui::InputFloat3("PickPos3", fRot);

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
		SaveNav();
	}
	ImGui::SameLine();
	ImGui::Text("                           ");
	ImGui::SameLine();
	if (ImGui::Button("Load Nav"))
	{
		LoadNav();
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
	for (auto& filename : m_FileNames) {
		Safe_Delete_Array(filename);
	}
	m_FileNames.clear();

	char szFolderFolderFullPath[128] = "../Bin/ModelData/NonAnim/Map/";    // 상대 경로
	char szFolderPathReset[128] = "../Bin/ModelData/NonAnim/Map/";
	char szFolderPath[128] = "../Bin/ModelData/NonAnim/Map/";
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
	strcat_s(szFolderPathReset, szFolderName);

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

		strcpy_s(szFolderPath, szFolderPathReset);
		strcat_s(szFolderPath , fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFolderPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

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

	//전체 레이어 수 저장
	_uint iLayerCount = m_pGameInstance->Get_Object_Layer_Count(LEVEL_TOOL);
	iLayerCount -= 3;//카메라 레이어, 애니메이션 Tool용 모델 레이어, Background 제외

	//strUint = to_string(iLayerCount);
	//fout.write(strUint.c_str(), sizeof(strUint));
	//fout.write(strUint.c_str(), sizeof(strUint));
	
	fout.write(reinterpret_cast<const char*>(&iLayerCount), sizeof(_uint));

	_wstring sLayerTag = {};
	string strLayerTag = {};
	std::string::size_type strSize = {};
	_uint ObjectCount = 0;
	CGameObject* pGameObject = { nullptr };
	_float4x4 ObjectWorldMatrix = {};
	string strFloat = {};
	string strInt = {};
	_float3 vfloat3 = {};

	for (_uint i = 0; i < iLayerCount + 3; ++i)
	{
		if (i < 3)
			continue;

		sLayerTag = m_pGameInstance->Get_LayerTag(LEVEL_TOOL, i);

		//레이어 이름 저장
		strLayerTag = std::string().assign(sLayerTag.begin(), sLayerTag.end());
		strSize = strLayerTag.size();
		fout.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize)); //문자열 사이즈 저장
		fout.write(strLayerTag.c_str(), strSize);	//문자열 저장

		ObjectCount = m_pGameInstance->Get_Layer_ObjectCount(LEVEL_TOOL, sLayerTag);
		fout.write(reinterpret_cast<const char*>(&ObjectCount), sizeof(_uint)); //오브젝트 개수 저장

		//// Terrain사이즈 저장
		//if (sLayerTag == TEXT("Layer_BackGround"))
		//{
		//	fout.write(reinterpret_cast<const char*>(&m_iFloorSizeX), sizeof(_int));	
		//	fout.write(reinterpret_cast<const char*>(&m_iFloorSizeZ), sizeof(_int));
		//}

		if(sLayerTag != TEXT("Layer_Light"))
		{
			for (_uint j = 0; j < ObjectCount; ++j)
			{
				pGameObject = m_pGameInstance->Find_Object(LEVEL_TOOL, sLayerTag, j);

				OBJECT_DEFAULT_DESC pDesc = {};
				wstring wstrModelTag = static_cast<CNonAnimModel*>(pGameObject)->Get_ModelTag();
				wcscpy_s(pDesc.szModelTag, wstrModelTag.c_str());
				XMStoreFloat3(&pDesc.vPosition, pGameObject->Get_Transform()->Get_State(CTransform::STATE_POSITION));
				pDesc.vScale = pGameObject->Get_Transform()->Get_Scaled();
				pDesc.vRotation = pGameObject->Get_Transform()->Get_CurrentRotation();
				pDesc.isInstance = static_cast<CNonAnimModel*>(pGameObject)->Get_isInstance();
				pDesc.iID = static_cast<CNonAnimModel*>(pGameObject)->Get_RenderTargetId();
				pDesc.bShadow = static_cast<CNonAnimModel*>(pGameObject)->Get_bShadow();

				//현재 위치한 Cell 번호 저장
				_int iCellnum = m_pNavigationController->Get_WhereCell(pDesc.vPosition);
				pDesc.iCurrentCellNum = iCellnum;

				fout.write(reinterpret_cast<const char*>(&pDesc), sizeof(pDesc));
			}
		}
		else
		{
			for (_uint j = 0; j < m_pGameInstance->Get_Total_LightCount(); ++j)
			{
				if (j == 0)
					continue;

				LIGHT_DESC* pLightDesc = {};
				pLightDesc = m_pGameInstance->Get_LightDesc(j);

				fout.write(reinterpret_cast<const char*>(pLightDesc), sizeof(*pLightDesc));
			}
		}
	}

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

	string line;
	_uint LayerCount = { 0 };
	//getline(fin, line);
	//LayerCout = std::stoi(line);
	fin.read(reinterpret_cast<char*>(&LayerCount), sizeof(LayerCount));

	string strLayerTag = {};
	std::string::size_type iStrSize = { 0 };
	_uint iObjectCount = 0;
	int i = 0;
	int iObjectType = 0;
	int iObjectListIndex = 0;
	_float3 fPos = {};
	_float3 fScaled = {};
	_float3 fRot = {};
	_int iCellNum = {};
	_uint iCellRoomNum = {};

	while (i < LayerCount)
	{
		//getline(fin, strLayerTag);
		//fin >> iObjectCount;
		fin.read(reinterpret_cast<char*>(&iStrSize), sizeof(iStrSize));
		strLayerTag.resize(iStrSize);
		fin.read(&strLayerTag[0], iStrSize);
		fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(iObjectCount));

		for (int j = 0; j < iObjectCount; ++j)
		{
			if(strLayerTag != "Layer_Light")
			{
				OBJECT_DEFAULT_DESC pDesc = {};

				fin.read(reinterpret_cast<char*>(&pDesc), sizeof(pDesc));

				CNonAnimModel::NONMODEL_DESC nonDesc = {};

				int bufferSize = WideCharToMultiByte(CP_ACP, 0, pDesc.szModelTag, -1, NULL, 0, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, pDesc.szModelTag, -1, nonDesc.szModelTag, bufferSize, NULL, NULL);

				nonDesc.vPosition = pDesc.vPosition;
				nonDesc.vScale = pDesc.vScale;
				nonDesc.vRotation = pDesc.vRotation;
				nonDesc.isLight = { false };
				nonDesc.isInstance = pDesc.isInstance;
				nonDesc.iRenderGroupID = pDesc.iID;
				nonDesc.bShadow = pDesc.bShadow;
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Map"), TEXT("Prototype_GameObject_NonAnim"), &nonDesc)))
					return;
			}
			else
			{
				LIGHT_DESC pDesc = {};

				fin.read(reinterpret_cast<char*>(&pDesc), sizeof(pDesc));

				if (FAILED(m_pGameInstance->Add_Light(pDesc)))
					return;

				CNonAnimModel::NONMODEL_DESC Desc{};
				Desc.vPosition.x = pDesc.vPosition.x;
				Desc.vPosition.y = pDesc.vPosition.y;
				Desc.vPosition.z = pDesc.vPosition.z;
				Desc.vScale = { 1.f,1.f,1.f };
				Desc.vRotation = { 0.f,0.f,0.f };
				Desc.iRenderGroupID = 0;
				Desc.isLight = true;
				strcpy_s(Desc.szModelTag, "Light");

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Light"), TEXT("Prototype_GameObject_NonAnim"), &Desc)))
					return;
			}

		}

		++i;
	}

	fin.close();
	MSG_BOX(TEXT("파일 읽기를 성공했습니다.."));
}

void CController_MapTool::SaveNav()
{
	const char cFile[128] = "../Bin/DataFiles/Nav_Data.dat";
	ofstream fout(cFile, ios::out | ios::binary);

	//	fout.open();
	if (!fout.is_open())    // 파일 열었다면
	{
		MSG_BOX(TEXT("Nav 파일 쓰기를 실패"));
		return;
	}

	string strUint = {};

	//전체 Cell개수 저장
	_uint iCellCount = m_pNavigationController->Get_CellSize();
	fout.write(reinterpret_cast<const char*>(&iCellCount), sizeof(_uint));

	_float3 vPos = {};

	for (_uint i = 0; i < iCellCount; ++i)
	{
		_uint iCellAreaNum = m_pNavigationController->Get_AreaNum(i);
		fout.write(reinterpret_cast<const char*>(&iCellAreaNum), sizeof(_uint));

		_uint iCellTypeNum= m_pNavigationController->Get_TypeNum(i);
		fout.write(reinterpret_cast<const char*>(&iCellTypeNum), sizeof(_uint));

		m_pNavigationController->Set_SelectCell(i);

		XMStoreFloat3(&vPos, m_pNavigationController->Get_SelectCell()->Get_Point(CCell::POINT_A));
		fout.write(reinterpret_cast<const char*>(&vPos.x), sizeof(_float));
		fout.write(reinterpret_cast<const char*>(&vPos.y), sizeof(_float));
		fout.write(reinterpret_cast<const char*>(&vPos.z), sizeof(_float));


		XMStoreFloat3(&vPos, m_pNavigationController->Get_SelectCell()->Get_Point(CCell::POINT_B));
		fout.write(reinterpret_cast<const char*>(&vPos.x), sizeof(_float));
		fout.write(reinterpret_cast<const char*>(&vPos.y), sizeof(_float));
		fout.write(reinterpret_cast<const char*>(&vPos.z), sizeof(_float));


		XMStoreFloat3(&vPos, m_pNavigationController->Get_SelectCell()->Get_Point(CCell::POINT_C));
		fout.write(reinterpret_cast<const char*>(&vPos.x), sizeof(_float));
		fout.write(reinterpret_cast<const char*>(&vPos.y), sizeof(_float));
		fout.write(reinterpret_cast<const char*>(&vPos.z), sizeof(_float));
	}

	fout.close();
	MSG_BOX(TEXT("파일 쓰기를 성공"));
}

void CController_MapTool::LoadNav()
{
	m_pNavigationController->Delete_All_Cell();

	const char cFile[128] = "../Bin/DataFiles/Nav_Data.dat";
	ifstream fin(cFile, ios::in | ios::binary);

	//	fin.open("../Bin/Map_Data.txt");
	if (!fin.is_open())    // 파일 열었다면
	{
		MSG_BOX(TEXT("파일 읽기를 실패했어요.."));
		return;
	}

	_uint iCellCout = { 0 };
	fin.read(reinterpret_cast<char*>(&iCellCout), sizeof(iCellCout));

	_float3 vPos = {};

	for (_uint i = 0; i < iCellCout; ++i)
	{
		_uint iCellAreaNum = { };
		fin.read(reinterpret_cast<char*>(&iCellAreaNum), sizeof(iCellAreaNum));
		m_pNavigationController->Set_AreaNum(iCellAreaNum);

		_uint iCellTypeNum = { };
		fin.read(reinterpret_cast<char*>(&iCellTypeNum), sizeof(iCellTypeNum));
		m_pNavigationController->Set_TypeNum(iCellTypeNum);

		for (int j = 0; j < 3; ++j)
		{
			fin.read(reinterpret_cast<char*>(&vPos.x), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&vPos.y), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&vPos.z), sizeof(_float));

			m_pNavigationController->Add_Point(XMLoadFloat3(&vPos));
		}
	}

	fin.close();
	MSG_BOX(TEXT("파일 읽기를 성공했습니다.."));
}

void CController_MapTool::Find_PickObject()
{
	//모든 레이어를 돌면서 물체들의 id값을 비교해야한다.
	_uint iLayerCount = m_pGameInstance->Get_Object_Layer_Count(LEVEL_TOOL);	//전체 레이어 수

	_wstring sLayerTag = {};
	_uint ObjectCount = 0;
	CGameObject* pGameObject = { nullptr };
	CNonAnimModel* pNonAnimObject = { nullptr };

	for (_uint i = 0; i < iLayerCount; i++)
	{
		sLayerTag = m_pGameInstance->Get_LayerTag(LEVEL_TOOL, i); //i번째 레이어 태그

		ObjectCount = m_pGameInstance->Get_Layer_ObjectCount(LEVEL_TOOL, sLayerTag);

		for (_uint i = 0; i < ObjectCount; ++i)
		{
			pGameObject = m_pGameInstance->Find_Object(LEVEL_TOOL, sLayerTag, i);

			pNonAnimObject = dynamic_cast<CNonAnimModel*>(pGameObject);
			if (pNonAnimObject == nullptr)
				continue;

			if (m_iPickObject_ID == pNonAnimObject->Get_HashId())
			{
				m_pSelectObject = pGameObject;
				pNonAnimObject->Set_Selected(true);
				return;
			}
		}
	}
}

void CController_MapTool::Map_Menu()
{
	string strLayerName;
	_wstring wstrLayerName;

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

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

	static _bool bInstance = false;
	ImGui::Checkbox("Draw Instance", &bInstance);

	static _bool bShadow = false;
	ImGui::Checkbox("Render Shadow", &bShadow);

	//오브젝트 생성
	if (ImGui::Button("Create Model") || m_pGameInstance->Get_KeyState(C) == AWAY)
	{
		wstrLayerName.assign(strLayerName.begin(), strLayerName.end());

		//통으로 저장
		CNonAnimModel::NONMODEL_DESC Desc{};
		Desc.vPosition = m_vPickPos;	
		Desc.vScale = { 1.f,1.f,1.f };	
		Desc.vRotation = { 0.f,0.f,0.f };	
		Desc.iRenderGroupID = i0;	
		Desc.isInstance = bInstance;	
		Desc.bShadow = bShadow;

		strcpy_s(Desc.szModelTag, m_FileNames[m_iListSelectNum]);

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, wstrLayerName, TEXT("Prototype_GameObject_NonAnim"), &Desc)))
			return;

		//새로 만든 오브젝트 자동 선택
		if(m_pSelectObject != nullptr)
		{
			//이전에 만든거 해제
			static_cast<CNonAnimModel*>(m_pSelectObject)->Set_Selected(false);
			m_pPreSelectObject = m_pSelectObject;
			if(static_cast<CNonAnimModel*>(m_pSelectObject)->Get_isLight())
			{
				m_iPreSelectedLightIndex = m_iSelectedLightIndex;
				m_iSelectedLightIndex = -1;
			}
		}
		
		int LayerObjectCount = m_pGameInstance->Get_Layer_ObjectCount(LEVEL_TOOL, wstrLayerName);
		m_pSelectObject = m_pGameInstance->Find_Object(LEVEL_TOOL, wstrLayerName, LayerObjectCount-1);
		static_cast<CNonAnimModel*>(m_pSelectObject)->Set_Selected(true);
		m_iPre_Picked_ID = m_iPickObject_ID;
		m_iPickObject_ID = static_cast<CNonAnimModel*>(m_pSelectObject)->Get_HashId();



	}ImGui::SameLine();
	ImGui::Text("or Press \"C\" to Create");

}

void CController_MapTool::Nav_Menu()
{
	//버튼 누를 때 한번만 실행
	enum Nav_Mode
	{
		Mode_Create_Cell,
		Mode_Cell_Select,
		Mode_Point_Select
	};

	static int iMode = 0;

	if (ImGui::RadioButton("Create Cell", iMode == Mode_Create_Cell))
	{
		iMode = Mode_Create_Cell;
	} ImGui::SameLine();

	if (ImGui::RadioButton("Select Cell", iMode == Mode_Cell_Select))
	{
		iMode = Mode_Cell_Select;
	} ImGui::SameLine();

	if (ImGui::RadioButton("Select Point", iMode == Mode_Point_Select))
	{
		iMode = Mode_Point_Select;
	}

	//////////////////////////////////////

	ImGui::Text("");
	ImGui::Text("[Cell_List]");

	static _int iSelectedCellNum = -1;

	ImGui::BeginChild("Cell List", ImVec2(150, 350), true);

	_uint iCellCount = 0;

	iCellCount = m_pNavigationController->Get_CellSize();

	if (iCellCount != 0)
	{
		for (_uint i = 0; i < iCellCount; i++)
		{
			_char szCell[MAX_PATH] = "Cell ";
			_char szNum[MAX_PATH];
			sprintf_s(szNum, "%d", i);
			strcat_s(szCell, szNum);

			if (ImGui::Selectable(szCell, iSelectedCellNum == i))
			{
				iSelectedCellNum = i;

				m_pNavigationController->Set_SelectCell(iSelectedCellNum);
			}
		}
	}
	ImGui::EndChild();
	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::BeginChild("Detail view", ImVec2(0, 400)); // Leave room for 1 line below us

	ImGui::Text("[Selected Cell Inform]");

	ImGui::Text("Total Cell Count : %d", iCellCount);
	ImGui::Text("Select Cell Index : %d", iSelectedCellNum);
	ImGui::SameLine(); ImGui::Text("    "); 	ImGui::SameLine();
	if (ImGui::Button("Delete Cell"))
	{
		m_pNavigationController->Delete_Selected(iSelectedCellNum);
		iSelectedCellNum = -1;
	}

	//ABC 점 좌표 띄우기 ->확인용

	CCell* pCurrentCell = m_pNavigationController->Get_SelectCell();

	_float3 vPoints[3] = { _float3(0.f,0.f,0.f),_float3(0.f,0.f,0.f),_float3(0.f,0.f,0.f) };

	if (pCurrentCell != nullptr )
	{
		vPoints[0].x = XMVectorGetX(pCurrentCell->Get_Point(CCell::POINT_A));
		vPoints[0].y = XMVectorGetY(pCurrentCell->Get_Point(CCell::POINT_A));
		vPoints[0].z = XMVectorGetZ(pCurrentCell->Get_Point(CCell::POINT_A));

		vPoints[1].x = XMVectorGetX(pCurrentCell->Get_Point(CCell::POINT_B));
		vPoints[1].y = XMVectorGetY(pCurrentCell->Get_Point(CCell::POINT_B));
		vPoints[1].z = XMVectorGetZ(pCurrentCell->Get_Point(CCell::POINT_B));

		vPoints[2].x = XMVectorGetX(pCurrentCell->Get_Point(CCell::POINT_C));
		vPoints[2].y = XMVectorGetY(pCurrentCell->Get_Point(CCell::POINT_C));
		vPoints[2].z = XMVectorGetZ(pCurrentCell->Get_Point(CCell::POINT_C));
	}

	ImGui::Text("PointA :"); ImGui::SameLine(); ImGui::InputFloat3("##PointA", (_float*)&vPoints[0]);
	ImGui::Text("PointB :"); ImGui::SameLine(); ImGui::InputFloat3("##PointB", (_float*)&vPoints[1]);
	ImGui::Text("PointC :"); ImGui::SameLine(); ImGui::InputFloat3("##PointC", (_float*)&vPoints[2]);

	int iSelectCellArea = 0;
	int iSelectCellType = 0;

	if(pCurrentCell != nullptr)
	{
		iSelectCellArea = pCurrentCell->Get_AreaNum();
		iSelectCellType = pCurrentCell->Get_CellTypeNum();
	}

	ImGui::Text("Cell Type : %d", iSelectCellArea);
	ImGui::Text("Cell Area Num : %d", iSelectCellType);

	if (iMode == Mode_Create_Cell)
	{
		Cell_Create_Menu();
	}
	else if (iMode == Mode_Cell_Select)
	{
		//Cell Picking
		if(m_pGameInstance->Get_KeyState(RBUTTON) == AWAY)
			m_pNavigationController->SelectCell(m_vPickPos,&iSelectedCellNum);

		Cell_Select_Menu();
	}
	else
	{
		//선택한 점 좌표 띄우기
		Cell_Select_Point_Menu();
	}

	ImGui::EndChild();
	ImGui::EndGroup();
}

void CController_MapTool::Cell_Create_Menu()
{
	ImGui::SeparatorText("Create Cell");

	//Cell 타입 설정
	static int iCellAreaNum = 0;
	static int iCellTypeNum = 0;

	ImGui::InputInt("Cell Area Num", &iCellAreaNum);
	ImGui::InputInt("Cell Type ID", &iCellTypeNum);

	m_pNavigationController->Set_AreaNum(iCellAreaNum);
	m_pNavigationController->Set_TypeNum(iCellTypeNum);


	//저장한 점들 개수
	int iSavePointCount = m_pNavigationController->Get_SavePointsCount();
	ImGui::Text("Save Point Count : %d", iSavePointCount);

	//Cell 생성기능
	if (ImGui::Button("Add Point") || m_pGameInstance->Get_KeyState(C) == AWAY)
	{
		m_pNavigationController->Add_Point(m_vPickPos);
	}
	ImGui::SameLine();
	ImGui::Text("or Press \"C\" to add Point");


}

void CController_MapTool::Cell_Select_Menu()
{
	ImGui::SeparatorText("Change Cell Inform");

	CCell* pCurrentCell = m_pNavigationController->Get_SelectCell();

	if (pCurrentCell == nullptr)
		return;

	int iCellAreaNum = 0;
	int iCellTypeNum = 0;

	//새로운 셀을 고르면 정보 업데이트
	if (m_iPrePickedCellIndex != m_pNavigationController->Get_SelectCellIndex() && m_pNavigationController->Get_SelectCellIndex() != -1)
	{
		iCellAreaNum = pCurrentCell->Get_AreaNum();
		iCellTypeNum = pCurrentCell->Get_CellTypeNum();
	}

	ImGui::InputInt("Cell Area Num", &iCellAreaNum);
	ImGui::InputInt("Cell Type ID", &iCellTypeNum);

	if(pCurrentCell != nullptr)
	{
		pCurrentCell->Set_AreaNum(iCellAreaNum);
		pCurrentCell->Set_CellTypeNum(iCellTypeNum);
	}
}

void CController_MapTool::Cell_Select_Point_Menu()
{
	ImGui::SeparatorText("Change Vertex Pos");

	static _float3 vSelectVertexPos = _float3(0.f, 0.f, 0.f);
	static float fPos[3] = { 0.f,0.f,0.f };
	static bool bChangeVertexPos = false;
	ImGui::Checkbox("##Picking_TerrainPos", &bChangeVertexPos);	ImGui::SameLine();
	ImGui::Text("Change_Pos (Click C)");

	if (bChangeVertexPos == false)
	{
		//존재하는 점 중 가까운곳으로 피킹좌표 이동
		m_vPickPos = m_pNavigationController->Select_Vertex(XMLoadFloat3(&m_vPickPos));

		vSelectVertexPos = m_vPickPos;
		fPos[0] = vSelectVertexPos.x;
		fPos[1] = vSelectVertexPos.y;
		fPos[2] = vSelectVertexPos.z;
	}

	ImGui::DragFloat3("Rotation(X, Y, Z)", &fPos[0], 0.05f);

	if (KEY_AWAY(KEY::C))
	{
		vSelectVertexPos.x = fPos[0];
		vSelectVertexPos.y = fPos[1];
		vSelectVertexPos.z = fPos[2];
		m_pNavigationController->Set_All_Selected_Vertex_to_this(vSelectVertexPos);
	}

}

void CController_MapTool::Light_Create()
{
	enum Light_Type
	{
		Light_Point,
		Light_Spot,
		Light_GodRay
	};

	//구조체 세부 설정
	static int iLightType = 0;

	if (ImGui::RadioButton("Point", iLightType == Light_Point))
	{
		iLightType = Light_Point;
	} ImGui::SameLine();

	if (ImGui::RadioButton("Spot", iLightType == Light_Spot))
	{
		iLightType = Light_Spot;
	} ImGui::SameLine();

	if (ImGui::RadioButton("GodRay", iLightType == Light_GodRay))
	{
		iLightType = Light_GodRay;
	}

	static _Vec4 vDirection = { 1.f,1.f,1.f,0.f };
	static _Vec4 vPosition = { 0.f,0.f,0.f,1.f };
	static _float fRange = { 10.f };
	static _Vec4 vDiffuse = { 1.f,1.f,1.f,1.f };
	static _Vec4 vAmbient = { 1.f,1.f,1.f,1.f };
	static _Vec4 vSpecular = { 1.f,1.f,1.f,1.f };

	vPosition.x = m_vPickPos.x;
	vPosition.y = m_vPickPos.y;
	vPosition.z = m_vPickPos.z;

	//방향, 위치, 범위
	ImGui::DragFloat3("Direction", (_float*)&vDirection, 0.05f, -1.f, 1.f);
	ImGui::DragFloat4("Position(X, Y, Z)", (_float*)&vPosition, 0.05f, -5000.f, 5000.f);
	ImGui::DragFloat("Range", (_float*)&fRange, 0.05f, 0.f, 1000.f);

	//색상값
	static ImVec4 color = ImVec4(255.f / 255.0f, 255.f / 255.0f, 255.f / 255.0f, 255.f / 255.0f);
	ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_NoOptions;

	ImGui::ColorEdit3("MyColor##1", (float*)&color, misc_flags);
	vDiffuse.x = color.x;
	vDiffuse.y = color.y;
	vDiffuse.z = color.z;

	ImGui::DragFloat4("Ambient", (_float*)&vAmbient, 0.05f, 0.f, 1.f);
	ImGui::DragFloat4("Specular", (_float*)&vSpecular, 0.05f, 0.f, 1.f);

	//렌더타겟 아이디 설정 가능
	static int i0 = 0;
	ImGui::InputInt("ID", &i0);

	//오브젝트 생성

	string strLayerName;
	_wstring wstrLayerName;

	if (ImGui::Button("Create Light") || m_pGameInstance->Get_KeyState(C) == AWAY)
	{
		strLayerName = "Layer_Light";
		wstrLayerName.assign(strLayerName.begin(), strLayerName.end());

		CNonAnimModel::NONMODEL_DESC Desc{};
		Desc.vPosition = m_vPickPos;
		Desc.vScale = { 1.f,1.f,1.f };
		Desc.vRotation = { 0.f,0.f,0.f };
		Desc.iRenderGroupID = i0;
		Desc.isLight = true;
		strcpy_s(Desc.szModelTag, "Light");

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, wstrLayerName, TEXT("Prototype_GameObject_NonAnim"), &Desc)))
			return;

		//진짜 빛 넣기
		//방향 노멀라이즈
		LIGHT_DESC newLightDesc = {};
		newLightDesc.eType = (LIGHT_DESC::TYPE)(iLightType + 1);
		vDirection.Normalize();
		newLightDesc.vDirection = vDirection;
		newLightDesc.vPosition = vPosition;
		newLightDesc.fRange = fRange;
		newLightDesc.vDiffuse = vDiffuse;
		newLightDesc.vAmbient = vAmbient;
		newLightDesc.vSpecular = vSpecular;

		if (FAILED(m_pGameInstance->Add_Light(newLightDesc)))
			return ;

		//새로 만든 오브젝트 자동 선택
		if (m_pSelectObject != nullptr)
		{
			static_cast<CNonAnimModel*>(m_pSelectObject)->Set_Selected(false);
			m_pPreSelectObject = m_pSelectObject;
			if (static_cast<CNonAnimModel*>(m_pSelectObject)->Get_isLight())
			{
				m_iPreSelectedLightIndex = m_iSelectedLightIndex;
				m_iSelectedLightIndex = -1;
			}
		}

		int LayerObjectCount = m_pGameInstance->Get_Layer_ObjectCount(LEVEL_TOOL, wstrLayerName);
		m_pSelectObject = m_pGameInstance->Find_Object(LEVEL_TOOL, wstrLayerName, LayerObjectCount - 1);
		static_cast<CNonAnimModel*>(m_pSelectObject)->Set_Selected(true);
		m_iPre_Picked_ID = m_iPickObject_ID;
		m_iPickObject_ID = static_cast<CNonAnimModel*>(m_pSelectObject)->Get_HashId();

		m_iSelectedLightIndex = m_pGameInstance->Get_Total_LightCount()-1;

	}ImGui::SameLine();
	ImGui::Text("or Press \"C\" to Create");
}

void CController_MapTool::Light_Modify()
{
	if (m_iSelectedLightIndex == -1)	//선택한게 없다
	{
		m_iPreSelectedLightIndex = -1;
		return;
	}
	
	LIGHT_DESC* pDesc = {};
	
	static _Vec4 vDirection = { 1.f,1.f,1.f,0.f };
	static _Vec4 vPosition = { 0.f,0.f,0.f,1.f };
	static _float fRange = { 10.f };
	static _Vec4 vDiffuse = { 1.f,1.f,1.f,1.f };
	static _Vec4 vAmbient = { 1.f,1.f,1.f,1.f };
	static _Vec4 vSpecular = { 1.f,1.f,1.f,1.f };

	pDesc = m_pGameInstance->Get_LightDesc(m_iSelectedLightIndex);

	if (m_iPreSelectedLightIndex != m_iSelectedLightIndex) //새로 선택하면 정보를 불러온다
	{
		
		vDirection = pDesc->vDirection;
		vPosition = pDesc->vPosition;
		fRange = pDesc->fRange;
		vDiffuse = pDesc->vDiffuse;
		vAmbient = pDesc->vAmbient;
		vSpecular = pDesc->vSpecular;

		m_iPreSelectedLightIndex = m_iSelectedLightIndex;
	}
	else   //수정한 내용 적용
	{
		pDesc->vDirection = vDirection;
		pDesc->vPosition = m_pSelectObject->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		pDesc->fRange = fRange;
		pDesc->vDiffuse = vDiffuse;
		pDesc->vAmbient = vAmbient;
		pDesc->vSpecular = vSpecular;
	}

	//방향, 위치, 범위
	ImGui::DragFloat3("Direction", (_float*)&vDirection, 0.05f, -1.f, 1.f);
	//ImGui::DragFloat4("Position(X, Y, Z)", (_float*)&vPosition, 0.05f, -5000.f, 5000.f);
	ImGui::DragFloat("Range", (_float*)&fRange, 0.05f, 0.f, 1000.f);

	//색상값
	static ImVec4 color = {};
	color = ImVec4(vDiffuse.x , vDiffuse.y , vDiffuse.z , 1.f );
	ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_NoOptions;

	ImGui::ColorEdit3("MyColor##1", (float*)&color, misc_flags);
	vDiffuse.x = color.x ;
	vDiffuse.y = color.y ;
	vDiffuse.z = color.z ;

	ImGui::DragFloat4("Ambient", (_float*)&vAmbient, 0.05f, 0.f, 1.f);
	ImGui::DragFloat4("Specular", (_float*)&vSpecular, 0.05f, 0.f, 1.f);
}

void CController_MapTool::Decal_Create()
{
	//CDecal::DECAL_DESC desc = {};
	//desc.vPosition = m_vPickPos;

	CNonAnimModel::NONMODEL_DESC Desc{};
	Desc.vPosition = m_vPickPos;
	Desc.vScale = { 1.f,1.f,1.f };
	Desc.vRotation = { 0.f,0.f,0.f };
	Desc.iRenderGroupID = 0;
	Desc.isLight = false;
	Desc.isDecal = true;
	
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Decal"), TEXT("Prototype_GameObject_NonAnim"),&Desc)))
		return;
}

void CController_MapTool::Light_Menu()
{
	ImGui::Text("Total Light Count : %d", m_pGameInstance->Get_Total_LightCount());
	//ImGui::Text("Select Light Index : %d", )

	
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("AboutLIght", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Create"))
		{
			Light_Create();

			ImGui::EndTabItem();
		}	
		
		if (ImGui::BeginTabItem("Modify"))
		{
			Light_Modify();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	
}

void CController_MapTool::Decal_Menu()
{
#pragma region SHOW FOLDER LIST
	//내용물 초기화 (capacity는 그냥 냅둠)
	for (auto& filename : m_Decal_Folder_Names) {
		Safe_Delete_Array(filename);
	}
	m_Decal_Folder_Names.clear();

	char szFolderFindPath[128] = "../Bin/Resources/Textures/Decal/tga/*";    // 상대 경로 -> 모든 파일을 돌겠다
	char szFolderPathReset[128] = "../Bin/Resources/Textures/Decal/tga/";
	char szFolderPath[128] = "../Bin/Resources/Textures/Decal/tga/";

	_finddata_t fd;
	intptr_t handle = _findfirst(szFolderFindPath, &fd);

	if (handle == -1)
		return;

	int iResult = 0;
	int iFolderCount = 0;

	while (iResult != -1)
	{
		strcpy_s(szFolderPath, szFolderPathReset);
		strcat_s(szFolderPath, fd.name);

		_char szDirName[MAX_PATH] = "";
		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFolderPath, nullptr, 0, szDirName, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(szFileName, ".") || !strcmp(szFileName, "..") || !strcmp(szFileName, ""))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		iFolderCount++;

		//_strup : 문자열 내용을 복사해 그 주소를 저장-> 주소에 따라 문자열이 바뀌는걸 막아 모두 동일해지는걸 막음
		m_Decal_Folder_Names.push_back(_strdup(szFileName));

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	static int item_selected_idx = 0; // Here we store our selected data as an index.
	static bool item_highlight = false;
	int item_highlighted_idx = -1; // Here we store our highlighted data as an index.

	if (ImGui::BeginListBox("Folders"))
	{
		for (int n = 0; n < iFolderCount; n++)
		{
			const bool is_selected = (item_selected_idx == n);
			if (ImGui::Selectable(m_Decal_Folder_Names[n], is_selected))
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
#pragma endregion

	//diffuse이미지는 기본으로 사용하고, normal이나 ARMT/ARMH를 사용할지 선택
	static _bool bUseNormal = false;
	static _bool bUseARM = false;
	ImGui::Checkbox("Use Normal", &bUseNormal);
	ImGui::SameLine();
	ImGui::Checkbox("Use ARMT/ARMH", &bUseARM);

#pragma region SHOW IMAGES LIST_DIFFUSE

	if (ImGui::CollapsingHeader("Diffuse_Images"))
	{
		//내용물 초기화 (capacity는 그냥 냅둠)
		for (auto& filename : m_Decal_File_Names_Diffuse) {
			Safe_Delete_Array(filename);
		}
		m_Decal_File_Names_Diffuse.clear();

		char szImageFindPath[128] = "../Bin/Resources/Textures/Decal/tga/";    // 상대 경로 -> 모든 파일을 돌겠다
		char szImagePathReset[128] = "../Bin/Resources/Textures/Decal/tga/";
		char szImagePath[128] = "../Bin/Resources/Textures/Decal/tga/";

		//폴더 리스트에서 선택한 인덱스로 파일 검색 경로 생성
		strcat_s(szImageFindPath, m_Decal_Folder_Names[m_iListSelectNum]);
		strcat_s(szImageFindPath, "/Diffuse/*.*");

		handle = _findfirst(szImageFindPath, &fd);

		if (handle == -1)
			return;

		iResult = 0;
		int iImagesCount = 0;

		while (iResult != -1)
		{
			strcpy_s(szImagePath, szImageFindPath);
			strcat_s(szImagePath, fd.name);

			_char szDirName[MAX_PATH] = "";
			_char szFileName[MAX_PATH] = "";
			_char szExt[MAX_PATH] = "";
			_splitpath_s(szImagePath, nullptr, 0, szDirName, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);

			if (!strcmp(szExt, ".") || !strcmp(szExt, "..") || strcmp(szExt, ".tga"))
			{
				iResult = _findnext(handle, &fd);
				continue;
			}

			iImagesCount++;

			//_strup : 문자열 내용을 복사해 그 주소를 저장-> 주소에 따라 문자열이 바뀌는걸 막아 모두 동일해지는걸 막음
			m_Decal_File_Names_Diffuse.push_back(_strdup(fd.name));

			//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
			iResult = _findnext(handle, &fd);
		}

		static bool item_Image_highlight = false;
		int item_Image_highlighted_idx = -1; // Here we store our highlighted data as an index.

		if (ImGui::BeginListBox("Diffuse_Images"))
		{
			for (int n = 0; n < iImagesCount; n++)
			{
				const bool is_selected = (m_iSelected_Decal_Diffuse_ID == n);
				if (ImGui::Selectable(m_Decal_File_Names_Diffuse[n], is_selected))
					m_iSelected_Decal_Diffuse_ID = n;

				if (item_Image_highlight && ImGui::IsItemHovered())
					item_Image_highlighted_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();

				m_iSelected_Decal_Diffuse_ID;
			}
			ImGui::EndListBox();
		}

	#pragma region SHOW IMAGES PREVIEW_Diffuse
		static _bool bShowPreview_Diffuse = false;
		ImGui::Checkbox("Image Preview_Diffuse", &bShowPreview_Diffuse);

		if (bShowPreview_Diffuse)
		{
			ImGui::Begin("Preview_Diffuse", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar
				| ImGuiWindowFlags_AlwaysVerticalScrollbar);

			int my_image_width = 0;
			int my_image_height = 0;

			if (m_my_texture) {
				Safe_Release(m_my_texture);
				m_my_texture = NULL;
			}

			char szImageReadPath[128] = "";

			if (m_iSelected_Decal_Diffuse_ID != -1)
			{
				strcpy_s(szImageReadPath, szImagePathReset);
				strcat_s(szImageReadPath, m_Decal_Folder_Names[item_selected_idx]);
				strcat_s(szImageReadPath, "/Diffuse/");
				strcat_s(szImageReadPath, m_Decal_File_Names_Diffuse[m_iSelected_Decal_Diffuse_ID]);

				bool ret = LoadTextureFromFile(szImageReadPath, &m_my_texture, &my_image_width, &my_image_height);
				IM_ASSERT(ret);

				ImGui::Image((ImTextureID)(intptr_t)m_my_texture, ImVec2(my_image_width, my_image_height));
			}

			ImGui::End();
		}

	#pragma endregion
	}
#pragma endregion

#pragma region SHOW IMAGES LIST_NORMAL

	if (ImGui::CollapsingHeader("Normal_Images"))
	{
		//내용물 초기화 (capacity는 그냥 냅둠)
		for (auto& filename : m_Decal_File_Names_Normal) {
			Safe_Delete_Array(filename);
		}
		m_Decal_File_Names_Normal.clear();

		char szImageFindPath[128] = "../Bin/Resources/Textures/Decal/tga/";    // 상대 경로 -> 모든 파일을 돌겠다
		char szImagePathReset[128] = "../Bin/Resources/Textures/Decal/tga/";
		char szImagePath[128] = "../Bin/Resources/Textures/Decal/tga/";

		//폴더 리스트에서 선택한 인덱스로 파일 검색 경로 생성
		strcat_s(szImageFindPath, m_Decal_Folder_Names[m_iListSelectNum]);
		strcat_s(szImageFindPath, "/Normal/*.*");

		handle = _findfirst(szImageFindPath, &fd);

		if (handle == -1)
			return;

		iResult = 0;
		int iImagesCount = 0;

		while (iResult != -1)
		{
			strcpy_s(szImagePath, szImageFindPath);
			strcat_s(szImagePath, fd.name);

			_char szDirName[MAX_PATH] = "";
			_char szFileName[MAX_PATH] = "";
			_char szExt[MAX_PATH] = "";
			_splitpath_s(szImagePath, nullptr, 0, szDirName, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);

			if (!strcmp(szExt, ".") || !strcmp(szExt, "..") || strcmp(szExt, ".tga"))
			{
				iResult = _findnext(handle, &fd);
				continue;
			}

			iImagesCount++;

			//_strup : 문자열 내용을 복사해 그 주소를 저장-> 주소에 따라 문자열이 바뀌는걸 막아 모두 동일해지는걸 막음
			m_Decal_File_Names_Normal.push_back(_strdup(fd.name));

			//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
			iResult = _findnext(handle, &fd);
		}

		static bool item_Image_highlight = false;
		int item_Image_highlighted_idx = -1; // Here we store our highlighted data as an index.

		if (ImGui::BeginListBox("Normal"))
		{
			for (int n = 0; n < iImagesCount; n++)
			{
				const bool is_selected = (m_iSelected_Decal_Normal_ID == n);
				if (ImGui::Selectable(m_Decal_File_Names_Normal[n], is_selected))
					m_iSelected_Decal_Normal_ID = n;

				if (item_Image_highlight && ImGui::IsItemHovered())
					item_Image_highlighted_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();

				m_iSelected_Decal_Normal_ID;
			}
			ImGui::EndListBox();
		}

#pragma region SHOW IMAGES PREVIEW_Normal
		static _bool bShowPreview_Normal = false;
		ImGui::Checkbox("Image Preview_Normal", &bShowPreview_Normal);

		if (bShowPreview_Normal)
		{
			ImGui::Begin("Preview_Normal", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar
				| ImGuiWindowFlags_AlwaysVerticalScrollbar);

			int my_image_width = 0;
			int my_image_height = 0;

			if (m_my_texture) {
				Safe_Release(m_my_texture);
				m_my_texture = NULL;
			}

			char szImageReadPath[128] = "";

			if (m_iSelected_Decal_Normal_ID != -1)
			{
				strcpy_s(szImageReadPath, szImagePathReset);
				strcat_s(szImageReadPath, m_Decal_Folder_Names[item_selected_idx]);
				strcat_s(szImageReadPath, "/Normal/");
				strcat_s(szImageReadPath, m_Decal_File_Names_Normal[m_iSelected_Decal_Normal_ID]);

				bool ret = LoadTextureFromFile(szImageReadPath, &m_my_texture, &my_image_width, &my_image_height);
				IM_ASSERT(ret);

				ImGui::Image((ImTextureID)(intptr_t)m_my_texture, ImVec2(my_image_width, my_image_height));
			}

			ImGui::End();
		}

#pragma endregion
	}
#pragma endregion

#pragma region SHOW IMAGES LIST_ARM

	if (ImGui::CollapsingHeader("ARMT/ARMH"))
	{
		//내용물 초기화 (capacity는 그냥 냅둠)
		for (auto& filename : m_Decal_File_Names_Arm) {
			Safe_Delete_Array(filename);
		}
		m_Decal_File_Names_Arm.clear();

		char szImageFindPath[128] = "../Bin/Resources/Textures/Decal/tga/";    // 상대 경로 -> 모든 파일을 돌겠다
		char szImagePathReset[128] = "../Bin/Resources/Textures/Decal/tga/";
		char szImagePath[128] = "../Bin/Resources/Textures/Decal/tga/";

		//폴더 리스트에서 선택한 인덱스로 파일 검색 경로 생성
		strcat_s(szImageFindPath, m_Decal_Folder_Names[m_iListSelectNum]);
		strcat_s(szImageFindPath, "/ARM/*.*");

		handle = _findfirst(szImageFindPath, &fd);

		if (handle == -1)
			return;

		iResult = 0;
		int iImagesCount = 0;

		while (iResult != -1)
		{
			strcpy_s(szImagePath, szImageFindPath);
			strcat_s(szImagePath, fd.name);

			_char szDirName[MAX_PATH] = "";
			_char szFileName[MAX_PATH] = "";
			_char szExt[MAX_PATH] = "";
			_splitpath_s(szImagePath, nullptr, 0, szDirName, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);

			if (!strcmp(szExt, ".") || !strcmp(szExt, "..") || strcmp(szExt, ".tga"))
			{
				iResult = _findnext(handle, &fd);
				continue;
			}

			iImagesCount++;

			//_strup : 문자열 내용을 복사해 그 주소를 저장-> 주소에 따라 문자열이 바뀌는걸 막아 모두 동일해지는걸 막음
			m_Decal_File_Names_Arm.push_back(_strdup(fd.name));

			//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
			iResult = _findnext(handle, &fd);
		}

		static bool item_Image_highlight = false;
		int item_Image_highlighted_idx = -1; // Here we store our highlighted data as an index.

		if (ImGui::BeginListBox("ARM"))
		{
			for (int n = 0; n < iImagesCount; n++)
			{
				const bool is_selected = (m_iSelected_Decal_ARM_ID == n);
				if (ImGui::Selectable(m_Decal_File_Names_Arm[n], is_selected))
					m_iSelected_Decal_ARM_ID = n;

				if (item_Image_highlight && ImGui::IsItemHovered())
					item_Image_highlighted_idx = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();

				m_iSelected_Decal_ARM_ID;
			}
			ImGui::EndListBox();
		}

#pragma region SHOW IMAGES PREVIEW_Normal
		static _bool bShowPreview_ARM = false;
		ImGui::Checkbox("Image Preview_ARM", &bShowPreview_ARM);

		if (bShowPreview_ARM)
		{
			ImGui::Begin("Preview_ARM", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar
				| ImGuiWindowFlags_AlwaysVerticalScrollbar);

			int my_image_width = 0;
			int my_image_height = 0;

			if (m_my_texture) {
				Safe_Release(m_my_texture);
				m_my_texture = NULL;
			}

			char szImageReadPath[128] = "";

			if (m_iSelected_Decal_ARM_ID != -1)
			{
				strcpy_s(szImageReadPath, szImagePathReset);
				strcat_s(szImageReadPath, m_Decal_Folder_Names[item_selected_idx]);
				strcat_s(szImageReadPath, "/ARM/");
				strcat_s(szImageReadPath, m_Decal_File_Names_Arm[m_iSelected_Decal_ARM_ID]);

				bool ret = LoadTextureFromFile(szImageReadPath, &m_my_texture, &my_image_width, &my_image_height);
				IM_ASSERT(ret);

				ImGui::Image((ImTextureID)(intptr_t)m_my_texture, ImVec2(my_image_width, my_image_height));
			}

			ImGui::End();
		}

#pragma endregion
	}
#pragma endregion


	if (ImGui::Button("Create_Decal"))
	{
		CNonAnimModel::NONMODEL_DESC Desc{};
		Desc.vPosition = m_vPickPos;
		Desc.vScale = { 1.f,1.f,1.f };
		Desc.vRotation = { 0.f,0.f,0.f };
		Desc.iRenderGroupID = 0;
		Desc.isLight = false;
		Desc.isDecal = true;
		size_t length = strlen(m_Decal_File_Names_Diffuse[m_iSelected_Decal_Diffuse_ID]);
		strncpy_s(Desc.szTextureTag_Diffuse, m_Decal_File_Names_Diffuse[m_iSelected_Decal_Diffuse_ID], length -4); //파일 이름만
		if (bUseNormal)
		{
			Desc.isNormal = true;
			size_t length = strlen(m_Decal_File_Names_Normal[m_iSelected_Decal_Normal_ID]);
			strncpy_s(Desc.szTextureTag_Normal, m_Decal_File_Names_Normal[m_iSelected_Decal_Normal_ID], length - 4); //파일 이름만
		}
		if (bUseARM)
		{
			Desc.isARM = true;
			size_t length = strlen(m_Decal_File_Names_Arm[m_iSelected_Decal_ARM_ID]);
			strncpy_s(Desc.szTextureTag_ARM, m_Decal_File_Names_Arm[m_iSelected_Decal_ARM_ID], length - 4); //파일 이름만
		}

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Decal"), TEXT("Prototype_GameObject_NonAnim"), &Desc)))
			return;
	}
}

_bool CController_MapTool::LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	m_pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	if (pTexture == nullptr)
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	Safe_Release(pTexture);

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}

_bool CController_MapTool::LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	FILE* f;
	fopen_s(&f, file_name, "rb");
	if (f == NULL)
		return false;
	fseek(f, 0, SEEK_END);
	size_t file_size = (size_t)ftell(f);
	if (file_size == -1)
		return false;
	fseek(f, 0, SEEK_SET);
	void* file_data = IM_ALLOC(file_size);
	fread(file_data, 1, file_size, f);
	bool ret = LoadTextureFromMemory(file_data, file_size, out_srv, out_width, out_height);
	IM_FREE(file_data);

	fclose(f);

	return ret;
}

void CController_MapTool::Free()
{
	__super::Free();

	for (auto& filename : m_FileNames) {
		Safe_Delete_Array(filename);
	}
	m_FileNames.clear();
	
	for (auto& filename : m_Decal_Folder_Names) {
		Safe_Delete_Array(filename);
	}
	m_Decal_Folder_Names.clear();
	
	for (auto& filename : m_Decal_File_Names_Diffuse) {
		Safe_Delete_Array(filename);
	}
	m_Decal_File_Names_Diffuse.clear();	

	for (auto& filename : m_Decal_File_Names_Normal) {
		Safe_Delete_Array(filename);
	}
	m_Decal_File_Names_Normal.clear();

	for (auto& filename : m_Decal_File_Names_Arm) {
		Safe_Delete_Array(filename);
	}
	m_Decal_File_Names_Arm.clear();

	Safe_Release(m_my_texture);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pNavigationController);
	Safe_Release(m_pNavigationController);
	Safe_Release(m_pGameInstance);
}
