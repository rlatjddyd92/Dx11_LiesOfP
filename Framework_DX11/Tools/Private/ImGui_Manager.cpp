#include "stdafx.h"
#include "ImGui_Manager.h"

#include "Controller_MapTool.h"
#include "Controller_EffectTool.h"
#include "Controller_UITool.h"
#include "Controller_AnimationTool.h"
#include "Controller_PostProcess.h"
#include "Controller_Cutscene.h"

#include <iostream>
using namespace std;

IMPLEMENT_SINGLETON(CImGui_Manager)



CImGui_Manager::CImGui_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}



HRESULT CImGui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	Ready_Controllers();

	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)(clientRect.right - clientRect.left),
		(float)(clientRect.bottom - clientRect.top));

	return S_OK;
}

void CImGui_Manager::Render_ImGui()
{
	// 다른 함수에서 분리돼서 호출하면 에러가 남
	// 그래서 구조가 좀 다름

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(false);



	// 업데이트 내에서 수치 조정할 것들을 처리해 줄 것임
	Update_ImGui();

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGui_Manager::Update_ImGui()
{
	ImGui::Begin("TOOL PROJECT", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar
		| ImGuiWindowFlags_AlwaysVerticalScrollbar);

	// 위젯 안에 있는지 판단함
	_bool isInUI = false;

	ImVec2 window_pos = ImGui::GetWindowPos();
	ImVec2 window_size = ImGui::GetWindowSize();

	ImVec2 mouse_pos = ImGui::GetMousePos();
	if (mouse_pos.x >= window_pos.x && mouse_pos.x <= (window_pos.x + window_size.x) &&
		mouse_pos.y >= window_pos.y && mouse_pos.y <= (window_pos.y + window_size.y)) 
	{
		isInUI = true;
	}

	Tool_Cutscene();

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		//이펙트 툴에서 플레이어 보이게 하기 위한 변수
		bAnimCtr = false;

		Tool_Map();

		Tool_Effect();

		Tool_UI();

		Tool_Animation();

		Tool_PostProcess();
	}

	ImGui::EndTabBar();

	ImGui::End();

	
}

void CImGui_Manager::Tool_Cutscene()
{
	static _bool bShow_Cutscene_Tool = false;
	ImGui::Checkbox("Show Cuscene Tool", &bShow_Cutscene_Tool);

	ImGui::Text("");

	if (bShow_Cutscene_Tool == false)
		return;

	ImGui::Begin("Cutscene_Tool", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar
		| ImGuiWindowFlags_AlwaysVerticalScrollbar);
	
	m_pController_Cutscene->Update(m_pGameInstance->Compute_TimeDelta(TEXT("Timer_60")));

	ImGui::End();

}

void CImGui_Manager::Tool_Map()
{
	if (ImGui::BeginTabItem("Map Tool"))
	{
		m_pController_MapTool->PickPos();
		m_pController_MapTool->Pick_Object();

		m_pController_MapTool->Control_Player();
		m_pController_MapTool->Create_Map();
		m_pController_MapTool->Save_Load();

		ImGui::EndTabItem();
	}
}

void CImGui_Manager::Tool_Effect()
{
	if (ImGui::BeginTabItem("Effect Tool"))
	{
		m_pController_EffectTool->Set_JumhoCamera(true);
		m_pController_EffectTool->Render();
		bAnimCtr = true;
	}
	else
		m_pController_EffectTool->Set_JumhoCamera(false);
}

void CImGui_Manager::Tool_UI()
{
	// 24-12-01 김성용 
   // UI 연출 관련 테스트 코드 
	if (KEY_HOLD(KEY::CTRL))
	{
		if (KEY_TAP(KEY::NUM6))
			m_pController_UITool->Fade_Out(TEXT("FadeOut_Test"), TEXT("FadeOut_Desc"), { 0.f,0.f,0.f }, 2.f);
		if (KEY_TAP(KEY::NUM7))
			m_pController_UITool->Fade_In(2.f);
		/*if (KEY_TAP(KEY::NUM8))
			m_pController_UITool->Show_Script(TEXT("테스트 대사 2줄 버전"), TEXT("테스트 대사 2줄 버전"), 3.f);
		if (KEY_TAP(KEY::NUM9))
			m_pController_UITool->Show_Script(TEXT("테스트 대사 1줄 버전"), TEXT("none"), 3.f);*/
	}


	if (ImGui::BeginTabItem("UI Tool"))
	{
		m_pController_UITool->UITool_Edit();

		ImGui::EndTabItem();

		m_pController_UITool->UITool_Render();
	}
	
	if (m_pController_UITool->Get_EffectOn())
		m_pController_UITool->UpdateEffect(m_pGameInstance->Compute_TimeDelta(TEXT("Timer_60")));
}

void CImGui_Manager::Tool_Animation()
{
	if (ImGui::BeginTabItem("Animation Tool"))
	{
		ImGui::CollapsingHeader("Animation");

		m_pController_AnimationTool->SetUp_AnimTool();

		m_pController_AnimationTool->EndFrame_AnimTool();

		ImGui::EndTabItem();
	}
	else
	{
		if (!bAnimCtr)
		{
			m_pController_AnimationTool->Set_BlockObjCtr(false);
		}
		else
			m_pController_AnimationTool->Set_BlockObjCtr(true);
	}
}

void CImGui_Manager::Tool_PostProcess()
{
	if (ImGui::BeginTabItem("PostProcess Tool"))
	{
		m_pController_PostProcess->Update_CameraSetting();
		m_pController_PostProcess->Update_SSAO();
		m_pController_PostProcess->Update_HDR();
		m_pController_PostProcess->Update_BLOOM();
		m_pController_PostProcess->Update_DOF();
		m_pController_PostProcess->Update_Radial();

		ImGui::EndTabItem();
	}
}

void CImGui_Manager::ImGui_Spacing(_uint iSpaceCount)
{
	for (_uint i = 0; i < iSpaceCount; ++i)
	{
		ImGui::Spacing();
	}
}
HRESULT CImGui_Manager::Ready_Controllers()
{
	m_pController_MapTool = CController_MapTool::Get_Instance();
	if (nullptr == m_pController_MapTool)
		return E_FAIL;
	m_pController_MapTool->Initialize(m_pDevice, m_pContext);

	m_pController_EffectTool = CController_EffectTool::Get_Instance();
	if (nullptr == m_pController_EffectTool)
		return E_FAIL;
	//m_pController_EffectTool->Initialize();

	m_pController_UITool = CController_UITool::Get_Instance();
	if (nullptr == m_pController_UITool)
		return E_FAIL;
	m_pController_UITool->Initialize(m_pDevice, m_pContext);

	m_pController_AnimationTool = CController_AnimationTool::Get_Instance();
	if (nullptr == m_pController_AnimationTool)
		return E_FAIL;
	m_pController_AnimationTool->Initialize(m_pDevice, m_pContext);

	m_pController_PostProcess = CCamera_Manager::Get_Instance();
	if (nullptr == m_pController_PostProcess)
		return E_FAIL;
	m_pController_PostProcess->Initialize(m_pDevice, m_pContext);

	m_pController_Cutscene = CController_Cutscene::Get_Instance();
	if (nullptr == m_pController_Cutscene)
		return E_FAIL;
	m_pController_Cutscene->Initialize(m_pDevice, m_pContext);

	return S_OK;
}
void CImGui_Manager::Free()
{
	__super::Free();

	m_pController_PostProcess->Destroy_Instance();
	m_pController_EffectTool->Destroy_Instance();
	m_pController_UITool->Destroy_Instance();
	m_pController_AnimationTool->Destroy_Instance();
	m_pController_MapTool->Destroy_Instance();
	m_pController_Cutscene->Destroy_Instance();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
}